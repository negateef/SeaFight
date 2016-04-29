//
//  Server.h
//  SeaFight
//
//  Created by Misha Babenko on 4/20/16.
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
    void MakeMove(const Position position, Callback got_status) {
        sf::IpAddress ip;
        sf::Packet packet;
        packet << Server::kDoMoveRequest;
        packet << position.row << position.col;
        sf::Packet receive_packet;
        unsigned short port;
        socket_.send(packet, kServerAddress, Server::kServerPort);
        socket_.receive(receive_packet, ip, port);
        
        ServerMoveStatus status(ServerMoveStatus::Code::kError, {-1, -1});
        receive_packet >> status.position.row >> status.position.col;
        int raw_code;
        receive_packet >> raw_code;
        status.code = static_cast<ServerMoveStatus::Code>(raw_code);
        got_status(status);
    }
    
    template <class Callback> 
    void GetEnemyMove(Callback got_move) {
        sf::IpAddress ip;
        sf::Packet packet;
        packet << Server::kGetMoveRequest;
        sf::Packet receive_packet;
        int row, col;
        unsigned short port;
        while (true) {
            socket_.send(packet, kServerAddress, Server::kServerPort);
            socket_.receive(receive_packet, ip, port);
            std::string status;
            receive_packet >> status;
            if (status == Server::kFailedResponse) {
                continue;
            } else {
                receive_packet >> row >> col;
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        got_move(Position(row, col));
    }

    enum class StartGameStatus {
        kOk,
        kFailed
    };
    template<class Callback>
    void StartGame(const Field &field, Callback got_status) {
        sf::IpAddress ip;
        sf::Packet packet;
        packet << Server::kInitRequest;
        packet << field;
        sf::Packet receive_packet;
        unsigned short port;
        socket_.send(packet, kServerAddress, Server::kServerPort);
        socket_.receive(receive_packet, ip, port);
        std::string status;
        receive_packet >> status;
        if (status == Server::kSuccessResponse) {
            got_status(StartGameStatus::kOk);
        } else {
            got_status(StartGameStatus::kFailed);
        }
    }

    enum class GameStatus {
        kYourTurn,
        kEnemyTurn,
        kFinished
    };

    template<class Callback>
    void GetGameStatus(Callback got_status) {
        sf::IpAddress ip;
        sf::Packet packet;
        packet << Server::kGetTurnOrder;
        sf::Packet receive_packet;
        unsigned short port;
        socket_.send(packet, kServerAddress, Server::kServerPort);
        socket_.receive(receive_packet, ip, port);
        Server::SendGameStatus game_status;
        receive_packet >> game_status;
        
        if (game_status == Server::SendGameStatus::FinishedOrNotStarted) {
            got_status(GameStatus::kFinished);
        }
        if (game_status == Server::SendGameStatus::YourTurn) {
            got_status(GameStatus::kYourTurn);
        }
        
        if (game_status == Server::SendGameStatus::EnemyTurn) {
            got_status(GameStatus::kEnemyTurn);
        }
    }
};

#endif /* ServerAPI_h */
