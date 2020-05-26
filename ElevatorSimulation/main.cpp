///////////////////////////////////////////////////////////////////////////////
// main.cpp						                                             //
// ver 1.0                                                                   //
// Parth Jaradi, CIS600 - Parallel Prog & Multi-Threading, Spring 2020       //
///////////////////////////////////////////////////////////////////////////////

/*
*
*  Package Operations :
*  ------------------ -
*  The main.cpp file contains the entry point for the Elevator Simulation Project.
*  The main thread is used to create subsequent new threads, one each for 
*  Elevator Control which starts the Elevator System and the other one for creating
*  requests of  People to access the Elevators.
*
*  Required Files:
*  ---------------
*  people.h							  // People Class
*  
*  Maintenance History:
*  --------------------
*  ver 1.0 : May 07 2020
*  - first release
*/




#pragma once
#include "people.h"
#include<string>

int currentTime;
std::mutex coutMutex;

//  The function used to create the Elevator Design
//  on the Windows Console using Windows Graphics
void renderElevatorDesign() {

	Elevator e;
	COORD pos = { 3,3 };

	for (short i = 0; i < 4; ++i) {
		short startPosX = pos.X + (23 * i);
		short endPosX = pos.X + 20 + (23 * i);
		short startPosY = pos.Y;
		short endPosY = pos.Y + 20;
		short tempStartX = startPosX;
		short tempStartY = startPosY;
		while (startPosY != endPosY) {

			pos = { startPosX,startPosY };
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
			std::cout << "#";
			++startPosY;

		}
		while (startPosX != endPosX) {
			pos = { startPosX,startPosY };
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
			std::cout << "=";
			++startPosX;
		}
		pos = { startPosX,startPosY };
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
		{
			std::cout << "=";
			--startPosY;
		}
		while (startPosY != tempStartY) {

			pos = { startPosX,startPosY };
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
			std::cout << "#";
			--startPosY;
		}
		pos = { startPosX,startPosY };
		{
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);

			std::cout << "#";

			pos = { tempStartX + 1,endPosY + 2 };
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
			std::cout << "Elevator " << i + 1;

		}
		pos = { 3,3 };
	}

}






//  Upon the creation of requestThread, this
//  function is called and used to get user input for
//  number of people requesting for elevator at a 
//  given time
void createRequests() {

	int numberOfPassengers = 0;
	bool done = false;
	Elevator e;
	ElevatorController ec;
	short x = 3,y = 27;
	while (!done) {
		
	
		int input;
		COORD pos = {x,y};			//Setting the Co ordinates for the Windows Cursor
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);   //Setting the Cursor Position to the pos 
		{
			std::lock_guard<std::mutex> lock(e.logDataMutex);
			e.simulationLogData << "  Number of Passengers for elevator : " << std::endl;
		}
		{
			std::cout << " Number of Passengers for elevator : " << std::endl;
			++y;
			pos = { x,y };
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
			std::cin >> input;
		}
		e.simulationLogData << input << std::endl;

		if (input == 0) {		//If the user input is 0, the Simulation will stop.
			done = true;
			ElevatorController ec;
			for(auto itr:ec.elevatorObjects) {
				itr->setSystemStopped(true);
				itr->condVar.notify_one();
			}
			
			break;
		}
		else if (input < 0) {
			++y;
			pos = { x,y };
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
			std::cout << " Invalid Input. Enter any positive number" << std::endl;
		}

		else {

			for (int i = 0; i < input; ++i) {
				++numberOfPassengers;
				srand(numberOfPassengers);
				int currentFlr = rand() % 40;

				People p = People(numberOfPassengers, currentTime, currentFlr);		//New Instance of People is created.
				p.requestElevator();		//Each new instance calls the request Elevator function

			}
		

		}
		++y;


	}
	{
		std::lock_guard<std::mutex> lock(e.logDataMutex);
		e.simulationLogData << std::endl << " == Exiting Request Thread " << std::endl;
	}
	
	return;

}


int main() {

	std::ofstream simulationStatistics;
	Elevator e;

	{
		std::lock_guard<std::mutex> lock(e.logDataMutex);
		e.simulationLogData.open("simulationLogData.txt");
		e.simulationLogData << std::endl << "  " << std::string(30, '=') << std::endl;
		e.simulationLogData << "  " << " Elevator Simulation Log Data";
		e.simulationLogData << std::endl << "  " << std::string(30, '=') << std::endl << std::endl;

		e.simulationLogData << std::endl;
		//e.simulationLogData << std::endl << "  Main Thread ID is:  " << std::this_thread::get_id() << std::endl;
	}


	renderElevatorDesign();

	//Instantiating Elevator Controller
	ElevatorController ec;
	
	//Create Elevator Controller Thread
	std::thread controllerThread(&ElevatorController::startSystem,&ec);
	
	//Creating Reqeuest Thread
	std::thread requestThread(createRequests);
	
	controllerThread.join();
	requestThread.join();

	std::this_thread::yield();


	//Display the final elevator status 
	// using Windows Graphics
	std::system("cls");
	renderElevatorDesign();
	for (auto e : ec.elevatorObjects) {

		COORD pos = { 3,3 };
		short factor = short(e->getElevatorId()) - 1;
		short startPosX = pos.X + (23 * factor) + 1;
		short endPosX = pos.X + 20 + (23 * factor) - 1;
		short flrPos = 23 - short(e->getCurrentFloor() / 2);

		for (auto i = startPosX; i <= endPosX; ++i) {

			pos = { i,flrPos };
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);		//Set Cursor Position of Window Console
			std::cout << "=";

		}

	}


	
	{

		//Code to write Elevator Statistics to simulationStatistics.txt file
		simulationStatistics.open("SimulationStatistics.txt");
		std::lock_guard<std::mutex> lock(coutMutex);

		simulationStatistics << std::endl << "  " << std::string(30, '=') <<std::endl;
		simulationStatistics << "  " << " Elevator Simulation Statistics";
		simulationStatistics << std::endl << "  " << std::string(30, '=') << std::endl <<std::endl;

		for (auto e : ec.elevatorObjects) {

			simulationStatistics << "  Number Of People Served by Elevator " << e->getElevatorId() << " : " << e->getTotalPassengers() <<std::endl;
			simulationStatistics << "  Run Time for Elevator " << e->getElevatorId() << " : " << e->getRunTime() << "s" << std::endl;
			simulationStatistics << "  Idle Time for Elevator " << e->getElevatorId() << " : " << e->getIdleTime() << "s" << std::endl;

			simulationStatistics << std::endl << "  "<< std::string(40, '--') << std::endl <<std::endl;

		}

		e.simulationLogData << std::endl << " == Exiting Main Thread " << std::endl;
	}
	
	//Setting all th elevator pointers to NULL;
	for (auto e : ec.elevatorObjects)
		e = NULL;
	for (auto e : ec.movingUpElevators)
		e = NULL;
	for (auto e : ec.movingDownElevators)
		e = NULL;
	for (auto e : ec.stationaryElevators)
		e = NULL;

	COORD pos = { 30,30 };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}