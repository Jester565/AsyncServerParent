#pragma once
#include <boost/asio/io_service.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <vector>

class ServicePool
{
public:
		ServicePool(int usedCores = 1);

		virtual void run();
	
		boost::asio::io_service& getNextIOService()
		{
				if (serviceIter >= services.end())
				{
						serviceIter = services.begin();
				}
				return **(serviceIter++);
		}

		bool isRunning();

		void stop();

		~ServicePool();

protected:
		typedef boost::shared_ptr<boost::asio::io_service> io_service_ptr;
		void ioServiceRunFunc(io_service_ptr ioService);
		std::vector <io_service_ptr> services;
		std::vector <boost::thread> ioServiceRunners;
		std::vector <io_service_ptr>::iterator serviceIter;
		boost::mutex runningMutex;
		boost::mutex runCountMutex;

		bool running;
		int runCount;
		int numCores;
};

