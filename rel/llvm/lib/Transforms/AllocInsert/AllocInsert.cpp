#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/ValueSymbolTable.h" // For dumping symbol table values
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h" // For passing command-line params
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/RandomNumberGenerator.h"

#include <random>

using namespace llvm;

#define DEBUG_TYPE "alloc-insert"

static cl::opt<unsigned>
RandomSeed("rnd-seed", cl::desc("Seed used to generate pseudo-randomness"), cl::value_desc("seed value"));

static cl::opt<unsigned>
MaxAllocs("max-allocs", cl::desc("Maximum number of dummy variables that may be inserted"), cl::value_desc("max inserts value"));

static std::mt19937 rng;
//static std::uniform_int_distribution<uint32_t> dist(0, 3); // Restrict range to 0-3 FIXME: tweak this as appropriate
static bool set = false;

namespace {

	struct AllocInsert : public FunctionPass {
		static char ID; // Pass identification, replacement for typeid
		AllocInsert() : FunctionPass(ID) {}

		bool runOnFunction(Function &F) override {
			if(!set){
				unsigned int sd = RandomSeed;
				rng.seed(sd);
				set = true;
			}

			bool modified = false;

			BasicBlock &BB = F.getEntryBlock();
			Instruction &I = *(BB.getFirstNonPHI()); // First instruction in block that is not a PHINode instruction
			Type * Int32Type = IntegerType::getInt32Ty(getGlobalContext());

			// Insert a bounded random number of alloca instructions at the start of the function
			std::uniform_int_distribution<uint32_t> dist(0, MaxAllocs);
			int its = dist(rng);
			if(its > 0){
				int i;
				for(i = 0; i < its; i++){
					AllocaInst * inst = new AllocaInst(Int32Type, nullptr, "");
					BB.getInstList().insert(I, inst);
					modified = true;
				}
			}
			DEBUG(errs() << "AllocInsert: ");
			DEBUG(errs().write_escaped(F.getName()) << "\tAllocating space for " << its << " dummy variables\n");
			DEBUG(errs() << "-------\n");

			return modified;
		}
	};
}

char AllocInsert::ID = 0; // Initialize pass ID
static RegisterPass<AllocInsert> X("alloc-insert", "Alloc Insert Pass", true, false); // FIXME: check boolean params

