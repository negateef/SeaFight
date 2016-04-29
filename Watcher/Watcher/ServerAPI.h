//
//  ServerAPI.h
//  Watcher
//
//  Created by Misha Babenko on 4/29/16.
//  Copyright © 2016 Misha Babenko. All rights reserved.
//

#ifndef ServerAPI_h
#define ServerAPI_h


#include "Ship.h"
#include "Field.h"
#include <cstdlib>
#include <thread>
#include <chrono>
#include <string>
#include <iostream>

#include <SFML/Network.hpp>

#include "Server.h"

class ServerAPI {
private:
    sf::UdpSocket socket_;
    sf::IpAddress kServerAddress = "127.0.0.1";
    
    ServerAPI() {}
    
public:
    static ServerAPI &GetInstance() {
        static ServerAPI instance;
        return instance;
    }
    
    ServerAPI &operator =(const ServerAPI &) = delete;
    ServerAPI(const Server &) = delete;
    
    void Init() {
        if (socket_.bind(Server::kClientPort) != sf::Socket::Done) {
            std::cerr << "Failed to bind to port " << Server::kClientPort << std::endl;
            return;
        }
    }

    template <class Callback> 
    void GetFields(Callback got_fields) {
        sf::IpAddress ip;
        sf::Packet packet;
        packet << Server::kGetFieldsRequest;
        sf::Packet receive_packet;
        unsigned short port;
        socket_.send(packet, kServerAddress, Server::kServerPort);
        socket_.receive(receive_packet, ip, port);
        Field field1, field2;
        receive_packet >> field1 >> field2;
        got_fields(field1, field2);
    }
    
};


#endif /* ServerAPI_h */
