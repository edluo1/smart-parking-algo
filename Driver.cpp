// Driver.cpp

#include "Driver.h"
#include "Location.h"
#include "Lot.h"
#include "DriveVector.h"
#include <cmath>
#include <iostream>
using namespace std;

Driver::Driver(int ID, double arrivalTime, double weightScale, double maxDist, double maxPay, Location loc, double timeAtPark, Destination * toReach, Grid * as) {
	this->id = ID;
	try {
		if (weightScale < 0 || weightScale > 1) {
			throw weightScale;
		} else {
			this->importanceWeight = weightScale; // between 0 and 1
		}
	} catch (double e) {
		cout << "Scale value" << e << "must be between 0 and 1 inclusive" << endl;
	}
	this->timeOfArrival = arrivalTime;
	this->timeAtPark = timeAtPark;
	this->maxWalkDist = maxDist;
	this->maxCharge = maxPay;
	this->location = loc;
	this->dest = toReach;
	this->reserveSpot = -1;
	this->world = as;
	this->parked = false;
	this->state = 'z';
	this->speed = 5; // set speed to 5 by default
}

int Driver::getID() { // Function returns the ID value of driver
	return this->id;
}

Location Driver::getLocation() { // Returns driver's exact location
	return location;
}

bool Driver::accept(Lot) { // returns true if accepted, false if refused
	return true; // returns true by default, may be changed
}

bool Driver::isInIA() {
	if (this->getDistToDest() <= world->timeIncrement*speed) { 
		return true;
	} else return false;
}

bool Driver::departLot() { // return true if parked, else return false
	// when returns true, Grid will remove this parker from the list of parked people.
	if (parked) {
		this->parked = false;
		return true;
	} else {
		return false;
	}
}

double Driver::getArrivalTime() {
	return timeOfArrival;
}

double Driver::getTimeAtPark() {
	return timeAtPark;
}

Lot * Driver::makeReservation(double timeParking) { // finds potential lots
	// Currently based on the best option given at the time.
	// Later we plan to utilize it better.
	
	feasLots.empty();
	lotDist.empty();
	lotCharge.empty();
	lotCost.empty();
	
	feasLots = findLots(timeParking); // find parking lots
	
	int lotVectSize = feasLots.size();
	int bestLotAt;
	Lot * bestLot;
	double minCost = 10000; // arbitrarily large number. All costs are less than 10.
	for (int ii = 0; ii < lotVectSize; ii++) {
		if (lotCost[ii] < minCost) {
			bestLotAt = ii;
			minCost = lotCost[bestLotAt];
			bestLot = feasLots[bestLotAt];
		}
	}
	if (lotVectSize != 0) {
		cout << "Minimum lot at ID " << bestLot->getID() << "." << endl;
		cout << "Distance: " << lotDist[bestLotAt] << " Charge: " << lotCharge[bestLotAt] << endl;
		return bestLot;
	} else {
		return NULL; // no lots are available
	}
}

vector<Lot *> Driver::findLots(double timeParking) {
	// initialize vars
	vector<Lot *> lotsAvailable; 
	double distance;
	double charge;
	double cost;
	
	vector<Lot *> allLots = world->getAllLots();
	for (int ii = 0; ii < allLots.size(); ii++) {
		if (allLots[ii]->numNotReserved != 0) { // won't add lot that has no spaces available
			distance = dist(allLots[ii]->getLocation(), dest->getLocation()); // calculate distance
			if (distance <= this->maxWalkDist) { // if destination within walking distance
				charge = allLots[ii]->getCost(timeParking); // calculate cost
				if (charge <= this->maxCharge) { // if cost within specified range
					lotsAvailable.push_back(allLots[ii]); // add lot to lots available
					lotDist.push_back(distance);
					lotCharge.push_back(charge);
					cost = importanceWeight*(charge/maxCharge) + (1-importanceWeight)*(distance/maxWalkDist);
					lotCost.push_back(cost);
				}
			}
		}
	}
	if (lotsAvailable.size() == 0) { // No lots available for driver
		cout << "No lots are available." << endl; 
	}
	return lotsAvailable;
}

