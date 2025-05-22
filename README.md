# LAN Tanks Game 
Tanks in C++. Use arrow keys to move tank up, down, left, right. Press 'A' and 'D' keys to rotate turret. Press 'W' key to fire missile. Press 'F' key to drop bomb. Objective is to eliminate enemy players.  Designed for demonstration purposes, the program currently supports multiplayer mode on a single computer.


## Technologies Used  
- **Backend**: C++, Boost.Asio (TCP Sockets)


## Prerequisites
- C++ compiler (gcc, g++)
- Windows


# Installation
## Download Boost
1. Visit the Boost website: Go to the [Boost Downloads page](https://www.boost.org/users/download/).
2. Download "boost_1_86_0.zip" zip file for Windows.
3. Extract the files: Unzip the downloaded file to your desired location.


1. Navigate to the Boost directory:
   ```bash
   cd path/to/boost

2. Run bootstrap.bat:
   ```bash
   ./bootstrap.bat

2. Run b2.exe:
   ```bash
   ./b2.exe


## Download and build GitHub files
1. Clone the repository:
   ```bash
   git clone https://github.com/and-cuau/TankGame.git
   
2. Navigate into the project directory:
    ```bash
   cd directory

3. Build gameserver.cpp:
   ```bash
   g++ -std=c++20 -I"path\to\boost" gameserver.cpp -o gameserver -L"path\to\boost\stage\lib" -lstdc++ "path\to\boost\stage\lib\libboost_system-vc143-mt-x64-1_86.lib" -lws2_32

6. Build gameclient.cpp:
   ```bash
   g++ -std=c++20 -I"path\to\boost" gameclient.cpp -o gameclient -L"path\to\boost\stage\lib" -lstdc++ "path\to\boost\stage\lib\libboost_system-vc143-mt-x64-1_86.lib" -lws2_32

# Play game

## Play multiplayer

1. Run the gameserver object file:
      ```bash
   ./gameserver
      
2. Open another terminal.

3. Run the gameclient object file:
      ```bash
   ./gameclient

4. You are now connected as player 'A'. Repeat steps 2-3 for each additional player. Each additional player connected will be assigned the following letter of the alphabet (e.g., player 'B', player 'C', etc.).



