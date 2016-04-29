//
//  Player.h
//  SeaFight
//
//  Created by Misha Babenko on 4/20/16.
//  Copyright © 2016 Misha Babenko. All rights reserved.
//

#ifndef Player_h
#define Player_h

#include "Field.h"
#include "Ship.h"

#include <cstdlib>
#include <iostream>


class Player {
private:
    Field &my_field_;
    Field &enemy_field_;
    
public:
    explicit Player(Field &my_field, Field &enemy_field) : my_field_(my_field), enemy_field_(enemy_field) {
    }
    
    void GotMoveStatus(ServerMoveStatus status) {
        enemy_field_.ShootStatus(status);
    }
    
    ServerMoveStatus::Code EnemyMadeShot(Position position) {
        return my_field_.Shoot(position);
    }
};


#endif /* Player_h */
