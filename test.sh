#!/bin/bash
# Test

mkdir bin

g++ -O3 -fopenmp simple_image_processing_pure.cpp -o bin/simple_image_processing_pure


./bin/simple_image_processing_pure 416 416 1
