#include "orderbook.hpp"
#include <cassert>
#include <sstream>
#include <iostream>
#include <chrono>

class OrderBookTester {
    OrderBook book;
    void testBasicAddOrder(){
        book.clear();
        book.processAction('A',1001,'B',10000,100,1,1,"SYM");
        book.processAction('A',1002,'S',10100,50,2,2,"SYM");
        std::ostringstream o; book.outputMBP10(o);
        std::string r=o.str();
        assert(r.find("100.00,100")!=std::string::npos);
        assert(r.find("101.00,50")!=std::string::npos);
    }
    void testCancellation(){
        book.clear();
        book.processAction('A',2001,'B',9900,200,1,1,"SYM");
        book.processAction('C',2001,'B',9900,200,2,2,"SYM");
        assert(book.isEmpty());
    }
    void testTradeSequence(){
        book.clear();
        book.processAction('A',3001,'S',10050,100,1,1,"SYM");
        book.processAction('T',3002,'B',10050,50,2,2,"SYM");
        book.processAction('F',3002,'B',10050,50,3,3,"SYM");
        book.processAction('C',3002,'B',10050,50,4,4,"SYM");
        std::ostringstream o; book.outputMBP10(o);
        assert(o.str().find("100.50,50")!=std::string::npos);
    }
    void testMultipleLevels(){
        book.clear();
        for(int i=0;i<15;i++)
            book.processAction('A',4000+i,'B',10000-i*10,100,i,i,"SYM");
        for(int i=0;i<15;i++)
            book.processAction('A',5000+i,'S',10100+i*10,50,i,i,"SYM");
        std::ostringstream o; book.outputMBP10(o);
        std::string r=o.str();
        assert(r.find("100.00,100")!=std::string::npos);
        assert(r.find("101.00,50")!=std::string::npos);
    }
    void testEmpty(){
        book.clear();
        assert(book.isEmpty());
        std::ostringstream o; book.outputMBP10(o);
        int zeros=0; for(size_t p=0;(p=o.str().find(",0",p))!=std::string::npos;p+=2) zeros++;
        assert(zeros>=30);
    }
    void testIgnoreN(){
        book.clear();
        book.processAction('A',6001,'B',9950,100,1,1,"SYM");
        book.processAction('T',6002,'N',9950,50,2,2,"SYM");
        std::ostringstream o; book.outputMBP10(o);
        assert(o.str().find("99.50,100")!=std::string::npos);
    }
public:
    void runAll(){
        testBasicAddOrder();
        testCancellation();
        testTradeSequence();
        testMultipleLevels();
        testEmpty();
        testIgnoreN();
        std::cout<<"All tests passed!\\n";
    }
    void perfTest(){
        OrderBook b;
        auto start=std::chrono::high_resolution_clock::now();
        const int N=100000;
        for(int i=0;i<N;i++){
            char sd=(i%2?'S':'B');
            int64_t pr=(sd=='B'?10000-i%1000:10100+i%1000);
            b.processAction('A',i,sd,pr,100,i,i,"SYM");
        }
        auto end=std::chrono::high_resolution_clock::now();
        auto d=std::chrono::duration_cast<std::microseconds>(end-start).count();
        std::cout<<N<<" adds in "<<d<<" µs => "<<(N*1e6/d)<<" ops/s\\n";
    }
};

#ifdef UNIT_TEST_MAIN
int main(){
    OrderBookTester t;
    t.runAll();
    t.perfTest();
    return 0;
}
#endif

