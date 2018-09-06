#include "OPacket.h"
#include "IPacket.h"
#include "Client.h"
#include <boost/make_shared.hpp>

OPacket::OPacket(const std::string& loc, IDType senderID)
	:senderID(senderID)
{
	setLocKey(loc);
}

OPacket::OPacket(const std::string& loc, IDType senderID, IDType sendToID)
	: senderID(senderID)
{
	setLocKey(loc);
	addSendToID(sendToID);
}

OPacket::OPacket(const std::string& loc, IDType senderID, IDType* sendToIDs, unsigned int sendToIDsSize)
	: senderID(senderID)
{
	setLocKey(loc);
	for (int i = 0; i < sendToIDsSize; i++)
	{
		addSendToID(sendToIDs[i]);
	}
}

OPacket::OPacket(const std::string& loc, IDType senderID, std::vector <IDType> sendToIDs)
	: senderID(senderID)
{
	setLocKey(loc);
	this->sendToIDs = sendToIDs;
}

OPacket::OPacket(IPacket* iPack, bool copyData)
{
	locKey = iPack->getLocKey();
	senderID = iPack->getSenderID();
	sendToIDs = iPack->getSendToClients();
	if (copyData)
	{
		data = iPack->getData();
	}
}

OPacket::~OPacket()
{

}