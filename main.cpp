#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "RegexSplitter.h"

static std::vector<std::string> ReadFromFile(std::string const &fileName) {
    std::vector<std::string> list;
    std::ifstream file(fileName);
    if (!file) {
        throw std::runtime_error("file not accessible!");
    }
    for (std::string line; std::getline(file, line); /* */) {
        list.push_back(line);
    }
    return list;
}

namespace {
    using namespace RegexSplitter;

    std::ostream& operator<<(std::ostream& os, Token::Kind k) {
        switch(k) {
            case Token::Group:    return os << "UNBREAKABLE_c";
            case Token::Set:      return os << "CHARSET_c";
            case Token::Nongroup: return os << "BREAKABLE_c";
        }
        return os << "?";
    }
}

void TestRegexSplitter(std::string const &input) {
    static const Grammar p{};

    std::cout << "TEST:" << input << std::endl;

    Tokens tokens;
    auto begin = input.begin(), end = input.end();

    if (parse(begin, end, p/* >> eoi*/, tokens))
        for (auto v: tokens)
            std::cout << " - " << v.kind << " " << v.range << "\n";
    else std::cout << "*** Parse Failed\n";

    if (begin != end)
        std::cout << "*** Remaining: '" << std::string(begin, end) << "'\n";
}

int main(int argc, char *argv[]) {
    std::vector<std::string> args(argv + 1, argv + argc);
    if (args.empty())
        args.emplace_back("testfile-ok");

    for (auto file : args)
    for (auto re : ReadFromFile(file))
        TestRegexSplitter(re);
}
