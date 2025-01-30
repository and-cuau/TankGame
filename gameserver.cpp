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
void access_item(int & j, int & k);

// GLOBAL VARIABLES
std::mutex mtx;
// std::counting_semaphore<1> semaphore{1}; //max // initial
int tank_num;
const int ARENA_HEIGHT = 12; // 9/28 originally
const int ARENA_WIDTH = 29;

class Tank;
class Bomb;  // not forward declaring these caused weird error  error: request for member 'push_back' in 'missiles', which is of non-clas type 'int'424 |missiles.push_back(missile);
class Missile;
class Item;

std::vector<Tank*> tanks;
std::vector<Bomb*> bombs;
std::vector<Missile*> missiles;
std::vector<std::thread> threads;
std::vector<tcp::socket> sockets;
std::vector<Item*> items;

 // " N                         "
//"+-------------------------+"
//"                           "
char arena[ARENA_HEIGHT][ARENA_WIDTH] = {
    "                           ", //0
    "+-------------------------+", //1    
    "|                         |", //2
    "| A      #                |", //3
    "|  ! ###    B       ###   |", //4
    "|         ###             |", //5
    "|   $       #      C  #   |", //6
    "|                  $  #   |", //7
    "|       ###               |", //8
    "|  D      $      ##       |", //9
    "|                         |",
    "+-------------------------\n"
};

 int tank_positions[4][2] = {
        {3, 2}, // y, x
        {4, 12},
        {6, 19},
        {9, 3}
    };


int item_positions[4][2] = {
        {6, 4}, // y, x
        {7, 19},
        {9, 10}
};


char letters[4] = {'A', 'B', 'C', 'D'};

class Tank{

Tank(char letter1, int j, int k){
    letter = letter1;
    this->j = j;
    this->k = k;
};

public:
char letter;
int health = 7, score;
int j;
int k;
Direction direction = Direction::STATIC;
int turretDirection = 0;

void cycle_turret_left();
void cycle_turret_right();

void handle_msgs(std::shared_ptr<tcp::socket> socket_ptr, int connect_num);
void obey_command(string message);

void move_up();
void move_down();
void move_left();
void move_right();
void move_switch();

void attack_up();
void attack_down();
void attack_left();
void attack_right();


static std::vector<std::shared_ptr<boost::asio::ip::tcp::socket>> socket_ptrs;

void set_direction(){ 
     cout << "turret direction: " << turretDirection << endl;
     switch(turretDirection){
            case 0:
            arena[0][0] = 'N'; arena[0][1] = ' ';
                break;
            case 1:
            arena[0][0] = 'N'; arena[0][1] = 'E';
                break;
            case 2:
            arena[0][0] = 'E'; arena[0][1] = ' ';
                break;
            case 3:
            arena[0][0] = 'S'; arena[0][1] = 'E';
                break;
            case 4:
            arena[0][0] = 'S'; arena[0][1] = ' ';
                break;
            case 5:
            arena[0][0] = 'S'; arena[0][1] = 'W';
                break;
            case 6:
            arena[0][0] = 'W'; arena[0][1] = ' ';
                break;
            case 7:
            arena[0][0] = 'N'; arena[0][1] = 'W';
                break;
            default:
                break;
        }
}

void set_letter(char letter){
}

void create_UI(){
    const int livesPos =  5;
    for (int i = 0; i < health; i++){
        arena[0][livesPos + i] = 'H';
    }
    set_direction();
}


static Tank * create_tank(char letter, int j, int k){
    return new Tank(letter, j, k);
}

};  // END OF CLASS // END OF CLASS // END OF CLASS // END OF CLASS // END OF CLASS // END OF CLASS

// Tank* findTankByLetter(char search_letter) {
//     auto it = std::find_if(tanks.begin(), tanks.end(),
//                            [search_letter](const Tank& t) { return t.letter == search_letter; });

//     if (it != tanks.end()) {
//         return &(*it);  // Return a pointer to the found Tank
//     }

//     return nullptr;  // Return nullptr if not found
// }


int charToInt(char c) {
    return std::toupper(c) - 'A';  // Convert to uppercase and map A = 1, B = 2, ...
}


