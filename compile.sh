#!/bin/bash

rm MyAnalysis
cd src
make clean
make
cp main ../MyAnalysis
cd ..
