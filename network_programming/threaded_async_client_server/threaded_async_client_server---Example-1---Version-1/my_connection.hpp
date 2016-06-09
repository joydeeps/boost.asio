#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

class my_connection {
  public:
    my_connection() // constructor
		{
			close = false;
			// create new socket into which to receive the new connection
			this->socket = boost::shared_ptr<boost::asio::ip::tcp::socket>(
											new boost::asio::ip::tcp::socket(this->io_service)
										);
		}
 
    // we must have a new io_service for EVERY THREAD!
    boost::asio::io_service io_service;
 
    // where we receive the accepted socket and endpoint
    boost::shared_ptr<boost::asio::ip::tcp::socket> socket;
    boost::asio::ip::tcp::endpoint endpoint;
 
    // keep track of the thread for this connection
    boost::shared_ptr<boost::thread> thread;
 
    // keep track of the acceptor io_service so we can call stop() on it!
    boost::asio::io_service *master_io_service;
 
    // boolean to indicate a desire to kill this connection
    bool close;
 
    // NOTE: you can add other variables here that store connection-specific
    // data, such as received HTML headers, or logged in username, or whatever
    // else you want to keep track of over a connection
};