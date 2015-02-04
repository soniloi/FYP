//#include "llvm/ADT/Statistic.h" // FIXME: this is in Hello pass sample code, do we need it?
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

#include <random>
#include <time.h>

using namespace llvm;

//#define DEBUG_TYPE "func-reorder" // FIXME: do we need this?

//STATISTIC(FuncReorderCounter, "Reorders functions within a module at random"); // FIXME: do we need this?

namespace {
	struct FuncReorder : public ModulePass {
		static char ID; // Pass identification, replacement for typeid
		FuncReorder() : ModulePass(ID) {}

		bool runOnModule(Module &M) override {
			bool modified = false;

			errs() << "FuncReorder: ";
			errs().write_escaped(M.getName()) << '\n';

			// Initialize RNG
			std::mt19937 rng;
			rng.seed(time(NULL)); // FIXME: use seed passed from command-line
			std::uniform_int_distribution<uint32_t> dist(0, 1); // Restrict range to 0-1

			// Move some functions to start of module at random
			iplist<Function> &funcs = M.getFunctionList();
			iplist<Function>::iterator it = funcs.begin();
			while(it != funcs.end()){
				Function &F = (*it);
				it++;

				errs() << '\t';
				errs().write_escaped(F.getName());

				int chance = dist(rng);
				if(chance == 0){
					F.removeFromParent(); // Unlink function from parent module, without removing it
					funcs.push_front(&F); // Put function at start of module
					errs() << "\tSending to start.";
					modified = true;
				}
				errs() << '\n';
			}

			errs() << "-------\n";
			return modified;
		}
	};
}

char FuncReorder::ID = 0; // Initialize pass ID
static RegisterPass<FuncReorder> X("func-reorder", "Function Reordering Pass", true, false); // FIXME: check boolean params

