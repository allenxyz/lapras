#ifndef VIEW_H
#define VIEW_H
/* *** View class ***
The View class encapsulates the data and functions needed to generate the map
display, and control its properties. It has a "memory" for the names and locations
of the to-be-plotted objects.

Usage: 
1. Call the update_location function with the name and position of each object
to be plotted. If the object is not already in the View's memory, it will be added
along with its location. If it is already present, its location will be set to the 
supplied location. If a single object changes location, its location can be separately
updated with a call to update_location. 

2. Call the update_remove function with the name of any object that should
no longer be plotted. This must be done *after* any call to update_location that
has the same object name since update_location will add any object name supplied.

3. Call the draw function to print out the map.

4. As needed, change the origin, scale, or displayed size of the map
with the appropriate functions. Since the view "remembers" the previously updated
information, the draw function will print out a map showing the previous objects
using the new settings.
*/

/* 
*** This skeleton file shows the required public and protected interface for the class, 
which you may not modify.
If no protected members are shown, there must be none in your version. 
If any protected or private members are shown here, then your class must also have them 
and use them as intended.
You should delete this comment.
*/
#include "Geometry.h"
#include <string>
#include <vector>
#include <map>

class View {
protected:
	// default constructor sets the default size, scale, and origin, outputs constructor message
	View(); 
public:	
	// Save the supplied name and location for future use in a draw() call
	// If the name is already present,the new location replaces the previous one.
	void update_location(const std::string& name, Point location);

	//save information about ships for bridge/sailing view
	void update_ship_info(const std::string & name, double fuel, double course, double speed);
	
	// Remove the name and its location; no error if the name is not present.
	virtual void update_remove(const std::string& name);

	// prints out the current map
	virtual void draw();
	
	// Discard the saved information - drawing will show only a empty pattern
	void clear(); 
	
	// modify the display parameters
	// if the size is out of bounds will throw Error("New map size is too big!")
	// or Error("New map size is too small!")
	virtual void set_size(int size_);
	
	// If scale is not postive, will throw Error("New map scale must be positive!");
	virtual void set_scale(double scale_);
	
	// any values are legal for the origin
	virtual void set_origin(Point origin_);
	
	// set the parameters to the default values
	virtual void set_defaults();
				



protected:
	struct Ship_info {
		double fuel;
		double course;
		double speed;
	};
	//returns a vector of all object names
	std::vector<std::string> get_all_object_names();

	//returns a vector of all ship names
	std::vector<std::string> get_all_ship_names();

	//finds a point/location where a Sim_object with object_name is located at
	Point get_object_location(std::string object_name);

	//finds the ship_info associated with the ship
	Ship_info get_ship_info(std::string ship_name);


private:
	//member variables needed to track information about sim_objects
	std::map<std::string, Point> object_locations;
	std::map<std::string, Ship_info> ship_infos;			
};

#endif