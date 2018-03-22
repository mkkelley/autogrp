//
// Created by Michael Kelley on 3/21/2018.
//

#include "Sgf.h"

void print_cvalue(const CValueType& cvalue) {
    std::cout << '[';
    if (auto pStr = std::get_if<std::string>(&cvalue)) {
        std::cout << *pStr;
    } else {
        auto compose = std::get<Compose>(cvalue);
        std::cout << compose.first << ':' << compose.second;
    }
    std::cout << ']';
}

void print_property(const Property& prop) {
    std::cout << prop.identifier;
    for (const auto& value : prop.values) {
        print_cvalue(value);
    }
}

void print_node(const Node& node) {
    std::cout << ';';
    for (const auto& prop : node) {
        print_property(prop);
    }
    std::cout << std::endl;
}

void print_sequence(const Sequence& seq) {
    for (const auto& node : seq) {
        print_node(node);
    }
}

void print_game_tree(const GameTree& game_tree) {
    print_sequence(game_tree.sequence_);
    for (const auto& sub_tree : game_tree.sub_trees) {
        print_game_tree(sub_tree);
    }
}
