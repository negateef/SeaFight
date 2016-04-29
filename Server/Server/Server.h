//
//  Server.h
//  Server
//
//  Created by Misha Babenko on 4/27/16.
//  Copyright © 2016 Misha Babenko. All rights reserved.
//

#ifndef Server_h
#define Server_h

#include <string>
#include <vector>
#include <SFML/Network.hpp>
#include "Field.h"
#include "Ship.h"


class Server {
private:
    enum class GameStatus {
        FinishedOrNotStarted,
        FirstPlayerTurn,
        SecondPlayerTurn
    };
    
    std::vector<Field> fields_;
    std::vector<sf::IpAddress> players_ips_;
    std::vector<Position> moves_;
    std::vector<sf::IpAddress> player_ip_move_;
    
    
    GameStatus game_status_;
    sf::UdpSocket socket_;
    
    Server();
    
    void InitGame(sf::Packet &packet, const sf::IpAddress &sender, unsigned short port);
    
    void GetTurnOrder(const sf::IpAddress &sender, unsigned short port);
    
    void GetMove(const sf::IpAddress &sender, unsigned short port);
    
    void MakeMove(sf::Packet &packet, const sf::IpAddress &sender, unsigned short port);
    
    void GetFields(const sf::IpAddress &sender, unsigned short port);
    
public:
    
    enum class SendGameStatus {
        FinishedOrNotStarted,
        YourTurn,
        EnemyTurn
    };
    
    const static std::string kInitRequest;
    const static std::string kGetTurnOrder;
    const static std::string kGetMoveRequest;
    const static std::string kDoMoveRequest;
    const static std::string kGetFieldsRequest;
    
    const static std::string kSuccessResponse;
    const static std::string kFailedResponse;
    
    const static unsigned short kServerPort;
    const static unsigned short kClientPort;
    const static unsigned short kWatcherPort;
    
    

    
    Server &operator =(const Server &) = delete;
    Server(const Server &) = delete;
    
    static Server &GetInstance();
    
    void Init();
};

sf::Packet& operator << (sf::Packet &packet, const Field &field);
sf::Packet& operator >> (sf::Packet &packet, Field &field);

sf::Packet& operator << (sf::Packet &packet, const Server::SendGameStatus &status);
sf::Packet& operator >> (sf::Packet &packet, Server::SendGameStatus &status);


#endif /* Server_h */
