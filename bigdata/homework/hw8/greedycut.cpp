#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <set>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <climits>
#include <iostream>
#include <fstream>
#include <string>

typedef int VertexId;

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "usage: [executable] [input] [numpartitions]\n");
        exit(-1);
    }
    
	
	int N = atoi(argv[2]);
	FILE *fin;

	// Create and open a text file
	std::string inFileName, outFileName;
	inFileName = argv[1];
	outFileName = inFileName.substr(0, inFileName.size() - 6) + std::to_string(N) + "_greedycut.txt";
	std::ofstream MyFile(outFileName);
	
	std::vector<unsigned long int> all_stats(N*4, 0); //partition ID, numMaster, totV, numEdges
	int numV;

	
	for (unsigned long int p=0; p<N; p++) {
		all_stats[p*4] = p;
	}
	
	//first pass: calculate max vertex
	int maxV = 0;
	fin = fopen(argv[1], "rb");
	while (true) {
		VertexId src, dst;
		if (fread(&src, sizeof(src), 1, fin) == 0) break;
		if (fread(&dst, sizeof(dst), 1, fin) == 0) break;

		if (src > maxV) {
			maxV = src;
		}
		if (dst > maxV) {
			maxV = dst;
		}
	}
	fclose(fin);

	printf("MaxV: %d", maxV);

	
	//initialise vertex location vector
	std::vector<std::set<int>> A;
	for (int i=0; i<=maxV; i++) {
		std::set<int> tmp;
		A.push_back(tmp);
	}


	unsigned long int count = 0;
	//put vertices and edges into each of the N partitions
	fin = fopen(argv[1], "rb");

	//go through all edges 
	while (true) {
		VertexId src, dst;
		if (fread(&src, sizeof(src), 1, fin) == 0) break;
		if (fread(&dst, sizeof(dst), 1, fin) == 0) break;

		count++;
		if (count%10000000 == 0) {
			printf("edge num: %lu", count);
		}

		std::vector<int> v_intersection;
		std::set_intersection(A[src].begin(), A[src].end(), A[dst].begin(), A[dst].end(), std::back_inserter(v_intersection));

		//4 cases for greedy heuristic
		//#case 1
		if (v_intersection.size() > 0) {
			int partition = v_intersection[0]; //arbitrary partition in intersection
			all_stats[4*partition + 3] ++; //add edge to partition
			//no need update A since vertices already in partition
		}
		//#case 2
		else if (!A[src].empty() && !A[dst].empty()) { //if both sets not empty, and no intersection
			unsigned long int min = ULONG_MAX;
			int minPart = 0;
			int minNode = src;

			std::set<int>::iterator it;
			for (it = A[src].begin(); it != A[src].end(); ++it) {
				int partition = *it;
				int numEdges = all_stats[4*partition + 3];
				if (numEdges < min) {
					min = numEdges;
					minPart = partition;
				}
			}

			for (it = A[dst].begin(); it != A[dst].end(); ++it) {
				int partition = *it;
				int numEdges = all_stats[4*partition + 3];
				if (numEdges < min) {
					min = numEdges;
					minPart = partition;
					minNode = dst; 
				}
			}

			all_stats[4*minPart + 3] ++; //add edge to partition
			if (minNode == src) {
				A[dst].insert(minPart);
			}
			else {
				A[src].insert(minPart);
			}
		}
		//#case 3
		else if (A[src].empty() && A[dst].empty()) { //if both sets empty
			unsigned long int min = ULONG_MAX;
			int minPart = 0;
			int minNode = src;

			for (int p=0; p<N; p++) {
				unsigned long int numEdges = all_stats[4*p + 3];
				if (numEdges < min) {
					min = numEdges;
					minPart = p;
				}
			}

			all_stats[4*minPart + 3] ++; //add edge to partition
			A[src].insert(minPart);
			A[dst].insert(minPart);
		}
		//#case 4
		else { // one empty, one not empty
			if (!A[src].empty() ) {
				int partition = *(A[src].begin());
				all_stats[4*partition + 3] ++; //add edge to partition
				A[dst].insert(partition);
			}
			else {
				int partition = *(A[dst].begin());
				all_stats[4*partition + 3] ++; //add edge to partition
				A[src].insert(partition);
			}
		}	
	}
	fclose(fin);

	//update number of vertices in each partition
	std::set<int>::iterator it;
	for (int i=0; i<=maxV; i++) {
		if (!A[i].empty()) {
			for (it = A[i].begin(); it != A[i].end(); ++it) {
				int partition = *it;
				all_stats[4*partition + 2] ++; //add vertex
			}

			int chooseMaster = rand()%A[i].size();
			it = A[i].begin();
			for (int j=0; j<chooseMaster; j++) {
				++it;
			}
			int masterPart = *it;
			all_stats[4*masterPart + 1] ++; //add master
		}
	}

    
	for (int i=0; i<N; i++) {
		unsigned long int numMasterV = all_stats[i*4 + 1];
		unsigned long int numTotV = all_stats[i*4 + 2];
		unsigned long int numEdges = all_stats[i*4 + 3];
		printf("Partition %d : \n numMasterV : %lu \n numTotV: %lu \n numEdges: %lu \n \n", i, numMasterV, numTotV, numEdges);
		
		MyFile << "Partition " + std::to_string(i) +  "\n" + std::to_string(numMasterV) + "\n" + std::to_string(numTotV) + "\n" + std::to_string(numEdges) + "\n";
	}

	MyFile.close();
	
    return 0;
}