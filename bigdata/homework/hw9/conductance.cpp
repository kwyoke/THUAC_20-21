/*
Copyright (c) 2014-2015 Xiaowei Zhu, Tsinghua University

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "core/graph.hpp"

int main(int argc, char ** argv) {
	if (argc<2) {
		fprintf(stderr, "usage: conductance [path] [memory budget in GB]\n");
		exit(-1);
	}
	std::string path = argv[1];
	long memory_bytes = (argc>=3)?atol(argv[2])*1024l*1024l*1024l:8l*1024l*1024l*1024l;

	Graph graph(path);
	graph.set_memory_bytes(memory_bytes);

	long vertex_data_bytes = (long)graph.vertices * ( sizeof(VertexId) + sizeof(float) + sizeof(float) );
	graph.set_vertex_data_bytes(vertex_data_bytes);

	double begin_time = get_time();

	int crossover_count, red_count, black_count, count;
	crossover_count = red_count = black_count = count = 0;

	graph.stream_edges<VertexId>(
		[&](Edge & e){
			if ((e.source&1) != (e.target&1)) {
				write_add(&crossover_count, 1);
			}
			else if ((e.source&1) !=0) {
				write_add(&red_count, 1);
			}
			else {
				write_add(&black_count, 1);
			}

			write_add(&count, 1);
			return 0;
		}, nullptr, 0, 0
	);

	float conductance;
	if (red_count < black_count) {
		conductance = (float)crossover_count/(float)red_count;
	}
	else {
		conductance = (float)crossover_count/(float)black_count;
	}

	printf("conductance: %f, cross_count: %d, red_count: %d, black_count: %d, count: %d\n", conductance, crossover_count, red_count,black_count, count);
	printf("conductance calculation used %.2f seconds\n", get_time() - begin_time);
}