class Bomb {

Bomb(Direction direction, int j, int k){
    this->direction = direction;
    this->j = j;
    this->k = k;
};

public:
int count = 0;
int j, k;
char letter;
Direction direction = Direction::STATIC;
void attack_up();
void attack_down();
void attack_left();
void attack_right();
void attack_right2();
void attack_switch();

static Bomb * create_bomb(Direction direction, int j, int k){
    return new Bomb(direction, j, k);
}
};

class Missile {

Missile(int direction, int j, int k){
    this->direction = direction;
    this->j = j;
    this->k = k;
};

public:
int count = 0;
int j, k;
int direction;
char letter;
// Direction direction = Direction::STATIC;
void attack_up();
void attack_up_right();
void attack_right();
void attack_down_right();
void attack_down();
void attack_down_left();
void attack_left();
void attack_up_left();
void attack_switch();

static Missile * create_missile(int direction, int j, int k){
    return new Missile(direction, j, k);
}
};


class Item {

Item(int j, int k){
    this->j = j;
    this->k = k;
};

public:
int j, k;
std::counting_semaphore<2> semaphore{0};

static Item * create_item(int j, int k){
    return new Item(j, k);
}


void blink();
void unblink();

};


void Item::blink(){
    arena[j][k] = '$';
}

void Item::unblink(){
    arena[j][k] = '-';
}


void Tank::move_up(){ // j = row. k = column
     char upPos = arena[j-1][k];
     if (upPos != '-' && upPos != '#'){
        if (upPos == ' '){
            std::lock_guard<std::mutex> lock(mtx); 
            j--;
            arena[j][k] = letter;
            arena[j+1][k] = ' ';
        } 
        else if (upPos == '$'){
           access_item(j, k);
        }
     }
}

void access_item(int & j, int & k){
    int num_items = items.size();

    int jmm = j;

    for (int i = 0; i < num_items; i++){
        if (jmm == items.at(i)->j && k == items.at(i)->k){
            items.at(i)->semaphore.acquire();

            j--;
            arena[j][k] = '*';
            arena[j+1][k] = ' ';

            items.at(i)->semaphore.release();
        }

    }
}


void Tank::move_down(){
    char downPos = arena[j+1][k];
    char oldPos =arena[j][k];
    if (downPos != '-' && downPos != '#'){
        if (downPos == ' '){
            std::lock_guard<std::mutex> lock(mtx); 
            j++;
            arena[j][k] = letter; 
            arena[j-1][k] = ' ';
        }
        else if (downPos == '$'){
            access_item(j,k);
        } else if ( downPos == 'L') {
            //semaphore.acquire();
            j++;
            arena[j][k] = '!'; 
            arena[j-1][k] = ' ';
            //semaphore.release();
        }
    }
}
void Tank::move_left(){
    char leftPos = arena[j][k-1];
    if (leftPos != '|' && leftPos != '#'){
        if (leftPos == ' '){
            std::lock_guard<std::mutex> lock(mtx); 
            k--;
            arena[j][k] = letter;
            arena[j][k+1] = ' ';
        }
        else if (leftPos == '$'){
            access_item(j,k);
        }
    }
}
void Tank::move_right(){
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
              access_item(j,k);
        }
    }
}

void Tank::cycle_turret_right(){
    turretDirection = (turretDirection + 1) % 8;
    Tank::set_direction();
}

void Tank::cycle_turret_left(){
    turretDirection = (turretDirection - 1 + 8) % 8;
    Tank::set_direction();
}

