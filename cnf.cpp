//
// Conversion of grammar to CNF
//

#include "tools/catch.hpp"

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <assert.h>


struct Grammar {
    using Symbol = std::string;
    using Rule = std::pair<Symbol, std::vector<Symbol>>;
    using Rules = std::unordered_multimap<Symbol, std::vector<Symbol>>;

    Grammar(Rules rules, Symbol start)
    : rules(rules)
    , S(start)
    {
        for (const auto kv : rules) {
            for (const auto& s : kv.second) {
                symbols.insert(s);
                if (rules.count(s) == 0) {
                    terminals.insert(s);
                }
            }
        }
    }

    bool is_terminal(const Symbol& s) const {
        return terminals.count(s) > 0;
    }

    Rules rules;
    Symbol S;  // start symbol
    std::unordered_set<Symbol> symbols;
    std::unordered_set<Symbol> terminals;
};


bool rule_exists(const Grammar::Rules& rules, const Grammar::Rule& rule) {
    auto range = rules.equal_range(rule.first);
    for (auto it = range.first; it != range.second; ++it) {
        if (it->second == rule.second) {
            return true;
            break;
        }
    }
    return false;
}


// START
Grammar cnf_start(const Grammar& gr) {
    Grammar::Rules rules = gr.rules;

    auto S = gr.S + "0";
    assert(gr.symbols.count(S) == 0);
    rules.insert(std::make_pair(S, std::vector<Grammar::Symbol>{gr.S}));

    return Grammar(std::move(rules), std::move(S));
}


// TERM
Grammar cnf_term(const Grammar& gr) {
    Grammar::Rules rules;

    for (const auto& rule : gr.rules) {
        if (rule.second.size() < 2) {
            // copy rule without changes
            rules.insert(rule);
            continue;
        }

        // copy rules and replace all terminals s by new symbol N_s
        auto it = rules.insert(rule);
        for (auto& s : it->second) {
            if (gr.is_terminal(s)) {
                Grammar::Symbol N_s = "N_" + s;
                assert(gr.symbols.count(N_s) == 0);
                if (rules.count(N_s) == 0) {
                    rules.insert(std::make_pair(
                        N_s, std::vector<Grammar::Symbol>{s}));
                }
                s = N_s;
            }
        }
    }

    return Grammar(std::move(rules), gr.S);
}


// BIN
Grammar cnf_bin(const Grammar& gr) {
    Grammar::Rules rules;

    for (const auto& rule : gr.rules) {
        if (rule.second.size() < 3) {
            rules.insert(rule);
            continue;
        }

        // We assume that TERM reduction was already done and all symbols in
        // the rule are non-terminal.

        const Grammar::Symbol s = rule.first;
        const auto& rhs = rule.second;

        // find the first non-used consecutive name of s
        size_t num = 0;
        Grammar::Symbol si = s + std::to_string(num);
        while (rules.count(si) > 0 || gr.symbols.count(si) > 0) {
            num += 1;
            si = s + std::to_string(num);
        }

        // A → X1 X2 ... Xn
        //
        // is transformed to
        //
        // A    → X1   A1,
        // A1   → X2   A2,
        // ... ,
        // An-2 → Xn-1 Xn,

        si = s;
        for (auto it = rhs.begin(); it != rhs.end() - 2; ++it)
        {
            auto si_next = s + std::to_string(num);

            rules.insert(std::make_pair(si,
                std::vector<Grammar::Symbol>{*it, si_next}));
            si = si_next;
            num += 1;
        }
        rules.insert(std::make_pair(
            si,
            std::vector<Grammar::Symbol>{*(rhs.end() - 2), *(rhs.end() - 1)}));
    }
    // for (const auto& rule : rules) {
    //     auto range = rules.equal_range(rule.first);
    //     bool exists = false;
    //     for (auto it = range.first; it != range.second; ++it) {
    //         if (it->second == rule.second) {
    //             exists = true;
    //             break;
    //         }
    //     }

    //     if (!exists) {
    //         rules.insert(rule);
    //     }
    // }

    return Grammar(std::move(rules), gr.S);
}


std::vector<std::vector<Grammar::Symbol>> replace_nullable(
    const std::vector<Grammar::Symbol>& rhs,
    const std::unordered_set<Grammar::Symbol> nullable)
{
    if (rhs.empty()) {
        return {};
    }

    std::vector<std::vector<Grammar::Symbol>> result{rhs};
    for (auto i = rhs.begin(); i != rhs.end(); ++i) {
        if (nullable.count(*i) > 0) {
            std::vector<Grammar::Symbol> new_rhs(rhs.begin(), i);
            for (auto j = i + 1; j != rhs.end(); ++j) {
                new_rhs.push_back(*j);
            }

            auto rules = replace_nullable(new_rhs, nullable);
            for (const auto rule : rules) {
                result.push_back(rule);
            }
        }
    }
    return result;
}


