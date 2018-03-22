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
#include <boost/fusion/include/std_pair.hpp>
#include <boost/bind.hpp>
//#include <boost/spirit/include/classic_debug.hpp>
//#include <boost/spirit/include/classic_debug_node.hpp>
#include "Sgf.h"

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
         value_type = *(qi::char_ - ']') [_val += _1];
         compose = value_type >> ':' >> value_type;
         c_value_type = value_type | compose;
         prop_value = '[' >> c_value_type [_val = _1] >> ']' >> *qi::space;
         prop_ident = +qi::upper [_val += _1];
         property = *qi::space >> prop_ident [at_c<0>(_val) = _1] >> +prop_value [push_back(at_c<1>(_val), _1)];
         node = ';' >> +property;
         sequence = +node [push_back(_val, _1)] >> *qi::space;
         single_tree = '(' >> sequence [at_c<0>(_val) = _1] >> ')' ;
         game_tree = '(' >> sequence [at_c<0>(_val) = _1] >> *single_tree [push_back(at_c<1>(_val), _1)] >> ')' >> *qi::space;
    }

    qi::rule<InputIterator, GameTree()> single_tree, game_tree;
    qi::rule<InputIterator, Sequence()> sequence;
    qi::rule<InputIterator, Node()> node;
    qi::rule<InputIterator, Property()> property;
    qi::rule<InputIterator, Compose()> compose;
    qi::rule<InputIterator, std::string()> value_type, prop_ident;
    qi::rule<InputIterator, std::variant<std::string, Compose>()> c_value_type, prop_value;

};

