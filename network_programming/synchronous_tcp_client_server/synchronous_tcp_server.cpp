// The below code shows how to use Boost.Asio to implement a server application with TCP.

#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

std::string make_daytime_string()
{
	std::time_t now = std::time(0);
	return std::ctime(&now);
}

int main()
{
	try
	{
		// any program that uses asio need to have at least one io_service object 
		boost::asio::io_service io_service;

		// boost::asio::ip::tcp::end_point type object is created to pass it to acceptor
		tcp::endpoint tcp_endpoint(tcp::v4(), 13);

		// boost::asio::ip::tcp::acceptor object needs to be created to listen for new connections
		tcp::acceptor 	acceptor(io_service, tcp_endpoint);

		for (;;)
		{
			// creates a socket
			tcp::socket socket(io_service);

			// wait and listen
			acceptor.accept(socket);
	
			// prepare message to send back to client
			std::string message = make_daytime_string();

			boost::system::error_code ignored_error;
	
			// writing the message for current time
			write(socket, boost::asio::buffer(message), ignored_error);
		}
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	
	return 0;
}
