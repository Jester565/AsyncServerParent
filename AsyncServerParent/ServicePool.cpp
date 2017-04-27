#include "ServicePool.h"
#include "Logger.h"
#include <thread>

ServicePool::ServicePool(int usedCores)
		:running(false), runCount(0)
{
	numCores = std::thread::hardware_concurrency();
	if (numCores == 0)
	{
		Logger::Log(LOG_LEVEL::Warning, "Could not detect number of cores, assume 4");
		numCores = 4;
	}
	numCores -= usedCores;
	if (numCores <= 0)
	{
		numCores = 1;
	}
	for (int i = 0; i < numCores; i++)
	{
		io_service_ptr ioService(new boost::asio::io_service());
		services.push_back(ioService);
	}
	serviceIter = services.begin();
}

void ServicePool::run()
{
		running = true;
		runCount = numCores;
		for (int i = 0; i < numCores; i++)
		{
				ioServiceRunners.emplace_back(boost::bind(&ServicePool::ioServiceRunFunc, this, services.at(i)));
				ioServiceRunners.at(i).detach();
		}
}

bool ServicePool::isRunning()
{
		boost::mutex::scoped_lock lock(runCountMutex);
		return runCount > 0;
}

void ServicePool::stop()
{
		runningMutex.lock();
		running = false;
		for (int i = 0; i < services.size(); i++) {
				services.at(i)->stop();
		}
		runningMutex.unlock();
}

ServicePool::~ServicePool()
{
}

void ServicePool::ioServiceRunFunc(io_service_ptr ioService)
{
		while (true) {
				runningMutex.lock();
				if (!running) {
						runningMutex.unlock();
						runCountMutex.lock();
						runCount--;
						runCountMutex.unlock();
						return;
				}
				runningMutex.unlock();
				ioService->run();
		}
}
