///////////////////////////////////////////////////////////////////////////////
// elevator.h					                                             //
// ver 1.0                                                                   //
// Parth Jaradi, CIS600 - Parallel Prog & Multi-Threading, Spring 2020       //
///////////////////////////////////////////////////////////////////////////////

/*
*
*  Package Operations :
*  ------------------ -
*  The elevator.h file contains the class for the Elevators. 
*  Elevator class is used to handle all the operations related to the 
*  operations of the elevator and the information about each of the elevator 
*  is stored in this class. Each elevator has an individual instance of this class.
*
*
*  Maintenance History:
*  --------------------
*  ver 1.0 : May 07 2020
*  - first release
*/

#pragma once
#include<iostream>
#include<thread>
#include<list>
#include<condition_variable>
#include<atomic>
#include<algorithm>
#include<windows.h>
#include<map>
#include<fstream>
enum class State {DOWN=-1,STATIONARY,UP};

class Elevator {
	private:
		int elevatorId;
		int state;
		int currentFloor;
		int totalPassengersServed;
		bool outOfOrder;
		int idleTime;
		int runTime;
		State moveDirection;
		int topLandingFlr;
		bool systemStopped;
		int passengersInElevator;
		

	public:
		void setElevatorId(int id);
		void setState(int st);
		void setCurrentFloor(int flr);
		void setTotalPassengers();
		void setOutOfOrder(bool flag);
		void setIdleTime();
		void setRunTime();
		void setMoveDirection(State d);
		void setTopLandingFlr(int flr);
		void setSystemStopped(bool sFlag);
		void setPassengersInElevator(int p);

		int&  getElevatorId();
		int& getState();
		int& getCurrentFloor();
		int& getTotalPassengers();
		bool getOutOfOrder();
		int& getIdleTime();
		int& getRunTime();
		int& getTopLandingFlr();
		bool& getSystemStopped();
		int& getPassengersInElevator();

		State& getMoveDirection();
		std::list<int> floorsRequested;
		mutable std::mutex mutex_;
		std::condition_variable condVar;
		std::list<int> peopleRequests;
		std::map<int, int> enterPeopleMap;
		std::map<int, int> exitPeopleMap;
		std::list<int> peopleInElevator;
		static std::ofstream simulationLogData;
		static std::mutex logDataMutex;

		void updatePassenger(int startFlr,int endFlr);
		void nowMoving();
		
		Elevator();
		Elevator(int i);

		~Elevator();
};