void parse_sgf() {
//    std::string example_file = "C:/users/purti/documents/go/ogsgames/12135339.rsgf";

    std::string example = "(;FF[4]\n"
            "GM[1]\n"
            "DT[2018-03-21]\n"
            "PC[OGS: http://online-go.com/game/12135339]\n"
            "PB[#albatros]\n"
            "PW[mythres]\n"
            "BR[13k]\n"
            "WR[14k]\n"
            "CP[online-go.com]\n"
            "RE[W+R]\n"
            "SZ[19]\n"
            "KM[6.5]\n"
            "RU[japanese]\n"
            "C[mythres: Hi\n"
            "]\n"
            ";B[cc]\n"
            ";W[pp]\n"
            "C[#albatros: hi\n"
            "]\n"
            ";B[jj]\n"
            ";W[pc]\n"
            ";B[cq]\n"
            ";W[dp]\n"
            ";B[dq]\n"
            ";W[ep]\n"
            ";B[eq]\n"
            ";W[fp]\n"
            ";B[cm]\n"
            ";W[fq]\n"
            ";B[cp]\n"
            ";W[jp]\n"
            ";B[mq]\n"
            ";W[oq]\n"
            ";B[jr]\n"
            ";W[qe]\n"
            ";B[qj]\n"
            ";W[dd]\n"
            ";B[cd]\n"
            ";W[de]\n"
            ";B[ce]\n"
            ";W[df]\n"
            ";B[gc]\n"
            ";W[cf]\n"
            ";B[dc]\n"
            ";W[cj]\n"
            ";B[ch]\n"
            ";W[dh]\n"
            ";B[dg]\n"
            ";W[cg]\n"
            ";B[di]\n"
            ";W[eh]\n"
            ";B[ci]\n"
            ";W[ge]\n"
            ";B[jc]\n"
            ";W[lc]\n"
            ";B[qg]\n"
            ";W[qn]\n"
            ";B[rm]\n"
            ";W[qm]\n"
            ";B[rn]\n"
            ";W[ro]\n"
            ";B[ql]\n"
            ";W[pl]\n"
            ";B[qk]\n"
            ";W[ei]\n"
            ";B[dj]\n"
            ";W[fr]\n"
            ";B[hq]\n"
            ";W[iq]\n"
            ";B[hr]\n"
            ";W[hp]\n"
            ";B[ir]\n"
            ";W[mo]\n"
            ";B[lp]\n"
            ";W[lo]\n"
            ";B[kp]\n"
            ";W[ko]\n"
            ";B[jo]\n"
            ";W[jn]\n"
            ";B[io]\n"
            ";W[ip]\n"
            ";B[in]\n"
            ";W[jm]\n"
            ";B[hm]\n"
            ";W[fm]\n"
            ";B[fk]\n"
            ";W[gl]\n"
            ";B[hl]\n"
            ";W[gk]\n"
            ";B[ik]\n"
            ";W[fj]\n"
            ";B[ek]\n"
            ";W[le]\n"
            ";B[gj]\n"
            ";W[hj]\n"
            ";B[gi]\n"
            ";W[hi]\n"
            ";B[gh]\n"
            ";W[hh]\n"
            ";B[gg]\n"
            ";W[if]\n"
            ";B[fi]\n"
            ";W[eg]\n"
            ";B[ej]\n"
            ";W[hk]\n"
            ";B[il]\n"
            ";W[nr]\n"
            ";B[mr]\n"
            ";W[kh]\n"
            ";B[lj]\n"
            ";W[pk]\n"
            ";B[jl]\n"
            ";W[kl]\n"
            ";B[kk]\n"
            ";W[ki]\n"
            ";B[km]\n"
            ";W[lm]\n"
            ";B[ll]\n"
            ";W[kn]\n"
            ";B[kl]\n"
            ";W[kj]\n"
            ";B[nj]\n"
            ";W[oj]\n"
            ";B[oi]\n"
            ";W[ni]\n"
            ";B[mi]\n"
            ";W[nh]\n"
            ";B[nk]\n"
            ";W[mk]\n"
            ";B[nl]\n"
            ";W[mm]\n"
            ";B[ml]\n"
            ";W[pm]\n"
            ";B[pj]\n"
            ";W[ok]\n"
            ";B[oh]\n"
            ";W[mh]\n"
            ";B[ij]\n"
            ";W[mj]\n"
            ";B[li]\n"
            ";W[lh]\n"
            ";B[lk]\n"
            ";W[og]\n"
            ";B[pg]\n"
            ";W[rg]\n"
            ";B[rh]\n"
            ";W[rf]\n"
            ";B[of]\n"
            ";W[ng]\n"
            ";B[pf]\n"
            ";W[ne]\n"
            ";B[nf]\n"
            ";W[mf]\n"
            ";B[hf]\n"
            ";W[gf]\n"
            ";B[hg]\n"
            ";W[hd]\n"
            ";B[hc]\n"
            ";W[id]\n"
            ";B[ig]\n"
            ";W[jg]\n"
            ";B[ie]\n"
            ";W[jf]\n"
            ";B[he]\n"
            ";W[ic]\n"
            ";B[gd]\n"
            ";W[ib]\n"
            ";B[fe]\n"
            ";W[ff]\n"
            ";B[ed]\n"
            ";W[ee]\n"
            ";B[fd]\n"
            ";W[bh]\n"
            ";B[bi]\n"
            ";W[ag]\n"
            ";B[fg]\n"
            ";W[be]\n"
            ";B[ef]\n"
            ";W[ff]\n"
            ";B[bd]\n"
            ";W[co]\n"
            ";B[bo]\n"
            ";W[cn]\n"
            ";B[bn]\n"
            ";W[en]\n"
            ";B[bf]\n"
            ";W[ae]\n"
            ";B[go]\n"
            ";W[gn]\n"
            ";B[gp]\n"
            ";W[ih]\n"
            ";B[fo]\n"
            ";W[eo]\n"
            ";B[fn]\n"
            ";W[dm]\n"
            ";B[gm]\n"
            ";W[fl]\n"
            ";B[cl]\n"
            ";W[np]\n"
            ";B[nm]\n"
            ";W[nn]\n"
            ";B[jd]\n"
            ";W[jb]\n"
            ";B[je]\n"
            ";W[cs]\n"
            ";B[br]\n"
            ";W[ms]\n"
            ";B[lr]\n"
            ";W[fa]\n"
            ";B[fb]\n"
            ";W[ga]\n"
            ";B[ea]\n"
            ";W[hb]\n"
            ";B[eb]\n"
            ";W[pe]\n"
            ";B[oe]\n"
            ";W[od]\n"
            ";B[ld]\n"
            ";W[md]\n"
            ";B[kd]\n"
            ";W[kc]\n"
            ";B[es]\n"
            ";W[er]\n"
            ";B[cr]\n"
            ";W[dr]\n"
            ";B[bs]\n"
            ";W[ds]\n"
            ";B[so]\n"
            ";W[rp]\n"
            ";B[sn]\n"
            ";W[sp]\n"
            ";B[sl]\n"
            ";W[pi]\n"
            ";B[ph]\n"
            ";W[ri]\n"
            ";B[qi]\n"
            ";W[sh]\n"
            ";B[qh]\n"
            ";W[si]\n"
            ";B[rj]\n"
            ";W[sj]\n"
            ";B[sk]\n"
            ";W[sg]\n"
            ";B[dl]\n"
            ";W[el]\n"
            ";B[ai]\n"
            ";W[bg]\n"
            ";B[gf]\n"
            ";W[ef]\n"
            ";B[rc]\n"
            ";W[rd]\n"
            ";B[qb]\n"
            ";W[pb]\n"
            ";B[nc]\n"
            ";W[mc]\n"
            ";B[nd]\n"
            ";W[me]\n"
            ";B[ob]\n"
            ";W[nb]\n"
            ";B[sb]\n"
            ";W[ra]\n"
            ";B[pa]\n"
            ";W[oc]\n"
            ";B[sd]\n"
            ";W[re]\n"
            ";B[ji]\n"
            ";W[jh]\n"
            ";B[oo]\n"
            ";W[on]\n"
            ";B[nq]\n"
            ";W[or]\n"
            ";B[mp]\n"
            ";W[po]\n"
            ";B[ls]\n"
            ";W[gq]\n"
            ";B[ho]\n"
            ";W[ns]\n"
            ";B[gb]\n"
            ";W[ha]\n"
            ";B[qq]\n"
            ";W[pq]\n"
            ";B[qr]\n"
            ";W[sr]\n"
            ";B[qp]\n"
            ";W[qo]\n"
            ";B[ss]\n"
            ";W[rr]\n"
            ";B[rq]\n"
            ";W[sq]\n"
            ";B[om]\n"
            ";W[ad]\n"
            ";B[ol]\n"
            ";W[ac]\n"
            ";B[im]\n"
            ";W[no]\n"
            ";B[bb]\n"
            ";W[ab]\n"
            ";B[bc]\n"
            ";W[aa]\n"
            ";B[ba]\n"
            ";W[af]\n"
            ";B[ah]\n"
            ";W[fs]\n"
            ";B[gs]\n"
            ";W[gr]\n"
            ";B[hs]\n"
            ";W[ke]\n"
            ";B[se]\n"
            ";W[]\n"
            ";B[qf]\n"
            ";W[qc]\n"
            ";B[qa]\n"
            ";W[sf]\n"
            ";B[mb]\n"
            ";W[oa]\n"
            ";B[kb]\n"
            ";W[lb]\n"
            ";B[ka]\n"
            ";W[la]\n"
            "C[mythres: did we really have to go through all of that?\n"
            "]\n"
            ")";

    std::string::const_iterator sstart = example.begin();
    std::string::const_iterator send = example.end();
//    bool r = qi::phrase_parse(sstart, send, collection, boost::spirit::ascii::space);
//    std::cout << std::boolalpha << r << std::endl;
//    std::cout << send - sstart << std::endl;
//    std::cout << example.size() << std::endl;
//    char const* result;
    sgf_parser<std::string::const_iterator> sgf_parser;
//    BOOST_SPIRIT_DEBUG_NODE(sgf_parser);
//    bool r = qi::phrase_parse(sstart, send, sgf_parser, boost::spirit::ascii::space);
    GameTree ast;
    bool r = qi::parse(sstart, send, sgf_parser, ast);
    std::cout << r << std::endl;
    print_game_tree(ast);

}