bool Driver::update() { // update driver parking, returns true on state change
	/*
	 *	Updates what the driver does.
	 *  State 'z': Driver has not yet appeared on the grid as it's not his time.
	 *  State 'n': Driver has nowhere to park. Will drive towards destination.
	 *  State 'd': Driver is driving to parking lot. When reached, will park.
	 *  State 'p': Driver is parked and will stay parked until time is up.
	 *  State 'g': Driver has finished parking and will leave list of people
	 *  in the next state.
	*/
	switch(this->state) {
		case 'z':
			if (world->getTime() >= timeOfArrival) { // if car should arrive at this time
				reserved = makeReservation(timeAtPark); // reserving spot
				if (reserved == NULL) { // if reserved nothing
					setup_destination(dest->getLocation());
					this->state = 'n'; // driving towards destination
				} else {
					setup_destination(reserved->getLocation());
					reserved->numNotReserved--;
					this->state = 'd'; // driving to lot
				}
				return true;
			} else {
				return false;
			}
			break;
		case 'n': // drive towards destination, but waiting for lots
			update_location(); // move the car
			reserved = makeReservation(timeAtPark); // try reserving a spot again
			if (reserved != NULL) { // if reservation exists
				setup_destination(reserved->getLocation());
				reserved->numNotReserved--; // decrement number of nonreserved spaces
				this->state = 'd'; // driving to lot
				return true;
			}
			return false;
			break;
		case 'd': // drive towards parking lot
			if (update_location() == true) { // move car and check if arrived at parking lot
				this->state = 'p'; // set state to "parking"
				reserved->numFree--; // decrement number of spots available
				timeArrivedAtPark = world->getTime(); // store time of lot arrival
				return true;
			} else { // driver hasn't reached parking lot
				return false;
			}
			break;
		case 'p': // parking
			if (world->getTime() >= timeArrivedAtPark+timeAtPark) {
				this->state = 'g'; // state is now 'leaving the area'
				reserved->numFree++; // increment number of spots available
				reserved->numNotReserved++;
				return true;
			} else {
				return false;
			}
			break;
		case 'g': // leaving parking
			return false;
	}
}

bool Driver::update_location() { // Moves towards destination. Return true if reached.
	DriveVector distDiff = travelPoint-location;
	if (fabs(distDiff.x) <= fabs(driveDirection.x) && fabs(distDiff.y) <= fabs(driveDirection.y)) { // if the fish can reach destination
		location = travelPoint; // set location to destination
		return true;
	} else {
		location = location + driveDirection; // move driver closer
		return false; 
	}
}

void Driver::setup_destination(Location dest) { // setup where to go
  this->travelPoint = dest; // Destination is set to value
  double totalDistance = dist(travelPoint,location);
  if (totalDistance != 0) { // check if destination is the same place or not
    this->driveDirection = (travelPoint - location) * (this->speed/totalDistance)*world->timeIncrement; // delta equation
  } else {
    this->driveDirection = DriveVector(0,0);
  }
}

double Driver::getDistToDest() {
	return dist(this->location, dest->getLocation());
}

char Driver::getState() {
	return this->state;
}

void Driver::show_status() { // output driver ID, location, destination, and lot if applicable
	switch (this->state) {
		case 'n': // seeking lots
			cout << "Driver " << this->id << " located at " << this->location << " is seeking lots and heading for Destination " << dest->getID() << endl;
			break;
		case 'd': // heading to lot
			cout << "Driver " << this->id << " located at " << this->location << " is heading to Lot " << reserved->getID() << endl;
			break;
		case 'p': // parking
			cout << "Driver " << this->id << " located at " << this->location << " is parked at Lot " << reserved->getID() << endl;
			break;
		case 'g':
			cout << "Driver " << this->id << " has left the map." << endl;
		default:
			break; // do nothing
	}
}