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
#include <cstring>

using boost::asio::ip::tcp;

using namespace std;

enum Direction {STATIC = -1, UP = 0, DOWN, LEFT, RIGHT};

void testConnection();
std::string intro(); 

// GLOBAL VARIABLES
std::mutex mtx;
std::counting_semaphore<2> semaphore{2};
int bot_num;
class Bot;
class Projectile;

std::vector<Bot*> bots;
std::vector<Projectile*> projectiles;
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

class Bot{
int health;
char letter;
int score;


Bot(char letter1, int j, int k){
    letter = letter1;
    this->j = j;
    this->k = k;
};

public:
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
int count = 0;
int j, k;
char letter;
Direction direction = Direction::STATIC;
void attack_up(char arena[9][28]);
void attack_down(char arena[9][28]);
void attack_left(char arena[9][28]);
void attack_right(char arena[9][28]);
void attack_right2(char arena[9][28]);
void attack_switch();

static Projectile * create_projectile(Direction direction, int j, int k){
    return new Projectile(direction, j, k);
}
};

class Missile {

Missile(Direction direction, int j, int k){
    this->direction = direction;
    this->j = j;
    this->k = k;
};

public:
int count = 0;
int j, k;
char letter;
Direction direction = Direction::STATIC;
void attack_up(char arena[9][28]);
void attack_up_right(char arena[9][28]);
void attack_right(char arena[9][28]);
void attack_down_right(char arena[9][28]);
void attack_down(char arena[9][28]);
void attack_down_left(char arena[9][28]);
void attack_left(char arena[9][28]);
void attack_up_left(char arena[9][28]);
void attack_switch();

static Missile * create_missile(Direction direction, int j, int k){
    return new Missile(direction, j, k);
}
};

void Bot::move_up(char arena[9][28]){ // j = row. k = column
     char upPos = arena[j-1][k];
     if (upPos != '-' && upPos != '#'){
        if (upPos == ' '){
            std::lock_guard<std::mutex> lock(mtx); 
            j--;
            arena[j][k] = letter;
            arena[j+1][k] = ' ';
        } 
        else if (upPos == '$'){
            semaphore.acquire();
            j--;
            arena[j][k] = '*';
            arena[j+1][k] = ' ';
            semaphore.release();
        }
     }
   
}
void Bot::move_down(char arena[9][28]){
    char downPos = arena[j+1][k];
    if (downPos != '-' && downPos != '#'){
        if (downPos == ' '){
            std::lock_guard<std::mutex> lock(mtx); 
            j++;
            arena[j][k] = letter; 
            arena[j-1][k] = ' ';
        }
        else if (downPos == '$'){
            semaphore.acquire();
            j++;
            arena[j][k] = '*'; 
            arena[j-1][k] = ' ';
            semaphore.release();
        }
    }
}
void Bot::move_left(char arena[9][28]){
    char leftPos = arena[j][k-1];
    if (leftPos != '|' && leftPos != '#'){
        if (leftPos == ' '){
            std::lock_guard<std::mutex> lock(mtx); 
            k--;
            arena[j][k] = letter;
            arena[j][k+1] = ' ';
        }
        else if (leftPos == '$'){
               semaphore.acquire();
               k--;
               arena[j][k] = '*'; 
               arena[j][k+1] = ' ';
               semaphore.release();
        }
    }
}
void Bot::move_right(char arena[9][28]){
     char rightPos = arena[j][k+1];
     char currPos = arena[j][k];
     if (rightPos != '|' && rightPos != '#'){
        if (rightPos == ' '){
            std::lock_guard<std::mutex> lock(mtx); 
            k++;
            arena[j][k] = letter;
            // if (currPos == '*'){
                arena[j][k-1] = ' ';
            // }
        }
        else if(rightPos == '$'){
              semaphore.acquire();
              k++;
              arena[j][k] = '*'; 
              arena[j][k-1] = ' ';
              semaphore.release();
        }
    }
}


void Projectile::attack_right(char arena[9][28]){

}



void Missile::attack_up(char arena[9][28]){ // j = row. k = column
        cout << "projectile count: " << count << endl;
        j--;
        if (count>0) {
            arena[j][k] = '%';
            arena[j+1][k] = ' ';
        } else if (count == 0){
            arena[j][k] = ' '; 
        } else {
            // do nothing
        }
}

void Missile::attack_up_right(char arena[9][28]){ // j = row. k = column
        cout << "projectile count: " << count << endl;
        j--; k++;
        if (count>0) {
            arena[j][k] = '%';
            arena[j+1][k-1] = ' ';
        } else if (count == 0){
            arena[j][k] = ' '; 
        } else {
            // do nothing
        }
}

void Missile::attack_right(char arena[9][28]){ // j = row. k = column
        cout << "projectile count: " << count << endl;
        k++;
        if (count>0) {
            arena[j][k] = '%';
            arena[j][k-1] = ' ';
        } else if (count == 0){
            arena[j][k] = ' '; 
        } else {
            // do nothing
        }
}

