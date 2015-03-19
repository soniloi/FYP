#include <algorithm> // shuffle, sort
#include <assert.h>
#include <cfloat> // MAX_DBL stuff
#include <climits> // MAX_INT stuff
#include <cstdlib>
#include <dirent.h> // directory operations
#include <fstream>
#include <iomanip> // cout formatting stuff
#include <iostream>
#include <map>
#include <random>
#include <sstream>
#include <vector>

#define MAX_BUFFER 128
#define TAB "        "

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

const string link = "-lpthread -ldl";
const string libfn = "_IO_putc";
const string bufsize = "2031";
const string samplein = "sample1.sql"; // Sample file used in testing

const int baserun_test_count = 3; // The number of times each base version should have tests run; base statistics will be an average of these
const int testrun_count = 2; // The number of times each randomization pass is to be run

const string label_summary = "[summary] ";

static map<string, string> checkscripts =
	{{"smashed", "smashcheck.sh"},
	 {"size", "sizecheck.sh"},
	 {"retired", "retiredcheck.sh"},
	 {"heap", "heapcheck.sh"}};

static vector<string> optimizations =
	{"-alloc-insert",
	 "-func-reorder",
	 "-bb-reorder"};

static vector<string> metrics =
	{"smashed",
	 "size",
	 "retired",
	 "heap"};

class ResultBundle{
public:
	std::map<string, uint> metrics;
	void set(string key, uint value);
	uint get(string key);
	string to_string();
};

void ResultBundle::set(string key, uint value){
	this->metrics[key] = value;
}

uint ResultBundle::get(string key){
	return this->metrics[key];
}

