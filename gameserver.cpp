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
#include <mutex>
#include <semaphore>

#include <boost/asio.hpp>
using boost::asio::ip::tcp;

#include <cstring>

using namespace std;

enum Direction {STATIC = -1, UP = 0, DOWN, LEFT, RIGHT};

void testConnection();
// void handle_msgs(std::shared_ptr<tcp::socket> socket_ptr, int connect_num); // Pass shared_ptr
std::string intro();

// GLOBAL VARIABLES
std::mutex mtx;
int bot_num;
class Bot;
class Projectile;

std::vector<Bot*> bots;
std::vector<std::thread> threads;
std::vector<tcp::socket> sockets;

char arena[9][28] = {
    "+-------------------------+", //0    2,1
    "| A      #         $     |", // 1
    "|    ###    B            |", // 2
    "|         ###            |", // 3
    "|   $       #      C     |",
    "|                       #|",
    "|       ###              |",
    "|  D      $      #       |",
    "+------------------------\n"
};


 int positions[4][2] = {
        {1, 2},
        {2, 12},
        {4, 20},
        {7, 3}
    };

char letters[4] = {'A', 'B', 'C', 'D'};

int pj = -1;
int pk = -1;

std::vector<Projectile*> projectiles;

class Bot{
int health;
char letter;

Bot(char letter1, int j, int k){
    letter = letter1;
    this->j = j;
    this->k = k;
};

public:
int weaponOut = 0;
int j;
int k;
Direction direction = Direction::STATIC;
void handle_msgs(std::shared_ptr<tcp::socket> socket_ptr, int connect_num);
void obey_command(string message);

void move_up(char arena[9][28]);
void move_down(char arena[9][28]);
void move_left(char arena[9][28]);
void move_right(char arena[9][28]);
void move_switch(char arena[9][28]);

void attack_up(char arena[9][28]);
void attack_down(char arena[9][28]);
void attack_left(char arena[9][28]);
void attack_right(char arena[9][28]);
void attack_switch(char arena[9][28]);



void dec_weapon_out();

static std::vector<std::shared_ptr<boost::asio::ip::tcp::socket>> socket_ptrs;

void set_letter(char letter){
}

static Bot * create_bot(char letter, int j, int k){
    return new Bot(letter, j, k);
}

};  // END OF CLASS // END OF CLASS // END OF CLASS // END OF CLASS // END OF CLASS // END OF CLASS


class Projectile {

Projectile(Direction direction, int j, int k){
    this->direction = direction;
    this->j = j;
    this->k = k;
};

public:
int count;
int j, k;
char letter;
Direction direction = Direction::STATIC;
void attack_up(char arena[9][28]);
void attack_down(char arena[9][28]);
void attack_left(char arena[9][28]);
void attack_right(char arena[9][28]);
void attack_switch(char arena[9][28]);

static Projectile * create_projectile(Direction direction, int j, int k){
    return new Projectile(direction, j, k);
}
};

void Bot::dec_weapon_out(){ // j = row. k = column
   if (weaponOut > 0){
    weaponOut--;
   }
}


void Bot::move_up(char arena[9][28]){ // j = row. k = column
     if (arena[j-1][k] != '-'){
        j--;
        arena[j][k] = letter;
        arena[j+1][k] = ' ';
     }
   
}
void Bot::move_down(char arena[9][28]){
    if (arena[j+1][k] != '-'){
        j++;
        arena[j][k] = letter; 
        arena[j-1][k] = ' ';
    }
}
void Bot::move_left(char arena[9][28]){
    if (arena[j][k-1] != '|'){
        k--;
        arena[j][k] = letter;
        arena[j][k+1] = ' ';
    }
}
void Bot::move_right(char arena[9][28]){
     if (arena[j][k+1] != '|'){
        k++;
        arena[j][k] = letter;
        arena[j][k-1] = ' ';
    }
}


void Projectile::attack_up(char arena[9][28]){ // j = row. k = column
        arena[j-1][k] = '%';
        j--;
}

void Projectile::attack_down(char arena[9][28]){ // j = row. k = column
        arena[j+1][k] = '%';
        j++;
}

void Projectile::attack_left(char arena[9][28]){ // j = row. k = column
        arena[j][k-1] = '%';
        k--;
}

void Projectile::attack_right(char arena[9][28]){ // j = row. k = column
        arena[j][k+1] = '%';
        k++;
}



void Bot::move_switch(char arena[9][28]){
        switch(direction){
            case 0:
                if (arena[j-1][k] == '#'){
                    // std::lock_guard<std::mutex> lock(mtx);
                    move_up(arena);
                } else{
                    move_up(arena);
                }
                break;
            case 1:
                 if (arena[j+1][k] == '#'){
                    //std::lock_guard<std::mutex> lock(mtx);
                    move_down(arena);
                } else{
                    move_down(arena);
                }
                break;
            case 2:
                if (arena[j][k-1] == '#'){
                    //std::lock_guard<std::mutex> lock(mtx);
                    move_left(arena);
                } else{
                    move_left(arena);
                }
                break;
            case 3:
                if (arena[j][k+1] == '#'){
                    //std::lock_guard<std::mutex> lock(mtx);
                    move_right(arena);
                } else{
                    move_right(arena);
                }
                break;
            default:
                break;
        }
}

