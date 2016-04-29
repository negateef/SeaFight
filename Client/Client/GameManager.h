//
//  GameManager.h
//  SeaFight
//
//  Created by Misha Babenko on 4/20/16.
//  Copyright © 2016 Misha Babenko. All rights reserved.
//

#ifndef GameManager_h
#define GameManager_h

#include "Visualizer.h"
#include "ServerAPI.h"
#include "Player.h"
#include "Field.h"

#include <thread>
#include <mutex>
#include <chrono>
#include <atomic>
#include <string>

#include <iostream>

class GameManager {
private:
    Visualizer visualizer_;
    Player player_;
    Field my_field_;
    Field enemy_field_;

    GameManager() : player_(my_field_, enemy_field_) {}

    enum class GameStatus {
        kMyTurn,
        kEnemyTurn,
        kWaitingServerResponse,
        kInitializingGame,
        kWaitingOtherPlayer
    };

    void GenerateField() {
        Ship ship4(4, Position(0, 0), true);
        my_field_.AddShip(ship4);

        Ship ship2(2, Position(5, 5), false);
        my_field_.AddShip(ship2);
    }
    
    
public:
    GameManager &operator =(const GameManager &) = delete;
    GameManager(const GameManager &) = delete;
    
    static GameManager &GetInstance() {
        static GameManager manager;
        return manager;
    }

    void Init() {
        visualizer_.Init();
    }

    
    
    void Play() {    
        ServerAPI::GetInstance().Init();
        
        GenerateField();

        ServerAPI::GetInstance().StartGame(my_field_, [] (ServerAPI::StartGameStatus status) {
            if (status == ServerAPI::StartGameStatus::kFailed) {
                std::cerr << "Game can't be started!" << std::endl;
                return;
            }
        });

        std::atomic<GameStatus> game_status;
        std::mutex mutex;

        game_status = GameStatus::kInitializingGame;
        
        // Game loop
        while (true) {

            std::unique_lock<std::mutex> lock(mutex);
            
            std::string display_status = "";
            if (game_status == GameStatus::kWaitingOtherPlayer) {
                display_status = "Connecting to other player";
            }
            if (game_status == GameStatus::kMyTurn) {
                display_status = "Your turn";
            }
            if (game_status == GameStatus::kWaitingServerResponse) {
                display_status = "Waiting response";
            }

            visualizer_.SetStatus(display_status);
            visualizer_.DrawFields(my_field_, enemy_field_);
            lock.unlock();

           
            if (game_status == GameStatus::kWaitingServerResponse || 
                game_status == GameStatus::kWaitingOtherPlayer) {
                continue;
            }

            if (game_status == GameStatus::kInitializingGame) {
                game_status = GameStatus::kWaitingOtherPlayer;
                std::thread wait_player([&game_status] () {
                    ServerAPI::GetInstance().GetGameStatus([&game_status] (ServerAPI::GameStatus status) {
                        if (status == ServerAPI::GameStatus::kYourTurn) {
                            game_status = GameStatus::kMyTurn;
                        } else {
                            game_status = GameStatus::kEnemyTurn;
                        }
                    });
                });

                wait_player.detach();
            }
            
            if (game_status == GameStatus::kMyTurn) {
                Position position = visualizer_.GetMovePosition();
                game_status = GameStatus::kWaitingServerResponse;
                std::thread make_move_thread([this, &position, &game_status, &mutex] () {
                    ServerAPI::GetInstance().MakeMove(position, [this, &game_status, &mutex] (ServerMoveStatus status) {
                        std::lock_guard<std::mutex> lock(mutex);
                        player_.GotMoveStatus(status);
                        if ((status.code == ServerMoveStatus::Code::kHurt) || 
                            (status.code == ServerMoveStatus::Code::kKill)) {
                            game_status = GameStatus::kMyTurn;
                        } else {
                            game_status = GameStatus::kEnemyTurn;
                        }
                    }); 
                }); 

                make_move_thread.detach();
            }
            if (game_status == GameStatus::kEnemyTurn) {
                game_status = GameStatus::kWaitingServerResponse;
    
                std::thread get_move_thread([this, &game_status, &mutex] () {
                    ServerAPI::GetInstance().GetEnemyMove([this, &game_status, &mutex] (Position position) {
                        std::lock_guard<std::mutex> lock(mutex);
                        ServerMoveStatus::Code code = player_.EnemyMadeShot(position);
                        if (code == ServerMoveStatus::Code::kMiss) {
                            game_status = GameStatus::kMyTurn;
                        }
                    });
                });

                get_move_thread.detach();

            }  
        }
    }
};


#endif /* GameManager_h */
