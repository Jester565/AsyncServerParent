/*
Sorry for the bad naming. THIS IS NOT JUST THE locKey, it also contains the callback to be called whenever a packet with the matching locKey is received
*/

#pragma once
#include <string>
#include <functional>
#include <boost/shared_ptr.hpp>

//Forward declaration
class IPacket;

class PKey
{
public:
	//Create PKey object specifying the locKey (only call this if you are overriding the run function instead of passing in a callback function)
	PKey(const std::string& key);

	/*Sets the locKey to key and binds runMethod to the function passed in
		key: The key contained in every packet that indicates which callback should be called
		obj: The object that owns the callback
		func: The callback function (should just take a IPacket as a paramteter with a void return type)
	*/
	template <typename T, typename D>
	PKey(std::string key, T obj, D func)
	{
		this->key = key;
		runMethod = std::bind(func, obj, std::placeholders::_1);
	}

	//Accessor for the locKey
	const std::string& getKey() const
	{
		return key;
	}

	//Returns a description for what the callback does
	virtual std::string getDescription() const
	{
		return key + ": ";
	}

	//Run the callback with the iPacket
	virtual void run(boost::shared_ptr<IPacket> iPack)
	{
		if (runMethod != nullptr)
			runMethod(iPack);
		else
			throw "Run was not overloaded or provided with a runMethod";
	}

	friend bool operator< (const PKey& pKey1, const PKey& pKey2)
	{
		return (pKey1.getKey() < pKey2.getKey());
	}

	friend bool operator> (const PKey& pKey1, const PKey& pKey2)
	{
		return (pKey1.getKey() > pKey2.getKey());
	}

	friend bool operator== (const PKey& pKey1, const PKey& pKey2)
	{
		return (pKey1.getKey() == pKey2.getKey());
	}

	friend bool operator!= (const PKey& pKey1, const PKey& pKey2)
	{
		return (pKey1.getKey() != pKey2.getKey());
	}

	friend bool operator< (const std::string& key, const PKey& pKey)
	{
		return (key < pKey.getKey());
	}

	friend bool operator> (const std::string& key, const PKey& pKey)
	{
		return (key > pKey.getKey());
	}

	friend bool operator== (const std::string& key, const PKey& pKey)
	{
		return (key == pKey.getKey());
	}

	friend bool operator != (const std::string& key, const PKey& pKey)
	{
		return (key == pKey.getKey());
	}

	virtual ~PKey();

protected:
	//The callback to be called whenever a IPacket with the matching locKey is received
	std::function<void(boost::shared_ptr<IPacket> ipack)> runMethod;
	//THe locKey, which is sent in each packet to determine what function they want to call
	std::string key;
};