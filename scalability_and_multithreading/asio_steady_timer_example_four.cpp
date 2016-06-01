// One thread for each of two I/O service objects to execute handlers concurrently
#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <chrono>
#include <thread>
#include <iostream>

using namespace boost::asio;

int main()
{
	io_service ioservice1;
	io_service ioservice2;

	steady_timer timer1(ioservice1, std::chrono::seconds(3));
	timer1.async_wait([](const boost::system::error_code& ec)
	{
		std::cout << "Thread1: " << "3 seconds.\n";
	});

	steady_timer timer2(ioservice2, std::chrono::seconds(3));
	timer2.async_wait([](const boost::system::error_code& ec)
	{
		std::cout << "Thread2: " << "3 seconds.\n";	
	});

	std::thread thread1([&ioservice1]()
	{
		ioservice1.run();
	});
	std::thread thread2([&ioservice2]()
	{
		ioservice2.run();
	});
	thread1.join();
	thread2.join();
}

/**
 * Calling run() repeatedly on a single I/O service object is the recommended method
 * to make a program based on Boost.Asio more scalable. However, instead of providing
 * several threads to one I/O service object, you could also create multiple I/O 
 * service objects.
 *
 * Two I/O service objects are used next to two alarm clocks of type 
 * boost::asio::steady_timer. The program is based on two threads, with each thread 
 * bound to another I/O service object. The two I/O objects "timer1" and "timer2"
 * aren't bound to the same I/O service object anymore. They are bound to different 
 * objects.
 *
 * It's not possible to give a general advice about when it makes sense to use more
 * than one I/O service object. Because boost::asio::io_service represents an
 * operating system interface, any decision depends on particular interface.
 *
 * In Linux, boost::asio::io_service is usually based on epoll(). Having several I/O
 * service objects means that several I/O completion ports will be used or epoll() 
 * will be called multiple times. Whether this is better than using just one I/O 
 * completion port or one call to epoll() depends on the individual case.
 *
 */
