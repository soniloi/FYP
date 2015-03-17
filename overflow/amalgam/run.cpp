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

#define MAX_BUFFER 128

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

const string binbasename = "humpty";
const string fileoutbasepath = "humpty.c";

const string wd = "./";
const string ctoir = "CToIR.sh";
const string irtobin = "IRToBin.sh";
const string overflow = "overflow.sh";

/*
const string smashcheck = "smashcheck.sh";
const string sizecheck = "sizecheck.sh";
const string retiredcheck = "retiredcheck.sh";
const string heapcheck = "heapcheck.sh";
*/

const string link = "-lpthread -ldl";
const string libfn = "_IO_putc";
const string bufsize = "2031";
const string samplein = "sample1.sql"; // Sample file used in testing

const int testrun_count = 3; // The number of times each randomization pass is to be run

static map<string, string> checkscripts =
	{{"smashed", "smashcheck.sh"},
	 {"size", "sizecheck.sh"},
	 {"retired", "retiredcheck.sh"},
	 {"heap", "heapcheck.sh"}};

class ResultBundle{
private:
	std::map<string, int> metrics;
public:
	void set(string key, int value);
	int get(string key);
	string to_string();
};

void ResultBundle::set(string key, int value){
	this->metrics[key] = value;
}

int ResultBundle::get(string key){
	return this->metrics[key];
}

string ResultBundle::to_string(){
	stringstream ss;
	for(map<string, int>::iterator it = this->metrics.begin(); it != this->metrics.end(); it++){
		ss << it->first << ": " << it->second << ' ';
	}
	return ss.str();
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

// Get a pseudo-randomly generated array of seeds
vector<int> get_seed_array(int len, std::mt19937 rng){
	vector<int> possible_seeds = identity_array(16777215);
	shuffle(possible_seeds.begin(), possible_seeds.end(), rng);
	vector<int> result;
	for(int i = 0; i < len; i++)
		result.push_back(possible_seeds[i]);
	return result;
}

// Call the compilation pipeline, including randomization passes where requested
void compile_pipeline(string daa, string binname, int seed, vector<string> optflags){
	stringstream command;
	command << wd << irtobin << ' ' << daa << ' ' << binname << ' ' << seed << " '" << link << "' ";
	for(vector<string>::iterator it = optflags.begin(); it != optflags.end(); it++)
		command << (*it) << ' ';
	//cout << command.str() << endl;
	system(command.str().c_str());
}

// Run an external command and return its output as a string
string check_output(string command){
	string result = "";
	char buffer[MAX_BUFFER];
	FILE * pipe = popen(command.c_str(), "r");
	while(!feof(pipe)){
		if(fgets(buffer, MAX_BUFFER, pipe))
			result += buffer;
	}
	pclose(pipe);
	return result;
}

// Run tests
ResultBundle run_tests(string binname){
	ResultBundle rbundle;

	for(map<string, string>::iterator it = checkscripts.begin(); it != checkscripts.end(); it++){
		stringstream command;
		command << wd << it->second << ' ' << binname << ' ' << wd << samplein;
		cout << command.str() << endl;
		string output = check_output(command.str());
		rbundle.set(it->first, atoi(output.c_str()));
	}
/*
	stringstream smashed_command;
	smashed_command << "./" << smashcheck << ' ' << binname;
	cout << smashed_command.str() << endl;
	string smashed_output = check_output(smashed_command.str());
	rbundle.set("smashed", atoi(smashed_output.c_str()));

	stringstream size_command;
	size_command << "./" << sizecheck << ' ' << binname;
	cout << sizecheck.str() << endl;
	string size_output = check_output(size_command.str());
	rbundle.set("size", atoi(size_output.c_str()));
*/
	return rbundle;
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
		stringstream version_number;
		version_number << "version-" << version;

		stringstream version_heading;
		version_heading << endl << "[=== Source " << version_number.str() << " ===]" << endl;
		cout << version_heading.str();

		std::mt19937 rng_permutations_local;
		rng_permutations_local.seed(permutation_seeds[version-1]);

		stringstream dirout;
		dirout << wd << version_number.str();

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
		stringstream binname;
		binname << dirout.str() << '/' << binbasename;

		// Compile to IR (needed for both normal and randomized versions)
		stringstream run_ctoir;
		run_ctoir << wd << ctoir << ' ' << daa << ' ' << binname.str();
		cout << run_ctoir.str() << endl;
		system(run_ctoir.str().c_str());

		// Compile base version without randomization
		vector<string> dummy;
		compile_pipeline(daa, binname.str(), 0, dummy); // Pass a zero seed, because there are no randomization passes

		// Run overflow script on base version
		stringstream run_overflow;
		run_overflow << wd << overflow << ' ' << binname.str() << ' ' << libfn << ' ' << bufsize;
		cout << run_overflow.str() << endl;
		system(run_overflow.str().c_str());

		ResultBundle results = run_tests(binname.str());
		cout << version_number.str() << ": " << results.to_string() << endl;

	}

	return 0;
}
