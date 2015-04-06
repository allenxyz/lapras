# enables c++11 on CAEN
PATH := /usr/um/gcc-4.7.0/bin:$(PATH)
LD_LIBRARY_PATH := /usr/um/gcc-4.7.0/lib64
LD_RUN_PATH := /usr/um/gcc-4.7.0/lib64


CC = g++
LD = g++

CFLAGS = -c -pedantic-errors -std=c++11 -Wall -g
LFLAGS = -pedantic -Wall

OBJS = p5_main.o \
	   Geometry.o \
	   Island.o \
	   Navigation.o \
	   Ship_factory.o \
	   Sim_object.o \
	   Controller.o \
	   Cruiser.o \
	   Model.o \
	   Ship.o \
	   Tanker.o \
	   View.o \
	   Warship.o \
	   Track_base.o \
	   Utility.o \
	   Cruise_ship.o \
	   Views.o

CPP = Geometry.cpp \
	   Island.cpp \
	   Navigation.cpp \
	   Ship_factory.cpp \
	   Sim_object.cpp \
	   Controller.cpp \
	   Cruiser.cpp \
	   Model.cpp \
	   Ship.cpp \
	   Tanker.cpp \
	   Warship.cpp \
	   Track_base.cpp \
	   Utility.cpp \
	   View.cpp \
	   Cruise_ship.cpp \
	   Views.cpp \
	   p5_main.cpp


H = Geometry.h \
	   Island.h \
	   Navigation.h \
	   Ship_factory.h \
	   Sim_object.h \
	   Controller.h \
	   Cruiser.h \
	   Model.h \
	   Ship.h \
	   Tanker.h \
	   Warship.h \
	   Track_base.h \
	   Utility.h \
	   View.h \
	   Cruise_ship.h \
	   Views.h

SSH_PATH = ~/Documents/eecs381/p5
SSH_USER = allenxyz
SSH_DOMAIN = login.engin.umich.edu

PROG = p5exe
SUBMIT_FILE = submit.tar.gz

default: $(PROG)

$(PROG): $(OBJS)
	$(LD) $(LFLAGS) $(OBJS) -o $(PROG)

p5_main.o: p5_main.cpp
	$(CC) $(CFLAGS) p5_main.cpp

Geometry.o: Geometry.cpp 
	$(CC) $(CFLAGS) Geometry.cpp	

Island.o: Island.cpp
	$(CC) $(CFLAGS) Island.cpp

Navigation.o: Navigation.cpp
	$(CC) $(CFLAGS) Navigation.cpp

Ship_factory.o: Ship_factory.cpp
	$(CC) $(CFLAGS) Ship_factory.cpp

Sim_object.o: Sim_object.cpp
	$(CC) $(CFLAGS) Sim_object.cpp

Controller.o: Controller.cpp
	$(CC) $(CFLAGS) Controller.cpp

Cruiser.o: Cruiser.cpp
	$(CC) $(CFLAGS) Cruiser.cpp

Model.o: Model.cpp
	$(CC) $(CFLAGS) Model.cpp

Ship.o: Ship.cpp
	$(CC) $(CFLAGS) Ship.cpp

Tanker.o: Tanker.cpp
	$(CC) $(CFLAGS) Tanker.cpp

View.o: View.cpp
	$(CC) $(CFLAGS) View.cpp

Warship.o: Warship.cpp
	$(CC) $(CFLAGS) Warship.cpp

Track_base.o: Track_base.cpp
	$(CC) $(CFLAGS) Track_base.cpp
	   
Utility.o: Utility.cpp
	$(CC) $(CFLAGS) Utility.cpp

Cruise_ship.o: Cruise_ship.cpp
	$(CC) $(CFLAGS) Cruise_ship.cpp

Views.o: Views.cpp
	$(CC) $(CFLAGS) Views.cpp


clean:
	rm -f *.o *exe *.gch $(SUBMIT_FILE) out a.out

submit: 
	tar -zcvf $(SUBMIT_FILE)  $(CPP) $(H)

test1: 
	g++ test.cpp $(CPP) $(H) -std=c++11

push_submit: 
	scp $(SUBMIT_FILE) $(SSH_USER)@$(SSH_DOMAIN):$(SSH_PATH)