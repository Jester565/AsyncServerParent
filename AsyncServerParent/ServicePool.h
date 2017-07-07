/*
Runs one boost ASIO io_service on multiple threads. This allows multiple threads to handle packets
at the same time.
*/

#pragma once
#include <boost/asio/io_service.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <vector>

class ServicePool
{
public:
	//Determines number of threads need by subtracting the number of used cores by the total number of cores your computer has
		ServicePool(int usedCores = 1);

		//Runs the io_services (so they begin handling connections, packets, etc)
		virtual void run();
	
		//Iterates through the io_services so that TCPConnections are well distributed over separate io_services
		boost::asio::io_service& getNextIOService()
		{
				if (serviceIter >= services.end())
				{
						serviceIter = services.begin();
				}
				return **(serviceIter++);
		}

		//Get the front io_service
		boost::asio::io_service& getFirstIOService()
		{
				return *services.front();
		}

		//Checks if the io_services are running
		bool isRunning();

		//Stops the io_services (must be called before destructor)
		void stop();

		//empty destructor
		~ServicePool();

protected:
	//Represents a pointer to an io_service
		typedef boost::shared_ptr<boost::asio::io_service> io_service_ptr;
		//The function the thread is running to run the io_service
		void ioServiceRunFunc(io_service_ptr ioService);
		//Stores all ioServices
		std::vector <io_service_ptr> services;
		//The threads running the io_services in services
		std::vector <boost::thread> ioServiceRunners;
		//Stores the current position in services
		std::vector <io_service_ptr>::iterator serviceIter;
		//Prevents multiple threads from checking the running boolean (I don't think this is necessary or we might be able to use atomic instead)
		boost::mutex runningMutex;
		//Prevents multiple threads from checking the runCount
		boost::mutex runCountMutex;

		//Indicates if running
		bool running;
		//The amount of threads running
		int runCount;
		//The number of cores on the machine
		int numCores;
};

