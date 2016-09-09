#include "ServicePool.h"
#include <thread>

ServicePool::ServicePool(int usedCores)
{
	int numCores = std::thread::hardware_concurrency();
	if (numCores == 0)
	{
		std::cerr << "Could not detect number of cores, assume 4" << std::endl;
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
		work_ptr work(new boost::asio::io_service::work(*ioService));
		services.push_back(ioService);
		works.push_back(work);
	}
	serviceIter = services.begin();
}

void ServicePool::run()
{
	for (int i = 0; i < services.size(); i++)
	{
		boost::shared_ptr<boost::thread> thread(new boost::thread(boost::bind(&boost::asio::io_service::run, services.at(i))));
		thread->detach();
	}
}

ServicePool::~ServicePool()
{
	for (int i = 0; i < services.size(); i++)
	{
		services.at(i)->stop();
	}
}
