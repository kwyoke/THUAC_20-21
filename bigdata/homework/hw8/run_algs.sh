#!/bin/bash

#compile c++ scripts
g++ edgecut.cpp -o edgecut
g++ vertexcut.cpp -o vertexcut
g++ hybridcut.cpp -o hybridcut
g++ greedycut.cpp -o greedycut

#run basic cuts for all datasets
N_array=(2 3 4 8)
graphs_array=(roadNet-PA.graph synthesized-1b.graph twitter-2010.graph)
for N in "${N_array[@]}"
do
	for file in "${graphs_array[@]}"
	do
		echo $file $N "edgecut"
		./edgecut $file $N
		echo $file $N "vertexcut"
		./vertexcut $file $N
	done
done

# hybrid cut, different thresholds for each dataset
for N in "${N_array[@]}"
do
	echo roadNet $N "hybridcut"
	./hybridcut roadNet-PA.graph $N 3
	echo syn $N "hybridcut"
	./hybridcut synthesized-1b.graph $N 13
	echo twit $N "hybridcut"
	./hybridcut twitter-2010.graph $N 100
done

#shuffle lines (remember to download and make terashuf from https://github.com/alexandres/terashuf)
xxd -p -c 8 roadNet-PA.graph | shuf -| xxd -p -r - roadNet-PA_shuffled.graph
xxd -p -c 8 synthesized-1b.graph | shuf -| xxd -p -r - synthesized-1b_shuffled.graph
xxd -p -c 8 twitter-2010.graph | ./terashuf -| xxd -p -r - twitter-2010_shuffled.graph

shuffled_graphs_array=(roadNet-PA_shuffled.graph synthesized-1b_shuffled.graph twitter-2010_shuffled.graph)
for N in "${N_array[@]}"
do
	for file in "${shuffled_graphs_array[@]}"
	do
		echo $file $N "greedycut"
		./greedycut $file $N
	done
done