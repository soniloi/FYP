#include <algorithm>
#include <cstdlib>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

const string sep = "/*****************************************************************************/";

const string funcdir_prog = "./funcs";
const string funcdir_smash = "./smashfuncs";
const string headerpath = "./header.c";
const string macropath = "./macros.c";
const string prototypespath = "./prototypes.lst";
const string fileout_basename = "humpty.c";

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

// Output a formatted heading to file
void output_heading(ofstream &fileout, string label){
	fileout << endl << endl << sep << endl << "/** " << label << " **/" << endl << sep << endl << endl;
}

// Concatenate file at filepathin onto fileout
void cat_file(ofstream &fileout, string filepathin, string label=""){
	if(!label.empty())
		output_heading(fileout, label);
	else
		fileout << endl << endl;
	ifstream filein(filepathin);
	if(filein){
		string line;
		while(getline(filein, line))
			fileout << line << endl;
		filein.close();
	}
}

// Concatenate constituents of a source file into complete source file at fileoutpath
void concat_source(string fileoutpath, string headerpath, string prototypespath, string macropath, vector<string> funcarr){
	ofstream fileout(fileoutpath);

	// Output 'header'
	cat_file(fileout, headerpath, "Global Declarations");

	// Concatenate forward declarations
	cat_file(fileout, prototypespath, "Forward Declarations");

	// Concatenate macros
	cat_file(fileout, macropath, "Macros");

	//Concatenate functions in specified order
	output_heading(fileout, "Functions");
	int len = funcarr.size();
	for(int i = 0; i < len; i++){
		cat_file(fileout, funcarr[i]);
	}

	fileout.close();
}


int main(){

	uint initial_seed_choices = 13;
	uint max_arrangements = 3;
	uint max_choices = 2;

	vector<string> funcarr_prog = get_filenames(funcdir_prog);
	vector<string> funcarr_smash = get_filenames(funcdir_smash);

	std::mt19937 rng_choices;
	rng_choices.seed(initial_seed_choices);

	vector<vector<int> > arrangement_choices;
	for(int j = 0; j < max_arrangements; j++){
		vector<int> choices = get_permutation(funcarr_prog.size(), rng_choices);
		arrangement_choices.push_back(choices);
	}

	for(int i = 0; i <= max_choices; i++){ // i is the number of functions we will be selecting

		cout << "---" << endl;
		for(int j = 0; j < max_arrangements; j++){ // j is each test with the i functions
			// Version numbering
			stringstream version_ss;
			version_ss << "version-" << i << '-' << (j+1);
			string version = version_ss.str();
			cout << version << endl;

			// Directory and file paths
			string dirout(version);
			dirout.append("/");
			string command_mkdir("mkdir -p ");
			command_mkdir.append(dirout);
			system(command_mkdir.c_str());
			string fileout(dirout);
			fileout.append(fileout_basename);

			// Function arrangement
			vector<string> arrangement;	
			for(int k = 0; k < i; k++)
				arrangement.push_back(funcarr_prog[arrangement_choices[j][k]]);
			for(auto &it : funcarr_smash)
				arrangement.push_back(it);

			for(auto &it : arrangement)
				cout << it << ' ';

			// FIXME: shuffle the arrangement

			concat_source(fileout, headerpath, prototypespath, macropath, arrangement);

			cout << endl;

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
