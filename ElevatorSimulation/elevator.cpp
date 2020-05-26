///////////////////////////////////////////////////////////////////////////////
// elevator.cpp					                                             //
// ver 1.0                                                                   //
// Parth Jaradi, CIS600 - Parallel Prog & Multi-Threading, Spring 2020       //
///////////////////////////////////////////////////////////////////////////////

#include "elevator.h"

std::ofstream Elevator::simulationLogData;
std::mutex Elevator::logDataMutex;

// Setter Methods For Private Variables

void Elevator::setElevatorId(int id) {
	elevatorId = id;
}

void Elevator::setState(int st) {
	state = st;
}
void Elevator::setCurrentFloor(int flr) {
	currentFloor = flr;
}
void Elevator::setTotalPassengers() {
	++totalPassengersServed;
}
void Elevator::setOutOfOrder(bool flag) {
	outOfOrder = flag;
}
void Elevator::setIdleTime() {
	idleTime++;
}
void Elevator::setRunTime() {
	++runTime;
}

void Elevator::setMoveDirection(State d) {
	moveDirection = d;
}

void Elevator::setTopLandingFlr(int flr) {
	topLandingFlr = flr;
}

void Elevator::setSystemStopped(bool sFlag) {
	systemStopped = sFlag;
	outOfOrder = sFlag;
}

void Elevator::setPassengersInElevator(int p) {
	passengersInElevator = p;
}


// Getter Methods For Private Variables


int& Elevator::getElevatorId() {
	return elevatorId;
}

int& Elevator::getState() {
	return state;
}
int& Elevator::getCurrentFloor() {
	return currentFloor;
}
int& Elevator::getTotalPassengers() {
	return totalPassengersServed;
}
bool Elevator::getOutOfOrder() {
	return outOfOrder;
}
int& Elevator::getIdleTime() {
	return idleTime;
}
int& Elevator::getRunTime() {
	return runTime;
}

State& Elevator::getMoveDirection() {
	return moveDirection;
}

int& Elevator::getTopLandingFlr() {
	return topLandingFlr;
}

bool& Elevator::getSystemStopped() {
	return systemStopped;
}

int& Elevator::getPassengersInElevator() {
	return passengersInElevator;
}


void Elevator::updatePassenger(int startFlr, int endFlr) {
	for (int i = startFlr; i <= endFlr; ++i)
		floorsRequested.push_back(i);
}


Elevator::Elevator(int i) {

	elevatorId = i;
	outOfOrder = false;
	moveDirection = State::STATIONARY;
	topLandingFlr = -1;
	systemStopped = false;
	passengersInElevator = 0;
	totalPassengersServed = 0;
}



Elevator::Elevator(){}


Elevator::~Elevator() {}







