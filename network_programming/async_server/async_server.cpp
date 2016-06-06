#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/make_shared.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <istream>
#include <list>
#include <string>

using boost::asio::ip::tcp;

// types declared for acceptor and socket
typedef boost::asio::ip::tcp::acceptor	acceptor_type;
typedef boost::asio::ip::tcp::socket	socket_type;

// port no. to bind the server to.
const short PORT = 11235;

class MyConnection : public boost::enable_shared_from_this<MyConnection>
{
	public:
		MyConnection(boost::asio::io_service& ioservice) : socket(ioservice) 
		{}
		
		~MyConnection() {}
		
		socket_type& Socket()
		{
			return socket;
		}
		
		void Session()
		{
			asyncRead();
		}
		
		void Stop()
		{
			socket.cancel();
		}
		
		typedef boost::shared_ptr<MyConnection> shared_ptr_to_myconnection;
		
	protected: 
		// memeber variables
		socket_type					socket;
		boost::asio::streambuf		stream_buffer;
		std::string					message;
		
		void asyncRead()
		{
			boost::asio::async_read_until(socket, 
											stream_buffer,
											'\0',
											boost::bind(
												&MyConnection::readHandler,
												shared_from_this(),
												boost::asio::placeholders::error,
												boost::asio::placeholders::bytes_transferred
												)
											);
		}
		
		void asyncWrite()
		{
			
		}
		
		void readHandler(const boost::system::error_code& ec, size_t bytes_transferred)
		{
			
		}
		
		void writeHandler()
		{
			
		}
};

class MyServer
{
	
};
									
int main()
{
	
	return 0;
}