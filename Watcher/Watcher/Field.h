//
//  Field.h
//  SeaFight
//
//  Created by Misha Babenko on 4/20/16.
//  Copyright © 2016 Misha Babenko. All rights reserved.
//

#ifndef Field_h
#define Field_h

#include <vector>
#include <algorithm>

#include "Ship.h"

struct ServerMoveStatus {
    enum Code {
        kError,
        kMiss, 
        kHurt,
        kKill
    };
    
    Code code;
    Position position;
    
    ServerMoveStatus(Code code, Position position) : code(code), position(position) {
    }
};

class Field {
public:
    static const int kSize = 10;
    enum class CellType {
        kEmpty,
        kShot,
        kHurt,
        kKilled
    };
    
private:
    CellType field_[kSize][kSize];
    std::vector<Ship> ships_;
    
public:
    Field(std::vector<Ship> ships = std::vector<Ship>()) : ships_(ships) {
        for (int i = 0; i < kSize; ++i)
            for (int j = 0; j < kSize; ++j)
                field_[i][j] = CellType::kEmpty;
    }
    
    void ShootStatus(ServerMoveStatus status) {
        Position position = status.position;
        if (status.code == ServerMoveStatus::Code::kMiss && field_[position.row][position.col] != CellType::kEmpty) {
            return;
        }
        switch (status.code) {
            case ServerMoveStatus::Code::kError: return;
            case ServerMoveStatus::Code::kMiss: field_[position.row][position.col] = CellType::kShot; break;
            case ServerMoveStatus::Code::kHurt: field_[position.row][position.col] = CellType::kHurt; break;
            case ServerMoveStatus::Code::kKill: field_[position.row][position.col] = CellType::kKilled; break;
        }
    }

    ServerMoveStatus::Code Shoot(Position position) {
        if (field_[position.row][position.col] != CellType::kEmpty) {
            return ServerMoveStatus::Code::kMiss;
        }
        field_[position.row][position.col] = CellType::kShot;
        for (const auto &ship : ships_) {
            std::vector<Position> cells = ship.GetCells();
            if (std::any_of(cells.begin(), cells.end(), 
                            [position] (Position cell) { return position == cell; })) {
                
                if (std::all_of(cells.begin(), cells.end(),
                                [this] (Position cell) { 
                                    return field_[cell.row][cell.col] == CellType::kHurt || 
                                           field_[cell.row][cell.col] == CellType::kShot; 
                                })) {
                    field_[position.row][position.col] = CellType::kKilled;
                    return ServerMoveStatus::Code::kKill;
                } else {
                    field_[position.row][position.col] = CellType::kHurt;
                    return ServerMoveStatus::Code::kHurt;
                }
                
            }
        }
        return ServerMoveStatus::Code::kMiss;
    }

    void AddShip(const Ship &ship) {
        ships_.push_back(ship); 
    }

    std::vector<Ship> GetShips() const {
        return ships_;
    }

    CellType GetCellType(int row, int col) const {
        return field_[row][col];
    }
    
    void SetCellType(int row, int col, CellType new_type) {
        field_[row][col] = new_type;
    }
    
    void AddShipFromKilled(Position position) {
        int d_row = -1, d_col = -1;
        if (position.row < kSize - 1 && field_[position.row + 1][position.col] == CellType::kHurt) {
            d_row = 1;
            d_col = 0;
        }
        if (position.row > 0 && field_[position.row - 1][position.col] == CellType::kHurt) {
            d_row = -1;
            d_col = 0;
        }
        if (position.col < kSize - 1 && field_[position.row][position.col + 1] == CellType::kHurt) {
            d_row = 0;
            d_col = 1;
        }
        if (position.col > 0 && field_[position.row][position.col - 1] == CellType::kHurt) {
            d_row = 0;
            d_col = -1;
        }
        
        if (d_row == -1 && d_col == -1) {
            AddShip(Ship(1, position, true));
            return;
        }
        
        Position start = position;
        
        while (position.row + d_row >= 0 && position.row + d_row < kSize &&
               position.col + d_col >= 0 && position.col + d_col < kSize &&
               (field_[position.row + d_row][position.col + d_col] == CellType::kHurt ||
                field_[position.row + d_row][position.col + d_col] == CellType::kKilled)) {
                   position.row += d_row;
                   position.col += d_col;
                   if (position.row <= start.row && position.col <= start.col) {
                       start = position;
                   }
               }
        
        int length = 1;
        while (position.row - d_row >= 0 && position.row - d_row < kSize &&
               position.col - d_col >= 0 && position.col - d_col < kSize &&
               (field_[position.row - d_row][position.col - d_col] == CellType::kHurt ||
                field_[position.row - d_row][position.col - d_col] == CellType::kKilled)) {
                   position.row -= d_row;
                   position.col -= d_col;
                   ++length;
                   
                   if (position.row <= start.row && position.col <= start.col) {
                       start = position;
                   }
               }
        AddShip(Ship(length, start, (d_row == 0) ? true : false));
    }
};


#endif /* Field_h */
