# Blockhouse Quantitative Developer Work Trial — Order Book Reconstruction

---

## Overview

This project provides a high-performance C++ solution to reconstruct Market-By-Price (MBP-10) top-of-book snapshots from Market-By-Order (MBO) order flow data. 

The solution consumes an MBO CSV file (`mbo.csv`) as input and produces a reconstructed MBP-10 CSV output representing the top 10 bid and ask price levels aggregated by size and count, in exact compliance with the trial specification.

---

## Repository Structure

```
orderbook-reconstruction/
├── .gitignore
├── Makefile
├── README.md 
├── README.txt 
├── build_and_test.sh 
├── main.cpp 
├── orderbook.hpp 
├── orderbook.cpp 
├── test_orderbook.cpp 
├── mbo.csv 
├── mbp.csv 
```

---

## Usage Instructions

### Build

The project uses `Makefile` for building with optimizations:

```
make 
make perf 
make test 
make clean 
```

### Run Reconstruction

Ensure your `mbo.csv` is in the repository root, then run:

`./reconstruction_blockhouse mbo.csv > output_mbp.csv`

This will read MBO input from `mbo.csv` and write MBP-10 order book snapshots to `output_mbp.csv`.

### Verify Output

Compare the generated `output_mbp.csv` with the reference MBP CSV to confirm correctness.

---

## Design Highlights

- **Efficient Data Structures:**  
  Uses `std::map` with descending order for bids and ascending order for asks, and `std::unordered_map` for O(1) order ID lookup.

- **Correct Special Handling:**  
  - Skips initial `R` (refresh) action.  
  - Correctly handles combined Trade-Fill-Cancel (`T-F-C`) sequences.  
  - Ignores side 'N' trades as required.

- **MBP-10 Aggregation:**  
  Aggregates order sizes and counts at each price level, outputs top 10 bid and ask levels interleaved.

- **Performance Oriented:**  
  Single-pass processing with cache-friendly data layout and modern compiler optimizations.

- **Robust Parsing:**  
  Safe and resilient CSV parsing guarding against malformed data.

---

## Testing & Quality Assurance

- Unit tests implemented in `test_orderbook.cpp`.  
- Automated build and test script `build_and_test.sh` for convenience.
- Includes edge case handling and performance benchmarks.

---

## Notes

- The project is written using C++17, tested on a POSIX-compatible system with `g++`.  
- No external dependencies required beyond the standard library.  
- The solution prioritizes both correctness and performance per the trial evaluation criteria.

---

## Contact

For any questions or clarifications regarding the implementation, feel free to contact me.

---

*Prepared for Blockhouse Quantitative Developer Work Trial — July 2025*

