//
// Created by Michael Kelley on 3/19/2018.
//

#pragma once

#include <memory>
#include <vector>

struct Move {
    Move(int row, int col) : row(row), col(col) { }

    explicit Move(const std::string& p) : row(-1), col(-1), pass(false)
    {
        if (p.empty()) pass = true;
        if (p[0] >= 'a') row = p[0] - 'a' + 1;
        else row = p[0] - 'A' + 1;
        if (p[1] >= 'a') col = p[1] - 'a' + 1;
        else col = p[1] - 'A' + 1;
    }
    int row;
    int col;
    bool pass;

    std::vector<std::unique_ptr<Move>> next_moves;
};