void Bomb::attack_right(){
    if (count > 3){
        arena[j][k] = '*';
    } else if (count > 2){
       if (std::isalpha(arena[j-1][k])) { // Up
    try {
        tanks.at(charToInt(arena[j-1][k]))->health--;
    } catch (const std::out_of_range& e) {
        // Handle the out-of-bounds access, e.g., log the error or do nothing
        std::cout << "Index out of bounds: " << e.what() << std::endl;
    }
} else {
    arena[j-1][k] = '%';
}

if (std::isalpha(arena[j-1][k+1])) { // UpRight
    try {
        tanks.at(charToInt(arena[j-1][k+1]))->health--;
    } catch (const std::out_of_range& e) {
        std::cout << "Index out of bounds: " << e.what() << std::endl;
    }
} else {
    arena[j-1][k+1] = '%';
}

if (std::isalpha(arena[j][k+1])) { // Right
    try {
        tanks.at(charToInt(arena[j][k+1]))->health--;
    } catch (const std::out_of_range& e) {
        std::cout << "Index out of bounds: " << e.what() << std::endl;
    }
} else {
    arena[j][k+1] = '%';
}

if (std::isalpha(arena[j+1][k+1])) { // DownRight
    try {
        tanks.at(charToInt(arena[j+1][k+1]))->health--;
    } catch (const std::out_of_range& e) {
        std::cout << "Index out of bounds: " << e.what() << std::endl;
    }
} else {
    arena[j+1][k+1] = '%';
}

if (std::isalpha(arena[j+1][k])) { // Down
    try {
        tanks.at(charToInt(arena[j+1][k]))->health--;
    } catch (const std::out_of_range& e) {
        std::cout << "Index out of bounds: " << e.what() << std::endl;
    }
} else {
    arena[j+1][k] = '%';
}

if (std::isalpha(arena[j+1][k-1])) { // DownLeft
    try {
        tanks.at(charToInt(arena[j+1][k-1]))->health--;
    } catch (const std::out_of_range& e) {
        std::cout << "Index out of bounds: " << e.what() << std::endl;
    }
} else {
    arena[j+1][k-1] = '%';
}

if (std::isalpha(arena[j][k-1])) { // Left
    try {
        tanks.at(charToInt(arena[j][k-1]))->health--;
    } catch (const std::out_of_range& e) {
        std::cout << "Index out of bounds: " << e.what() << std::endl;
    }
} else {
    arena[j][k-1] = '%';
}

if (std::isalpha(arena[j-1][k-1])) { // UpLeft
    try {
        tanks.at(charToInt(arena[j-1][k-1]))->health--;
    } catch (const std::out_of_range& e) {
        std::cout << "Index out of bounds: " << e.what() << std::endl;
    }
} else {
    arena[j-1][k-1] = '%';
}



    }
    else if (count > 1){
         arena[j-1][k] = ' ';
        arena[j-1][k+1] = ' ';
        arena[j][k+1] = ' ';
        arena[j+1][k+1] = ' ';
        arena[j+1][k] = ' ';
        arena[j+1][k-1] = ' ';
        arena[j][k-1] = ' ';
        arena[j-1][k-1] = ' ';
    }

}

char check_clear(int j, int k){
int num_tanks = tanks.size();
    for (int i = 0; i < num_tanks; i++){
        if (tanks.at(i)->j == j && tanks.at(i)->k == k){
            return tanks.at(i)->letter;
        } 
    }
    return 'X';
}

void Missile::attack_up(){ // j = row. k = column   // 0
        cout << "bomb count: " << count << endl;

         if (count>0) {
            
            if (std::isalpha(arena[j-1][k])) {
                count = -1;
                arena[j][k] = ' ';
                tanks.at(charToInt(arena[j-1][k]))->health--;
            }
            else if (arena[j-1][k] == '-' || arena[j-1][k] == '#'){
                direction = 4;
            } 
            else {
                j--;
                arena[j][k] = '*';
                 if (count < 25){
                    arena[j+1][k] = ' ';
                 }
            }

        } else if (count < 1){
            arena[j][k] = ' '; 
        } else {
            // do nothing
        }

}

void Missile::attack_up_right(){ // j = row. k = column  // 1
        cout << "bomb count: " << count << endl;

        if (count>0) {
            
            if (std::isalpha(arena[j-1][k+1])) {
                count = -1;
                arena[j][k] = ' ';
                tanks.at(charToInt(arena[j-1][k+1]))->health--;
            } 
            else if (arena[j-1][k] == '-' || arena[j-1][k] == '#'){  // hits horizontal 
                direction = 3;
            }
            else if (arena[j-1][k] == ' ' && arena[j-1][k+1] == '#'){  // hits corner dead on
                direction = 5;
            }
             else if (arena[j-1][k+1] == '|' || arena[j-1][k+1] == '#'){  // hits vertical
                direction = 7;
            }
            else {
                 j--; k++;
                 arena[j][k] = '*';
                 if (count < 25){
                    arena[j+1][k-1] = ' ';
                 }
            }

        } else if (count < 1){
            arena[j][k] = ' '; 
        } else {
            // do nothing
        }
       
}

