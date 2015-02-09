//#include "llvm/ADT/Statistic.h" // FIXME: this is in Hello pass sample code, do we need it?
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

#include <vector>
#include <random>
#include <time.h>

using namespace llvm;

static std::mt19937 rng;
static bool set = false;

//#define DEBUG_TYPE "func-reorder" // FIXME: do we need this?

//STATISTIC(FuncReorderCounter, "Reorders functions within a module at random"); // FIXME: do we need this?

namespace {
	struct FuncReorder : public ModulePass {
		static char ID; // Pass identification, replacement for typeid
		FuncReorder() : ModulePass(ID) {}

		bool runOnModule(Module &M) override {
			bool modified = false;

			// Initialize RNG
			if(!set){
				rng.seed(time(NULL)); // FIXME: use seed passed from command-line
				set = true;
			}

			errs() << "FuncReorder: ";
			errs().write_escaped(M.getName()) << '\n';

			iplist<Function> &funcs = M.getFunctionList();
			std::vector<Function*> funcs_tmp;

			// Remove functions from list in sequence
			iplist<Function>::iterator it = funcs.begin();
			while(it != funcs.end()){
				Function * F = it;
				it++;
				errs() << "\tfound: ";
				errs().write_escaped(F->getName()) << '\n';
				F->removeFromParent();
				funcs_tmp.push_back(F);
			}

			// Insert functions back into list in random order
			while(funcs_tmp.size() > 0){
				std::uniform_int_distribution<uint32_t> dist(0, funcs_tmp.size()-1);
				int index = dist(rng);
				Function * F = funcs_tmp[index];
				funcs_tmp.erase(funcs_tmp.begin() + index);
				funcs.push_back(F);
				errs() << "\tpushing: ";
				errs().write_escaped(F->getName()) << '\n';
				if(index > 0)
					modified = true;
			}
			
			errs() << "-------\n";
			return modified;
		}
	};
}

char FuncReorder::ID = 0; // Initialize pass ID
static RegisterPass<FuncReorder> X("func-reorder", "Function Reordering Pass", true, false); // FIXME: check boolean params

