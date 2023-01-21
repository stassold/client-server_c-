#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include <string>

#define PORT 9999

using boost::asio::ip::udp;

boost::system::error_code ec_acceptor;
boost::asio::io_service the_io_service;
udp::endpoint the_endpoint(udp::v4(), PORT);
udp::socket the_socket(the_io_service);
char buff[32];
std::string data;


void cheker_data(std::string& data)
{
	int sum = stoi(data);
	if ((data.size() > 2) && (sum % 32) == 0)
		std::cout << sum << std::endl;
	else
		std::cout << "Полученное значение не кратно 32 или меньше двух символов" << std::endl;
}

void on_read(const boost::system::error_code& err, std::size_t
	read_bytes)
{
	data = buff;
	cheker_data(data);
	the_socket.async_receive_from(boost::asio::buffer(buff), the_endpoint, on_read);
	memset(buff, 0, 32);
}


int main()
{

	std::string data;
	std::cout << "Server_Started" << std::endl;
	while (true)
	{
		setlocale(LC_ALL, "Russian");
		the_socket.open(the_endpoint.protocol());
		the_socket.set_option(boost::asio::ip::udp::socket::reuse_address(true));
		the_socket.bind(the_endpoint);
		the_socket.async_receive_from(boost::asio::buffer(buff), the_endpoint, on_read);
		the_io_service.run();
	}
}

