//#include "llvm/ADT/Statistic.h" // FIXME: this is in Hello pass sample code, do we need it?
//#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/RandomNumberGenerator.h"

#include <random>
#include <time.h>

using namespace llvm;

//#define DEBUG_TYPE "alloc-insert" // FIXME: do we need this?

//STATISTIC(AllocInsertCounter, "Allocates dummy local variables at random"); // FIXME: do we need this?

//uncomment when enabling seed passed from command-line
//static cl::opt<unsigned>
//RandomSeed("random-seed", cl::desc("Seed used to generate pseudo-randomness"), cl::value_desc("seed value"));

namespace {
	struct AllocInsert : public FunctionPass {
		static char ID; // Pass identification, replacement for typeid
		AllocInsert() : FunctionPass(ID) {}

		bool runOnFunction(Function &F) override {
			bool modified = false;

			// Initialize RNG
			std::mt19937 rng;
			rng.seed(time(NULL)); // FIXME: use seed passed from command-line
			std::uniform_int_distribution<uint32_t> dist(0, 3); // Restrict range to 0-3 FIXME: tweak this as necessary

			errs() << "AllocInsert: ";
			errs().write_escaped(F.getName()) << '\n';

			BasicBlock &BB = F.getEntryBlock();
			Instruction &I = *(BB.getFirstNonPHI()); // First instruction in block that is not a PHINode instruction

			Type * Int32Type = IntegerType::getInt32Ty(getGlobalContext());

			// Insert a (restricted) random number of alloca instructions at the start of the function
			int its = dist(rng);
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

