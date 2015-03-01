#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/ValueSymbolTable.h" // For dumping symbol table values
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h" // For passing command-line params
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/RandomNumberGenerator.h"

#include <random>

using namespace llvm;

//#define DEBUG_TYPE "alloc-insert" // FIXME: do we need this?

static cl::opt<unsigned>
RandomSeed("rnd-seed", cl::desc("Seed used to generate pseudo-randomness"), cl::value_desc("seed value"));

static std::mt19937 rng;
static std::uniform_int_distribution<uint32_t> dist(0, 3); // Restrict range to 0-3 FIXME: tweak this as appropriate
static bool set = false;

namespace {

	struct AllocInsert : public FunctionPass {
		static char ID; // Pass identification, replacement for typeid
		AllocInsert() : FunctionPass(ID) {}

		bool runOnFunction(Function &F) override {
			if(!set){
				unsigned int sd = RandomSeed;
				errs() << "seed: " << sd << "\n";
				rng.seed(sd);
				set = true;
			}

			bool modified = false;

			errs() << "AllocInsert: ";
			errs().write_escaped(F.getName()) << '\n';

			BasicBlock &BB = F.getEntryBlock();
			Instruction &I = *(BB.getFirstNonPHI()); // First instruction in block that is not a PHINode instruction

			Type * Int32Type = IntegerType::getInt32Ty(getGlobalContext());

			// Insert a (restricted) random number of alloca instructions at the start of the function
			int its = dist(rng);
			errs() << "its: " << its << '\n';
			if(its > 0){
				int i;
				for(i = 0; i < its; i++){
					AllocaInst * inst = new AllocaInst(Int32Type, nullptr, "");
					BB.getInstList().insert(I, inst);
					modified = true;
				}
			}
			errs() << "\tAllocating space for " << its << " dummy variables\n";

			errs() << "-------\n";
			return modified;
		}
	};
}

char AllocInsert::ID = 0; // Initialize pass ID
static RegisterPass<AllocInsert> X("alloc-insert", "Alloc Insert Pass", true, false); // FIXME: check boolean params

