#pragma once
#include "Macros.h"
#include <vector>
#include <string>
#include <iostream>
#include <boost/shared_ptr.hpp>

class OPacket;

class IPacket
{
	friend class HeaderManager;
public:
	IPacket();

	IDType getSentFromID()
	{
		return sentFromID;
	}

	const char* getLocKey() const
	{
		return locKey;
	}

	std::vector <IDType> getSendToClients()
	{
		return sendToClients;
	}

	bool getServerRead()
	{
		return serverRead;
	}

	const boost::shared_ptr<std::string> getData() const
	{
		return data;
	}

	friend std::ostream& operator << (std::ostream& oStream, IPacket& iPack)
	{
		oStream << "Printing out packet: " << std::endl << "loc key: " << iPack.locKey << std::endl;
		oStream << "Sent from: " << iPack.sentFromID << std::endl;
		oStream << "Send to clients: size = " << iPack.sendToClients.size() << "  Items: ";
		for (int i = 0; i < iPack.sendToClients.size(); i++)
		{
			oStream << iPack.sendToClients.at(i) << " ";
		}
		oStream << std::endl;
		return oStream;
	}

	virtual std::vector<boost::shared_ptr<OPacket>>* convertToOPacks(bool copyData);

	virtual ~IPacket();

protected:
	IDType sentFromID;
	char locKey[3];
	bool serverRead;
	boost::shared_ptr<std::string> data;
	std::vector <IDType> sendToClients;
};
