///////////////////////////////////////////////////////////////////////////////
// people.cpp					                                             //
// ver 1.0                                                                   //
// Parth Jaradi, CIS600 - Parallel Prog & Multi-Threading, Spring 2020       //
///////////////////////////////////////////////////////////////////////////////



#include "people.h"

// Setter Methods For Private Variables

void People::setPeopleId(int id) {
	peopleId = id;
}
void People::setArrivalTime(int at) {
	arrivalTime = at;
}
void People::setDepartureTime(int dt) {
	departureTime = dt;
}
void People::setcurrentFloor(int cf) {
	currentFloor = cf;
}
void People::setDestFloor(int df) {
	destinationFloor = df;
}
void People::setElevatorServed(int eId) {
	elevatorServed = eId;
}


void People::setRequestServed(bool flag) {
	requestServed == flag;
}

// Getter Methods For Private Variables
int People::getPeopleId() {
	return peopleId;
}
int People::getArrivalTime() {
	return arrivalTime;
}
int People::getDepartureTime() {
	return departureTime;
}
int People::getcurrentFloor() {
	return currentFloor;
}
int People::getDestFloor() {
	return destinationFloor;
}
int People::getElevatorServed() {
	return elevatorServed;
}

bool& People::getRequestServed() {
	return requestServed;
}


People::People(int id, int at, int currentFlr) {
	peopleId = id;
	arrivalTime = at;
	currentFloor = currentFlr;
	requestServed = false;
}


// Each person who wants to access the Elevator,
// requests for one using this function.
// It calls the getElevator() function of ElevatorControl class
void People::requestElevator() {

	int destinationFlr = rand() % 40;
	destinationFloor = destinationFlr;

	Elevator e;

	//Call ElevatorController's method of selecting a new elevator
	
	ElevatorController ec = ElevatorController();
	auto startTime = std::chrono::system_clock::now();

	while(!requestServed && (std::chrono::system_clock::now() - startTime <= std::chrono::milliseconds(10)))
		requestServed = ec.getElevator(this->peopleId,this->currentFloor,this->destinationFloor);

	if (!requestServed) {
		{
			std::lock_guard<std::mutex> lock(e.logDataMutex);
			e.simulationLogData << "  Request for People" << this->peopleId << "could not be served" << std::endl;
		}
		
	}
		
	
}