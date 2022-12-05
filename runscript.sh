#!/bin/bash
gcc p1.c -pthread -o p1.out
gcc p2.c -pthread -o p2.out
./p1.out 50 20 50 inp1.txt inp2.txt out.txt
./p2.out 50 20 50 inp1.txt inp2.txt out.txt
ipcrm --all
