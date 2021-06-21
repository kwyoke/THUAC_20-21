
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
	outFileName = inFileName.substr(0, inFileName.size() - 6) + std::to_string(N) + "_edgecut.txt";
	std::ofstream MyFile(outFileName);

	std::vector<unsigned long int> all_stats(N*5, 0); //partition ID, numMaster, totV, numRepEdges, numEdges

	
	for (unsigned long int p=0; p<N; p++) {
		all_stats[p*5] = p;
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

	//allocate edges and vertices
	unsigned long int count = 0;
	fin = fopen(argv[1], "rb");
	//go through all edges 
	while (true) {
		VertexId src, dst;
		if (fread(&src, sizeof(src), 1, fin) == 0) break;
		if (fread(&dst, sizeof(dst), 1, fin) == 0) break;
		count++;
		if (count%10000000 == 0) {
			printf("edge num: %lu \n", count);
		}

		int srcLoc = src%N;
		int dstLoc = dst%N;

		A[src].insert(srcLoc);
		A[dst].insert(dstLoc);

		if (srcLoc == dstLoc) { //edge gets inserted into partition srcLoc=dstLoc
			all_stats[5*srcLoc + 4] ++;
		}
		else { //edge gets inserted into both partitions srcLoc and dstLoc
			all_stats[5*srcLoc + 4] ++;
			all_stats[5*dstLoc + 4] ++;

			all_stats[5*srcLoc + 3] ++;
			all_stats[5*dstLoc + 3] ++;

			A[src].insert(dstLoc);
			A[dst].insert(srcLoc);
		}
	}
	fclose(fin);

	//update number of vertices in each partition
	std::set<int>::iterator it;
	for (int i=0; i<=maxV; i++) {
		if (!A[i].empty()) {
			int partition = i%N;
			all_stats[5*partition + 1] ++; //add master vertex
			for (it = A[i].begin(); it != A[i].end(); ++it) {
				int partition = *it;
				all_stats[5*partition + 2] ++; //add vertex
			}
		}
	}

	for (int i=0; i<N; i++) {
		unsigned long int numMasterV = all_stats[i*5 + 1];
		unsigned long int numTotV = all_stats[i*5 + 2];
		unsigned long int numRepEdges = all_stats[i*5 + 3];
		unsigned long int numEdges = all_stats[i*5 + 4];
		printf("Partition %d : \n numMasterV : %lu \n numTotV: %lu \n numRepEdges: %lu \n numEdges: %lu \n \n", i, numMasterV, numTotV, numRepEdges, numEdges);
		
		MyFile << "Partition " + std::to_string(i) +  "\n" + std::to_string(numMasterV) + "\n" + std::to_string(numTotV) + "\n" + std::to_string(numRepEdges) + "\n" + std::to_string(numEdges) + "\n";
	}

	MyFile.close();
	
    return 0;
}