void Missile::attack_right(){ // j = row. k = column    // 2
        cout << "bomb count: " << count << endl;
        
        if (count>0) {
            if (std::isalpha(arena[j][k+1])) {
                count = -1;
                arena[j][k] = ' ';
                tanks.at(charToInt(arena[j][k+1]))->health--;
            } 
            else if (arena[j][k+1] == '|' || arena[j][k+1] == '#'){
                direction = 6;
            } else {
                 k++;
                 arena[j][k] = '*';
                 if (count < 25){
                    arena[j][k-1] = ' ';
                 }
            }
        } else if (count < 1){
            arena[j][k] = ' '; 
        } else {
            // do nothing
        }
}

void Missile::attack_down_right(){ // j = row. k = column // 3
        cout << "DOWN RIGHT " << count << endl;
        
        if (count>0) {
            cout <<"next space: "<< arena[j+1][k+1] << endl;
            
            if (std::isalpha(arena[j+1][k+1])) {
                count = -1;
                arena[j][k] = ' ';
                tanks.at(charToInt(arena[j+1][k+1]))->health--;
            } 
            else if (arena[j+1][k] == '-' || arena[j+1][k] == '#'){
                direction = 1;
            } else if (arena[j+1][k] == ' ' && arena[j+1][k+1] == '#'){
                direction = 7;
            }
            else if (arena[j+1][k+1] == '|' || arena[j+1][k+1] == '#'){
                direction = 5;
            } else {
                j++; k++;
                arena[j][k] = '*';
                 if (count < 25){
                    arena[j-1][k-1] = ' ';
                 }
            }

        } else if (count < 1){
            arena[j][k] = ' '; 
        } else {
            // do nothing
        }
}


void Missile::attack_down(){ // j = row. k = column // 4
        cout << "bomb count: " << count << endl;
        
        if (count>0) {
            
            if (std::isalpha(arena[j+1][k])) {
                count = -1;
                arena[j][k] = ' ';
                tanks.at(charToInt(arena[j+1][k]))->health--;
            } 
            else if (arena[j+1][k] == '-' || arena[j+1][k] == '#'){
                direction = 0;
            } else {
                j++;
                arena[j][k] = '*';
                 if (count < 25){
                    arena[j-1][k] = ' ';
                 }
            }

        } else if (count < 1){
            arena[j][k] = ' '; 
        } else {
            // do nothing
        }
}

void Missile::attack_down_left(){ // j = row. k = column // 5
        cout << "bomb count: " << count << endl;
        
        if (count>0) {
            if (std::isalpha(arena[j+1][k-1])) {
                count = -1;
                arena[j][k] = ' ';
                tanks.at(charToInt(arena[j+1][k-1]))->health--;
            } 
            else if (arena[j+1][k-1] == '-' || arena[j+1][k-1] == '#'){
                direction = 7;
            } 
            else if (arena[j+1][k-1] == '#' && arena[j+1][k] == ' '){ // hits corner dead on
                direction = 1;
            }            
            else if (arena[j+1][k-1] == '|' || arena[j+1][k-1] == '#'){
                direction = 3;
            } else {
                j++; k--;
                arena[j][k] = '*';
                 if (count < 25){
                arena[j-1][k+1] = ' ';
                 }
            }

        } else if (count < 1){
            arena[j][k] = ' '; 
        } else {
            // do nothing
        }
}

void Missile::attack_left(){ // j = row. k = column // 6
        cout << "bomb count: " << count << endl;
       
        if (count>0) {
            if (std::isalpha(arena[j][k-1])) {
                count = -1;
                arena[j][k] = ' ';
                tanks.at(charToInt(arena[j][k-1]))->health--;
            } 
            else if (arena[j][k-1] == '|' || arena[j][k-1] == '#'){
                direction = 2;
            } else{
                k--;
                arena[j][k] = '*';
                if (count < 25){
                    arena[j][k+1] = ' ';
                }
            }

        } else if (count < 1){
            arena[j][k] = ' '; 
        } else {
            // do nothing
        }
}