// Whenever an Elevator is serving a request
// the below function is called. Depending on
// the number of requests, the function will continue
// running and will exit once all requests are fulfilled.
void Elevator::nowMoving() {

	Elevator e;
	
	while (peopleRequests.size() != 0) {

		
		int currentRequest = this->peopleRequests.front();
		int crntFlr = this->getCurrentFloor();
		int landingFlr = this->getTopLandingFlr();
		

		//Fetching people Id of current Request
		int peopleId=0;
		for (auto m : exitPeopleMap) {
			if (m.second == currentRequest)
				peopleId = m.first;
		}


		if (crntFlr > currentRequest) {

			this->setMoveDirection(State::DOWN);
		}
		else {

			this->setMoveDirection(State::UP);

		}
			

		while (crntFlr <= landingFlr && landingFlr !=-1 && crntFlr != -1) {


			{
				std::lock_guard<std::mutex> lock(e.logDataMutex);
				e.simulationLogData << "  Current Floor : " << crntFlr << " of Elevator " << this->getElevatorId() << std::endl;
			}



			for (int i = 0; i < 5; ++i)
				this->setRunTime();

			//Check any entry map
			//If any passenger is waiting at this floor, pick them Up.
			for (auto peopleWait : enterPeopleMap) {

				if (peopleInElevator.size() != 10 && peopleWait.second == crntFlr)
				{

					if(peopleWait.first==peopleId)
						peopleInElevator.push_back(peopleWait.first);
					else {

						if (std::find(peopleInElevator.begin(), peopleInElevator.end(), peopleId) != peopleInElevator.end()) {
							peopleInElevator.push_back(peopleWait.first);
							int p = this->getPassengersInElevator();
							p++;
							this->setPassengersInElevator(p);

							for (int i = 0; i < 2; ++i)
								this->setRunTime();

							// The person is entering the elevator
							{
								std::lock_guard<std::mutex> lock(e.logDataMutex);
								e.simulationLogData << std::endl << "  Person Entering the elevator " <<  this->getElevatorId()<< " is : " << peopleId << std::endl;
							}

						}

						else {
							if (peopleInElevator.size() < 10) {
								peopleInElevator.push_back(peopleWait.first);
								int p = this->getPassengersInElevator();
								p++;
								this->setPassengersInElevator(p);
								for (int i = 0; i < 2; ++i)
									this->setRunTime();

								// The person is entering the elevator
								{
									std::lock_guard<std::mutex> lock(e.logDataMutex);
									e.simulationLogData << std::endl << "  Person Entering the elevator " << this->getElevatorId() << " is : " << peopleWait.first << std::endl;
								}


							}
								
						}
					}
				}
				
			}

			//Remove the people in Elevator from Entry People Map
			for (auto people : peopleInElevator) {

				auto itr = enterPeopleMap.find(people);
				if(itr!=enterPeopleMap.end())
				enterPeopleMap.erase(itr);

			}



			//Check exit map
			std::list<int> peopleLeavingFlr;
			for (auto people : peopleInElevator) {
				if (exitPeopleMap[people] == crntFlr) {
					// The person is leaving the elevator
					{
						std::lock_guard<std::mutex> lock(e.logDataMutex);
						e.simulationLogData << std::endl << "  Person Leaving the elevator " << this->getElevatorId() <<" is : " << people << std::endl;
					}
					
					
					for (int i = 0; i < 2; ++i)
						this->setRunTime();


					auto  itr = exitPeopleMap.find(people);
					exitPeopleMap.erase(itr);
					peopleLeavingFlr.push_back(people);

					auto startTime = std::chrono::system_clock::now();
					while (std::chrono::system_clock::now() - startTime <= std::chrono::milliseconds(2)) {

					}
					if (crntFlr == currentRequest)
						break;
				}
			}
			//Removing people from People In Elevator list
			for (auto people : peopleLeavingFlr) {
				peopleInElevator.remove(people);
				int p = this->getPassengersInElevator();
				p--;
				this->setPassengersInElevator(p);
			}
			
			if (this->getMoveDirection() == State::UP) {

				++crntFlr;
				this->setCurrentFloor(crntFlr);
				
				auto startTime = std::chrono::system_clock::now();
				while (std::chrono::system_clock::now() - startTime <= std::chrono::milliseconds(5)) {
				}
			
			
				// =============== Graphic Part ==============

				COORD pos = { 3,3 };
				short factor = short(this->getElevatorId()) - 1;
				short startPosX = pos.X + (23 * factor) + 1;
				short endPosX = pos.X + 20 + (23 * factor) - 1;
				short startPosY = pos.Y;
				short endPosY = pos.Y + 20;
				short flrPos = 23 - short(crntFlr / 2)- 1;
				while (startPosY != endPosY) {
					for (auto i = startPosX; i != endPosX; ++i) {

						pos = { i,startPosY };

						SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);		//Set Cursor Position of Window Console
						if (startPosY == flrPos) {

							std::cout << "=";
						}
						else
						{
							std::cout << " ";
						}

					}

					++startPosY;
				}

				


			}
			else if (this->getMoveDirection() == State::DOWN) {

			--crntFlr;
			this->setCurrentFloor(crntFlr);
			auto startTime = std::chrono::system_clock::now();
			while (std::chrono::system_clock::now() - startTime <= std::chrono::milliseconds(5)) {
			}


			// =========== Graphic Part ===============

			COORD pos = { 3,3 };
			short factor = short(this->getElevatorId()) - 1;
			short startPosX = pos.X + (23 * factor) + 1;
			short endPosX = pos.X + 20 + (23 * factor) - 1;
			short startPosY = pos.Y;
			short endPosY = pos.Y + 20;
			short flrPos = 23 - short(crntFlr / 2) -1 ;
			while (startPosY != endPosY) {
				for (auto i = startPosX; i != endPosX; ++i) {

					pos = { startPosX,startPosY };
					SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);		//Set Cursor Position of Window Console
					if (startPosY == flrPos) {
						{
							std::cout << "=";
						}
					}
					else
					{
						std::cout << " ";
					}

				}

				++startPosY;
			}

			


			}



		}

		peopleRequests.pop_front();

		//Checking if all the requests are served
		if (peopleRequests.size() == 0) {	//If yes, the function will exit in next iteration
			{
				std::lock_guard<std::mutex> lock(e.logDataMutex);
				e.simulationLogData << std::endl << "  All the requests served for Elevator " << elevatorId << std::endl;
			}

			landingFlr = -1;
			this->setMoveDirection(State::STATIONARY);
		}

		else {	//If there are still requests to be fulfilled, fetch new one from peopleRequests.


			int nextRequest = peopleRequests.front();

			if (nextRequest > this->getCurrentFloor()) {
				this->setMoveDirection(State::UP);
				auto landingFlrPtr = std::max_element(peopleRequests.begin(), peopleRequests.end());
				landingFlr = *landingFlrPtr;
			}

			else {
				this->setMoveDirection(State::DOWN);
				auto landingFlrPtr = std::min_element(peopleRequests.begin(), peopleRequests.end());
				landingFlr = *landingFlrPtr;
			}

			this->setTopLandingFlr(landingFlr);
		}

	}
	{
		std::lock_guard<std::mutex> lock(e.logDataMutex);
		e.simulationLogData << std::endl << " == Move Elevator Function Exited  for Elevator " << this->getElevatorId() << std::endl;
	}
	
}