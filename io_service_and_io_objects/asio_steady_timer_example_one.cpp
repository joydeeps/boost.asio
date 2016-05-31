/*
 * Programs that use Boost.Asio for asynchronous data processing are based on I/O
 * services and I/O objects.
 *
 * I/O services abstract the operating system interfaces that process data
 * asynchronously.
 *
 * I/O objects initiate asynchronous operations.
 *
 * As a user of Boost.Asio normally don't connect with I/O services directly. I/O
 * services are managed by an I/O service object. An I/O service object is like a 
 * registry where I/O services are listed. Every I/O object knows it's I/O service
 * and gets access to its I/O service through the I/O service object.
 *
 * Boost.Asio defines boost::asio::io_service, a single class for an I/O service
 * object. Every program based on Boost.Asio uses an object of type 
 * boost::asio::io_service. This can also be a global variable.
 *
 * While there is only one class for an I/O service object, several classes for I/O
 * objects exit. Because I/O objects are task oriented, the class that needs to be 
 * instantiated depends on the task. For example if the data has to be sent or 
 * received over a TCP/IP connection, an I/O object of type 
 * boost::asio::ip::tcp::socket can be used. If you want to wait for a time period
 * to expire, you can use the I/O object boost::asio::steady_timer.
 *
 * boost::asio::steady_timer is like an alarm clock. Instead of waiting for a
 * blocking function to return when the alarm clock rings, your program will be
 * notified.
 */
// Using boost::asio::steady_timer
//
#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <chrono>
#include <iostream>

using namespace boost::asio;

int main()
{
	io_service ioservice;
	
	steady_timer timer{ioservice, std::chrono::seconds(3)};
	timer.async_wait([](const boost::system::error_code& ec) {
											std::cout << "3 Second" << std::endl;	
									});

	ioservice.run();

	return EXIT_SUCCESS;
}

// The above example creates an I/O service object "ioservice", and uses it to 
// initialize the I/O object "timer". 
// Like boost::asio::steady_timer, all I/O objects expect an I/O service object as 
// a first parameter in their constructor.
// Since "timer" represents an alarm clock, a second parameter can be passed to the
// constructor of boost::asio::steady_timer that defines the specific time or 
// time period when the alarm clock should ring.
// The time starts with the definition of "timer".
//
// Instead of calling a blocking function thst will return when the alarm clock
// rings, Boost.Asio lets you start an asynchronous operation. To do this, call
// the member function async_wait(), which expects a "handler" as the sole parameter
// 
// A handler is a function or function object that is called when the asynchronous
// operation ends. In the above example, a lambda function is passed as a handler.
//
// async_wait() returns immediately. Instead of waiting three seconds until the
// alarm clock rings, the lambda function is called after three seconds. 
// When async_wait() returns, a program can do something else.
//
// A member function like async_wait() is called non-blocking. I/O objects usually 
// also provide blocking member functions as alternatives. For example, you can 
// call the blocking member function wait() on the boost::asio::steady_timer.
// Because this member function is blocking, no handler is passed. wait() returns 
// at a specific time or after a time period.
//
// The last statement in main() is a call to run() on the I/O service object. This
// call is required because operating system-specific functions have to take over
// control. Remember that it's the I/O service in the I/O service object which 
// implements asynchronous operations based on operating-systems-specific functions.
// 
// While async_wait() initiates an asynchronous operation and returns immediately, 
// run() blocks. Many operating systems support asynchronous operations only 
// through a blocking function.
//
// The next example shows why this is usually not a problem.
