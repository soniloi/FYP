#include <algorithm> // shuffle, sort
#include <assert.h>
#include <dirent.h> // directory operations
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <map>
#include <random>
#include <sstream>
#include <vector>

using namespace std;

const string usage = "run <path-to-build-tree> <total-generations> <initial-permutation-seed> <initial-run-seed>";
const string sep = "/*****************************************************************************/";

const string original = "./amalgamation.c";
const string headerpath = "./header.c";
const string macropath = "./macros.c";
const string funcdir = "./funcs";
const string prototypespath = "./prototypes.lst";
const string protogen = "cproto";
const string protogenopt = "-si";

const string binname = "humpty";
const string fileoutbasepath = "humpty.c";

const string ctoir = "CToIR.sh";
const string irtobin = "IRToBin.sh";
const string overflow = "overflow.sh";

const int testrun_count = 3; // The number of times each randomization pass is to be run

class ResultBundle{
private:
	bool smashed;
	std::map<string, int> metrics;
public:
	ResultBundle();
	std::string to_string();
};

ResultBundle::ResultBundle(){
	this->smashed = false;
}

// Generate file consisting of all prototypes of original C source file
void generate_prototypes(){
	// Generate prototypes file
	stringstream protocom;
	protocom << protogen << ' ' << protogenopt << ' ' <<  "-o " << prototypespath << ' ' << original;
	system(protocom.str().c_str());
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

// Create array for which every value is the same as its index
vector<int> identity_array(int len){
	vector<int> result;
	for(int i = 0; i < len; i++){
		result.push_back(i);
	}
	return result;
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
void concat_source(string fileoutpath, string headerpath, string prototypespath, string macropath, vector<string> funcarr, vector<int> indarr){
	assert(funcarr.size() == indarr.size());	

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
		cat_file(fileout, funcarr[indarr[i]]);
	}

	fileout.close();
}

vector<int> get_seed_array(int len, std::mt19937 rng){
	vector<int> possible_seeds = identity_array(16777215);
	shuffle(possible_seeds.begin(), possible_seeds.end(), rng);
	vector<int> result;
	for(int i = 0; i < len; i++)
		result.push_back(possible_seeds[i]);
	return result;
}

int main(int argc, char ** argv){

	// Parse args
	if(argc != 5){
		cout << usage << endl;
		exit(1);
	}
	string daa = argv[1]; // Path to Debug+Asserts build
	int generation_count = atoi(argv[2]); // Number of permutations of source to generate
	int initial_seed_permutations = atoi(argv[3]);
	int initial_seed_runs = atoi(argv[4]);

	// Generate common file listing function prototypes
	generate_prototypes();

	// Retrieve sorted list of function filenames
	vector<string> funcarr = get_filenames(funcdir);

	// Construct identity array of the appropriate size
	vector<int> identity = identity_array(funcarr.size());
	
	// Initialize RNGs 
	std::mt19937 rng_permutations; // RNG for generating permutations
	rng_permutations.seed(initial_seed_permutations);
	vector<int> permutation_seeds = get_seed_array(generation_count, rng_permutations);
	//for(int i = 0; i < permutation_seeds.size(); i++)
	//	cout << "perm seed " << i << ": " << permutation_seeds[i] << endl;

	std::mt19937 rng_runs; // RNG for generating seeds to be passed to compiler randomization runs
	rng_runs.seed(initial_seed_runs);
	vector<int> run_seeds = get_seed_array(testrun_count, rng_runs);
	//for(int i = 0; i < run_seeds.size(); i++)
	//	cout << "run seed " << i << ": " << run_seeds[i] << endl;

	//#pragma omp parallel for
	for(int version = 1; version <= generation_count; version++){
		cout << endl << "[=== Source version " << version << " ===]" << endl;

		std::mt19937 rng_permutations_local;
		rng_permutations_local.seed(permutation_seeds[version]);

		stringstream dirout;
		dirout << "./version-" << version;

		stringstream dirmake;
		dirmake << "mkdir " << dirout.str();
		system(dirmake.str().c_str());

		stringstream fileoutpath;
		fileoutpath << dirout.str() << '/' << fileoutbasepath;

		vector<int> indexarray(identity);
		shuffle(indexarray.begin(), indexarray.end(), rng_permutations_local);
		/*
		for(int i = 0; i < indexarray.size(); i++){
			stringstream ss;
			ss << "v" << version << ": " << i << ": " << indexarray[i] << endl;
			cout << ss.str();
		}
		*/

		// Generate base source version
		concat_source(fileoutpath.str(), headerpath, prototypespath, macropath, funcarr, indexarray);

		// Compile to IR (needed for both normal and randomized versions)
		stringstream run_ctoir;
		run_ctoir << "./" << ctoir << ' ' << daa << ' ' << dirout.str() << '/' << binname;
		cout << run_ctoir.str() << endl;
		system(run_ctoir.str().c_str());

	}

	return 0;
}
