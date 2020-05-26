///////////////////////////////////////////////////////////////////////////////
// people.h						                                             //
// ver 1.0                                                                   //
// Parth Jaradi, CIS600 - Parallel Prog & Multi-Threading, Spring 2020       //
///////////////////////////////////////////////////////////////////////////////

/*
*
*  Package Operations :
*  ------------------ -
*  The people.h file contains the class for the People who will be accessing 
*  Elevators. It contains information like peopleId, the floor on which they
*  are arriving and the destination floor to which they want to go.
*
*  Required Files:
*  ---------------
*  elevatorControl.h				  // Elevator Control class
*
*  Maintenance History:
*  --------------------
*  ver 1.0 : May 07 2020
*  - first release
*/



#pragma once
#include <iostream>
#include"elevatorControl.h"

class People {
	private:
		int peopleId;
		int arrivalTime;
		int departureTime;
		int currentFloor;
		int destinationFloor;
		int elevatorServed;
		bool requestServed;

	public:
		void setPeopleId(int id);
		void setArrivalTime(int at);
		void setDepartureTime(int dt);
		void setcurrentFloor(int cf);
		void setDestFloor(int df);
		void setElevatorServed(int eId);
		void setRequestServed(bool flag);
		
		int getPeopleId();
		int getArrivalTime();
		int getDepartureTime();
		int getcurrentFloor();
		int getDestFloor();
		int getElevatorServed();
		bool& getRequestServed();

		People(int id, int at, int currentFlr);
		void requestElevator();

};