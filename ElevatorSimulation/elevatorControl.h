///////////////////////////////////////////////////////////////////////////////
// elevatorControl.h			                                             //
// ver 1.0                                                                   //
// Parth Jaradi, CIS600 - Parallel Prog & Multi-Threading, Spring 2020       //
///////////////////////////////////////////////////////////////////////////////

/*
*
*  Package Operations :
*  ------------------ -
*  The elevatorControl.h file contains the class for ElevatorController.
*  ElevatorController class acts as a middleman between the requests for the 
*  Elevator which is done using the People Class and Elevator Class regarding how 
*  the elevators are going to be assigned to each request. It will try to find the
*  best possible elevator depending on timing, availability and capacity.

*
*  Required Files:
*  ---------------
*  elevator.h				  // Elevator class
*
*  Maintenance History:
*  --------------------
*  ver 1.0 : May 07 2020
*  - first release
*/


#pragma once
#include "../ElevatorSimulation/elevator.h"
#include<iostream>
#include<thread>
#include<vector>
#include<map>
#include <chrono>
using namespace std::chrono_literals;

class ElevatorController {
	private:
		
		static bool elevatorInitiated;
	public:
		std::vector<std::thread> elevatorThreads;
		static std::map<std::thread::id, int> threadElevatorMap;
		static std::vector<Elevator *> elevatorObjects;
		static std::list<Elevator*> movingUpElevators;
		static std::list<Elevator*> movingDownElevators;
		static std::list<Elevator*> stationaryElevators;

		ElevatorController();
		void startSystem();
		void startElevators(Elevator* elevObj);
		bool getElevator(int peopleId, int currentFlr, int destinationFlr);
		

};

