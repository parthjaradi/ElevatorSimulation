///////////////////////////////////////////////////////////////////////////////
// elevatorControl.cpp			                                             //
// ver 1.0                                                                   //
// Parth Jaradi, CIS600 - Parallel Prog & Multi-Threading, Spring 2020       //
///////////////////////////////////////////////////////////////////////////////



#include "elevatorControl.h"

std::vector<Elevator *> ElevatorController:: elevatorObjects;
std::map<std::thread::id, int> ElevatorController:: threadElevatorMap;

std::list<Elevator*> ElevatorController::movingUpElevators;
std::list<Elevator*> ElevatorController::movingDownElevators;
std::list<Elevator*> ElevatorController::stationaryElevators;


//  Upon the creation of elevator thread, this
//  function is called. It checks if the System hasn't stopped yet and
//  are there any new requests that need to be fulfilled.
//  If yes, it will call nowMoving() function else will wait until notified
//  by the condition variable.
void ElevatorController::startElevators(Elevator* newElevator) {
	
	Elevator e;

	//e.simulationLogData << " New Thread has been created " << std::endl;
	{
		std::lock_guard<std::mutex> lock(e.logDataMutex);

		e.simulationLogData << std::endl << " Thread " << std::this_thread::get_id() <<
			" is associated with Elevator " << newElevator->getElevatorId() << std::endl;
	}
	
	
	threadElevatorMap[std::this_thread::get_id()] = newElevator->getElevatorId();


	while (!(newElevator->getSystemStopped())) {

		std::unique_lock<std::mutex> lck(newElevator->mutex_);
		while (newElevator->peopleRequests.size() == 0 && (!newElevator->getOutOfOrder()))
		{
			stationaryElevators.push_back(newElevator);
			newElevator->setIdleTime();
			newElevator->condVar.wait(lck);
		}

		newElevator->nowMoving();

	}

	{
		std::lock_guard<std::mutex> lock(e.logDataMutex);
		e.simulationLogData << std::endl << " == Exiting Thread for Elevator " << newElevator->getElevatorId() << std::endl;

	}
	
	
}


//  Upon the creation of ElevatorController thread, this 
//  function is called to start the Elevator System
//  It also creates new threads, one each for the elevator
void ElevatorController::startSystem() {
	
	//Creating instances of each Elevator Object
	for (int i = 1; i < 5; ++i) {
		Elevator* newElevator = new Elevator(i);
		elevatorObjects.push_back(newElevator);
	}


	//Creating 4 new threads, one each for the Elevators
	for (auto elevObj : elevatorObjects)
		elevatorThreads.push_back(std::thread(&ElevatorController::startElevators, this , elevObj));

	for (auto& th : elevatorThreads)
		th.join();

}

ElevatorController::ElevatorController() {
	
}



//  This function is used to allocate the requests made by the people to
//  the avaialble elevator.  It first tries to allocate the stationary 
//  elevator else it will find the nearest available elevator moving in the 
//  direction of the request made.
bool  ElevatorController::getElevator(int peopleId, int currentFlr, int destinationFlr) {
	
	Elevator e;
	
	{
		std::lock_guard<std::mutex> lock(e.logDataMutex);

		e.simulationLogData << std::endl << "  People " << peopleId << "requested from Floor " << currentFlr << " to " << destinationFlr << std::endl;
		e.simulationLogData << "  " << std::string(35, '--') << std::endl;
	}
	if (currentFlr == destinationFlr)
		return false;


	//get the direction new person wants to go
	State requestDirection;
	if (destinationFlr > currentFlr)
		requestDirection = State::UP;
	else
		requestDirection = State::DOWN;

	//Check if any of the elevators are stationary or not
	//If so, then assign one randomly;

	ElevatorController ec;
	if (ec.stationaryElevators.size()!=0)
	{
			int selectedElevator = rand() % ec.stationaryElevators.size();
			auto elevatorItr = ec.stationaryElevators.begin();
			int i = 0;

			while (i != selectedElevator && elevatorItr!=ec.stationaryElevators.end()) {
				
				++elevatorItr;
				++i;
			}


			(*elevatorItr)->setTopLandingFlr(destinationFlr);
			(*elevatorItr)->peopleRequests.push_back(destinationFlr);

			if (requestDirection == State::UP) {
				movingUpElevators.push_back(*elevatorItr);
			}

			else {
				movingDownElevators.push_back(*elevatorItr);
			}
			
			
			(*elevatorItr)->condVar.notify_one();
			
			{
				std::lock_guard<std::mutex> lock(e.logDataMutex);
				e.simulationLogData << std::endl << "  Elevator " << (*elevatorItr)->getElevatorId() << " will serve the request" << std::endl;
			}
			
			(*elevatorItr)->setTotalPassengers();
			
			
			// Add the people request in enter Map
			(*elevatorItr)->enterPeopleMap[peopleId] = currentFlr;

			// Add the people request in Exit Map
			(*elevatorItr)->exitPeopleMap[peopleId] = destinationFlr;



			stationaryElevators.remove(*elevatorItr);
			{
				std::lock_guard<std::mutex> lock(e.logDataMutex);
				e.simulationLogData << std::endl << " == Get Elevator Function Exited " << std::endl;
			}
			
			return true;

	}
	else {

			//find the closest flr elevator;
			auto nearestElevator = elevatorObjects.begin();
			auto elevatorItr = elevatorObjects.begin();
			int elevatorItFlrDiff;
			int nearestFlrDiff;
			
			
			while (elevatorItr != elevatorObjects.end())
			{
				elevatorItFlrDiff = (*elevatorItr)->getCurrentFloor() - currentFlr;
				nearestFlrDiff = (*nearestElevator)->getCurrentFloor() - currentFlr;
				if ( elevatorItFlrDiff < nearestFlrDiff  && (*elevatorItr)->getPassengersInElevator()<10  && (*elevatorItr)->getMoveDirection() == requestDirection)
					nearestElevator = elevatorItr;
				
				++elevatorItr;
			}


			(*nearestElevator)->peopleRequests.push_back(destinationFlr);
				
			(*nearestElevator)->setTotalPassengers();
			(*nearestElevator)->condVar.notify_one();


			// Add the people request in enter Map
			(*nearestElevator)->enterPeopleMap[peopleId] = currentFlr;

			// Add the people request in Exit Map
			(*nearestElevator)->exitPeopleMap[peopleId] = destinationFlr;


			{
				std::lock_guard<std::mutex> lock(e.logDataMutex);
				e.simulationLogData << std::endl << " Elevator " << (*nearestElevator)->getElevatorId() << " will serve the request" << std::endl;
				e.simulationLogData << std::endl << " == Get Elevator Function Exited " << std::endl;
			}
			return true;

		}
	
	
	return false;
	
}