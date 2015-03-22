#include <algorithm>
#include <cstdlib>
#include <dirent.h>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

const string funcdir_prog = "./funcs";
const string funcdir_smash = "./smashfuncs";

// Create array for which every value is the same as its index
vector<int> identity_array(int len){
	vector<int> result;
	for(int i = 0; i < len; i++){
		result.push_back(i);
	}
	return result;
}

// Get a pseudo-random permutation of an identity array of a certain length
vector<int> get_permutation(int len, std::mt19937 &rng){
	vector<int> result = identity_array(len);
	shuffle(result.begin(), result.end(), rng);
	return result;
}

// Get sorted relative list of filenames from a directory
vector<string> get_filenames(string dirpath){
	vector<string> filearr;

	DIR * dir;
	struct dirent * ent;

	dir = opendir(dirpath.c_str());
	if(dir){
		while((ent = readdir(dir))){
			string dname = ent->d_name;
			if(dname.compare(".") && dname.compare("..")){
				stringstream filepath;
				filepath << dirpath << '/' << dname;
				filearr.push_back(filepath.str());
			}
		}
	}

  std::sort(filearr.begin(), filearr.end());
  return filearr;
}

int main(){

	uint initial_seed_choices = 13;
	uint max_arrangements = 3;
	uint max_choices = 4;

	vector<string> funcarr_prog = get_filenames(funcdir_prog);
	vector<string> funcarr_smash = get_filenames(funcdir_smash);

	std::mt19937 rng_choices;
	rng_choices.seed(initial_seed_choices);

	vector<vector<int> > arrangement_choices;
	for(int j = 0; j < max_arrangements; j++){
		vector<int> choices = get_permutation(funcarr_prog.size(), rng_choices);
		arrangement_choices.push_back(choices);
	}

	for(int i = 1; i < max_choices; i++){ // i is the number of functions we will be selecting

		cout << "---" << endl;
		for(int j = 0; j < max_arrangements; j++){ // j is each test with the i functions
			for(int k = 0; k < i; k++){
				cout << arrangement_choices[j][k] << " (" << funcarr_prog[arrangement_choices[j][k]] << ") ";
			}
			cout << endl;
			/*
				 Select i functions from funcs at random
						count the number of available funcs
							list directory (will need this anyway)
						get a unique unbiased distribution
			*/

			/* Shuffle i (together with main and spawn_shell)
						another unique unbiased distribution
			*/ 

			/*
				Some linear stuff
					compile (including the common IR copy)
					calculate overflow payloads
					run base tests
			*/

			/*
				for optimization in optimizations:
					for(int k = 0; k < 3; k++){ // The number of times to run each optimization
						run optimization
						run tests
					}
					compare optimizations, normalizing
			*/
		}

	}

	return 0;
}
