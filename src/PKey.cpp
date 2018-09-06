#include "PKey.h"

PKey::PKey(const std::string& key)
	:key(key), runMethod(nullptr)
{
}

PKey::~PKey()
{
}
