#include "IPacket.h"
#include "OPacket.h"
#include "Client.h"
#include <boost/make_shared.hpp>

IPacket::IPacket()
	:serverRead(false)
{
	locKey[0] = UNDEFINED_LOC;
	locKey[1] = UNDEFINED_LOC;
	locKey[2] = '\0';
}

int IPacket::getSenderID()
{
	return sender->getID();
}

boost::shared_ptr<OPacket> IPacket::toOPack(bool copyData)
{
	return boost::make_shared<OPacket>(this, copyData);
}

boost::shared_ptr<std::vector<boost::shared_ptr<OPacket>>> IPacket::convertToOPacks(bool copyData)
{
	auto oPacks = boost::make_shared<std::vector<boost::shared_ptr<OPacket>>>();
	for (int i = 0; i < sendToClients.size(); i++)
	{
		auto oPack = toOPack(copyData);
		oPack->clearSendToIDs();
		oPack->addSendToID(sendToClients.at(i));
		oPacks->push_back(oPack);
	}
	return oPacks;
}

IPacket::~IPacket()
{
		
}