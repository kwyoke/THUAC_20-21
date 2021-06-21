#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <set>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

typedef int VertexId;

int main(int argc, char **argv) {
    if (argc < 4) {
        fprintf(stderr, "usage: [executable] [input] [numpartitions] [thresh]\n");
        exit(-1);
    }
    
	int N = atoi(argv[2]);
	int thresh = atoi(argv[3]);
	FILE *fin;

	// Create and open a text file
	std::string inFileName, outFileName;
	inFileName = argv[1];
	outFileName = inFileName.substr(0, inFileName.size() - 6) + std::to_string(N) + "_hybridcut.txt";
	std::ofstream MyFile(outFileName);
	
	std::vector<unsigned long int> all_stats(N*4, 0); //partition ID, numMaster, totV, numEdges

	
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

	printf("MaxV: %d \n", maxV);

	//initialise vertex location vector
	std::vector<std::set<int>> A;
	for (int i=0; i<=maxV; i++) {
		std::set<int> tmp;
		A.push_back(tmp);
	}

	//second pass: calculate vertex in-degrees
	std::vector<int> inDeg(maxV + 1, 0); //vertices might start from 0 to maxV
	fin = fopen(argv[1], "rb");
	while (true) {
		VertexId src, dst;
		if (fread(&src, sizeof(src), 1, fin) == 0) break;
		if (fread(&dst, sizeof(dst), 1, fin) == 0) break;

		inDeg[dst] ++;
	}
	fclose(fin);

	//third and final pass: allocate edge and vertices
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

		//differentiated edge assignment based on dst in-degree
		if (inDeg[dst] <= thresh) {
			A[src].insert(dstLoc);
			A[dst].insert(dstLoc);
			all_stats[4*dstLoc + 3] ++; //add edge
		}
		else {
			A[src].insert(srcLoc);
			A[dst].insert(srcLoc);
			all_stats[4*srcLoc + 3] ++;
		}
		
	}
	fclose(fin);

	//update number of vertices in each partition
	std::set<int>::iterator it;
	for (int i=0; i<=maxV; i++) {
		if (!A[i].empty()) {
			int partition = i%N;
			all_stats[4*partition + 1] ++; //add master vertex
			for (it = A[i].begin(); it != A[i].end(); ++it) {
				int partition = *it;
				all_stats[4*partition + 2] ++; //add vertex
			}
		}
	}
    
	for (int i=0; i<N; i++) {
		unsigned long int numMasterV = all_stats[i*4 + 1];
		unsigned long int numTotV = all_stats[i*4 + 2];
		unsigned long int numEdges = all_stats[i*4 + 3];
		printf("Partition %d : \n numMasterV : %lu \n numTotV: %lu \n numEdges: %lu \n \n", i, numMasterV, numTotV, numEdges);
		// Write to the file
		MyFile << "Partition " + std::to_string(i) +  "\n" + std::to_string(numMasterV) + "\n" + std::to_string(numTotV) + "\n" + std::to_string(numEdges) + "\n";
		
	}

	MyFile.close();
	
    return 0;
}