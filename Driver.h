#ifndef DRIVER_H
#define DRIVER_H

#include <iostream>
#include <vector>
#include "Location.h"
#include "Destination.h"
#include "Lot.h"
#include "Grid.h"
using namespace std;

class Grid;
class Destination;
class Lot;

class Driver {
	public:
		Driver();
		Driver(int, double, Location, double, Destination *, Grid *); // creates user
		Lot * reserved; // lot which the driver has reserved
		double speed; // speed the driver travels at
		vector<Lot *> feasLots; // lists all lots available for the driver
		vector<double> lotDist; // distance from each lot to destination
		vector<double> lotCharge; // how much it costs to stay there
		vector<double> lotCost; // cost function of using the parking lot
		double lowestLotCost; // measured when assigned a lot to ensure that this driver gets no worse lot
		Destination * dest;
		
		int getID(); // return id
		bool isInIA(); // checks if driver is to be allocated immediately
		Location getLocation(); // return location
		bool accept(Lot); // choose to accept the lot
		Lot * makeReservation(double); // makes reservation. If not satisfied, then will try to find a better one
		double getDistToDest(); // return distance from driver to destination
		bool departLot(); // if parked, leave parking lot
	
	private:
		int id;
		Location location; // gets location on map (GPS realistically)
		double importanceWeight; // between 0 and 1: used to determine balance between walking and cost
		double timeInReserve; // time spent in reserve queue. 0 if not in it
		int reserveSpot; // 0 or -1 if not reserved, else ID of reserved spot
		double maxWalkDist; // longest distance able to walk to get to destination
		double maxCharge; // largest amount able to pay to park
		double timeAtPark; // how long the driver wishes to park for
		Grid * world; // full allocation system
		bool parked;
		
		vector<Lot *> findLots(double); // Calculates feasLot and lotDist
		Lot * findOptLot(); // Calculates lotCost
};

#endif