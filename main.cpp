#include "orderbook.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <cmath>
#include <iomanip>

class MBOParser {
private:
    std::ifstream file;
    std::string line;
public:
    MBOParser(const std::string& fn) : file(fn) {
        if (!file.is_open()) throw std::runtime_error("Cannot open file: " + fn);
        std::getline(file, line);  // Skip header
    }

    bool parseLine(int &rtype,
                   int &pub_id,
                   std::string &ts_recv,
                   std::string &ts_event,
                   char &action,
                   char &side,
                   std::string &price_str,
                   uint32_t &size,
                   uint64_t &order_id,
                   int &flags,
                   uint64_t &ts_in_delta,
                   uint64_t &sequence,
                   std::string &symbol) {
        if (!std::getline(file, line) || line.empty()) return false;
        std::istringstream ss(line);
        std::vector<std::string> fields;
        std::string fld;
        while (std::getline(ss, fld, ',')) fields.push_back(fld);
        if (fields.size() < 15) return false;

        try {
            ts_recv     = fields[0];
            ts_event    = fields[1];
            rtype       = std::stoi(fields[2]);
            pub_id      = std::stoi(fields[3]);
            action      = fields[5].empty() ? 'N' : fields[5][0];
            side        = fields[6].empty() ? 'N' : fields[6][0];
            price_str   = fields[7];
            size        = std::stoul(fields[8]);
            order_id    = std::stoull(fields[10]);
            flags       = std::stoi(fields[11]);
            ts_in_delta = std::stoull(fields[12]);
            sequence    = std::stoull(fields[13]);
            symbol      = fields[14];
        } catch (...) {
            return false;
        }
        return true;
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <mbo.csv>\n";
        return 1;
    }

    MBOParser parser(argv[1]);
    std::unordered_map<uint64_t, std::unique_ptr<OrderBook>> books;

    int rtype, pub_id, flags;
    uint32_t size;
    uint64_t order_id, ts_in_delta, sequence;
    char action, side;
    std::string ts_recv, ts_event, price_str, symbol;
    bool first = true;
    size_t count = 0;

    while (parser.parseLine(rtype, pub_id,
                            ts_recv, ts_event,
                            action, side,
                            price_str,
                            size, order_id,
                            flags, ts_in_delta,
                            sequence, symbol)) {
        if (first && action == 'R') {
            first = false;
            continue;  // skip initial refresh
        }
        first = false;

        double pd = 0.0;
        try {
            pd = std::stod(price_str);
        } catch (...) {
            pd = 0.0;
        }
        int64_t price = static_cast<int64_t>(std::round(pd * 100));

        if (!books.count(order_id))
            books[order_id] = std::make_unique<OrderBook>(order_id);

        books[order_id]->processAction(rtype, pub_id,
                                       ts_recv, ts_event,
                                       action, side,
                                       price, size,
                                       flags, ts_in_delta,
                                       sequence, symbol,
                                       order_id);

        ++count;
        if (count % 100000 == 0)
            std::cerr << "Processed " << count << " lines...\n";
    }

    // Output MBP-10 header
    std::cout << "ts_recv,ts_event,rtype,publisher_id,instrument_id,action,side,depth,flags,ts_in_delta,sequence";
    for (int i = 0; i < 10; ++i) {
        std::cout << ",bid_px_" << std::setw(2) << std::setfill('0') << i
                  << ",bid_sz_" << std::setw(2) << std::setfill('0') << i
                  << ",bid_ct_" << std::setw(2) << std::setfill('0') << i
                  << ",ask_px_" << std::setw(2) << std::setfill('0') << i
                  << ",ask_sz_" << std::setw(2) << std::setfill('0') << i
                  << ",ask_ct_" << std::setw(2) << std::setfill('0') << i;
    }
    std::cout << ",symbol,order_id\n";

    for (auto &kv : books) {
        if (!kv.second->isEmpty())
            kv.second->outputMBP10(std::cout);
    }

    std::cerr << "Processed " << count << " lines.\n";

    return 0;
}

