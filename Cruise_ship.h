#ifndef CRUISE_SHIP_H
#define CRUISE_SHIP_H

#include "Ship.h"

#include <set>
#include <string>
#include <memory>
#include <vector>

class Point;

class Cruise_ship : public Ship {
public: 
	Cruise_ship(const std::string name_, Point position_);

	//sets desintation and speed (ship function) and checks if it's time to go on a cruise!
	void set_destination_position_and_speed(Point destination, double speed) override;

	//these need to be overrided to check to cancel if on a cruise
	void set_course_and_speed(double course, double speed) override;
	void stop() override;


	void update() override;
	void describe() const override;


private:
	//keeps track of all possible islands when on a cruise
	std::vector<std::weak_ptr<Island>> all_islands;

	//keeps track of where the cruise has been
	std::vector<bool> visited_island;
	
	//how fast the cruise should be going
	double crusing_speed;

	//number of visited islands, this is the number of true's in visited_island
	int num_visited_islands;

	//reflects where the cruise is heading
	int next_location_index;

	//where the cruise started and ends
	int cruise_begin_index;


	enum class Cruise_state {NOT_ON_CRUISE, 
							 REFUEL, 
							 WAIT, 
							 PREP_NEXT_LOC,
							 MOVING_TO_NEXT, 
							 RETURNING};
	Cruise_state cruise_state;

	//finds the next closest island and updates the next location
	//returns true if a closest island was found
	//false if there is nowhere else to go
	bool find_next_stop();

	//resets variables to reflect cruise has ended
	void end_cruise();
	void cancel_cruise();
};

#endif