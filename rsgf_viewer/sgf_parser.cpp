#include "sgf_parser.h"

#include <iostream>
#include <iomanip>


#define BOOST_SPIRIT_DEBUG
#define BOOST_SPIRIT_QI_DEBUG
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <boost/bind.hpp>
#include <fstream>
//#include <boost/spirit/include/classic_debug.hpp>
//#include <boost/spirit/include/classic_debug_node.hpp>
#include "Sgf.h"
#include "../src/logutils.h"

namespace qi = boost::spirit::qi;

BOOST_FUSION_ADAPT_STRUCT(
    Property,
    (std::string, identifier)
    (PropValueList, values)
)

BOOST_FUSION_ADAPT_STRUCT(
    GameTree,
    (std::vector<Node>, sequence_)
    (std::vector<GameTree>, sub_trees)
)

template <typename InputIterator>
struct sgf_parser : public boost::spirit::qi::grammar<InputIterator, GameTree()> {
    sgf_parser() : sgf_parser::base_type(game_tree) {
//         point = (qi::alpha | qi::upper) >> (qi::alpha | qi::upper);
//         color = qi::char_('B') | qi::char_('W');
//         value_type = (qi::int_ | qi::double_ | point | color | *(qi::char_ - ']'));
        using boost::spirit::_val;
        using boost::spirit::_1;
        using boost::spirit::_2;
        using boost::phoenix::at_c;
        using boost::phoenix::push_back;
        escChar = '\\' >> qi::char_("\\[]:");
        value_type = *(~qi::char_("\\]") | escChar) [_val += _1];
        compose = value_type >> ':' >> value_type;
        c_value_type = value_type | compose;
        prop_value = '[' >> c_value_type [_val = _1] >> ']' >> *qi::space;
        prop_ident = +qi::upper [_val += _1];
        property = *qi::space >> prop_ident [at_c<0>(_val) = _1] >> +prop_value [push_back(at_c<1>(_val), _1)];
        node = ';' >> +property;
        sequence = +node [push_back(_val, _1)] >> *qi::space;
        game_tree = '(' >> *qi::space >> sequence [at_c<0>(_val) = _1] >> *game_tree [push_back(at_c<1>(_val), _1)] >> ')' >> *qi::space;
    }

    qi::rule<InputIterator, char()> escChar;
    qi::rule<InputIterator, GameTree()> single_tree, game_tree;
    qi::rule<InputIterator, Sequence()> sequence;
    qi::rule<InputIterator, Node()> node;
    qi::rule<InputIterator, Property()> property;
    qi::rule<InputIterator, Compose()> compose;
    qi::rule<InputIterator, std::string()> value_type, prop_ident;
    qi::rule<InputIterator, std::variant<std::string, Compose>()> c_value_type, prop_value;

};

void parse_sgf(const std::string& filename) {
    std::ifstream sgf_file(filename);
    sgf_file.unsetf(std::ios::skipws);

    using base_it_t = boost::spirit::istream_iterator;
    using fwd_it_t = boost::spirit::multi_pass<base_it_t>;
    base_it_t begin(sgf_file);
    base_it_t end;
    sgf_parser<base_it_t> sgf_parser;
    GameTree ast;
    bool r = qi::parse(begin,
                       end,
                       sgf_parser, ast);
    std::cout << r << std::endl;
    print_game_tree(ast);

}