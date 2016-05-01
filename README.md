# SeaFight
Server-Client Sea fight game written in C++

## Compilation

To compile using XCode you need to copy SFML/Frameworks and SFML/extlibs to /Library/Frameworks and set header and framework search paths in Build settings.


You can also manually compile everything using gcc.
First of all copy frameworks SFML/Frameworks and SFML/extlibs to your default framework folder.
Then run following commands:

### Server
g++ -Ipath_to_sfml/SFML/include/ -framework sfml-network -framework sfml-graphics -framework sfml-window -framework sfml-system -std=c++11 main.cpp Server.cpp -oserver

### Client
g++ -Ipath_to_sfml/SFML/include/ -framework sfml-network -framework sfml-graphics -framework sfml-window -framework sfml-system -std=c++11 main.cpp Server.cpp -oclient


### Watcher
g++ -Ipath_to_sfml/SFML/include/ -framework sfml-network -framework sfml-graphics -framework sfml-window -framework sfml-system -std=c++11 main.cpp Server.cpp -owatcher


## Running the game

Note that first of all you need to run a server program. Then, need to run two clients passing the server's ip address as the argument.

It will look like this:

./server               (on computer #1)
./client 192.168.0.1   (on computer #2)
./client 192.168.0.1   (on computer #3)
./watcher 192.168.0.1  (on computer #4)

You can run both server and client and watcher on the same machine. You can't only run both clients on the same computer.


## Placing the ships

Now you can only set your ships through the text file "field.txt". The structure of this file is as follows: 10 rows, each representing a ship. Each row has row and column of the head of ship (the top-left corner), length, and position (horizontal or vertical). You can look at Client/Client/field.txt to get a better understanding/


## Other notes

The game has now a lot of bugs and can't pretend to be an example of REALLY working game. Still it's a nice starting point to build more reliable game.

When something goes wrong, just rerun the game :) 
