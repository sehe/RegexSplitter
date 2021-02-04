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

template <typename Parser, typename... Args>
bool Parse(const std::string &input, const Parser &p, Args &&...args) {
    auto begin = input.begin(), end = input.end();

    bool result = parse(begin, end, p, std::forward<Args>(args)...);

    if (!result || begin != end) {
        std::cout << "Unparseable: '" << input << "', failed at: '"
                  << std::string(begin, end) << "'" << std::endl;

        result = false;
    }

    return result;
}

void TestRegexSplitter(std::string const &input) {
    RegexSplitter::Tokens out;

    std::cout << "TEST:" << input << std::endl;

    bool result = Parse(input, RegexSplitter::Grammar(), out);
    if (result) {
        //for (auto const &token : out) {
            //std::cout << "tok\n";
        //}
    }
}

int main(int argc, char *argv[]) {
    std::vector<std::string> args(argv + 1, argv + argc);
    if (args.empty()) {
        args.emplace_back("testfile-ok");
    }

    for (auto &file : args) {
        for (auto &&re : ReadFromFile(file)) {
            TestRegexSplitter(re);
        }
    }
}

/******************************************************************************/
