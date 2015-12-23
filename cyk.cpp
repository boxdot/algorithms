#include "tools/catch.hpp"

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
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


// transform grammar to CNF (Chomsky Normal Form)
Grammar cnf(const Grammar& gr) {

    Grammar::Rules rules;
    Grammar::Symbol S;

    // START
    S = gr.S + "0";
    assert(gr.symbols.count(S) == 0);
    rules.insert(std::make_pair(S, std::vector<Grammar::Symbol>{gr.S}));

    // TERM
    for (const auto& rule : gr.rules) {
        if (rule.second.size() < 2) {
            continue;
        }

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

    // BIN
    Grammar::Rules buffer;
    for (auto rule_it = rules.begin(); rule_it != rules.end();) {
        if (rule_it->second.size() < 3)
        {
            ++rule_it;
            continue;
        }

        // after TERM we know that all symbols in the rule are non-terminal

        const Grammar::Symbol s = rule_it->first;
        const auto& rhs = rule_it->second;

        size_t num = 0;
        Grammar::Symbol si = s + std::to_string(num);
        while (buffer.count(si) > 0 || gr.symbols.count(si) > 0) {
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

            buffer.insert(std::make_pair(si,
                std::vector<Grammar::Symbol>{*it, si_next}));
            si = si_next;
            num += 1;
        }
        buffer.insert(std::make_pair(
            si,
            std::vector<Grammar::Symbol>{*(rhs.end() - 2), *(rhs.end() - 1)}));

        // erase old rule
        rule_it = rules.erase(rule_it);
    }
    for (const auto& rule : buffer) {
        auto range = rules.equal_range(rule.first);
        bool exists = false;
        for (auto it = range.first; it != range.second; ++it) {
            if (it->second == rule.second) {
                exists = true;
                break;
            }
        }

        if (!exists) {
            rules.insert(rule);
        }
    }

    // DEL

    // TODO: Eliminate eps-rules
    // Now, we just check that there are no eps- rules. eps is reprensented by
    // an empty string.z
    for (const auto& rule : rules) {
        for (const auto& s : rule.second) {
            assert(s != "");
        }
    }

    // UNIT

    buffer.clear();
    for (auto rule_it = rules.begin(); rule_it != rules.end();) {
        if (rule_it->second.size() == 1) {
            // rule: A → B
            const auto& A = rule_it->first;
            const auto& B = rule_it->second.front();
            auto range = rules.equal_range(B);
            for (auto it = range.first; it != range.second; ++it) {
                buffer.insert(std::make_pair(A, it->second));
            }

            rule_it = rules.erase(rule_it);
        } else {
            ++rule_it;
        }
    }
    for (const auto& rule : buffer) {
        auto range = rules.equal_range(rule.first);
        bool exists = false;
        for (auto it = range.first; it != range.second; ++it) {
            if (it->second == rule.second) {
                exists = true;
                break;
            }
        }

        if (!exists) {
            rules.insert(rule);
        }
    }


    return Grammar{rules, S};
}


template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
    if (v.empty()) {
        return os << "[]";
    }

    os << "[" << v.front();
    for (auto it = v.begin() + 1; it != v.end(); ++it) {
        os << ", " << *it;
    }
    return os << "]";
}


template<typename K, typename V>
std::ostream& operator<<(std::ostream& os, const std::pair<K, V>& kv) {
    return os << kv.first << ": " << kv.second;
}



TEST_CASE("Parse long input from adventofcode.com, day19, part 2", "[cyk]") {
    Grammar::Rules rules {
        {"Al", {"Th", "F"}},
        {"Al", {"Th", "Rn", "F", "Ar"}},
        {"B", {"B", "Ca"}},
        {"B", {"Ti", "B"}},
        {"B", {"Ti", "Rn", "F", "Ar"}},
        {"Ca", {"Ca", "Ca"}},
        {"Ca", {"P", "B"}},
        {"Ca", {"P", "Rn", "F", "Ar"}},
        {"Ca", {"Si", "Rn", "F", "Y", "F", "Ar"}},
        {"Ca", {"Si", "Rn", "Mg", "Ar"}},
        {"Ca", {"Si", "Th"}},
        {"F", {"Ca", "F"}},
        {"F", {"P", "Mg"}},
        {"F", {"Si", "Al"}},
        {"H", {"C", "Rn", "Al", "Ar"}},
        {"H", {"C", "Rn", "F", "Y", "F", "Y", "F", "Ar"}},
        {"H", {"C", "Rn", "F", "Y", "Mg", "Ar"}},
        {"H", {"C", "Rn", "Mg", "Y", "F", "Ar"}},
        {"H", {"H", "Ca"}},
        {"H", {"N", "Rn", "F", "Y", "F", "Ar"}},
        {"H", {"N", "Rn", "Mg", "Ar"}},
        {"H", {"N", "Th"}},
        {"H", {"O", "B"}},
        {"H", {"O", "Rn", "F", "Ar"}},
        {"Mg", {"B", "F"}},
        {"Mg", {"Ti", "Mg"}},
        {"N", {"C", "Rn", "F", "Ar"}},
        {"N", {"H", "Si"}},
        {"O", {"C", "Rn", "F", "Y", "F", "Ar"}},
        {"O", {"C", "Rn", "Mg", "Ar"}},
        {"O", {"H", "P"}},
        {"O", {"N", "Rn", "F", "Ar"}},
        {"O", {"O", "Ti"}},
        {"P", {"Ca", "P"}},
        {"P", {"P", "Ti"}},
        {"P", {"Si", "Rn", "F", "Ar"}},
        {"Si", {"Ca", "Si"}},
        {"Th", {"Th", "Ca"}},
        {"Ti", {"B", "P"}},
        {"Ti", {"Ti", "Ti"}},
        {"e", {"H", "F"}},
        {"e", {"N", "Al"}},
        {"e", {"O", "Mg"}},
    };

    Grammar gr(rules, "e");
    auto gr_cnf = cnf(gr);

    // print terminals
    std::cout << "Terminals: ";
    for (auto x : gr.terminals) {
        std::cout << x << " ";
    }
    std::cout << std::endl;

    // print cnf grammar
    auto cnf_gr = cnf(gr);
    std::vector<Grammar::Rule> sortedRules;
    for (const auto& rule : cnf_gr.rules) {
        sortedRules.push_back(rule);
    }
    std::sort(sortedRules.begin(), sortedRules.end());
    for (const auto& rule : sortedRules) {
        std::cout << rule << std::endl;
    }
}
