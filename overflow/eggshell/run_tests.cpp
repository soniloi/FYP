#include <algorithm>
#include <cstdlib>
#include <dirent.h>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

const string funcdir_prog = "./funcs";
const string funcdir_smash = "./smashfuncs";

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

	vector<string> funcarr_prog = get_filenames(funcdir_prog);
	vector<string> funcarr_smash = get_filenames(funcdir_smash);

	for(auto &it : funcarr_prog){
		cout << it << endl;
	}

	for(auto &it : funcarr_smash){
		cout << it << endl;
	}

	for(int i = 1; i < 2; i++){ // i is the number of functions we will be selecting
		for(int j = 0; j < 1; j++){ // j is each test with the i functions
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
