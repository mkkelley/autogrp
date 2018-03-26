#include "move.h"

std::unique_ptr<Move> parse_node(const Node& node) {
    for (const auto& property : node) {
        if (property.identifier == "B") {
            return std::make_unique<Move>(Color::B, std::get<std::string>(property.values[0]));
        } else if (property.identifier == "W") {
            return std::make_unique<Move>(Color::W, std::get<std::string>(property.values[0]));
        }
    }
    return nullptr;
}

void append_move_to_end(Move* head, std::unique_ptr<Move> end) {
    if (head->next_moves.empty()) {
        head->next_moves.push_back(std::move(end));
    } else {
        append_move_to_end(head->next_moves[0].get(), std::move(end));
    }
}

std::unique_ptr<Move> parse_sequence(const Sequence& sequence) {
    std::vector<std::unique_ptr<Move>> move_list;

    for (const auto& node : sequence) {
        auto move = parse_node(node);
        if (move) {
            move_list.push_back(std::move(move));
        }
    }

    if (move_list.empty()) {
        return nullptr;
    }

    std::unique_ptr<Move> head(std::move(move_list[0]));
    for (int i = 1; i < move_list.size(); ++i) {
        append_move_to_end(head.get(), std::move(move_list[i]));
    }
    return head;
}

std::unique_ptr<Move> parse_game_tree(const GameTree& tree) {
    auto main_line = parse_sequence(tree.sequence_);
    if (!main_line) {
        main_line = std::make_unique<Move>(-1, -1);
    }
    for (const auto& sub_tree : tree.sub_trees) {
        auto variation = parse_game_tree(sub_tree);
        main_line->next_moves.push_back(std::move(variation));
    }
    return main_line;
}
