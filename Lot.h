// Lot.h

#ifndef LOT_H
#define LOT_H

#include "Location.h"
#include "Grid.h"

class Grid;

class Lot { // the ID of a location 
	public:
		Lot();
		Lot(int, Location, int, Grid *);
		
		int numFree; // spots not visually taken. Once car parks, this decreases.
		int numNotReserved; // spots not reserved. Once driver reserves, this decreases.
		// numNotReserved is always less than or equal to numFree
		int getID(); // return ID
		Location getLocation(); // return location
		double getCost(double); // get cost to use parking lot
		bool update(); // update number of spots
		void show_status(); // shows status of parking lot
	private:
		int id;
		Location location; // location of lot
		int capacity; // can be 1 for individual or total for parking lots
		double cost;
		Grid * world; // full allocation system
};

#endif