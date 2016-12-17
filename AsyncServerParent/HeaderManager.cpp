#include "HeaderManager.h"
#include "Server.h"
#include "IPacket.h"
#include <google/protobuf/any.h>
#include <iterator>

HeaderManager::HeaderManager(Server* server)
	:server(server)
{
	union {
		uint32_t i;
		char c[4];
	} tbend = { 0x01020304 };
	bEndian = (tbend.c[0] == 1);
}


boost::shared_ptr<std::vector<unsigned char>> HeaderManager::encryptHeader(boost::shared_ptr<OPacket> oPack)
{
	if (bEndian)
	{
		return encryptHeaderAsBigEndian(oPack);
	}
	return encryptHeaderToBigEndian(oPack);
}

boost::shared_ptr<IPacket> HeaderManager::decryptHeader(unsigned char* data, unsigned int size, IDType cID)
{
	if (bEndian)
	{
		return decryptHeaderAsBigEndian(data, size, cID);
	}
	return decryptHeaderFromBigEndian(data, size, cID);
}


boost::shared_ptr<std::vector<unsigned char>> HeaderManager::encryptHeaderAsBigEndian(boost::shared_ptr<OPacket> pack)
{
	return boost::make_shared<std::vector<unsigned char>>();
}

boost::shared_ptr<std::vector<unsigned char>> HeaderManager::encryptHeaderToBigEndian(boost::shared_ptr<OPacket> pack)
{
	return boost::make_shared<std::vector<unsigned char>>();
}

boost::shared_ptr<IPacket> HeaderManager::decryptHeaderAsBigEndian(unsigned char* data, unsigned int size, IDType cID)
{
	return boost::make_shared<IPacket>();
}

boost::shared_ptr<IPacket> HeaderManager::decryptHeaderFromBigEndian(unsigned char* data, unsigned int size, IDType cID)
{
	return boost::make_shared<IPacket>();
}

void HeaderManager::setIPack(boost::shared_ptr<IPacket> iPack, char * locKey, IDType sentFromID, std::vector<IDType>& sendToIDs, boost::shared_ptr<std::string> mainData, bool serverRead)
{
	iPack->locKey[0] = locKey[0];
	iPack->locKey[1] = locKey[1];
	iPack->locKey[2] = '\0';
	iPack->sentFromID = sentFromID;
	iPack->serverRead = serverRead;
	iPack->data = mainData;
	iPack->sendToClients = sendToIDs;
}

HeaderManager::~HeaderManager()
{
}
