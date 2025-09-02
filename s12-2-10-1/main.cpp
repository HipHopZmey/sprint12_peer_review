#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include <sstream>
#include <string_view>
#include <utility>
#include <vector>

using namespace std;

class Domain {
public:
    template<class Str>
    Domain(Str&& domain_string)
        : domain_(forward<Str>(domain_string)) { 
        if (domain_[0] != '.') {
            domain_.insert(0, "."s);
        }
    }

    bool operator==(const Domain& other) const {
        return domain_ == other.domain_;
    }

    bool operator<(const Domain& other) const {
        return lexicographical_compare(
            domain_.rbegin(),       domain_.rend(),
            other.domain_.rbegin(), other.domain_.rend());
    }

    bool IsSubdomain(const Domain& other) const {
        if (domain_.size() > other.domain_.size()) {
            return false;
        }
        int len_this  = static_cast<int>(domain_.size());
        int len_other = static_cast<int>(other.domain_.size());
        string_view sv_this  = string_view(domain_);
        string_view sv_other = string_view(other.domain_);
        // лучше чем цикл, потому что для sv нет копирования при вызове substr
        return sv_this == sv_other.substr(len_other - len_this);
    }
private:
    string domain_;
};

class DomainChecker {
public:
    template<class InputIt>
    DomainChecker(InputIt first_domain, InputIt last_domain)
        : forbidden_domains_(first_domain, last_domain) { 
        sort(forbidden_domains_.begin(), forbidden_domains_.end()
            , [](Domain& df, Domain& ds) { return df < ds; });
        auto last = unique(forbidden_domains_.begin(), forbidden_domains_.end()
            , [](const Domain& df, const Domain& ds) { 
                return ds.IsSubdomain(df) || df.IsSubdomain(ds); });
        forbidden_domains_.erase(last, forbidden_domains_.end());
    }

    bool IsForbidden(const Domain& check_domain) const {
        auto it = upper_bound(forbidden_domains_.begin(), forbidden_domains_.end()
            , check_domain); // , searcher       
        // вышли за границу вектора или пердыдущий 
        // запрещенный поддомен входит в проверяемый
        return it != forbidden_domains_.begin() 
            && (*prev(it)).IsSubdomain(check_domain);
    }
private:

    std::vector<Domain> forbidden_domains_;
};

vector<Domain> ReadDomains(istream& in, size_t domain_num) {
    vector<Domain> domains;
    string next_domain;
    for (size_t dnum = 0; dnum < domain_num; ++dnum) {
        getline(in, next_domain);
        domains.emplace_back(move(next_domain));
    }
    return domains;
}

template <typename Number>
Number ReadNumberOnLine(istream& input) {
    string line;
    getline(input, line);

    Number num;
    std::istringstream(line) >> num;

    return num;
}

bool TestBase() {
    stringstream in;
    stringstream out;
    in << "4\n"s
        << "gdz.ru\n" << "maps.me\n" << "m.gdz.ru\n"
        << "com\n";
    const std::vector<Domain> forbidden_domains = ReadDomains(in, ReadNumberOnLine<size_t>(in));
    DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());
    in.clear();

    in << "7\n"s
        << "gdz.ru\n" << "gdz.com\n" << "m.maps.me\n"
        << "alg.m.gdz.ru\n" << "maps.com\n" << "maps.ru\n" << "gdz.ua\n";
    const std::vector<Domain> test_domains = ReadDomains(in, ReadNumberOnLine<size_t>(in));
    for (const Domain& domain : test_domains) {
        out << (checker.IsForbidden(domain) ? "Bad"sv : "Good"sv) << endl;
    }
    return (out.str() == "Bad\nBad\nBad\nBad\nBad\nGood\nGood\n"s);
}

bool TestNew() {
    stringstream in;
    stringstream out;
    in << "5\n"s 
       << "google.com\n" << "yandex.ru\n" << "github.com\n"
       << "stackoverflow.com\n" << "habr.com\n";
    const std::vector<Domain> forbidden_domains = ReadDomains(in, ReadNumberOnLine<size_t>(in));
    DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());
    in.clear();

    in << "7\n"s
       << "mail.google.com\n" << "api.yandex.ru\n" << "gist.github.com\n"
       << "meta.stackoverflow.com\n" << "habr.com\n" << "yandex.com\n" << "google.ru\n";
    const std::vector<Domain> test_domains = ReadDomains(in, ReadNumberOnLine<size_t>(in));
    for (const Domain& domain : test_domains) {
        out << (checker.IsForbidden(domain) ? "Bad"sv : "Good"sv) << endl;
    }
    return (out.str() == "Bad\nBad\nBad\nBad\nBad\nGood\nGood\n"s);
}

int main() {

    assert(TestBase());
    assert(TestNew());

    const std::vector<Domain> forbidden_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin)); // 
    DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());

    const std::vector<Domain> test_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    for (const Domain& domain : test_domains) {
        cout << (checker.IsForbidden(domain) ? "Bad"sv : "Good"sv) << endl;
    }
}