//
//  Server.cpp
//  Server
//
//  Created by Misha Babenko on 4/28/16.
//  Copyright © 2016 Misha Babenko. All rights reserved.
//

#include "Server.h"
#include <iostream>
#include <string>
#include <vector>
#include <SFML/Network.hpp>
#include "Field.h"
#include "Ship.h"

#include <chrono>
#include <thread>

sf::Packet& operator << (sf::Packet &packet, const Field &field) {
    for (int i = 0; i < Field::kSize; ++i)
        for (int j = 0; j < Field::kSize; ++j) {
            packet << static_cast<int>(field.GetCellType(i, j));
        }
    packet << static_cast<int>(field.GetShips().size());
    for (auto ship : field.GetShips()) {
        packet << ship.length;
        packet << ship.position.row;
        packet << ship.position.col;
        packet << ship.horizontal;
    }
    return packet;
}

sf::Packet& operator >> (sf::Packet &packet, Field &field) {
    for (int i = 0; i < Field::kSize; ++i)
        for (int j = 0; j < Field::kSize; ++j) {
            int type;
            packet >> type;
            field.SetCellType(i, j, static_cast<Field::CellType>(type));
        }
    int n_ships;
    packet >> n_ships;
    for (int i = 0; i < n_ships; ++i) {
        int length, row, col;
        bool horizontal;
        packet >> length >> row >> col >> horizontal;
        field.AddShip({length, {row, col}, horizontal});
    }
    return packet;
}

sf::Packet& operator << (sf::Packet &packet, const Server::SendGameStatus &status) {
    packet << static_cast<int>(status);
    return packet;
}

sf::Packet& operator >> (sf::Packet &packet, Server::SendGameStatus &status) {
    int status_int;
    packet >> status_int;
    status = static_cast<Server::SendGameStatus>(status_int);
    return packet;
}


void Server::InitGame(sf::Packet &packet, const sf::IpAddress &sender, unsigned short port) {
    sf::Packet result_packet;
    if (fields_.size() == 2) {
        result_packet << kFailedResponse;
        socket_.send(result_packet, sender, port);
    } else {
        Field field;
        packet >> field;
        fields_.push_back(field);
        players_ips_.push_back(sender);
        result_packet << kSuccessResponse;
        
        if (fields_.size() == 2) {
            game_status_ = GameStatus::FirstPlayerTurn;
            for (auto ip : players_ips_) {
                socket_.send(result_packet, ip, port);
            }
        }
    }
}

void Server::GetTurnOrder(const sf::IpAddress &sender, unsigned short port) {
    static int players_cnt = 0;
    ++players_cnt;
    if (players_cnt == 2) {
        sf::Packet your_turn;
        your_turn << Server::SendGameStatus::YourTurn;
        sf::Packet enemy_turn;
        enemy_turn << Server::SendGameStatus::EnemyTurn;
        
        socket_.send(your_turn, players_ips_[0], kClientPort);
        socket_.send(enemy_turn, players_ips_[1], kClientPort);
    }
}

void Server::GetMove(const sf::IpAddress &sender, unsigned short port) {
    if (player_ip_move_.empty() || (!player_ip_move_.empty() && sender == player_ip_move_.back())) {
        sf::Packet reject_packet;
        reject_packet << kFailedResponse;
        socket_.send(reject_packet, sender, port);
    } else {
        sf::Packet result_packet;
        result_packet << kSuccessResponse;
        result_packet << moves_.back().row << moves_.back().col;
        socket_.send(result_packet, sender, port);
    }
}

void Server::MakeMove(sf::Packet &packet, const sf::IpAddress &sender, unsigned short port) {
    int row, col;
    packet >> row >> col;
    moves_.push_back({row, col});
    player_ip_move_.push_back(sender);
    sf::Packet result_packet;
    if (game_status_ == Server::GameStatus::FirstPlayerTurn) {
        ServerMoveStatus::Code code = fields_[1].Shoot({row, col});
        result_packet << row << col;
        result_packet << static_cast<int>(code);
        socket_.send(result_packet, sender, port);
        if (code != ServerMoveStatus::Code::kKill && code != ServerMoveStatus::Code::kHurt) {
            game_status_ = Server::GameStatus::SecondPlayerTurn;
        }
    } else { 
        ServerMoveStatus::Code code = fields_[0].Shoot({row, col});
        result_packet << row << col;
        result_packet << static_cast<int>(code);
        socket_.send(result_packet, sender, port);
        if (code != ServerMoveStatus::Code::kKill && code != ServerMoveStatus::Code::kHurt) {
            game_status_ = Server::GameStatus::FirstPlayerTurn;
        }
    }
}

void Server::GetFields(const sf::IpAddress &sender, unsigned short port) {
    sf::Packet result_packet;
    result_packet << fields_[0];
    result_packet << fields_[1];
    socket_.send(result_packet, sender, port);
}

Server &Server::GetInstance() {
    static Server server;
    return server;
}

Server::Server() {}

void Server::Init() {
    if (socket_.bind(kServerPort) != sf::Socket::Done) {
        std::cerr << "Failed to bind to port " << kServerPort << std::endl;
        return;
    }
    game_status_ = GameStatus::FinishedOrNotStarted;
    while (true) {
        sf::Packet packet; 
        sf::IpAddress sender;
        unsigned short client_port;
        socket_.receive(packet, sender, client_port);
        
        std::string query_type;
        packet >> query_type;
        
        if (query_type == kInitRequest) {
            InitGame(packet, sender, client_port);
        }
        
        if (query_type == kGetTurnOrder) {
            GetTurnOrder(sender, client_port);
        }
        
        if (query_type == kGetMoveRequest) {
            GetMove(sender, client_port);
        }
        
        if (query_type == kDoMoveRequest) {
            MakeMove(packet, sender, client_port);
        }
        
        if (query_type == kGetFieldsRequest) {
            GetFields(sender, client_port);
        }

    }
    
}

const unsigned short Server::kServerPort = 54000;
const unsigned short Server::kClientPort = 54001;
const unsigned short Server::kWatcherPort = 54002;

const std::string Server::kInitRequest = "Init";
const std::string Server::kGetTurnOrder = "GetStatus";
const std::string Server::kGetMoveRequest = "GetMove";
const std::string Server::kDoMoveRequest = "DoMove";
const std::string Server::kGetFieldsRequest = "GetFields";

const std::string Server::kSuccessResponse = "Success";
const std::string Server::kFailedResponse = "Failed";