#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/make_shared.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <istream>
#include <list>
#include <string>

using boost::asio::ip::tcp;

// types declared for acceptor and socket
typedef boost::asio::ip::tcp::acceptor	acceptor_type;
typedef boost::asio::ip::tcp::socket	socket_type;

// port no. to bind the server to.
const short PORT = 11235;

class MyConnection : public boost::enable_shared_from_this<MyConnection>
{
	public:
		MyConnection(boost::asio::io_service& ioservice) : socket(ioservice) 
		{}
		
		~MyConnection() {}
		
		socket_type& Socket()
		{
			return socket;
		}
		
		void Session()
		{
			asyncRead();
		}
		
		void Stop()
		{
			socket.cancel();
		}
		
		typedef boost::shared_ptr<MyConnection> shared_ptr_to_myconnection;
		
	protected: 
		// memeber variables
		socket_type								socket;
		boost::asio::streambuf		stream_buffer;
		std::string								message;
		
		void asyncRead()
		{
			boost::asio::async_read_until(
						socket, 
						stream_buffer,
						'\0',
						boost::bind(
							&MyConnection::readHandler,
							shared_from_this(),
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred
						)
			);
		}
		
		void asyncWrite(const std::string& s)
		{
			message = s;
			
			boost::asio::async_write(
						socket,
						boost::asio::buffer(message.c_str(), message.size() + 1),
						boost::bind(
							&MyConnection::writeHandler,
							shared_from_this(),
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred
						)
			);
		}
		
		void readHandler(const boost::system::error_code& ec, 
											size_t bytes_transferred)
		{
			if (!ec)
			{
				std::cout << extractString() << std::endl;
				asyncRead();			// read again
			}
			else
			{
				// do nothing, "this" will be deleted latter.
			}
		}
		
		void writeHandler(const boost::system::error_code& ec, 
												size_t bytes_transferred)
		{
			
		}
		
		std::string extractString()
		{
			std::istream is(&stream_buffer);
			std::string s;
			std::getline(is, s, '\0');
			return s;
		}
};

class MyServer
{
	public:
		MyServer() : 
			_service(),
			_work(boost::asio::io_service::work(_service)),
			_acc(_service, tcp::endpoint(tcp::v4(), PORT)),
			_thread(boost::bind(&boost::asio::io_service::run, &_service))
			{}
			
		~MyServer()
		{
			stop();
			_work.reset();
			
			if (_thread.joinable())
				_thread.join();
		}
		
		void start()
		{
			doAccept();
		}
		
		void stop()
		{
			_acc.cancel();
		}
		
		void stopAllConnections()
		{
			for (auto c: m_connections)
			{
				if (auto p = c.lock())
					p->Stop();
			}
		}
		
	protected:
		void acceptHandler(const boost::system::error_code& ec, 
						MyConnection::shared_ptr_to_myconnection accepted)
		{
			if (!ec)
			{
				m_connections.push_back(accepted);
				accepted->Session();
				
				doAccept(); 			// call again to listen for new connections
			}
		}
		
		void doAccept()
		{
			auto newaccept = boost::make_shared<MyConnection>(_service);
			_acc.async_accept(
							newaccept->Socket(),
							boost::bind(&MyServer::acceptHandler,
											this,
											boost::asio::placeholders::error,
											newaccept
							)
			);
		}
		
	protected:
		boost::asio::io_service 													_service;
		boost::optional<boost::asio::io_service::work> 		_work;
		acceptor_type																			_acc;
		boost::thread																			_thread;
		
	public:
		std::list<boost::weak_ptr<MyConnection> > m_connections;
};
									
int main()
{
	try
	{
		
		MyServer s;
		s.start();

		std::cerr << "Shutdown in 2 seconds.............\n";
	
		boost::this_thread::sleep_for(boost::chrono::seconds(2));
	
		std::cerr << "Shutdown............\n";
	
		s.stopAllConnections();		// interrupt ongoing connections!!!
	} 					// destructor of the server will join the service thread
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		return 1;
	}
	
	return 0;
}