void Missile::attack_down_right(char arena[9][28]){ // j = row. k = column
        cout << "projectile count: " << count << endl;
        j++; k++;
        if (count>0) {
            arena[j][k] = '%';
            arena[j-1][k-1] = ' ';
        } else if (count == 0){
            arena[j][k] = ' '; 
        } else {
            // do nothing
        }
}

void Missile::attack_down(char arena[9][28]){ // j = row. k = column
        cout << "projectile count: " << count << endl;
        j++;
        if (count>0) {
            arena[j][k] = '%';
            arena[j-1][k] = ' ';
        } else if (count == 0){
            arena[j][k] = ' '; 
        } else {
            // do nothing
        }
}

void Missile::attack_down_left(char arena[9][28]){ // j = row. k = column
        cout << "projectile count: " << count << endl;
        j++; k--;
        if (count>0) {
            arena[j][k] = '%';
            arena[j-1][k+1] = ' ';
        } else if (count == 0){
            arena[j][k] = ' '; 
        } else {
            // do nothing
        }
}

void Missile::attack_left(char arena[9][28]){ // j = row. k = column
        cout << "projectile count: " << count << endl;
        k--;
        if (count>0) {
            arena[j][k] = '%';
            arena[j][k+1] = ' ';
        } else if (count == 0){
            arena[j][k] = ' '; 
        } else {
            // do nothing
        }
}

void Missile::attack_up_left(char arena[9][28]){ // j = row. k = column
        cout << "projectile count: " << count << endl;
        j--; k--;
        if (count>0) {
            arena[j][k] = '%';
            arena[j+1][k+1] = ' ';
        } else if (count == 0){
            arena[j][k] = ' '; 
        } else {
            // do nothing
        }
}



void Bot::move_switch(char arena[9][28]){
        switch(direction){
            case 0:
                    move_up(arena);
                break;
            case 1:
                    move_down(arena);
                break;
            case 2:
                    move_left(arena);
                break;
            case 3:
                    move_right(arena);
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

            auto socket_ptr = std::make_shared<tcp::socket>(io_context); // Use shared_ptr
            Bot::socket_ptrs.push_back(socket_ptr);

            std::cout << "awaiting connection...." << std::endl;
            acceptor.accept(*socket_ptr);  // Dereference shared pointer

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
            direction = Direction::UP;
        } else if (message.find('s') != std::string::npos) {
            direction = Direction::DOWN;
        } else if (message.find('a') != std::string::npos) {
            direction = Direction::LEFT;
        } else if (message.find('d') != std::string::npos) {
            direction = Direction::RIGHT;
        } else if (message.find('f') != std::string::npos) {
            cout << "f received" << endl;
            Projectile * proj = Projectile::create_projectile(direction, j, k);
            proj->count = 5;
            proj->attack_switch();
            projectiles.push_back(proj);
            direction = Direction::STATIC;
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

        //mtx.lock();
        obey_command(message); //alters bots //alters projectiles
        move_switch(arena); //alters arena
        //mtx.unlock();

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

    }
}

void Projectile::attack_switch(){
    switch(direction){
            case 0:
            j--;
                break;
            case 1:
            j++;
                break;
            case 2:
            k--;
                break;
            case 3:
            k++;
                break;
            default:
                break;
        }
} 

void delete_timed_out_projectiles(std::vector<Projectile*>& projectiles){

    auto condition = [](Projectile* projectile) {
        return projectile->count < -1;
    };

    auto it = std::remove_if(projectiles.begin(), projectiles.end(), [&](Projectile* projectile) {
        if (condition(projectile)) {
            delete projectile; 
            return true; 
        }
        return false;
    });

   
    projectiles.erase(it, projectiles.end());

}

void game_loop(){
    boost::system::error_code ignored_error;
   
    for(;;){
        std::this_thread::sleep_for(std::chrono::milliseconds(1300)); //300 //600
        
        for (int i = 0; i < 9; ++i) {
            std::cout << arena[i] << std::endl;
        }

        auto buffer = boost::asio::buffer(reinterpret_cast<char*>(arena), sizeof(arena));

        int num_sock_ptrs = static_cast<int>(Bot::socket_ptrs.size());

        int num_bots = bots.size();

       //  cout << num_bots << endl;

        int num_projectiles = projectiles.size();

        for (int i = 0; i < num_projectiles; i++){
            projectiles.at(i)->attack_right(arena);
            projectiles.at(i)->count--;
        }

        delete_timed_out_projectiles(projectiles);

        int num_projectiles2 = projectiles.size();

        cout << "the num projs after deleting timed out ones: " << num_projectiles2 << endl;


        for (int i = 0; i < num_sock_ptrs; i++){
            try{
                boost::asio::write(*(Bot::socket_ptrs.at(i)), buffer, ignored_error);
            }
            catch(const std::exception& e){
                std::cout << "Error occurred, but it's ignored: " << e.what() << std::endl;
            }
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
