// Pricing Policy 3: Real-time, Proportional Dynamic Pricing Policy

#include <iostream>
#include <vector>
#include <Lot.h>
using namespace std;


// Adjusts prices of all lots based on current "reserved rate" - fraction of reserved (not necessarily occupied) spots 
// Each Lot's cost will be updated if its current reserved rate is outside of [targetResRate - limit, targetResRate + limit]
// The cost is changed by the following amount (current reserved rate - target reserved rate)/current reserved rate
// This function should be called after every event or every few events so that price updates are real-time
void updatePrices3(vector<Lot*> allLots, double targetResRate, double limit) {

	// Iterate through each Lot
	for (vector<Lot*>::iterator currentLot = allLots.begin(); currentLot != allLots.end(); currentLot++) {

		// Get the Lot's current occupancy rate
		double currentResRate = (*currentLot)->getReservedRate();

		// Update the Lot's cost if its current occupancy rate is outside of the desired occupancy rate range
		if ((currentResRate < (targetResRate - limit)) || (currentResRate >(targetResRate + limit))) {

			// Get the Lot's current cost
			double currentCost = (*currentLot)->getCost(1.0); // NEEDS TO BE ADDED TO LOT FUNCTIONS: getCost() should return the parking cost per unit time

			// Calculate the new cost  
			double newCost = currentCost + currentCost*((currentResRate - targetResRate) / currentResRate);

			// If the new cost is less than 0, set it to 0. If it is greater than 1, set it to 1. 
			if (newCost < 0) {
				newCost = 0.0;
			}
			else if (newCost > 1) {
				newCost = 1.0;
			}

			// Update the Lot's cost
			(*currentLot)->setCost(newCost); // NEEDS TO BE ADDED TO LOT FUNCTIONS: setCost(double) should set the parking cost per unit time
		}
	}
}