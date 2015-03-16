#include <iostream>
#include <cstdlib>
#include <random>

using namespace std;

const string usage = "run <path-to-build-tree> <total-generations> <initial-seed>";

int main(int argc, char ** argv){

	if(argc != 4){
		cout << usage << endl;
		exit(1);
	}
	string daa = argv[1]; // Path to Debug+Asserts build
	int generation_count = atoi(argv[2]); // Number of permutations of source to generate
	int initial_seed = atoi(argv[3]);

	cout << "daa: " << daa << " generation_count: " << generation_count << " initial_seed: " << initial_seed << endl;

	std::mt19937 rng;
	std::uniform_int_distribution<uint32_t> dist(0, 16777215);
	rng.seed(initial_seed);

	/*
	cout << "s1: " << dist(rng) << endl;
	cout << "s2: " << dist(rng) << endl;
	cout << "s3: " << dist(rng) << endl;
	*/

	return 0;
}
