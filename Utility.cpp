#include "Utility.h"
#include "Geometry.h"
#include "Model.h"
#include "Island.h"
#include "Ship.h"

#include <iostream>

using namespace std;

const double round_off_error_c = 0.005;

int read_int() {
	int read_value;
	cin >> read_value;
	if (!cin) throw Error("Expected an integer!");
	return read_value;
}

double read_double() {
	double read_value ;
	cin >> read_value;
	if (!cin) throw Error("Expected a double!");
	return read_value;
}

double read_speed() {
	double speed = read_double();
	if (speed < 0) throw Error("Negative speed entered!");;
	return speed;
}

double read_heading() {
	double heading = read_double();
	if (heading < 0.0 || heading >= 360) throw Error("Invalid heading entered!");
	return heading;
}

Point read_point() {
	double x_coord = read_double();
	double y_coord = read_double();
	return Point(x_coord, y_coord);
}

string read_name() {
	string name;
	cin >> name;
	if (name.size() < 2) throw Error("Name is too short!");
	return name;
}

shared_ptr<Island> read_island() {
	string island_name = read_name();
	return Model::get() -> get_island_ptr(island_name);
}

shared_ptr<Ship> read_ship() {
	string ship_name = read_name();
	return Model::get() -> get_ship_ptr(ship_name);
}