void Missile::attack_up_left(){ // j = row. k = column // 7
        cout << "bomb count: " << count << endl;
        
        if (count>0) {

            if (std::isalpha(arena[j-1][k-1])) {
                count = -1;
                arena[j][k] = ' ';
                tanks.at(charToInt(arena[j-1][k-1]))->health--;
            }
            else if (arena[j-1][k] == '-' || arena[j-1][k] == '#'){
                direction = 5;
            } else if (arena[j-1][k] == ' ' && arena[j-1][k-1] == '#'){    // hits corner dead on
                direction = 3;
            }
            else if (arena[j-1][k-1] == '|' || arena[j-1][k-1] == '#'){
                direction = 1;
            } else {
                j--; k--;
                arena[j][k] = '*';
                if (count < 25){
                arena[j+1][k+1] = ' ';
                }
            }
        } else if (count < 1){
            arena[j][k] = ' '; 
        } else {
            // do nothing
        }
}


void Tank::move_switch(){ // 8
        cout << "direction: " << direction << endl;
        switch(direction){
            case 0:
                    move_up();
                break;
            case 1:
                    move_down();
                break;
            case 2:
                    move_left();
                break;
            case 3:
                    move_right();
                break;
            default:
                    // cout << "test static" << endl;
                    // arena[j][k] = letter;
                break;
        }
}

std::vector<std::shared_ptr<boost::asio::ip::tcp::socket>> Tank::socket_ptrs;

