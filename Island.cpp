#include "Island.h"
#include "Model.h"

#include <iostream>

using namespace std;


Island::Island(const std::string& name_, Point position_, double fuel_, double production_rate_) : 
	Sim_object(name_), 
	position(position_), 
	fuel(fuel_), 
	production(production_rate_) {
	// cout << "Island " << get_name() << " constructed" << endl;
}

Island::~Island() {
	// cout << "Island " << get_name() << " destructed" << endl;
}

double Island::provide_fuel(double request) {
	//provide min(request, fuel)
	double provide = request < fuel ? request : fuel;
	fuel -= provide;
	cout << "Island " << get_name() << " supplied " << provide << " tons of fuel" << endl;
	return provide;
}

void Island::accept_fuel(double amount) {
	fuel += amount;
	cout << "Island " << get_name() << " now has " << fuel << " tons" << endl;
}

void Island::update() {
	if (production > 0) accept_fuel(production);
}

void Island::describe() const {
	cout << "\nIsland " << get_name() << " at position "  << position << endl;
	cout << "Fuel available: " << fuel << " tons" << endl;
}

void Island::broadcast_current_state() {
	Model::get() -> notify_location(get_name(), get_location());
}