#include <iostream>

#include "Utility.h"

#include "Geometry.h"
#include "Navigation.h"
#include "Track_base.h"

#include "Island.h"
#include "Ship.h"
#include "Cruiser.h"
#include "Warship.h"
#include "Tanker.h"
#include "Ship_factory.h"
#include "Model.h"
#include "Sim_object.h"
#include "View.h"

#include <string>
using namespace std;

int main () {
	cout << "----creatin stuff-----" << endl;
	Island* island1 = new Island("island1", Point(0,0), 300, 20);
	Island* island2 = new Island("far away", Point(500, 500), 300, 20);
	Ship* cruiser1 = create_ship("cruiser1", "Cruiser", Point(0,0));
	Ship* cruiser2 = create_ship("cruiser2", "Cruiser", Point(1, 1));
	Ship* tanker1 = create_ship("tanker1", "Tanker", Point(0,0));
	// Model::get() = new Model();

	cout << "-----stuff all created-----" << endl;


	cout << "-----testing ship stuff-----" << endl;
	cout << cruiser1 -> can_move() << cruiser1 -> is_moving() << cruiser1 -> is_docked() << cruiser1 -> is_afloat() << cruiser1 -> is_on_the_bottom() << endl;
	cout << "true: " << cruiser1 -> can_dock(island1);
	cout << "false: " << cruiser1 -> can_dock(island2);
	cruiser1 -> describe();

	cout << endl;
	cout << "sending all boats to faraway" << endl;
	cruiser1 -> set_destination_position_and_speed(island2 -> get_location(), 20); // max speed
	try {
	cruiser2 -> set_destination_position_and_speed(island2 -> get_location(), 25);
	}
	catch (Error & e) {
		cout << e.what() << endl;
	}
	cout << endl;
	cout << "update c1" << endl;
	cruiser1 -> update();
	cout << "c1 attack c2" << endl;
	cruiser1 -> attack(cruiser2);
	cout << "update c1" << endl;
	cruiser1 -> update();
	cout << "update c2" << endl;
	cruiser2 -> update();
	cout << "-----ship stuff end-----" << endl;



	cout << "---- testing views-----" << endl;
	View test;
	test.draw();
	test.update_location("one", Point(0,0));
	test.update_location("two", Point(15, 15));
	test.update_location("three", Point(90, 90));
	test.draw();

	test.update_location("one", Point(15, 15));
	test.update_location("three", Point(0, 0));

	test.draw();
	test.update_remove("one");
	test.draw();
	test.update_remove("thenn");


	delete island1;
	delete island2;
	delete cruiser1;
	delete cruiser2;
	delete tanker1;

	return 0;




}