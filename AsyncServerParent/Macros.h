/*
This is the file that most would call stdafx.h, it contains
constants that nearly all classes should know
*/

#pragma once
#include <stdint.h>
#include <boost/shared_ptr.hpp>

//The type of the clients unique id
typedef uint16_t IDType;

//The client id to send to that indicates you want to broadcast your message
static const unsigned int BROADCAST_ID = UINT16_MAX;

//The number of bits in the clients unique id
static const unsigned int ID_TYPE_SIZE_BITS = 16;

//Maximum number of bytes in a TCP packet
static const unsigned int MAX_DATA_SIZE = 6100;

//The number of bytes in the clients unique id
static const unsigned int ID_TYPE_SIZE_BYTES = ID_TYPE_SIZE_BITS / 8;

//If an IPacket or OPackets LocKey contains all of this character, the packet is undefined
static const char UNDEFINED_LOC = 'Z';

class Client;

typedef boost::shared_ptr<Client> ClientPtr;

class PKey;
typedef boost::shared_ptr<PKey> PKeyPtr;
