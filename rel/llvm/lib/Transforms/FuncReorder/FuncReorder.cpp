#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h" // For passing command-line params
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

#include <random>
#include <vector>

using namespace llvm;

#define DEBUG_TYPE "func-reorder"

static std::mt19937 rng;
static bool set = false;

static cl::opt<unsigned>
RandomSeed("rnd-seed", cl::desc("Seed used to generate pseudo-randomness"), cl::value_desc("seed value"));

namespace {

	struct FuncReorder : public ModulePass {
		static char ID; // Pass identification, replacement for typeid
		FuncReorder() : ModulePass(ID) {}

		// Return a random integer from a uniformly-distributed interval (start, end)
		int uniform(uint start, uint end, std::mt19937 &rng){
			std::uniform_int_distribution<uint32_t> dist(start, end);
			return dist(rng);
		}

		bool runOnModule(Module &M) override {
			bool modified = false;

			// Initialize RNG
			if(!set){
				unsigned int sd = RandomSeed;
				rng.seed(sd);
				set = true;
			}

			DEBUG(errs() << "FuncReorder seed: " << RandomSeed << '\n');

			iplist<Function> &funcs = M.getFunctionList();
			std::vector<Function*> funcs_tmp;

			// Remove functions from list in sequence
			iplist<Function>::iterator it = funcs.begin();
			while(it != funcs.end()){
				Function * F = it;
				it++;
				DEBUG(errs() << "\tfound: ");
				DEBUG(errs().write_escaped(F->getName()) << '\n');
				F->removeFromParent();
				funcs_tmp.push_back(F);
			}

			// Insert functions back into list in randomized order
			while(funcs_tmp.size() > 0){
				int index = uniform(0, funcs_tmp.size()-1, rng); // Choose one at random from the remaining list
				Function * F = funcs_tmp[index];
				funcs_tmp.erase(funcs_tmp.begin() + index);
				funcs.push_back(F);
				DEBUG(errs() << "\tpushing: ");
				DEBUG(errs().write_escaped(F->getName()) << '\n');
				if(index > 0)
					modified = true;
			}
		
			DEBUG(errs() << "-------\n");

			return modified;
		}
	};
}

char FuncReorder::ID = 0; // Initialize pass ID
static RegisterPass<FuncReorder> X("func-reorder", "Function Reordering Pass", true, false);
