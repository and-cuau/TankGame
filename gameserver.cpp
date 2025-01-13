#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <ctime>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <sys/types.h>
#include <queue>
#include <algorithm>

#include <boost/asio.hpp>
using boost::asio::ip::tcp;

#include <cstring>

using namespace std;

void handle_msgs2(std::shared_ptr<tcp::socket> socket_ptr, int connect_num);

void testConnection();
// void handle_msgs(std::shared_ptr<tcp::socket> socket_ptr, int connect_num); // Pass shared_ptr
std::string intro();
std::queue<std::string> q;
std::mutex mtx;
std::vector<std::thread> threads;
std::vector<tcp::socket> sockets;

//  std::vector<std::shared_ptr<boost::asio::ip::tcp::socket>> socket_ptrs;
int bot_num;

class Bot;
std::vector<Bot> bots;

int j = 2;
int k = 12;

 int positions[4][2] = {
        {1, 2},
        {2, 12},
        {4, 20},
        {7, 3}
    };

char letters[4] = {'A', 'B', 'C', 'D'};


class Bot{
int health;
char letter;
int j;
int k;

Bot(char letter1, int j, int k){
    letter = letter1;
    this->j = j;
    this->k = k;
};

public:
void handle_msgs(std::shared_ptr<tcp::socket> socket_ptr, int connect_num);
void obey_command(string message);

void move_up(char arena[9][28]);
void move_down(char arena[9][28]);
void move_left(char arena[9][28]);
void move_right(char arena[9][28]);

static std::vector<std::shared_ptr<boost::asio::ip::tcp::socket>> socket_ptrs;

void set_letter(char letter){

}

static Bot * create_bot(char letter, int j, int k){
    return new Bot(letter, j, k);
}

};  // END OF CLASS // END OF CLASS // END OF CLASS // END OF CLASS // END OF CLASS // END OF CLASS


void Bot::move_right(char arena[9][28]){
    arena[j][k] = letter;
    arena[j][k+1] = letter;
    k++;
}
void Bot::move_up(char arena[9][28]){
    arena[j][k] = letter;
    arena[j-1][k] = letter;
    j--;
}
void Bot::move_down(char arena[9][28]){
    arena[j][k] = letter;
    arena[j+1][k] = letter;
    j++;
}
void Bot::move_left(char arena[9][28]){
    arena[j][k] = letter;
    arena[j][k-1] = letter;
    k--;
}


std::vector<std::shared_ptr<boost::asio::ip::tcp::socket>> Bot::socket_ptrs;

char arena[9][28] = {
    "+-------------------------+",
    "| A      #         $     |",
    "|    ###    B            |",
    "|         ###            |",
    "|   $       #      C     |",
    "|                       #|",
    "|       ###              |",
    "|  D      $      #       |",
    "+-------------------------+"
};


void testConnection(){ 
    boost::system::error_code ignored_error;
    constexpr unsigned short PORT = 12345;
    try{
        boost::asio::io_context io_context;
        // A tcp::acceptor object to listen for new connections. Set to listen on TCP port 12345, IP version 4.
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), PORT));
        int connect_num = 0;
        for(int i = 0; i < 1; i++){
            int j = positions[connect_num][0];
            int k = positions[connect_num ][1];
            char letter = letters[connect_num];

            Bot * bot = Bot::create_bot(letter, j, k);

            // Shared pointer to dynamically allocated socket
            auto socket_ptr = std::make_shared<tcp::socket>(io_context); // Use shared_ptr here
            std::cout << "The address stored in pointer p 1: " << socket_ptr << std::endl;
            Bot::socket_ptrs.push_back(socket_ptr);

            std::cout << "awaiting connection...." << std::endl;
            acceptor.accept(*socket_ptr);  // Dereference the shared pointer to pass to acceptor
            
            
            
            // Pass the shared_ptr to the thread
            //threads.push_back(std::thread(std::ref(bot->handle_msgs), socket_ptr, connect_num)); // Pass the shared_ptr directly

            // threads.push_back(std::thread(std::bind(&Bot::handle_msgs, bot, socket_ptr, connect_num)));

             // threads.push_back(std::thread(handle_msgs2, socket_ptr, connect_num)); // chatserver method test

             threads.push_back(std::thread([bot, socket_ptr, connect_num]() { // new method. uncomment after tests
               bot->handle_msgs(socket_ptr, connect_num);
             }));

            connect_num++;
        }
        
        // Join all threads
        for (auto& t : threads) {
            t.join();
        }
        std::cout << "All threads have finished handling messages.\n";

    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}


