#include <algorithm> // shuffle, sort
#include <dirent.h> // directory operations
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <random>
#include <sstream>
#include <vector>

using namespace std;

const string usage = "run <path-to-build-tree> <total-generations> <initial-seed>";
const string sep = "/*****************************************************************************/";

const string original = "./amalgamation.c";
const string headerpath = "./header.c";
const string macropath = "./macros.c";
const string funcdir = "./funcs";
const string prototypespath = "./prototypes.lst";
const string protogen = "cproto";
const string protogenopt = "-si";

const string binname = "./humpty";
const string fileoutpath = "./humpty.c";

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
void cat_file(ofstream &fileout, string filepathin, string label){
	output_heading(fileout, label);
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
	ofstream fileout(fileoutpath);

	// Output 'header'
	cat_file(fileout, headerpath, "Global Declarations");

	// Concatenate forward declarations
	cat_file(fileout, prototypespath, "Forward Declarations");

	// Concatenate macros
	cat_file(fileout, macropath, "Macros");

	fileout.close();
}

int main(int argc, char ** argv){

	// Parse args
	if(argc != 4){
		cout << usage << endl;
		exit(1);
	}
	string daa = argv[1]; // Path to Debug+Asserts build
	int generation_count = atoi(argv[2]); // Number of permutations of source to generate
	int initial_seed = atoi(argv[3]);

	// Generate common file listing function prototypes
	generate_prototypes();

	// Retrieve sorted list of function filenames
	vector<string> funcarr = get_filenames(funcdir);
	for(vector<string>::iterator it = funcarr.begin(); it != funcarr.end(); it++){
		cout << (*it) << endl;
	}
	cout << "Total number of files: " << funcarr.size() << endl;

	// Construct identity array of the appropriate size
	vector<int> identity = identity_array(funcarr.size());
	
	// Initialize RNG
	std::mt19937 rng;
	std::uniform_int_distribution<uint32_t> dist(0, 16777215);
	rng.seed(initial_seed);

	vector<int> index_array = generate_random_permutation(identity, rng);
	for(int i = 0; i < index_array.size(); i++){
		cout << i << ": " << index_array[i] << endl;
	}

	int version = 0;
	concat_source(fileoutpath, headerpath, prototypespath, macropath, funcarr, index_array);
//void concat_source(string fileoutpath, string headerpath, string prototypespath, string macropath, vector<string> funcarr, vector<int> indarr){

	return 0;
}
