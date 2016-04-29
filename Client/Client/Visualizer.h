//
//  Visualizer.h
//  SeaFight
//
//  Created by Misha Babenko on 4/20/16.
//  Copyright © 2016 Misha Babenko. All rights reserved.
//

#ifndef Visualizer_h
#define Visualizer_h

#include <iostream>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include "Field.h"
#include "Ship.h"

class Visualizer {
private:
    sf::RenderWindow window_;
    Field my_field_;
    Field enemy_field_;
    std::string status_;
    
    const std::string kFontPath = "/Users/mishababenko/Projects/SHAD/Semester2/C++/SeaFightXCode/Client/Client/Timeless.ttf";

    void DrawField(const int height, const int width, double position_x, double position_y, const Field &field) {
        sf::RectangleShape rectangle(sf::Vector2f(height, width));
        rectangle.setPosition(position_x, position_y);
        rectangle.setOutlineColor(sf::Color::White);
        rectangle.setOutlineThickness(3);
        rectangle.setFillColor(sf::Color::Black);
        window_.draw(rectangle);
        double cell_height = height / static_cast<double>(Field::kSize);
        double cell_width = width / static_cast<double>(Field::kSize);

        for (int i = 1; i < Field::kSize; ++i) {
            sf::Vertex hor_line[] = {
                sf::Vertex(sf::Vector2f(position_x, position_y + cell_height * i)),
                sf::Vertex(sf::Vector2f(position_x + width, position_y + cell_height * i))
            };
            window_.draw(hor_line, 2, sf::Lines);
            sf::Vertex ver_line[] = {
                sf::Vertex(sf::Vector2f(position_x + i * cell_width, position_y)),
                sf::Vertex(sf::Vector2f(position_x + i * cell_width, position_y + height))
            };
            window_.draw(ver_line, 2, sf::Lines);
        }

        for (int i = 0; i < Field::kSize; ++i) {
            sf::Font font;
            if (!font.loadFromFile(kFontPath)) {
                std::cerr << "Couldn't load font file" << std::endl;
                exit(0);
            }
            sf::Text letter;
            letter.setString(sf::String(static_cast<char>(static_cast<int>('a') + i)));
            letter.setFont(font);
            letter.setPosition(position_x - 40, position_y + cell_height * i);
            letter.setColor(sf::Color::White);
            letter.setCharacterSize(50);
            window_.draw(letter);

            sf::Text digit;
            digit.setString(sf::String(static_cast<char>(static_cast<int>('1') + i)));
            digit.setFont(font);
            digit.setPosition(position_x + cell_width * (i + 0.3), position_y + height);
            digit.setColor(sf::Color::White);
            digit.setCharacterSize(50);
            window_.draw(digit);
        }

        for (auto ship : field.GetShips()) {
            sf::RectangleShape ship_rect;
            double left_x = position_x + ship.position.col * cell_width;
            double left_y = position_y + ship.position.row * cell_height;

            double right_x, right_y;
            if (ship.horizontal) {
                right_x = left_x + cell_width * ship.length;
                right_y = left_y + cell_height * 1;
            } else {
                right_x = left_x + cell_width * 1;
                right_y = left_y + cell_height * ship.length;
            }

            ship_rect.setSize(sf::Vector2f(right_x - left_x, right_y - left_y));
            ship_rect.setPosition(sf::Vector2f(left_x, left_y));
            ship_rect.setFillColor(sf::Color::Black);
            ship_rect.setOutlineThickness(5.0);
            ship_rect.setOutlineColor(sf::Color::White);

            window_.draw(ship_rect);
           
        }
    }

    void DrawMiss(int row, int col, int position_x, int position_y) {
        double cell_height = kHeight / static_cast<double>(Field::kSize);
        double cell_width = kWidth / static_cast<double>(Field::kSize);
        double radius = 5.0;
        sf::CircleShape circle(radius);
        circle.setFillColor(sf::Color::White);
        circle.setPosition(position_x + cell_width * col + cell_width / 2.0 - radius, position_y + cell_height * row + cell_height / 2.0 - radius);
        window_.draw(circle);
    }
    

    void DrawHit(int row, int col, int position_x, int position_y) {
        double cell_height = kHeight / static_cast<double>(Field::kSize);
        double cell_width = kWidth / static_cast<double>(Field::kSize);
        double border = cell_height / 10.0;

        sf::Vertex line1[] = {
            sf::Vertex(sf::Vector2f(position_x + cell_width * col + border, position_y + cell_height * row + border)),
            sf::Vertex(sf::Vector2f(position_x + cell_width * (col + 1) - border, position_y + cell_height * (row + 1) - border)),
        };
        window_.draw(line1, 2, sf::Lines);
        
        sf::Vertex line2[] = {
            sf::Vertex(sf::Vector2f(position_x + cell_width * (col + 1) - border, position_y + cell_height * row + border)),
            sf::Vertex(sf::Vector2f(position_x + cell_width * col + border, position_y + cell_height * (row + 1) - border)),
        };
        window_.draw(line2, 2, sf::Lines);
    }

