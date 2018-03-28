//
// Created by Michael Kelley on 3/19/2018.
//

#pragma once

#include <memory>
#include <vector>
#include "Sgf.h"

enum class Color { B, W };

struct Move {
    Move(int row, int col) : row(row), col(col) { }

    explicit Move(Color c, const std::string& p) : color(c), row(-1), col(-1), pass(false)
    {
        if (p.empty()) pass = true;
        if (p[0] >= 'a') row = p[0] - 'a' + 1;
        else row = p[0] - 'A' + 1;
        if (p[1] >= 'a') col = p[1] - 'a' + 1;
        else col = p[1] - 'A' + 1;
    }
    Color color;
    int row;
    int col;
    bool pass;

    std::vector<std::unique_ptr<Move>> next_moves;
};

std::unique_ptr<Move> parse_game_tree(const GameTree& tree);


struct Sgf {
    Sgf(const std::string& filename);

    std::string filename;
    std::string white;
    std::string black;
    std::time_t date;
    bool black_won;
    bool white_won;
    double margin;
    int size;
    double komi;
    std::string rules;
    int handicap;
    std::unique_ptr<Move> game_tree;
};
