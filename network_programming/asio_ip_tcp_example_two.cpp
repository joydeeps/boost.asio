// A time server with boost::asio::ip::tcp::acceptor
#include <boost/asio/io_service.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <string>
#include <ctime>

using namespace boost::asio;
using namespace boost::asio::ip;

io_service 				ioservice;
tcp::endpoint			tcp_endpoint(tcp::v4(), 2014);
tcp::acceptor			tcp_acceptor(ioservice, tcp_endpoint);
tcp::socket				tcp_socket(ioservice);
std::string				data;			

void write_handler(const boost::system::error_code& ec, std::size_t bytes_transferred)
{
	if (!ec)	
		tcp_socket.shutdown(tcp::socket::shutdown_send);
}

void accept_handler(const boost::system::error_code& ec)
{
	if (!ec)
	{
		std::time_t now = std::time(nullptr);
		data = std::ctime(&now);
		async_write(tcp_socket, buffer(data), write_handler);
	}
}

int main()
{
	tcp_acceptor.listen();
	tcp_acceptor.async_accept(tcp_socket, accept_handler);
	ioservice.run();
}

/*
 * The above example is a time server. You can connect with a telnet client to get the
 * current time.
 *
 * The time server uses the I/O object boost::asio::ip::tcp::acceptor to accept an 
 * incoming connection from another program. You must initialize the object so it knows
 * which protocol to use on which port. In the example, the variable "tcp_endpoint" of
 * type boost::asio::ip::tcp::end_point is used to tell "tcp_acceptor" to accept 
 * incoming connections of version 4 of the internet protocol on port 2014.
 *
 * After the acceptor has been initialized, listen() is called to make the acceptor
 * start listening. Then async_accept() is called to accept the first connection 
 * attempt. A socket has to be passed as a first parameter to async_accept(), which 
 * will be used to send and receive data on a new connection.
 *
 * Once another program establishes a connection, accept_handler() is called. If the 
 * connection was established successfully, the current time is sent with
 * boost::asio::async_write(). This function writes all data in "data" to the socket.
 * boost::asio::ip::tcp::socket also provides the memeber function async_write_some().
 * This function calls the handler when at least one byte has been sent. Then, once 
 * again, it has to call async_write_some(). Repeatedly calculating the number of bytes
 * left to send and calling async_write_some() can be avoided by using 
 * boost::asio::async_write(). The asynchronous operation that started with this
 * function is only complete when all bytes in "data" have been sent.
 *
 * After the data has been sent, write_handler() is called. This function calls
 * shutdown() with parameters boost::ip::tcp::socket::shutdown_send, which says the
 * program is done sending through the socket. since there are no pending 
 * asynchronous operations, the above program exits. 
 */  
