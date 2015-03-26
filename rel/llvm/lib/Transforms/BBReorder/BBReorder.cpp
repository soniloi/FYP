#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h" // For passing command-line params
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

#include <random>
#include <vector>

using namespace llvm;

#define DEBUG_TYPE "bb-reorder"

static std::mt19937 rng;
static bool set = false;

static cl::opt<unsigned>
RandomSeed("rnd-seed", cl::desc("Seed used to generate pseudo-randomness"), cl::value_desc("seed value"));

namespace {

	struct BBReorder : public FunctionPass {
		static char ID; // Pass identification, replacement for typeid
		BBReorder() : FunctionPass(ID) {}

		// Return a random integer from a uniformly-distributed interval start, end
		int uniform(uint start, uint end, std::mt19937 &rng){
			std::uniform_int_distribution<uint32_t> dist(start, end);
			return dist(rng);
		}

		bool runOnFunction(Function &F) override {
			bool modified = false;

			// Initialize RNG
			if(!set){
				unsigned int sd = RandomSeed;
				rng.seed(sd);
				set = true;
			}

			DEBUG(errs() << "BBReorder seed: " << RandomSeed);
			DEBUG(errs().write_escaped(F.getName()) << '\n');

			iplist<BasicBlock> &blocks = F.getBasicBlockList();
			std::vector<BasicBlock*> blocks_tmp;

			// Remove blocks from list in sequence
			iplist<BasicBlock>::iterator it = blocks.begin();
			if(it != blocks.end())
				it++; // Entry block has to stay where it is
			while(it != blocks.end()){
				BasicBlock * BB = it;
				it++;
				DEBUG(errs() << "\tfound: ");
				DEBUG(errs().write_escaped(BB->getName()) << '\n');
				BB->removeFromParent();
				blocks_tmp.push_back(BB);
			}

			// Insert blocks back into list in random order
			while(blocks_tmp.size() > 0){
				int index = uniform(0, blocks_tmp.size()-1, rng);
				BasicBlock * BB = blocks_tmp[index];
				blocks_tmp.erase(blocks_tmp.begin() + index);
				blocks.push_back(BB);
				DEBUG(errs() << "\tpushing: ");
				DEBUG(errs().write_escaped(BB->getName()) << '\n');
				if(index > 0)
					modified = true;
			}

			DEBUG(errs() << "-------\n");

			return modified;
		}
	};
}

char BBReorder::ID = 0; // Initialize pass ID
static RegisterPass<BBReorder> X("bb-reorder", "Basic Block Reorder Pass", true, false); // FIXME: check boolean params

