#include "OPacket.h"
#include "IPacket.h"
#include <boost/make_shared.hpp>

OPacket::OPacket(const char* loc, IDType senderID)
	:senderID(senderID)
{
	setLocKey(loc);
}

OPacket::OPacket(const char* loc, IDType senderID, IDType sendToID)
	: senderID(senderID)
{
	setLocKey(loc);
	addSendToID(sendToID);
}

OPacket::OPacket(const char* loc, IDType senderID, IDType* sendToIDs, unsigned int sendToIDsSize)
	: senderID(senderID)
{
	setLocKey(loc);
	for (int i = 0; i < sendToIDsSize; i++)
	{
		addSendToID(sendToIDs[i]);
	}
}

OPacket::OPacket(const char* loc, IDType senderID, std::vector <IDType> sendToIDs)
	: senderID(senderID)
{
	setLocKey(loc);
	this->sendToIDs = sendToIDs;
}

OPacket::OPacket(IPacket* iPack, bool copyData)
{
	locKey[0] = iPack->getLocKey()[0];
	locKey[1] = iPack->getLocKey()[1];
	locKey[2] = '\0';
	senderID = iPack->getSentFromID();
	sendToIDs = iPack->getSendToClients();
	if (copyData)
	{
		data = iPack->getData();
	}
}

OPacket::~OPacket()
{

}