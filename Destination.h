// Destination.h

#include "Location.h"

class Destination {
	public:
		Destination();
		Destination(int, Location);
		int id; // id of the destination
		int atDest; // number of people at destination
		double getRate(); // get rate of people entering to calculate costs
	private:
		Location location;
		double areaReadius; // radius of major parking destination
		double arrivalRate; // rate of entrance to destination
		double avgDuration; // average duration at destination
};