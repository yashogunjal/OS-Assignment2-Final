#!/bin/bash
echo "Benchmarking p1"
gcc p1.c -pthread -o p1.out
for i in {0..1000}
do
	for j in {0..1000}
	do
	
	 
		./p1.out 50 20 50 inp1.txt inp2.txt out.txt $j
	done
done

echo "Done with benchmarking p1"
echo "Benchmarking p2"
gcc p2.c -pthread -o p2.out
./p2.out 50 20 50 inp1.txt inp2.txt out.txt
ipcrm --all
