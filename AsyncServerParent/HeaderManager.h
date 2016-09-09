#pragma once
#include "Macros.h"
#include <boost/shared_ptr.hpp>
#include <vector>

class Server;
class OPacket;
class IPacket;

class HeaderManager
{
public:
	HeaderManager(Server* server);

	virtual boost::shared_ptr<std::vector<unsigned char>> encryptHeader(boost::shared_ptr<OPacket> pack);

	virtual boost::shared_ptr<IPacket> decryptHeader(std::vector<unsigned char>* data, unsigned int size, IDType cID);

	virtual ~HeaderManager();

protected:
	virtual boost::shared_ptr<std::vector<unsigned char>> encryptHeaderAsBigEndian(boost::shared_ptr<OPacket> pack);

	virtual boost::shared_ptr<std::vector<unsigned char>> encryptHeaderToBigEndian(boost::shared_ptr<OPacket> pack);

	virtual boost::shared_ptr<IPacket> decryptHeaderAsBigEndian(std::vector<unsigned char>* data, unsigned int size, IDType cID);

	virtual boost::shared_ptr<IPacket> decryptHeaderFromBigEndian(std::vector<unsigned char>* data, unsigned int size, IDType cID);

	void setIPack(boost::shared_ptr<IPacket> iPack, char* locKey, IDType sentFromID, std::vector<IDType>& sendToIDs, boost::shared_ptr<std::string> mainData, bool serverRead = true);

	Server* server;

	bool bEndian;
};
