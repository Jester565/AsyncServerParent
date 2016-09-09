#pragma once
#include <stdint.h>

typedef uint16_t IDType;

static const unsigned int BROADCAST_ID = UINT16_MAX;

static const unsigned int ID_TYPE_SIZE_BITS = 16;

static const unsigned int MAX_DATA_SIZE = 6100;

static const int DEFAULT_ERROR_MODE = 1;

static const unsigned int ID_TYPE_SIZE_BYTES = ID_TYPE_SIZE_BITS / 8;
static const char UNDEFINED_LOC = 'Z';
static const int THROW_ON_ERROR = 0;
static const int RETURN_ON_ERROR = 1;
static const int RECALL_ON_ERROR = 2;
static const int CONTINUE_ON_ERROR = 3;