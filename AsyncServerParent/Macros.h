/*
This is the file that most would call stdafx.h, it contains
constants that nearly all classes should know
*/

#pragma once
#include <stdint.h>

//The type of the clients unique id
typedef uint16_t IDType;

//The client id to send to that indicates you want to broadcast your message
static const unsigned int BROADCAST_ID = UINT16_MAX;

//The number of bits in the clients unique id
static const unsigned int ID_TYPE_SIZE_BITS = 16;

//Maximum number of bytes in a TCP packet
static const unsigned int MAX_DATA_SIZE = 6100;

//Specifies if a program should terminate on certain errors (ignore this)
static const int DEFAULT_ERROR_MODE = 1;

//The number of bytes in the clients unique id
static const unsigned int ID_TYPE_SIZE_BYTES = ID_TYPE_SIZE_BITS / 8;

//If an IPacket or OPackets LocKey contains all of this character, the packet is undefined
static const char UNDEFINED_LOC = 'Z';

//These are the ERROR_MODE values (ignore this)
static const int THROW_ON_ERROR = 0;
static const int RETURN_ON_ERROR = 1;
static const int RECALL_ON_ERROR = 2;
static const int CONTINUE_ON_ERROR = 3;