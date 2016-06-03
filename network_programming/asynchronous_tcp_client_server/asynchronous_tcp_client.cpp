// Below code is for synchronous TCP client

#include <iostream>
#include <array>
#include <boost/asio.hpp>
#include <string>

using boost::asio::ip::tcp;

int main(int argc, char* argv[])
{
	try
	{
		// user should specify the server ipaddress as the 2nd argument
		if (argc != 2)
		{
			std::cerr << "Usage: client <ip-address>\n";
			return 1;
		}
		
		// any program that uses Boost.Asio need to have at least one io_service object 
		boost::asio::io_service io_service;

		// convert the server name that was specified as a parameter to the application to a
		// TCP end_point. To do this use an object of type boost::asio::ip::tcp::resolver.
		tcp::resolver resolver(io_service);

		// A resolver takes a query object and turns it into a list of endpoints.
		// We construct a query using the name of the server, specified in argv[1]
		// and the name of the service, in this case "daytime".
		tcp::resolver::query query(argv[1], "daytime");
		
		// the list of endpoints is returned using an iterator of type 
		// boost::asio::ip::tcp::resolver::iterator.
		// a default constructed iterator object can be used as an end iterator.
		tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

		// now we create and connect the socket.
		// the list of endpoints obtained above botjh contain IPv4 and IPv6 endpoints,
		// so we need to try each one of them untill we find the one that works.
		// This keeps the client program independent of specific IP version.
		// boost::asio::connect() function does it for us automatically.
		tcp::socket socket(io_service);
	
		connect(socket, endpoint_iterator);

		// the connections is open. all we need to do now is read the response from the daytime 
		// service.
		for(;;)
		{
				// we use the boost array to hold the received data.
				std::array<char, 4096> buff;

				boost::system::error_code error;
				
				// the boost::asio::buffer() function automatically determines the size of the array
				// to help prevent buffer overruns.
				size_t len = socket.read_some(boost::asio::buffer(buff), error);
				
				// when the server closes the conection,
				// the boost::asio::ip::tcp::socket::read_some() function will exit with the 
				// boost::asio::error::eof_error, which is how we know to exit the loop.
				if (error == boost::asio::error::eof)
					break; // connection closed cleanly by peer
				else if (error)
					throw boost::system::system_error(error); // some other error

				std::cout.write(buff.data(), len);
		}
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
