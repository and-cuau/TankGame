#include <iostream>
#include <string>
#include <thread>
#include <conio.h>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
using namespace std;

void send_msgs(tcp::socket& socket) {
    boost::system::error_code error;
    std::string testmsg;

    while (true) {
        string keyStr;
        int key = _getch();
        keyStr = key;

       if (!(key == 'w' || key == 'a' || key == 's' || key == 'd')){
            key = _getch();
            keyStr = to_string(key);
       }

       // cout << keySTr << endl;

        boost::asio::write(socket, boost::asio::buffer(keyStr), error);

        if (error) {
            std::cerr << "Failed to send message: " << error.message() << std::endl;
            break;
        }
    }
}

void receive_msgs(tcp::socket& socket) {
    boost::system::error_code error;

    while (true) {
        std::string message;
        boost::asio::streambuf buffer;
        
        cout << "receive loop test" << endl;
        boost::asio::read_until(socket, buffer, "\n", error);
        cout << "receive loop test 2" << endl;
       
        if (error == boost::asio::error::eof)
            break; // Connection closed cleanly by peer
        else if (error)
            throw boost::system::system_error(error); // Other

        std::istream is(&buffer);
        std::getline(is, message);

        std::cout << message << std::endl;
    }
}

enum Key { UP = 0, DOWN, LEFT, RIGHT };
// bool keyPressed[4] = {false, false, false, false};



int main() {
    try {
        constexpr unsigned short PORT = 12345;

        boost::asio::io_context io_context;
        tcp::resolver resolver(io_context);
        tcp::resolver::results_type endpoints = resolver.resolve("127.0.0.1", std::to_string(PORT));
        tcp::socket socket(io_context);

        // Connect to the server
        boost::asio::connect(socket, endpoints);


        // Start the receive thread
        std::thread receiver(receive_msgs, std::ref(socket));

        // Handle sending messages
        send_msgs(socket);


        // Join the receive thread
        receiver.join();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