// DEL
Grammar cnf_del(const Grammar& gr) {
    Grammar::Rules rules;

    // compute nullable symbols
    std::unordered_set<Grammar::Symbol> nullable;
    size_t nullableSize = 0;
    while (true) {
        for (const auto& rule : gr.rules) {
            if (rule.second.size() == 1 && rule.second.front() == "") {
                nullable.insert(rule.first);
            } else {
                bool isNullable = true;
                for (const auto& s : rule.second) {
                    if (nullable.count(s) == 0) {
                        isNullable = false;
                        break;
                    }
                }

                if (isNullable) {
                    nullable.insert(rule.first);
                }
            }
        }

        if (nullableSize == nullable.size()) {
            break;
        }

        nullableSize = nullable.size();
    }

    // replace rules with nullable symbols
    for (const auto& rule : gr.rules) {
        for (const auto& rhs : replace_nullable(rule.second, nullable)) {
            auto new_rule = std::make_pair(rule.first, rhs);
            if (!rule_exists(rules, new_rule)) {
                rules.insert(std::move(new_rule));
            }
        }
    }

    // remove all rules with eps on the rhs
    for (auto it = rules.begin(); it != rules.end();) {
        if (it->first != gr.S &&
            it->second.size() == 1 && it->second.front() == "")
        {
            it = rules.erase(it);
        } else {
            ++it;
        }
    }

    return Grammar(std::move(rules), gr.S);
}



// UNIT
Grammar cnf_unit(const Grammar& gr) {
    Grammar::Rules rules;
    for (const auto& rule : gr.rules) {
        if (rule.second.size() != 1 || gr.is_terminal(rule.second.front())) {
            rules.insert(rule);
            continue;
        }

        // rule: A → B
        const auto& A = rule.first;
        const auto& B = rule.second.front();
        auto range = rules.equal_range(B);
        for (auto it = range.first; it != range.second; ++it) {
            rules.insert(std::make_pair(A, it->second));
        }
    }

    return Grammar(std::move(rules), gr.S);
}


// transform grammar to CNF (Chomsky Normal Form)
Grammar cnf(const Grammar& gr) {

    Grammar::Rules rules;
    Grammar::Symbol S;

    auto gr0 = cnf_start(gr);
    auto gr1 = cnf_term(gr0);
    auto gr2 = cnf_bin(gr1);
    auto gr3 = cnf_del(gr2);
    auto gr4 = cnf_unit(gr3);

    return gr4;
}


TEST_CASE("START: Eliminate the start symbol from right-handsides", "[CNF]") {
    Grammar gr({{"S", {"A"}}}, "S");
    auto gr0 = cnf_start(gr);
    REQUIRE(gr0.S != "S");
    REQUIRE(gr0.symbols.size() == 2);
}

TEST_CASE("TERM: Eliminate rules with non solitary terminals", "[CNF]") {
    Grammar gr(
        {
            {"S", {"A"}},
            {"A", {"t"}},
            {"A", {"A", "t"}},
            {"A", {"t", "A"}}
        },
        "S");

    auto gr0 = cnf_term(gr);
    REQUIRE(gr0.rules.size() == 5);
    for (auto rule : gr0.rules) {
        if (rule.second.size() < 2) {
            continue;
        }

        for (auto s : rule.second) {
            REQUIRE(
                std::find(gr0.terminals.begin(), gr0.terminals.end(), s) ==
                gr0.terminals.end());
        }
    }
}

TEST_CASE("BIN: Eliminate rhs with more than 2 non terminals", "[CNF]") {
    Grammar gr(
        {
            {"S", {"A"}},
            {"A", {"A", "B", "C"}},
            {"A", {"C", "B", "A"}},
            {"B", {"t"}},
            {"C", {"t"}}
        },
        "S");

    auto gr0 = cnf_bin(gr);
    REQUIRE(gr0.rules.size() == 7);
    for (auto rule : gr0.rules) {
        if (rule.second.size() < 2) {
            continue;
        }

        REQUIRE(rule.second.size() == 2);
        for (auto s : rule.second) {
            REQUIRE(
                std::find(gr0.terminals.begin(), gr0.terminals.end(), s) ==
                gr0.terminals.end());
        }
    }
}

TEST_CASE("DEL: Eliminate ε-rules", "[CNF]") {
    Grammar gr(
        {
            {"S0", {"A", "b", "B"}},
            {"S0", {"C"}},
            {"B", {"A", "A"}},
            {"B", {"A", "C"}},
            {"C", {"b"}},
            {"C", {"c"}},
            {"A", {"a"}},
            {"A", {""}}
        },
        "S0");

    auto gr0 = cnf_del(gr);
    REQUIRE(gr0.rules.size() == 12);
    for (auto rule : gr0.rules) {
        if (rule.first != gr0.S && rule.second.size() == 1) {
            REQUIRE(rule.second.front() != "");
        }
    }
}

TEST_CASE("UNIT: Eliminate unit rules", "[CNF]") {
    Grammar gr(
        {
            {"S", {"C"}},
            {"A", {"t0", "t1", "t2"}},
            {"C", {""}},
            {"C", {"A"}},
        },
        "S");

    auto gr0 = cnf_unit(gr);
    REQUIRE(gr0.rules.size() == (gr.rules.size() - 1));
    for (auto rule : gr0.rules) {
        if (rule.second.size() == 1) {
            REQUIRE(gr0.is_terminal(rule.second.front()));
        }
    }
}
