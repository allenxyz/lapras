#ifndef MODEL_H
#define MODEL_H

/*
Model is part of a simplified Model-View-Controller pattern.
Model keeps track of the Sim_objects in our little world. It is the only
component that knows how many Islands and Ships there are, but it does not
know about any of their derived classes, nor which Ships are of what kind of Ship. 
It has facilities for looking up objects by name, and removing Ships.  When
created, it creates an initial group of Islands and Ships using the Ship_factory.
Finally, it keeps the system's time.

Controller tells Model what to do; Model in turn tells the objects what do, and
when asked to do so by an object, tells all the Views whenever anything changes that might be relevant.
Model also provides facilities for looking up objects given their name.
*/

#include <string>
#include <set>
#include <map>
#include <vector>
#include <memory>

// Declare the global model pointer
class Model;

class Sim_object;
class Island;
class Ship;
class View;
struct Point;
struct Course_speed;


template <typename T>
struct Less_obj {
	bool operator()(const T p1, const T p2) const {return p1 -> get_name() < p2 -> get_name();}
};

struct Less_name {
	bool operator()(const std::string name1, const std::string name2) const {return name1 < name2;}
};

class Model {
public:
	// create the initial objects, output constructor message
	Model();
	
	// return the current time
	int get_time() {return time;}

	// is name already in use for either ship or island?
    // either the identical name, or identical in first two characters counts as in-use
	bool is_name_in_use(const std::string& name) const;

	// is there such an island?
	bool is_island_present(const std::string& name) const;
	// will throw Error("Island not found!") if no island of that name
	std::shared_ptr<Island> get_island_ptr(const std::string& name) const;

	// is there such an ship?
	bool is_ship_present(const std::string& name) const;
	// add a new ship to the list, and update the view
	void add_ship(std::shared_ptr<Ship>);
	// will throw Error("Ship not found!") if no ship of that name
	std::shared_ptr<Ship> get_ship_ptr(const std::string& name) const;
	
	// tell all objects to describe themselves
	void describe() const;
	// increment the time, and tell all objects to update themselves
	void update();	
	
	/* View services */
	// Attaching a View adds it to the container and causes it to be updated
    // with all current objects'location (or other state information.
	void attach(std::shared_ptr<View>);
	// Detach the View by discarding the supplied pointer from the container of Views
    // - no updates sent to it thereafter.
	void detach(std::shared_ptr<View>);
	
    // notify the views about an object's location
	void notify_location(const std::string& name, Point location);
	void notify_ship_info(const std::string & name, Course_speed cs, double fuel);
	
	// notify the views that an object is now gone
	void notify_gone(const std::string& name);

	//stops tracking a ship when a ship has sunk
	void remove_ship(std::shared_ptr<Ship> ship_ptr);

	static Model* get();

	//returns a vector of pointers to islands in alphabetical order
	std::vector<std::shared_ptr<Island>> get_all_islands();

	// disallow copy/move construction or assignment
	Model(const Model&) =delete;	
	Model(Model&&) =delete;	
	Model& operator= (const Model&) =delete;
	Model& operator= (Model&&) =delete; 
	
private:
	using Sim_obj_set = std::set<std::shared_ptr<Sim_object>, Less_obj<std::shared_ptr<Sim_object>>>;
	using Ship_map = std::map<std::string, std::shared_ptr<Ship>, Less_name>;
	using Island_map = std::map<std::string, std::shared_ptr<Island>, Less_name>;
	using Views_set = std::set<std::shared_ptr<View>>;

	int time;		// the simulated time
	Sim_obj_set all;
	Island_map islands;
	Ship_map ships;

	Views_set views;

	void track_island(std::shared_ptr<Island> island);
	void track_ship(std::shared_ptr<Ship> ship);

};



#endif