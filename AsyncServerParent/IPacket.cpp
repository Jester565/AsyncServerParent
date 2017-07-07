#include "IPacket.h"
#include "OPacket.h"
#include <boost/make_shared.hpp>

IPacket::IPacket()
	:serverRead(false)
{
	locKey[0] = UNDEFINED_LOC;
	locKey[1] = UNDEFINED_LOC;
	locKey[2] = '\0';
}

std::vector<boost::shared_ptr<OPacket>>* IPacket::convertToOPacks(bool copyData)
{
	std::vector <boost::shared_ptr<OPacket>>* oPacks = new std::vector<boost::shared_ptr<OPacket>>();
	for (int i = 0; i < sendToClients.size(); i++)
	{
		boost::shared_ptr<OPacket> oPack = boost::make_shared<OPacket>(this, copyData);
		oPack->setSenderID(sendToClients.at(i));
		oPacks->push_back(oPack);
	}
	return oPacks;
}

IPacket::~IPacket()
{
		
}