string ResultBundle::to_string(){
	stringstream ss;
	for(map<string, uint>::iterator it = this->metrics.begin(); it != this->metrics.end(); it++){
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
	command.append(" 2>&1");
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

	// Overall result structure
	map<string, double> global_smashed_counts;
	map<string, map<string, map<string, double> > > global_metric_counts;
	for(vector<string>::iterator optimization = optimizations.begin(); optimization != optimizations.end(); optimization++){
		global_smashed_counts[(*optimization)] = 0.0;
		for(vector<string>::iterator metric = metrics.begin(); metric != metrics.end(); metric++){
			global_metric_counts[(*optimization)][(*metric)]["min"] = DBL_MAX;
			global_metric_counts[(*optimization)][(*metric)]["max"] = 0;
			global_metric_counts[(*optimization)][(*metric)]["tot"] = 0;
		}
	}

	#pragma omp parallel for
	for(int version = 1; version <= generation_count; version++){
		stringstream version_number, version_number_ss;
		version_number << "version-" << version;
		version_number_ss << "[" << version_number.str() << "] ";
		string label_version = version_number_ss.str();

		stringstream version_heading;
		version_heading << endl << label_version << endl;
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
		stringstream binname, fileoutir, fileoutasm;
		binname << dirout.str() << '/' << binbasename;
		fileoutir << binname.str() << ".ll.optimized";
		fileoutasm << binname.str() << ".s";
		//cout << "base: " << binname.str() << " ir: " << fileoutir.str() << " asm: " << fileoutasm.str() << endl;

		// Compile to IR (needed for both normal and randomized versions)
		stringstream run_ctoir, run_ctoir_ss;
		run_ctoir << wd << ctoir << ' ' << daa << ' ' << binname.str();
		run_ctoir_ss << label_version << endl << label_version << "=== common ===" << endl << label_version << run_ctoir.str() << endl;
		cout << run_ctoir_ss.str();
		system(run_ctoir.str().c_str());

		// Compile base version without randomization
		vector<string> dummy;
		compile_pipeline(daa, binname.str(), 0, dummy); // Pass a zero seed, because there are no randomization passes

		// Run overflow script on base version
		stringstream run_overflow_cmd, run_overflow_ss;
		run_overflow_cmd << wd << overflow << ' ' << binname.str() << ' ' << libfn << ' ' << bufsize;
		run_overflow_ss << label_version << run_overflow_cmd.str() << endl;
		cout << run_overflow_ss.str() << endl;
		system(run_overflow_cmd.str().c_str());

		// Run tests on base version
		ResultBundle base_results = run_tests(binname.str());
		stringstream base_results_ss;
		base_results_ss << label_version << endl << label_version << "=== base version tests ===" << endl;
		base_results_ss << label_version << TAB << "run 1: " << base_results.to_string() << endl;
		for(int i = 0; i < baserun_test_count-1; i++){
			ResultBundle run_base_results = run_tests(binname.str());
			base_results_ss << label_version << TAB << "run " << (i+2) << ": " << run_base_results.to_string() << endl;
			for(map<string, uint>::iterator it = base_results.metrics.begin(); it != base_results.metrics.end(); it++){
				if(it->first.compare("smashed")){
					it->second += run_base_results.get(it->first);
				}
			}
		}
		for(map<string, uint>::iterator it = base_results.metrics.begin(); it != base_results.metrics.end(); it++){
			if(it->first.compare("smashed")){
				it->second /= baserun_test_count;
			}
		}
		base_results_ss << label_version << "average: " << base_results.to_string() << endl;
		cout << base_results_ss.str();

		stringstream opt_results_heading_ss;
		opt_results_heading_ss << label_version << endl << label_version << "=== randomization passes === " << endl;
		cout << opt_results_heading_ss.str();

		// Result structure for this version
		map<string, map<int, ResultBundle> > version_results;

		for(vector<int>::iterator it = run_seeds.begin(); it != run_seeds.end(); it++){
			uint seed = (*it);
			stringstream seed_ss;
			seed_ss << label_version << endl;
			cout << seed_ss.str();
			// Compile and test with each randomization technique
			for(vector<string>::iterator jt = optimizations.begin(); jt != optimizations.end(); jt++){
				string optimization = (*jt);

				// Clear artefacts from previous optimizer runs
				stringstream run_clear;
				run_clear << "rm -f " << fileoutir.str() << ' ' << fileoutasm.str();
				system(run_clear.str().c_str());

				vector<string> opts = {optimization};
				compile_pipeline(daa, binname.str(), seed, opts);
				ResultBundle optresults = run_tests(binname.str());
				stringstream opt_ss;
				opt_ss << label_version << "seed = " << seed << ", optimization = " << optimization << ": " << optresults.to_string() << endl;
				cout << opt_ss.str();
				version_results[optimization][seed] = optresults;
			}
			
		}

		stringstream vsummary_ss;
		vsummary_ss << label_version << endl << label_summary << "=== version summary ===" << endl;

		for(map<string, map<int, ResultBundle> >::iterator it = version_results.begin(); it != version_results.end(); it++){
			string optimization = it->first;
			map<int, ResultBundle> version_bundles = it->second;
			stringstream rand_ss;
			rand_ss << endl << label_version << endl << label_version << "--- summary for " << optimization << " ---" << endl;
			cout << rand_ss.str();

			uint version_smashed_count = 0;
			map<string, map<string, uint> > version_metric_counts;
			for(map<int, ResultBundle>::iterator jt = version_bundles.begin(); jt != version_bundles.end(); jt++){
				int seed = jt->first;
				ResultBundle version_bundle = jt->second;
				stringstream seedresult_ss;
				seedresult_ss << label_version << "seed: " << seed << " result: " << version_bundle.to_string() << endl;
				cout << seedresult_ss.str();

				version_smashed_count += version_bundle.get("smashed");
				for(map<string, uint>::iterator kt = version_bundle.metrics.begin(); kt != version_bundle.metrics.end(); kt++){
					string metricname = kt->first;
					if(metricname.compare("smashed")){
						if(version_metric_counts.find(metricname) == version_metric_counts.end()){
							version_metric_counts[metricname]["min"] = UINT_MAX;
							version_metric_counts[metricname]["max"] = 0;
							version_metric_counts[metricname]["tot"] = 0;
						}
						uint metricvalue = version_bundle.get(metricname);
						if(metricvalue < version_metric_counts[metricname]["min"])
							version_metric_counts[metricname]["min"] = metricvalue;
						if(metricvalue > version_metric_counts[metricname]["max"])
							version_metric_counts[metricname]["max"] = metricvalue;
						version_metric_counts[metricname]["tot"] += metricvalue;
					}
				}
			}
			stringstream ss;
			ss << std::setprecision(12) << label_version << "aggregated: " << endl << label_version << TAB << "smashed: " << version_smashed_count << endl;
			for(map<string, map<string, uint> >::iterator jt = version_metric_counts.begin(); jt != version_metric_counts.end(); jt++){
				string metric = jt->first;
				uint min = jt->second["min"];
				uint max = jt->second["max"];
				double avg = jt->second["tot"]/run_seeds.size();
				double minrat = (double)min/base_results.get(metric);
				double maxrat = (double)max/base_results.get(metric);
				double avgrat = (double)avg/base_results.get(metric);
				ss << label_version << TAB << metric << ": min: " << min << " (" << minrat << ") max: " << max << " (" << maxrat << ") avg: " << avg << " (" << avgrat << ")" << endl;

				#pragma omp critical
				{
					if(minrat < global_metric_counts[optimization][metric]["min"])
						global_metric_counts[optimization][metric]["min"] = minrat;
					if(maxrat > global_metric_counts[optimization][metric]["max"])
						global_metric_counts[optimization][metric]["max"] = maxrat;
					global_metric_counts[optimization][metric]["tot"] += avgrat;
				}
			}
			cout << ss.str();
		}

	}

	cout << label_summary << endl << label_summary << "=== grand summary ===" << endl;
	cout << label_summary << "optimization\tsmashed?\tbin size\t\t\tretired\t\t\theap use" << endl;
	cout << label_summary << TAB << "\t\tmin\tmax\tavg\tmin\tmax\tavg\tmin\tmax\tavg" << endl;
	for(vector<string>::iterator optimization = optimizations.begin(); optimization != optimizations.end(); optimization++){
		cout << label_summary << (*optimization) << '\t' << global_smashed_counts[(*optimization)];
		for(vector<string>::iterator metric = metrics.begin(); metric != metrics.end(); metric++){
			if((*metric).compare("smashed")){
				cout << '\t' << global_metric_counts[(*optimization)][(*metric)]["min"] <<
								'\t' << global_metric_counts[(*optimization)][(*metric)]["max"] <<
								'\t' << (global_metric_counts[(*optimization)][(*metric)]["tot"]/generation_count);
			}
		}
		cout << endl;
	}

	return 0;
}
