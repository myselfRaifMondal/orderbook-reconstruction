#include "orderbook.hpp"
#include <algorithm>

void OrderBook::addOrder(const Order& order) {
    orders[order.order_id] = order;
    if (order.side == 'B') {
        auto &lvl = bids[order.price];
        lvl.price = order.price;
        lvl.size  += order.size;
        lvl.count += 1;
    } else if (order.side == 'S') {
        auto &lvl = asks[order.price];
        lvl.price = order.price;
        lvl.size  += order.size;
        lvl.count += 1;
    }
}

void OrderBook::cancelOrder(uint64_t oid, uint32_t sz) {
    auto it = orders.find(oid);
    if (it == orders.end()) return;
    Order &o = it->second;
    uint32_t cs = (sz == 0 ? o.size : std::min(sz, o.size));

    if (o.side == 'B') {
        auto itb = bids.find(o.price);
        if (itb != bids.end()) {
            itb->second.size -= cs;
            if (cs == o.size) itb->second.count--;
            if (itb->second.size == 0) bids.erase(itb);
        }
    } else if (o.side == 'S') {
        auto ita = asks.find(o.price);
        if (ita != asks.end()) {
            ita->second.size -= cs;
            if (cs == o.size) ita->second.count--;
            if (ita->second.size == 0) asks.erase(ita);
        }
    }

    o.size -= cs;
    if (o.size == 0) orders.erase(it);
}

void OrderBook::modifyOrder(uint64_t oid, uint32_t ns, int64_t np) {
    auto it = orders.find(oid);
    if (it == orders.end()) return;
    Order old = it->second;
    cancelOrder(oid, old.size);
    addOrder(Order(oid, old.side, np, ns, old.ts_event));
}

void OrderBook::processTrade(const Order& to) {
    char opposite = (to.side == 'B' ? 'S' : 'B');
    if (opposite == 'B') {
        auto itb = bids.find(to.price);
        if (itb != bids.end()) {
            uint32_t tsz = std::min<uint32_t>(to.size, itb->second.size);
            itb->second.size -= tsz;
            if (itb->second.size == 0) bids.erase(itb);
        }
    } else {
        auto ita = asks.find(to.price);
        if (ita != asks.end()) {
            uint32_t tsz = std::min<uint32_t>(to.size, ita->second.size);
            ita->second.size -= tsz;
            if (ita->second.size == 0) asks.erase(ita);
        }
    }
}

void OrderBook::processAction(int rtype, int pub_id,
                              const std::string& ts_recv,
                              const std::string& ts_event,
                              char action, char side,
                              int64_t price, uint32_t size,
                              int flags, uint64_t ts_in_delta,
                              uint64_t sequence,
                              const std::string& sym,
                              uint64_t order_id) {

    last_rtype       = rtype;
    last_pub_id      = pub_id;
    last_ts_recv     = ts_recv;
    last_ts_event    = ts_event;
    last_flags       = flags;
    last_ts_in_delta = ts_in_delta;
    last_sequence    = sequence;
    symbol           = sym;

    switch (action) {
        case 'R':
            clear();
            break;
        case 'A':
            if (side != 'N') addOrder(Order(order_id, side, price, size, std::stoull(ts_event)));
            break;
        case 'C':
            if (pending_trade.has_trade && pending_trade.has_fill &&
                order_id == pending_trade.expected_order_id) {
                processTrade(pending_trade.trade_order);
                pending_trade = TradeSequence();
            } else {
                cancelOrder(order_id, size);
            }
            break;
        case 'M':
            if (side != 'N') modifyOrder(order_id, size, price);
            break;
        case 'T':
            if (side != 'N') {
                pending_trade.trade_order = Order(order_id, side, price, size, std::stoull(ts_event));
                pending_trade.has_trade = true;
                pending_trade.expected_order_id = order_id;
            }
            break;
        case 'F':
            if (pending_trade.has_trade && order_id == pending_trade.expected_order_id)
                pending_trade.has_fill = true;
            break;
        default:
            break;
    }
}

void OrderBook::clear() {
    bids.clear();
    asks.clear();
    orders.clear();
    pending_trade = TradeSequence();
}

void OrderBook::outputMBP10(std::ostream& out) const {
    out << last_ts_recv << ','
        << last_ts_event << ','
        << last_rtype    << ','
        << last_pub_id   << ','
        << instrument_id << ','
        << 'T'           << ','
        << 'N'           << ','
        << 10            << ','   // depth = 10
        << last_flags    << ','
        << last_ts_in_delta << ','
        << last_sequence;

    // Collect top 10 bids and asks
    std::vector<PriceLevel> bv, av;
    for (auto const& p : bids) {
        if (bv.size() == 10) break;
        bv.push_back(p.second);
    }
    for (auto const& p : asks) {
        if (av.size() == 10) break;
        av.push_back(p.second);
    }

    // Interleave bid and ask levels: (bid_px_i, bid_sz_i, bid_ct_i, ask_px_i, ask_sz_i, ask_ct_i)
    for (size_t i = 0; i < 10; ++i) {
        double bp = i < bv.size() ? bv[i].price / 100.0 : 0.0;
        uint64_t bs = i < bv.size() ? bv[i].size : 0;
        uint32_t bc = i < bv.size() ? bv[i].count : 0;
        double ap = i < av.size() ? av[i].price / 100.0 : 0.0;
        uint64_t asz = i < av.size() ? av[i].size : 0;
        uint32_t ac = i < av.size() ? av[i].count : 0;

        out << ',' << bp << ',' << bs << ',' << bc
            << ',' << ap << ',' << asz << ',' << ac;
    }

    out << ',' << symbol << ',' << 0 << '\n';
}

