#include "sgf_parser.h"

#include <iostream>
#include <iomanip>


#define BOOST_SPIRIT_USE_PHOENIX_V3
#define BOOST_SPIRIT_DEBUG
#define BOOST_SPIRIT_QI_DEBUG
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
//#include <boost/spirit/include/classic_debug.hpp>
//#include <boost/spirit/include/classic_debug_node.hpp>
#include "Sgf.h"

namespace qi = boost::spirit::qi;

template <typename InputIterator>
struct sgf_parser : public boost::spirit::qi::grammar<InputIterator, std::string()> {
    sgf_parser() : sgf_parser::base_type(start) {
        using boost::spirit::_val;
        using boost::spirit::_1;
        start = +(qi::char_ - ']') [_val = _1];

        BOOST_SPIRIT_DEBUG_NODE(start);
    }

    qi::rule<InputIterator, std::string()> start;

};

void parse_sgf() {
    sgf_parser<std::string::const_iterator> sgf_parser;

}

