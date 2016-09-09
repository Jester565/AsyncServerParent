#pragma once
#include <boost/asio/io_service.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <vector>

class ServicePool
{
public:
	ServicePool(int usedCores = 1);

	void run();
	
	boost::asio::io_service& getNextIOService()
	{
		if (serviceIter >= services.end())
		{
			serviceIter = services.begin();
		}
		return **(serviceIter++);
	}

	~ServicePool();

protected:
	typedef boost::shared_ptr<boost::asio::io_service> io_service_ptr;
	typedef boost::shared_ptr<boost::asio::io_service::work> work_ptr;
	std::vector <io_service_ptr> services;
	std::vector <work_ptr> works;
	std::vector <io_service_ptr>::iterator serviceIter;
};

