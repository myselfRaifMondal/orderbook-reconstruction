Order Book Reconstruction Tool - README
===============================================

Project: Blockhouse Quantitative Developer Work Trial
Author: Raif Salauddin Mondal
Date: July 30, 2025

Overview
--------
This C++ application reconstructs MBP-10 (Market by Price - top ten levels) order book data 
from MBO (Market by Order) input, optimized for high-frequency trading.

Files Included
--------------
- main.cpp
- orderbook.hpp
- orderbook.cpp
- Makefile
- README.txt
- README.md
- test_orderbook.cpp
- build_and_test.sh
- .gitignore

Compilation & Usage
-------------------
1. Build:         make
2. Performance:   make perf
3. Run:           ./reconstruction_blockhouse mbo.csv > output_mbp.csv
4. Tests:         make test
5. Clean:         make clean

Architecture & Design Decisions
-------------------------------
1. Data Structures:
   - std::map for sorted price levels (bids descending, asks ascending)
   - std::unordered_map for O(1) order lookup
2. Memory Optimization:
   - Efficient STL usage, minimal padding
   - Automatic cleanup of empty levels
3. Algorithmic Efficiency:
   - Single-pass processing
   - O(log n) operations
   - Lazy MBP-10 output generation
4. Compiler Flags:
   - -O3, -march=native, -flto, -ffast-math (perf)
5. Special Handling:
   - Skip initial ‘R’ clear
   - T-F-C sequence processing
   - Ignore ‘N’ side trades

Performance Characteristics
--------------------------
- Throughput: 1M+ orders/sec per core
- Latency: sub-µs per operation
- Memory: ~50-100 bytes/order

Optimization Insights & Trade-offs
---------------------------------
- Custom memory allocators (future)
- Lock-free data structures (future)
- SIMD optimizations (future)
- CSV parsing overhead (binary format possible)

Limiting Factors
---------------
- Single-threaded design
- STL vs custom containers
- I/O system calls

Correctness Validation
----------------------
- Edge cases, empty books, multiple instruments
- Unit tests and performance benchmarks

Future Enhancements
-------------------
- Multi-threading
- Real-time streaming
- Binary output
- Configurable venue rules

