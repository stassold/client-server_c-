#include <cstdlib>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <thread>
#include <algorithm>
#include <mutex>
#include <condition_variable>

#define PORT 9999

using boost::asio::ip::udp;
using std::cout;
using std::cin;
using std::endl;
using std::sort;
using std::mutex;
using std::condition_variable;

boost::asio::io_service the_io_service;
udp::socket the_client_socket(the_io_service);

class Digit_converter
{
    public:
        Digit_converter(udp::endpoint ep)
        {
            this->the_client_endpoint = ep;
        }
       
        void Input_message()
        {
            while (true)
            {

                    cin.clear();
                    cout << "Введите символьное число, но не больше 64 символов" << endl;
                    cin >> message;
                    if (message.size() > 64)
                        Error_input(2);
                    if (!IsDigitsOnly(message))
                        Error_input(1);
                    Sort_message(message);
                    Even_replace_mesage(message);
                    Work_of_Buf(1, message);
            }
        }
        void Read_message()
        {
            while (true)
            {
                while (rd)
                {
                    std::string mesg;
                    Work_of_Buf(2, mesg);
                    Print_buf(mesg);
                    send_data(mesg);
                }
            }
        }

    private:
        std::string data;
        udp::endpoint the_client_endpoint;
        std::string message;
        mutex mtx;
        condition_variable cv;
        std::string buf;
        bool rd = false;

        bool IsDigitsOnly(std::string str)
        {
            for  (char c : str)
            {
                if (c < '0' || c > '9')
                    return false;
            }
            return true;
        }

        void Error_input(int error)
        {
            switch (error) {
            case 1:
                cout << endl << "В строке не должно быть букв, только цифры" << endl << endl;
                message = "";
                break;
            case 2:
                cout << endl << "Размер строки превышен 64 символова" << endl << endl;
                message = "";
                break;
            default:
                cout << "Not Error" << endl;
                break;
            }
        }

        int Sum_number_message(std::string& msg)
        {
            int sum = 0;;
            for (char c : msg)
            {
                if (c > '0' && c <= '9')
                    sum += c - '0';
            }
            return sum;
        }

        void Sort_message(std::string &msg)
        {
            std::sort(msg.begin(), msg.end(), [](auto const& i, auto const& j) {
                return i > j;
                });
        }

        void Even_replace_mesage(std::string& msg)
        {
            std::string even_char = "2468";
            std::string sub;
            for (int i = 0; i < msg.size(); i++)
            {
                char c = msg[i];
                if (even_char.find(c) != std::string::npos)
                {
                    sub += "KB";
                    continue;
                }
                sub += c;
            }
            msg = sub;
        }

        void Work_of_Buf(int mode, std::string& msg)
        {
            mtx.lock();
            switch (mode) {
            case 1:
                buf += msg;
                if (buf == "")
                    rd = false;
                else
                    rd = true;
                mtx.unlock();
                break;
            case 2:
                msg = buf;
                buf = "";
                rd = false;
                mtx.unlock();
                break;
            default:
                cout << "Not mode" << endl;
                mtx.unlock();
                break;

            }
        }

        void Print_buf(std::string& msg)
            {
            mtx.lock();
            cout << endl << "В буфере получилось следующая строка " << msg << endl;
            mtx.unlock();

            }

        void send_data(std::string& msg)
        {
            data = std::to_string(Sum_number_message(msg));
            the_client_socket.connect(the_client_endpoint);
            the_client_socket.send_to(boost::asio::buffer(data), the_client_endpoint);
        }
};




int main()
{
    setlocale(LC_ALL, "Russian");
    udp::endpoint the_client_endpoint(boost::asio::ip::address::from_string("127.0.0.1"), PORT);
    Digit_converter dc(the_client_endpoint);
    std::thread t1(([&]()
        {
            dc.Input_message();
        }));
    std::thread t2(([&]()
        {
            dc.Read_message();
        }));
    t1.join();
    t2.join();
}