std::vector<std::shared_ptr<boost::asio::ip::tcp::socket>> Bot::socket_ptrs;

void testConnection(){ 
    boost::system::error_code ignored_error;
    constexpr unsigned short PORT = 12345;
    try{
        boost::asio::io_context io_context;
        // A tcp::acceptor object to listen for new connections. Set to listen on TCP port 12345, IP version 4.
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), PORT));
        int connect_num = 0;
        for(;;){
            int j = positions[connect_num][0];
            int k = positions[connect_num ][1];
            char letter = letters[connect_num];

            // Shared pointer to dynamically allocated socket
            auto socket_ptr = std::make_shared<tcp::socket>(io_context); // Use shared_ptr here
            Bot::socket_ptrs.push_back(socket_ptr);

            std::cout << "awaiting connection...." << std::endl;
            acceptor.accept(*socket_ptr);  // Dereference the shared pointer to pass to acceptor

            Bot * bot = Bot::create_bot(letter, j, k);
            bots.push_back(bot);
            
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
        // Bot::move_up(arena);
            direction = Direction::UP;
        } else if (message.find('s') != std::string::npos) {
        //    Bot::move_down(arena);
            direction = Direction::DOWN;
        } else if (message.find('a') != std::string::npos) {
        // Bot::move_left(arena);
            direction = Direction::LEFT;
        } else if (message.find('d') != std::string::npos) {
        //    Bot::move_right(arena);
            direction = Direction::RIGHT;
        } else if (message.find('f') != std::string::npos) {
        //    Bot::move_right(arena);
            cout << "space received" << endl;
            Projectile * proj = Projectile::create_projectile(direction, j, k);
            projectiles.push_back(proj);

        } else {
            int msg_int = stoi(message);
            if (msg_int == 72) {
               // Bot::move_up(arena);
               direction = Direction::UP;
            } else if (msg_int == 80) {
                direction = Direction::DOWN;
               // Bot::move_down(arena);
            } else if (msg_int == 75) {
                direction = Direction::LEFT;
               // Bot::move_left(arena);
            } else if (msg_int == 77) {
                direction = Direction::RIGHT;
               // Bot::move_right(arena);
            } else if (msg_int == 32){
                cout << "space received" << endl;
                weaponOut = 2;
            }
        }
}

void Bot::handle_msgs(std::shared_ptr<tcp::socket> socket_ptr, int connect_num) {  
    boost::system::error_code ignored_error;
    
    for(;;){
        std::string message(30, '\0'); // 1024
        boost::system::error_code error;
        socket_ptr->receive(boost::asio::buffer(message), 0, error);  
    
        std::cout << "TEST SERVER-SIDE MESSAGE: " << message<< std::endl;

        mtx.lock();
        obey_command(message);
        move_switch(arena);
        mtx.unlock();

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

        // int num_sock_ptrs = static_cast<int>(Bot::socket_ptrs.size());

        // cout << message << endl;

         //std::string message2(1, '\0'); // 1024
         //message2 = "F";

    }
}

void Projectile::attack_switch(char arena[9][28]){

    switch(direction){
            case 0:
                if (arena[j-1][k] == '#'){
                    // std::lock_guard<std::mutex> lock(mtx);
                    attack_up(arena);
                } else{
                    attack_up(arena);
                }
                break;
            case 1:
                 if (arena[j+1][k] == '#'){
                    //std::lock_guard<std::mutex> lock(mtx);
                    attack_down(arena);
                } else{
                    attack_down(arena);
                }
                break;
            case 2:
                if (arena[j][k-1] == '#'){
                    //std::lock_guard<std::mutex> lock(mtx);
                    attack_left(arena);
                } else{
                    attack_left(arena);
                }
                break;
            case 3:
                if (arena[j][k+1] == '#'){
                    //std::lock_guard<std::mutex> lock(mtx);
                    attack_right(arena);
                } else{
                    attack_right(arena);
                }
                break;
            default:
                break;
        }
} 

void game_loop(){
    boost::system::error_code ignored_error;
    int c = 0;

    arena[4][7] = 'X';
    for(;;){
        std::this_thread::sleep_for(std::chrono::milliseconds(600)); //300
        
        for (int i = 0; i < 9; ++i) {
            std::cout << arena[i] << std::endl;
        }

        auto buffer = boost::asio::buffer(reinterpret_cast<char*>(arena), sizeof(arena));

        int num_sock_ptrs = static_cast<int>(Bot::socket_ptrs.size());

        int num_bots = bots.size();

       //  cout << num_bots << endl;

        int num_projectiles = projectiles.size();

        for (int i = 0; i < num_projectiles; i++){
            projectiles.at(i)->attack_switch(arena);
        }

        for (int i = 0; i < num_sock_ptrs; i++){
            try{
                boost::asio::write(*(Bot::socket_ptrs.at(i)), buffer, ignored_error);
            }
            catch(const std::exception& e){
                std::cout << "Error occurred, but it's ignored: " << e.what() << std::endl;
            }
        }

         c++;

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
