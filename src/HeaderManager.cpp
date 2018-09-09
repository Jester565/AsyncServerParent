#include "HeaderManager.h"
#include "Server.h"
#include "IPacket.h"
#include <iterator>

HeaderManager::HeaderManager(GenIPacketHandler handler, unsigned int initialRecvSize)
	:genIPacketHandler(handler), initialReceiveSize(initialRecvSize)
{
	union {
		uint32_t i;
		char c[4];
	} tbend = { 0x01020304 };
	bEndian = (tbend.c[0] == 1);
}


HeaderManager::~HeaderManager()
{
}
