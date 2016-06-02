/**
 * Even though Boost.Asio can process any kind of data asynchronously, it is mainly
 * used for network programming. This is because Boost.Asio supported network 
 * functions long before additional I/O objects were added.
 *
 * Network functions are a perfect use for asynchronous operations because the
 * transmission of data over a network may take a long time, which means 
 * acknowledgments and errors may not be available may not be available as fast as 
 * the functions that send or receive data can execute.
 *
 * Boost.Asio provides many I/O objects to develop network programs. The example
 * below use the class boost::asio::ip::tcp::socket to establish a connection 
 * with another computer. This example sends a HTTP request to a web server to 
 * download the homepage.
 */

// A web client with boost::asio::ip::tcp::socket
#include <boost/asio/io_service.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <array>
#include <string>
#include <iostream>

using namespace boost::asio;
using namespace boost::asio::ip;

io_service 							ioservice;
tcp::resolver 					resolv(ioservice);
tcp::socket 						tcp_socket(ioservice);
std::array<char, 4096>	bytes;

void read_handler(const boost::system::error_code& ec, std::size_t bytes_transferred)
{
	if (!ec)
	{
		std::cout.write(bytes.data(), bytes_transferred);
		tcp_socket.async_read_some(buffer(bytes), read_handler);
	}
}

void connect_handler(const boost::system::error_code& ec)
{
	if (!ec)
	{
		std::string r = "GET / HTTP/1.1\r\nHost: theboostcpplibraries.com\r\n\r\n";
		write(tcp_socket, buffer(r));
		tcp_socket.async_read_some(buffer(bytes), read_handler);
	}
}

void resolve_handler(const boost::system::error_code& ec, tcp::resolver::iterator it)
{
	if (!ec)
		tcp_socket.async_connect(*it, connect_handler);
}
 
int main()
{
	tcp::resolver::query q("theboostcpplibraries.com", "80");
	resolv.async_resolve(q, resolve_handler);
	ioservice.run();
}

/*
 * The above example uses three handlers:
 * connect_handler() and read_handler() are called when the connection is established
 * and data is received.
 * resolve_handler() is used for name resolution.
 *
 * Because data can only be received after a connection has been established, and 
 * because a connection can only established after the name has been resolved, the 
 * various asynchronous operations are started in handlers. In resolve_handler(), the
 * iterator "it", which points to an endpoint resolved from the name, is used with 
 * "tcp_socket" to establish a connection. In connect_handler(), "tcp_socket" is 
 * accessed to send a HTTP request and start receiving data. Since all operations
 * are asynchronous, handlers are passed to the respective functions. Depending on 
 * the operations, additional parameters may need to be passed. For example, the 
 * iterator "it" refers to an endpoint resolved from a name. The array "bytes" is 
 * used to store data received.
 *
 * In main(), boost::asio::ip::tcp::resolver::query is instantiated to create an 
 * object "q". "q" represents a query for the name resolver, an I/O object of type 
 * boost::asio::ip::tcp::resolver. By passing "q" to async_resolve(), an
 * asynchronous operation is started to resolve the name. 
 *
 * The above example resolves the name "theboostcpplibraries.com". After the 
 * asynchronous operation has been started, run() is called on the I/O service 
 * object to pass control to the operating system.
 *
 * When the name has been resolved, resolve_handler() is called. The handler first
 * checks whether the name resolution has been successful. In this case "ec" is 0.
 * Only then is the socket accessed to establish a connection. The address of the 
 * server to connect to is provided by the second parameter, which is of type 
 * boost::asio::ip::tcp::resolver::iterator. The parameter is the result of the 
 * name resolution.
 *
 * The call to async_connect() is followed by a call to the handler connect_handler().
 * Again "ec" is checked first to find out whether a connection could be established.
 * If so, async_read_some() is called on the socket. With this call, reading data 
 * begins. Data being received is stored in the array "bytes", which is passed as
 * a first parameter to async_read_some().
 *
 * read_handler() is called when one or more bytes have been received and copied to 
 * "bytes". The parameter "bytes_transferred" of type std::size_t contains the 
 * number of bytes that have been received. As usual, the handler should check first
 * "ec" whether the asynchronous operation was completed successfully. Only if this
 * is the case is data written to standard output.
 *
 * Pleas note that read_handler() calls async_read_some() again after data has been 
 * written to "std::cout". This is required because you can't be sure that the 
 * entire homepage was downloaded and copied into "bytes" in a single asynchronous 
 * operation. The repeated calls to async_read_some() followed by the repeated
 * calls to read_handler() only end when the connection is closed, which happens when
 * the web server has sent the entire homepage. Then read_handler() reports an error 
 * in an "ec". At this point, no further data is written to "std::cout" and 
 * async_read() is not called on the socket. Because there are no pending 
 * asynchronous operations, the program exits.
 */ 
