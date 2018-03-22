//
// Created by Michael Kelley on 3/21/2018.
//

#pragma once

#include <string>
#include <variant>
#include <iostream>
#include <ctime>

#include "move.h"

using Compose = std::pair<std::string, std::string>;
using CValueType = std::variant<std::string, Compose>;
using PropValueList = std::vector<CValueType>;

struct Property {
    std::string identifier;
    PropValueList values;
};

using Node = std::vector<Property>;
using Sequence = std::vector<Node>;

struct GameTree {
    Sequence sequence_;
    std::vector<GameTree> sub_trees;
};

void print_cvalue(const CValueType& cvalue);

void print_property(const Property& prop);

void print_node(const Node& node);

void print_sequence(const Sequence& seq);

void print_game_tree(const GameTree& game_tree);

class Sgf {

    std::string white;
    std::string black;
    std::time_t date;
    bool black_won;
    bool white_won;
    double margin;
    int size;
    double komi;
    std::string rules;
};


