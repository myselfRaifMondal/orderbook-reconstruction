#ifndef ORDERBOOK_HPP
#define ORDERBOOK_HPP

#include <unordered_map>
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>

struct Order {
    uint64_t order_id;
    char side;
    int64_t price;      // price scaled by 100
    uint32_t size;
    uint64_t ts_event;
    Order(): order_id(0), side('N'), price(0), size(0), ts_event(0) {}
    Order(uint64_t id, char s, int64_t p, uint32_t sz, uint64_t ts)
      : order_id(id), side(s), price(p), size(sz), ts_event(ts) {}
};

struct PriceLevel {
    int64_t price;
    uint64_t size;
    uint32_t count;
    PriceLevel(): price(0), size(0), count(0) {}
    PriceLevel(int64_t p, uint64_t s, uint32_t c)
      : price(p), size(s), count(c) {}
};

class OrderBook {
private:
    // Bids sorted descending (highest price first)
    std::map<int64_t, PriceLevel, std::greater<int64_t>> bids;
    // Asks sorted ascending (lowest price first)
    std::map<int64_t, PriceLevel> asks;
    std::unordered_map<uint64_t, Order> orders;

    struct TradeSequence {
        Order trade_order;
        bool has_trade = false;
        bool has_fill  = false;
        uint64_t expected_order_id = 0;
    } pending_trade;

    uint64_t instrument_id;

    // Metadata carried from parse for output
    std::string last_ts_recv;
    std::string last_ts_event;
    int last_rtype = 0;
    int last_pub_id = 0;
    int last_flags = 0;
    uint64_t last_ts_in_delta = 0;
    uint64_t last_sequence = 0;
    std::string symbol;

    void addOrder(const Order& order);
    void cancelOrder(uint64_t order_id, uint32_t size);
    void modifyOrder(uint64_t order_id, uint32_t new_size, int64_t new_price);
    void processTrade(const Order& trade_order);

public:
    explicit OrderBook(uint64_t inst_id = 0)
      : instrument_id(inst_id)
    {}

    void processAction(int rtype, int pub_id,
                       const std::string& ts_recv,
                       const std::string& ts_event,
                       char action, char side,
                       int64_t price, uint32_t size,
                       int flags, uint64_t ts_in_delta,
                       uint64_t sequence,
                       const std::string& sym,
                       uint64_t order_id);

    void clear();
    void outputMBP10(std::ostream& out) const;
    bool isEmpty() const { return bids.empty() && asks.empty(); }
};

#endif // ORDERBOOK_HPP

