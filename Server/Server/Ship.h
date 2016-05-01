//
//  Ship.h
//  SeaFight
//
//  Created by Misha Babenko on 4/20/16.
//  Copyright © 2016 Misha Babenko. All rights reserved.
//

#ifndef Ship_h
#define Ship_h

#include <vector>

struct Position {
    int row;
    int col;
    
    Position (int row, int col) : row(row), col(col) {
    }
    
    bool operator == (const Position &position) const {
        return row == position.row && col == position.col;
    }
};

struct Ship {
    int length;
    Position position;
    bool horizontal;
    
    Ship (int length, Position position, bool horizontal) : length(length), position(position), horizontal(horizontal) {
    }
    
    std::vector<Position> GetCells() const {
        std::vector<Position> result;
        Position current = position;
        for (int i = 0; i < length; ++i) {
            result.push_back(current);
            if (horizontal) {
                ++current.col;
            } else {
                ++current.row;
            }

        }
        return result;
    }
};

#endif /* Ship_h */