void testConnection(){ 
    boost::system::error_code ignored_error;
    constexpr unsigned short PORT = 12345;
    try{
        boost::asio::io_context io_context;
        // A tcp::acceptor object to listen for new connections. Set to listen on TCP port 12345, IP version 4.
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), PORT));
        int connect_num = 0;
        for(;;){
            int j = tank_positions[connect_num][0];
            int k = tank_positions[connect_num ][1];
            char letter = letters[connect_num];

            auto socket_ptr = std::make_shared<tcp::socket>(io_context); // Use shared_ptr
            Tank::socket_ptrs.push_back(socket_ptr);

            std::cout << "awaiting connection...." << std::endl;
            acceptor.accept(*socket_ptr);  // Dereference shared pointer

            Tank * tank = Tank::create_tank(letter, j, k);
            tanks.push_back(tank);

            threads.push_back(std::thread([tank, socket_ptr, connect_num]() { // new method. uncomment after tests
               tank->handle_msgs(socket_ptr, connect_num);
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

void Tank::obey_command(string message){
         if (message.find('w') != std::string::npos) {
            Missile * missile = Missile::create_missile(turretDirection, j, k);
            missiles.push_back(missile);
            missile->count = 25;
            direction = Direction::STATIC;
        } else if (message.find('s') != std::string::npos) {
            //
        } else if (message.find('a') != std::string::npos) {
            this->cycle_turret_left();
            direction = Direction::STATIC;
        } else if (message.find('d') != std::string::npos) {
            cout << "correct shit being executed too" << endl;
            this->cycle_turret_right();
            direction = Direction::STATIC;
        } else if (message.find('f') != std::string::npos) {
            cout << "f received" << endl;
            Bomb * proj = Bomb::create_bomb(direction, j, k);
            proj->count = 10;
            proj->attack_switch();
            bombs.push_back(proj);
            direction = Direction::STATIC;
        } else {
            int msg_int = stoi(message);
            if (msg_int == 72) {
               // Tank::move_up(arena);
               direction = Direction::UP;
            } else if (msg_int == 80) {
                direction = Direction::DOWN;
               // Tank::move_down(arena);
            } else if (msg_int == 75) {
                direction = Direction::LEFT;
               // Tank::move_left(arena);
            } else if (msg_int == 77) {
                cout << "this shit happend idk why" << endl;
                direction = Direction::RIGHT;
               // Tank::move_right(arena);
            } else if (msg_int == 32){
                cout << "space received" << endl;
            }
        }
}

void Tank::handle_msgs(std::shared_ptr<tcp::socket> socket_ptr, int connect_num) {  
    boost::system::error_code ignored_error;
    
    for(;;){
        std::string message(30, '\0'); // 1024
        boost::system::error_code error;
        socket_ptr->receive(boost::asio::buffer(message), 0, error);  
    
        std::cout << "TEST SERVER-SIDE MESSAGE: " << message << std::endl;

        //mtx.lock();
        obey_command(message); //alters tanks //alters bombs
        move_switch(); //alters arena
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

        if (message.find("end") != std::string::npos || health < 1) {
            break;
        }

    }
    socket_ptr->close();
}

void Bomb::attack_switch(){
    switch(direction){
            case 0:
            j++;
                break;
            case 1:
            j--;
                break;
            case 2:
            k++;
                break;
            case 3:
            k--;
                break;
            default:
                break;
        }
} 

void Missile::attack_switch(){
    switch(direction){
            case 0:
            attack_up();
                break;
            case 1:
            attack_up_right();
                break;
            case 2:
            attack_right();
                break;
            case 3:
            attack_down_right();
                break;
            case 4:
            attack_down();
                break;
            case 5:
            attack_down_left();
                break;
            case 6:
            attack_left();
                break;
            case 7:
            attack_up_left();
                break;
            default:
                break;
        }
} 

void delete_timed_out_bombs(std::vector<Bomb*>& bombs){
    auto condition = [](Bomb* bomb) {
        return bomb->count < 1;
    };

    auto it = std::remove_if(bombs.begin(), bombs.end(), [&](Bomb* bomb) {
        if (condition(bomb)) {
            delete bomb; 
            return true; 
        }
        return false;
    });

   
    bombs.erase(it, bombs.end());

}


void delete_timed_out_missiles(std::vector<Missile*>& missiles){

    auto condition = [](Missile* missile) {
        return missile->count < 0;
    };

    auto it = std::remove_if(missiles.begin(), missiles.end(), [&](Missile* missile) {
        if (condition(missile)) {
            delete missile; 
            return true; 
        }
        return false;
    });

    missiles.erase(it, missiles.end());

}

void delete_dead_tanks(std::vector<Tank*>& tanks){

    auto condition = [](Tank* tank) {
        return tank->health < 1;
    };

    auto it = std::remove_if(tanks.begin(), tanks.end(), [&](Tank* tank) {
        if (condition(tank)) {
            arena[tank->j][tank->k] = 'X';
            delete tank; 
            return true; 
        }
        return false;
    });

    tanks.erase(it, tanks.end());

}



void test_semaphore(){
    for(;;){
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        //semaphore.acquire();
        int num_items = items.size();

        for (int i = 0; i < num_items; i++){
            items.at(i)->blink();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        
        for (int i = 0; i < num_items; i++){
            items.at(i)->unblink();
        }
        //semaphore.release();
    }
}

void game_loop(){
    boost::system::error_code ignored_error;
   
    for(;;){
        std::this_thread::sleep_for(std::chrono::milliseconds(800)); // was 1000
        for (int i = 0; i < ARENA_HEIGHT; ++i) {
            std::cout << arena[i] << std::endl;
        }

        auto buffer = boost::asio::buffer(reinterpret_cast<char*>(arena), sizeof(arena));

        int num_sock_ptrs = static_cast<int>(Tank::socket_ptrs.size());

        int num_tanks = tanks.size();

       //  cout << num_tanks << endl;

        int num_bombs = bombs.size();

        for (int i = 0; i < num_bombs; i++){
            bombs.at(i)->count--;
            bombs.at(i)->attack_right();
        }

        int num_missiles = missiles.size();

        for (int i = 0; i < num_missiles; i++){
            missiles.at(i)->attack_switch();
            // for (int j = 0; j < num_tanks; j++){
            //     if (missiles.at(i)->j == tanks.at(j)->j && missiles.at(i)->k == tanks.at(j)->k){
            //         tanks.at(i)->health--;
            //         //arena[tanks.at(j)->j][tanks.at(j)->k] = '%';
            //     }
            // }
            missiles.at(i)->count--;
        }

        delete_timed_out_bombs(bombs);
        delete_timed_out_missiles(missiles);

        int num_bombs2 = bombs.size();

        cout << "the num projs after deleting timed out ones: " << num_bombs2 << endl;


        for (int i = 0; i < num_sock_ptrs; i++){
            try{

                tanks.at(i)->create_UI();
                
                boost::asio::write(*(Tank::socket_ptrs.at(i)), buffer, ignored_error);

                std::fill(arena[0], arena[0] + ARENA_WIDTH - 1, ' ');
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


    int num_positions = sizeof(item_positions) / sizeof(item_positions[0]);
    
    for (int i = 0; i < num_positions; i++){
        Item * item = Item::create_item(item_positions[i][0], item_positions[i][1]);
        items.push_back(item);
    }

    thread game_thread(game_loop);

    thread test_sem(test_semaphore);
   
    testConnection();

    test_sem.join();
    game_thread.join();

    return 0; 
}