void Bot::obey_command(string message){
         if (message.find('w') != std::string::npos) {
        Bot::move_up(arena);
        } else if (message.find('s') != std::string::npos) {
            Bot::move_down(arena);
        } else if (message.find('a') != std::string::npos) {
        Bot::move_left(arena);
        } else if (message.find('d') != std::string::npos) {
            Bot::move_right(arena);
        } else {
            int direction = stoi(message);
            if (direction == 72) {
                Bot::move_up(arena);
            } else if (direction == 80) {
                Bot::move_down(arena);
            } else if (direction == 75) {
                Bot::move_left(arena);
            } else if (direction == 77) {
                Bot::move_right(arena);
            }
        }
}



void Bot::handle_msgs(std::shared_ptr<tcp::socket> socket_ptr, int connect_num) {  
    boost::system::error_code ignored_error;
    
    for(;;){
        std::string message(30, '\0'); // 1024
        boost::system::error_code error;
        socket_ptr->receive(boost::asio::buffer(message), 0, error);  
        
       //  std::cout << "The address stored in pointer p 2: " << socket_ptr << std::endl;  

        //std::cout << "TEST SERVER-SIDE MESSAGE: " << message<< std::endl;

        //message.erase(std::remove(message.begin(), message.end(), '\n'), message.end());

        obey_command(message);

        message = std::to_string(connect_num) + ": " + message + "\n";
        if (error) {
            if (error == boost::asio::error::eof) {
                std::cout << "Connection closed by client.\n";
                break; // Client closed connection gracefully
            } else if (error == boost::asio::error::would_block) {
                std::cout << "Socket would block. Data not yet available.\n";
                continue; // Retry if in non-blocking mode
            } else {
                std::cerr << "Error during receive: " << error.message() << std::endl;
                break; // Break on other errors
            }
        }

        if (message.find("end") != std::string::npos) {
            break;
        }
        
        // Lock and push to queue
        mtx.lock();
        q.push(message);
        // std::cout << "Added to queue: " << q.back() << std::endl;
        mtx.unlock();

        int num_sock_ptrs = static_cast<int>(Bot::socket_ptrs.size());

        cout << message << endl;

         //std::string message2(1, '\0'); // 1024
         //message2 = "F";

        for (int i = 0; i < num_sock_ptrs; i++){
            cout << "test write loop" << endl;
            try{
                std::cout << "vector pointer x: " << socket_ptr << std::endl;
                boost::asio::write(*(Bot::socket_ptrs.at(i)), boost::asio::buffer(message), ignored_error);
            }
            catch(const std::exception& e){
                std::cout << "Error occurred, but it's ignored: " << e.what() << std::endl;
            }
        }

    }
}





void game_loop(){
    for(;;){
        std::this_thread::sleep_for(std::chrono::milliseconds(2100)); //300
        
        for (int i = 0; i < 9; ++i) {
            std::cout << arena[i] << std::endl;
        }
    }
}



using boost::asio::ip::tcp;
int main(int argc, char* argv[]) { 

    std::string response;
    std::cout << "\nServer has been initialized successfully and is now ready to accept connections.\n";

    thread game_thread(game_loop);
   
    testConnection();

    game_thread.join();

    return 0; 
}