    void DrawMoves() {
        for (int row = 0; row < Field::kSize; ++row) {
            for (int col = 0; col < Field::kSize; ++col) {
                Field::CellType cur_type = enemy_field_.GetCellType(row, col);
                if (cur_type == Field::CellType::kShot) {
                    DrawMiss(row, col, kWindowWidth - kLeftBorder - kWidth, (kWindowHeight - kHeight) / 2.0);
                } else {
                    if ((cur_type == Field::CellType::kHurt) || 
                        (cur_type == Field::CellType::kKilled)) {
                        DrawHit(row, col, kWindowWidth - kLeftBorder - kWidth, (kWindowHeight - kHeight) / 2.0);
                    }
                }

            }
        }
        for (int row = 0; row < Field::kSize; ++row) {
            for (int col = 0; col < Field::kSize; ++col) {
                Field::CellType cur_type = my_field_.GetCellType(row, col);
                if (cur_type == Field::CellType::kShot) {
                    DrawMiss(row, col, kLeftBorder, (kWindowHeight - kHeight) / 2.0);
                } else {
                    if ((cur_type == Field::CellType::kHurt) || 
                        (cur_type == Field::CellType::kKilled)) {
                        DrawHit(row, col, kLeftBorder, (kWindowHeight - kHeight) / 2.0);
                    }
                }

            }
        }
    }

    void DrawStatus() { 
        sf::Font font;
        if (!font.loadFromFile(kFontPath)) {
            std::cerr << "Couldn't load font file" << std::endl;
            exit(0);
        }
        sf::Text text;
        text.setString(sf::String(status_));
        text.setFont(font);
        text.setCharacterSize(80);
        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width / 2.0f,
                       textRect.top  + textRect.height / 2.0f);
        text.setPosition(sf::Vector2f(kWindowWidth/2.0f, 60));
        text.setColor(sf::Color::White);
        window_.draw(text);
    }

    void DrawFields() {
        sf::Event event;
        while (window_.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window_.close();
            }
        }
        window_.clear();
        DrawField(kHeight, kWidth, kLeftBorder, (kWindowHeight - kHeight) / 2.0, my_field_);   
        DrawField(kHeight, kWidth, kWindowWidth - kLeftBorder - kWidth, (kWindowHeight - kHeight) / 2.0, enemy_field_);   
        DrawMoves();
        DrawStatus();
        window_.display();
    }

public:
    static const int kWindowHeight = 800;
    static const int kWindowWidth = 1200;

    static const int kHeight = 500;
    static const int kWidth = 500;
    static const int kLeftBorder = 50;

    Visualizer() : window_(sf::VideoMode(kWindowWidth, kWindowHeight), "Sea Fight", sf::Style::Close) {
    }
    
    void Init() {
        DrawField(kHeight, kWidth, kLeftBorder, (kWindowHeight - kHeight) / 2.0, my_field_);   
        DrawField(kHeight, kWidth, kWindowWidth - kLeftBorder - kWidth, (kWindowHeight - kHeight) / 2.0, enemy_field_);   
        window_.display();
    }


    void DrawFields(const Field &my_field, const Field &enemy_field) {
        my_field_ = my_field;
        enemy_field_ = enemy_field;
        DrawFields();
    }

    void SetStatus(std::string status) {
        status_ = status;
        DrawFields();
    }

    
    Position GetMovePosition() {
        status_ = "Your turn";
        while (true) {
            sf::Event event;
            while (window_.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window_.close();
                }

                if (event.type == sf::Event::MouseButtonPressed) {
                    if (event.mouseButton.button != sf::Mouse::Left)
                        continue;

                    double x = event.mouseButton.x;
                    double y = event.mouseButton.y; 

                    if ((x <= kWindowWidth - kLeftBorder - kWidth) || (x >= kWindowWidth - kLeftBorder) ||
                        (y <= (kWindowHeight - kHeight) / 2.0) || (y >= (kWindowHeight + kHeight) / 2.0)) 
                        continue;
                    double cell_height = kHeight / static_cast<double>(Field::kSize);
                    double cell_width = kWidth / static_cast<double>(Field::kSize);
                    int col = static_cast<int>((x - kWindowWidth + kLeftBorder + kWidth) / cell_width);
                    int row = static_cast<int>((y - (kWindowHeight - kHeight) / 2.0) / cell_height);

                    return Position(row, col);
                }

                DrawFields();
            } 
        }
    }
};

#endif /* Visualizer_h */
