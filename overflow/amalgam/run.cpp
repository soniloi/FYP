#include <algorithm> // shuffle
#include <iostream>
#include <cstdlib>
#include <random>
#include <sstream>
#include <vector>

using namespace std;

const string usage = "run <path-to-build-tree> <total-generations> <initial-seed>";
const string original = "./amalgamation.c";
const string prototypespath = "./prototypes.lst";
const string protogen = "cproto";
const string protogenopt = "-si";

vector<int> generate_random_permutation(vector<int> original, std::mt19937 rng){
	vector<int> result(original);
	shuffle(result.begin(), result.end(), rng);
	return result;
}

// Generate file consisting of all prototypes of original C source file
void generate_prototypes(){
	// Generate prototypes file
	stringstream protocom;
	protocom << protogen << ' ' << protogenopt << ' ' <<  "-o " << prototypespath << ' ' << original;
	system(protocom.str().c_str());
}

// Create array for which every value is the same as its index
vector<int> identity_array(int len){
	vector<int> result;
	for(int i = 0; i < len; i++){
		result.push_back(i);
	}
	return result;
}

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

	vector<int> identity = identity_array(1800);
	
	vector<int> index_array = generate_random_permutation(identity, rng);
	for(int i = 0; i < index_array.size(); i++){
		cout << i << ": " << index_array[i] << endl;
	}

		generate_prototypes(); // Generate the common file listing function prototypes

	

	return 0;
}
