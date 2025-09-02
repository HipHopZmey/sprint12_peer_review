#include <algorithm>
#include <iostream>
#include <iomanip>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

struct Reader {
    int id = 0;
    int page = 0;
};

struct ReaderHasher {
    size_t operator()(const Reader& reader) const {
        return reader.id;
    }
};

class ReadersData {
public:
    ReadersData(int page_num) {
        page_to_readers_count_ = vector<int>(page_num + 1, 0);
    }

    int CountReaders() const {
        return id_to_readers_.size();
    }

    bool ReaderExists(int id) const {
        return id_to_readers_.count(id) > 0;
    }

    double GetLosersFraction(int id) const {
        if (!ReaderExists(id) || id_to_readers_.at(id).page == 0) {
            return 0;
        }
        double active_readers = page_to_readers_count_[0] - 1;
        double losing_readers = page_to_readers_count_[id_to_readers_.at(id).page - 1] - 1;
        return active_readers == 0 ? 1. : 1. - losing_readers / active_readers;
    }

    void AddUser(int id, int page = 0) {
        id_to_readers_[id] = Reader{ id, page };
        RenewPagesCounter(0, page);
    }

    void SetPages(int id, int page) {
        RenewPagesCounter(id_to_readers_.at(id).page, page);
        id_to_readers_.at(id).page = page;
    }
private:
    unordered_map<int, Reader> id_to_readers_;

    vector<int> page_to_readers_count_;

    void RenewPagesCounter(int prev_page, int new_page) {
        for (int page = prev_page; page < new_page; ++page) {
            page_to_readers_count_[page] += 1;
        }
    }
};

int main() {
    ReadersData readers_db(1000);

    int request_num = 0;
    cin >> request_num;
    cin.get();
    cout << setprecision(6);
    while (request_num-- > 0) {
        string request;
        getline(cin, request, ' ');
        if (request == "CHEER"s) {
            string id;
            getline(cin, id);
            if (!readers_db.ReaderExists(stoi(id))) {
                readers_db.AddUser(stoi(id));
            }
            cout << readers_db.GetLosersFraction(stoi(move(id))) << endl;
        } else if (request == "READ"s) {
            string id, page_num;
            getline(cin, id, ' ');
            getline(cin, page_num);
            if (readers_db.ReaderExists(stoi(id))) {
                readers_db.SetPages(stoi(id), stoi(page_num));
            } else {
                readers_db.AddUser(stoi(id), stoi(page_num));
            }
        }
    }
}
