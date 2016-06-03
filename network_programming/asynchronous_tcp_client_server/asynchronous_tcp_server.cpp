#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

std::string make_daytime_string()
{
	std::time_t now = std::time(0);
	return std::ctime(&now);
}

// Using boost::shared_ptr and boost::enable_shared_from_this
// because we want to keep the tcp_connection object alive as long as 
// there is an operation that refers to it. 
class tcp_connection : public boost::enable_shared_from_this<tcp_connection>
{
	public:
		typedef boost::shared_ptr<tcp_connection> pointer;
		
		static pointer create(boost::asio::io_service& io_service)
		{
			return pointer(new tcp_connection(io_service));
		}
		
		tcp::socket& socket()
		{
			return socket_;
		}
		
		void start()
		{
			// The data to be sent is stored in the class member "m_mesage" as we 
			// need to keep the data valid till the asynchronous operation is complete.
			m_message = make_daytime_string();
			
			// Call boost::asio::async_write() to serve data to the client.
			// using boost::async_write() instead of boost::async_write_some() will ensure
			// that the entire block of data is sent.
			
			// When initializing the asynchronous operation, if we are using boost::bind()
			// then we must specify only the arguments that match the handler's parameter list.
			// In the below code, both the arguments placeholders boost::asio::placeholders::error and
			// boost::asio::placeholders::bytes_transferred could potentially have been removed, since 
			// they are not used being used in handle_write()
			
			
			// Not compiling --- Hence commenting for time moment
/*
			boost::asio::async_write(socket_, boost::asio::buffer(m_message), 
										boost::bind(&tcp_connection::handle_write, 
														shared_from_this(), 
														boost::asio::placeholders::error, 
														boost::asio::placeholders::bytes_transferred
													)
									);
*/						
			boost::asio::async_write(socket_, boost::asio::buffer(m_message), 
										boost::bind(&tcp_connection::handle_write, 
														shared_from_this() 
													)
									);
		}

	private:
		// member functions
		// constructor
		tcp_connection(boost::asio::io_service& io_service) : socket_(io_service)
		{}
		
		// handle_write is responsible for any further action for the client connections
		
		// Not compiling --- Hence commenting for time moment
/*		
		void handle_write(boost::system::error_code&, size_t)
		{}
*/		
		void handle_write()
		{}
		
		// member variables
		tcp::socket socket_;
		std::string m_message;
};

class tcp_server
{
	public:
		// constructor, it initializes an acceptor to listen on port 13
		tcp_server(boost::asio::io_service& io_service)
			: acceptor_(io_service, tcp::endpoint(tcp::v4(), 13))
		{
			// start_accept() creates a socket and initiates an asynchronous
			// operation to wait for a new connection.
			start_accept();
		}
	private:
		// member functions
		void start_accept()
		{
			// create a socket
			tcp_connection::pointer new_connection = tcp_connection::create(acceptor_.get_io_service());
			
			// initiates an asynchronous accept operation to wait for a new connection
			acceptor_.async_accept(new_connection->socket(),
										boost::bind(&tcp_server::handle_accept, 
													this,
													new_connection,
													boost::asio::placeholders::error
													)
									);
		}
		
		// handle_accept() is called when the asynchronous accept operation initiated by
		// start_accept() finishes. 
		// It services the client request.
		void handle_accept(tcp_connection::pointer& new_connection, const boost::system::error_code& error)
		{
			if (!error)
				new_connection->start();
			
			// Need to call the start_accept() again, for the next accept operation.
			start_accept();
		}
		
		// member variables
		tcp::acceptor acceptor_;
};

int main()
{
	try
	{
		// We need to create a server object to accept the incoming client connection.
		boost::asio::io_service io_service;
		
		// The I/O service object provides I/O service, such as sockets, 
		// that the server object will use 
		tcp_server server(io_service);
		
		// Run the I/O service object to perform an asynchronous operation.
		io_service.run();
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
