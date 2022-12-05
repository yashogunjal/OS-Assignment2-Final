#!/bin/bash
echo "Benchmarking p2"
gcc p2.c -pthread -o p2.out
for i in {0..1000}
do
	for j in {0..1000}
	do
	
	 
		./p2.out 50 20 50 inp1.txt inp2.txt out.txt $j
	done
done

echo "Done with benchmarking p2"

ipcrm --all
