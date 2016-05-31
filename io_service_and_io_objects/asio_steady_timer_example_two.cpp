// Two asynchronous operations with boost::asio::steady_timer
//
#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <chrono>
#include <iostream>

using namespace boost::asio;

int main()
{
	io_service ioservice;
	
	steady_timer timerOne(ioservice, std::chrono::seconds(3));
	timerOne.async_wait([](const boost::system::error_code& ec)
											{
												std::cout << "3 Seconds.\n";
											});

	steady_timer timerTwo(ioservice, std::chrono::seconds(4));
	timerTwo.async_wait([](const boost::system::error_code& ec)
											{
												std::cout << "4 Seconds.\n";	
											});

	ioservice.run();
}

/*
 * In the above example, two objects of type boost::asio::steady_timer are used. The 
 * first object is an alarm clock that rings after three seconds. The other is an 
 * alarm clock ringing after four seconds. After both time periods expire, the lambda 
 * functions that were passed to async_wait() will be called.
 *
 * run() is called on the only I/O service object in this example. This call passes 
 * control to the operating system functions that execute asynchronous operations. 
 * With their help, the first lambda functions is called after three seconds and the 
 * second lambda function is called after four seconds.
 *
 * It might come as a surprise that asynchronous operations require a call to a 
 * blocking function. However, this is not a problem because the program has to be
 * prevented from exiting.
 *
 * If run() wouldn't block,main() would return, and the progrsm would exit. If you 
 * don't want to wait for run() to return, you only need to call run() in a new thread.
 *
 * The reason why the examples above exit after a while is that run() returns if there
 * are no asynchronous operations. Once all alarm clocks have rung, no asynchronous 
 * operations exist thst the program needs to wait for.
 */
