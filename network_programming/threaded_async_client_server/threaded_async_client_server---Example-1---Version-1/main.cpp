#include <iostream>
#include <list>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#include <utility>
#include <istream>
#include <ostream>
#include "my_server.hpp"

const short PORT1 = 11235;
//const short PORT2 = 11236;
//const short PORT3 = 11237;
//const short PORT4 = 11238;
//const short PORT5 = 11239;
/**
 * hostname to ip address string (using DNS lookup)
 */
std::string get_ip_address(boost::asio::io_service &io_service, std::string hostname) 
{
    boost::asio::ip::tcp::resolver resolver(io_service);
    boost::asio::ip::tcp::resolver::query query(hostname, "time"); // any port number will do
    boost::asio::ip::tcp::resolver::iterator iterator;
    try {
        iterator = resolver.resolve(query);
    } 
		catch ( boost::system::system_error e ) 
		{
        std::cerr << "Error resolving " << hostname << ": " << e.what() << std::endl;
        return( "" );
    }
 
    boost::asio::ip::tcp::resolver::iterator end;
    if ( iterator == end )
        return("");
 
    return(iterator->endpoint().address().to_string());
}

/**
 * main I/O loop
 *  sets up the listening address(es) and runs I/O asynchronous service
 */
int do_input_output(std::list< std::pair<std::string, unsigned int> > listeners)
{
    // create I/O service
    boost::asio::io_service io_service;
 
    // start a server for each listen address
    std::list< boost::shared_ptr<my_server> > servers; // track in a list
    for (
        std::list< std::pair<std::string, unsigned int> >::iterator it = listeners.begin();
        it != listeners.end();
        it++
    ) 
		{
        std::string hostname = it->first;
        unsigned int port = it->second;
 
        // endpoint to assign to
        boost::asio::ip::tcp::endpoint endpoint;
        if (! hostname.empty()) 
				{
            std::string address = get_ip_address(io_service, hostname);
            if ( address.empty() ) 
						{
                std::cerr << "could not resolve " << hostname << std::endl;
                return( 1 );
            }
 
            endpoint = boost::asio::ip::tcp::endpoint(
                boost::asio::ip::address::from_string( address ),
                port
            );
        } 
				else 
				{
            endpoint = boost::asio::ip::tcp::endpoint(
                boost::asio::ip::tcp::v4(),
                port
            );
        }
 
        // create server
        boost::shared_ptr<my_server> server(
            new my_server( &io_service, endpoint )
        );
 
        if ( server->failed ) 
				{
            std::cerr << "Failure in creatig server" << std::endl;
            return( 1 );
        }
        servers.push_back( server );
 
        std::cout << "listen on \"" << endpoint << "\"" << std::endl;
    } // for each listener
 
    // now start the I/O service
    // can only stop by calling io_service.stop()
    io_service.run();
 
    return( 0 ); // everything went okay
}

int main()
{
	std::pair<std::string, unsigned int> pair1("127.0.0.1", PORT1);
	//std::pair<std::string, unsigned int> pair2("127.0.0.1", PORT2);
	//std::pair<std::string, unsigned int> pair3("127.0.0.1", PORT3);
	//std::pair<std::string, unsigned int> pair4("127.0.0.1", PORT4);
	//std::pair<std::string, unsigned int> pair5("127.0.0.1", PORT5);
	
	std::list<std::pair<std::string, unsigned int> > listeners;
	listeners.push_back(pair1);
//	listeners.push_back(pair2);
//	listeners.push_back(pair3);
//	listeners.push_back(pair4);
//	listeners.push_back(pair5);
	
	int retVal = do_input_output(listeners);
	
	return retVal;
}