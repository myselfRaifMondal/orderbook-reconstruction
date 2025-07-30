#!/bin/bash

echo "Blockhouse Quantitative Developer Trial - Order Book Reconstruction"
echo "=================================================================="
echo ""

echo "Cleaning previous builds..."
make clean

echo ""
echo "Building optimized version..."
make perf

if [ $? -eq 0 ]; then
    echo "✓ Build successful!"
    echo ""
    echo "Executable created: reconstruction_blockhouse"
    ls -la reconstruction_blockhouse
    echo ""
    if [ -f "mbo.csv" ]; then
        echo "Found mbo.csv, running reconstruction..."
        ./reconstruction_blockhouse mbo.csv > reconstructed_mbp.csv
        echo "✓ Reconstruction completed! Output saved to reconstructed_mbp.csv"
        if [ -f "mbp.csv" ]; then
            echo ""
            echo "Comparing first 5 lines of expected vs reconstructed:"
            echo "Expected:"
            head -5 mbp.csv
            echo "Reconstructed:"
            head -5 reconstructed_mbp.csv
        fi
    else
        echo "Place your mbo.csv file here and run:"
        echo "  ./reconstruction_blockhouse mbo.csv > output.csv"
    fi
else
    echo "✗ Build failed!"
    exit 1
fi

echo ""
echo "Build and setup completed successfully!"

