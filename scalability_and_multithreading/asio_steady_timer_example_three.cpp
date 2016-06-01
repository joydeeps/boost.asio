/*
 * Scalability describes the ability of a program to effectively benefit from 
 * additional resources. With Boost.Asio it is possible to benefit from the ability 
 * of external devices to execute operations concurrently. If threads are used, 
 * several functions can be executed concurrently on available cpu cores. Boost.Asio
 * with threads improves the scalability because your program can take advantage of
 * internal and external devices that can execute operations independently or in 
 * co-operation with each other.
 *
 * If the member function run() is called on an object of type boost::asio::io_service
 * the associated handlers are invoked within the same thread. By using multiple 
 * threads, a program can call run() multiple times. Once an asynchronous operation is
 * complete, the I/O service object will execute the handler in one of these threads. 
 * If a second operation is completed shortly after the first one, the I/O service 
 * object can execute the handler in a different thread. Now, not only can operations 
 * outside of a process be concurrently, but handlers within the process can be 
 * executed concurrently too.
 */

// Two threads for the I/O service object to execute handlers concurrently
#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <chrono>
#include <thread>
#include <iostream>

using namespace boost::asio;

int main()
{
	io_service ioservice;
	
	steady_timer timer1(ioservice, std::chrono::seconds(3));
	timer1.async_wait([](const boost::system::error_code& ec)
										{
											std::cout << "Thread1: " << "3 seconds.\n";
										});

	steady_timer timer2(ioservice, std::chrono::seconds(3));
	timer2.async_wait([](const boost::system::error_code& ec)
										{
											std::cout << "Thread2: " << "3 seconds.\n";
										});

	std::thread thread1([&ioservice]()
											{
												ioservice.run();
											});
	std::thread thread2([&ioservice]()
											{
												ioservice.run();
											});

	thread1.join();
	thread2.join();
}

/*
 * In the above example, both alarm clocks should ring after three seconds. Because
 * two threads are available, both lambda functions can be executed concurrently.
 * If the second alarm clock rings while the handler of the first alarm clock is being
 * executed, the handler can be executed in the second thread. If the handler of the
 * first larm clock has already returned, the I/O service object can use any thread 
 * to execute the second handler.
 *
 * Ofcourse, it doesn't always make sense to use threads. The above example might not
 * write the messages sequentially to the standard output stream. Instead, they might 
 * be mixed up. Both handlers, which may run in two threads concurrently, share the 
 * global resource std::cout. To avoid interruptions, access to std::cout would need
 * to be synchronized. The advantage of threads is lost if handlers can't be executed 
 * concurrently.
 */
