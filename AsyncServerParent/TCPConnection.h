/*
Handles sending and receiving OPackets and IPacket or raw data with a single client
*/

#pragma once
#include "Macros.h"
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/thread/mutex.hpp>
#include <vector>
#include <queue>

//Forward declaration
class Server;
class IPacket;
class OPacket;
class HeaderManager;

typedef std::function<void(boost::shared_ptr<IPacket> iPack)> DataHandler;
typedef std::function<void()> DisconnectHandler;

class TCPConnection : public boost::enable_shared_from_this<TCPConnection>
{
public:
	//Initialize TCPConnection using the already existing tcpConnection
	TCPConnection(boost::shared_ptr<boost::asio::ip::tcp::socket> boundSocket,
		boost::shared_ptr<HeaderManager> headerManager,
		DataHandler dataHandler = nullptr,
		DisconnectHandler disconnectHandler = nullptr);

	//Starts the receiving of packets
	virtual void start();

	//Send OPacket over the TCPConnection
	virtual void send(boost::shared_ptr<OPacket> oPack);

	//Send raw data over the TCPConnection
	virtual void send(boost::shared_ptr<std::vector<unsigned char>> sendData);

	//bind function to handle receiving (must be called after receive handler is called)
	virtual void read(unsigned int readSize = 0);

	//accessor for the socket
	boost::shared_ptr<boost::asio::ip::tcp::socket> getSocket()
	{
		return socket;
	}

	void setDataHandler(DataHandler handler) {
		dataHandler = handler;
	}

	void setDisconnectHandler(DisconnectHandler handler) {
		disconnectHandler = handler;
	}

	//Function that is called when data is received
	void asyncReceiveHandler(const boost::system::error_code& error, unsigned int bytes);

	//Function called when data is sent
	void asyncSendHandler(const boost::system::error_code& error, boost::shared_ptr<std::vector<unsigned char>> sendData);

	//Shutsdown the socket
	virtual void close();

	//Deletes headerManager and receive buffer
	virtual ~TCPConnection();

protected:
	DataHandler dataHandler;
	DisconnectHandler disconnectHandler;

	boost::shared_ptr<HeaderManager> headerManager;
	//Stores data to be sent when asyncSendHandler is called
	std::queue <boost::shared_ptr<std::vector <unsigned char>>> queueSendData;
	//The buffer to store received packets
	boost::shared_ptr<std::vector<unsigned char>> receiveStorage;
	//The boost socket representing a connection to a single client
	boost::shared_ptr<boost::asio::ip::tcp::socket> socket;
	//Prevents multiple threads from accessing the queueSendData
	boost::mutex queueSendDataMutex;

	//If false, receive will not be called again
	bool alive;

	template <typename Derived>
	std::shared_ptr<Derived> shared_from_base()
	{
		return std::static_pointer_cast<Derived>(shared_from_this());
	}
};
