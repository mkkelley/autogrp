#include "move.h"

#include "sgf_parser.h"

#include <boost/date_time.hpp>

const std::locale format = std::locale(std::locale::classic(), new boost::posix_time::time_input_facet("%Y-%m-%d"));

std::time_t pt_to_time_t(const boost::posix_time::ptime& pt) {
    boost::posix_time::ptime time_t_start(boost::gregorian::date(1970, 1, 1));
    boost::posix_time::time_duration diff = pt - time_t_start;
    return diff.ticks() / boost::posix_time::time_duration::rep_type::ticks_per_second;
}

boost::posix_time::ptime string_to_ptime(const std::string& time_string) {
    boost::posix_time::ptime pt;
    std::istringstream is(time_string);
    is.imbue(format);
    is >> pt;
    return pt;
}

Sgf::Sgf(const std::string& filename) : filename(filename) {
    GameTree ast = parse_sgf(filename);
    game_tree = parse_game_tree(ast);
    if (ast.sequence_.size() == 0) {
        return;
    }
    const auto& properties = ast.sequence_[0];
    for (const auto& property : properties) {
        const auto& id = property.identifier;
        const auto& prop_value = std::get<std::string>(property.values[0]);
        if (id == "PW") {
            white = prop_value;
        } else if (id == "PB") {
            black = prop_value;
        } else if (id == "DT") {
            date = pt_to_time_t(string_to_ptime(prop_value));
        } else if (id == "RE") {
            black_won = false;
            white_won = false;
            if (prop_value[0] == 'B') {
                black_won = true;
            } else if (prop_value[0] == 'W') {
                white_won = true;
            }
            if (prop_value.find('R') != std::string::npos) {
                margin = -1;
            } else {
                auto idx = prop_value.find('+');
                if (idx == std::string::npos) continue;
                std::string margin_string = prop_value.substr(idx + 1);
                margin = std::atof(margin_string.c_str());
            }
        } else if (id == "SZ") {
            size = std::atoi(prop_value.c_str());
        } else if (id == "KM") {
            komi = std::atof(prop_value.c_str());
        } else if (id == "HA") {
            handicap = std::atoi(prop_value.c_str());
        }
    }
}


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
