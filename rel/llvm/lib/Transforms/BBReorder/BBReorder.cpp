//#include "llvm/ADT/Statistic.h" // FIXME: this is in Hello pass sample code, do we need it?
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/ValueSymbolTable.h" // For dumping symbol table values
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/RandomNumberGenerator.h"

#include <random>
#include <time.h>
#include <vector>

using namespace llvm;

//#define DEBUG_TYPE "bb-reorder" // FIXME: do we need this?

//STATISTIC(BBReorderCounter, "Reorders basic blocks at random"); // FIXME: do we need this?

//uncomment when enabling seed passed from command-line
//static cl::opt<unsigned>
//RandomSeed("random-seed", cl::desc("Seed used to generate pseudo-randomness"), cl::value_desc("seed value"));

static std::mt19937 rng;
static std::uniform_int_distribution<uint32_t> dist(0, 3); // Restrict range to 0-3 FIXME: tweak this as appropriate
static bool set = false;

namespace {

	struct BBReorder : public FunctionPass {
		static char ID; // Pass identification, replacement for typeid
		BBReorder() : FunctionPass(ID) {}

		bool runOnFunction(Function &F) override {
			bool modified = false;

			if(!set){
				rng.seed(time(NULL));
				set = true;
			}

			errs() << "BBReorder: ";
			errs().write_escaped(F.getName()) << '\n';

			iplist<BasicBlock> &blocks = F.getBasicBlockList();
			std::vector<BasicBlock*> blocks_tmp;

			// Remove blocks from list in sequence
			iplist<BasicBlock>::iterator it = blocks.begin();
			if(it != blocks.end())
				it++; // Entry block has to stay where it is
			while(it != blocks.end()){
				BasicBlock * BB = it;
				it++;
				errs() << "\tfound: ";
				errs().write_escaped(BB->getName()) << '\n';
				BB->removeFromParent();
				blocks_tmp.push_back(BB);
			}

			// Insert blocks back into list in random order
			while(blocks_tmp.size() > 0){
				std::uniform_int_distribution<uint32_t> dist(0, blocks_tmp.size()-1);
				int index = dist(rng);
				BasicBlock * BB = blocks_tmp[index];
				blocks_tmp.erase(blocks_tmp.begin() + index);
				blocks.push_back(BB);
				errs() << "\tpushing: ";
				errs().write_escaped(BB->getName()) << '\n';
				if(index > 0)
					modified = true;
			}

							errs() << "-------\n";
			return modified;
		}
	};
}

char BBReorder::ID = 0; // Initialize pass ID
static RegisterPass<BBReorder> X("bb-reorder", "Basic Block Reorder Pass", true, false); // FIXME: check boolean params

