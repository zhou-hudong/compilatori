
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"


#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"


#include "llvm/Analysis/LoopInfo.h"

#include "llvm/IR/Dominators.h"
#include "llvm/ADT/BreadthFirstIterator.h"

#include "llvm/IR/CFG.h"
#include "llvm/Analysis/CFGPrinter.h"

#include "llvm/IR/Value.h"
#include "llvm/IR/Constants.h"

#include <unordered_set>


using namespace llvm;


  
/// Globale : insieme delle istruzioni identificate come loop invariant
static std::unordered_set<Instruction*> InvariantInsts;

/// verificare se l'istruzione è loop invariant
bool isLoopInvariant(Instruction *I, Loop *L) {
  // 1. se l'istruzione non è nel loop, considero invariata
  if (!L->contains(I->getParent()))
    return true;

  // 2. escludo istruzioni di controllo (phi, branch, alloca)
  if (isa<PHINode>(I) || isa<BranchInst>(I) || isa<AllocaInst>(I))
    return false;

  // 3. controllo tutti gli operandi
  for (Use &U : I->operands()) {
    if (Instruction *OpI = dyn_cast<Instruction>(U.get())) {
      // se il loop contiene istruzioni padre e non è nel set loop invariant
      if (L->contains(OpI->getParent()) &&
          InvariantInsts.count(OpI) == 0) {
        return false;
      }
    }
  }

  // 4. passato tutti i controlli, assegno come invariato, e lo salvo
  errs()<<"invariante trovato: "<<*I<<"\n";
  InvariantInsts.insert(I);
  return true;
}

///////////////////////////

/// raccolgo i blocchi di uscita del loop
static std::vector<BasicBlock*> getLoopExitBlocks(Loop *L) {
  std::vector<BasicBlock*> Exits;
  for (BasicBlock *BB : L->blocks()) {
    for (BasicBlock *S : successors(BB)) {
      if (!L->contains(S)) {
        Exits.push_back(BB);
        //errs()<<"exit block trovato: ";
        //BB->print(errs());
        //errs()<<"\n";
        break;
      }
    }
  }
  return Exits;
}

/// verifico se un istruzione è dead dopo le uscite del loop
static bool isDeadAtExits(Instruction *I, Loop *L) {
  for (User *U : I->users()) {
    if (Instruction *UI = dyn_cast<Instruction>(U)) {
      if (!L->contains(UI->getParent())){
        errs()<<"Non dead (uso fuori loop): "<< *I <<"\n usato in: "<< *UI<<"\n";
        return false;
      }
      // ricorsione
      if (!isDeadAtExits(UI, L))
        return false;
    }
  }
  errs()<<"dead at exits: "<< *I <<"\n";
  return true;
}

/// Controlla se un'istruzione domina le uscite o è dead at exits
bool canMoveAcrossExits(Instruction *I, Loop *L, DominatorTree &DT) {
  auto Exits = getLoopExitBlocks(L);
  BasicBlock *BB = I->getParent();

  // 1) il blocco domina tutte le uscite
  bool domAll = true;
  for (BasicBlock *ExitBB : Exits) {
    if (!DT.dominates(BB, ExitBB)) {
      domAll = false;
      break;
    }
  }
  if (domAll){
    errs()<<"domina tutte le uscite: "<< *I <<"\n";
    return true;
  }

  // 2) se è dead
  return isDeadAtExits(I, L);
}

////////////////////////////


bool isUniqueDefInLoop(Instruction *I, Loop *L) {
  // controllo tramite StoreInst, altri lo salto
  auto *SI = dyn_cast<StoreInst>(I);
  if (!SI)
    return true;

  // indirizzo memoria
  Value *Ptr = SI->getPointerOperand();

  // nel loop scansiono ogni blocco
  for (BasicBlock *BB : L->blocks()) {
    for (Instruction &J : *BB) {
      // salto se stesso
      if (&J == I)
        continue;
      // se incontro un altro Store che va allo stetto indirizzo
      if (auto *Other = dyn_cast<StoreInst>(&J)) {
        if (Other->getPointerOperand() == Ptr)
          // allora non è unico (assegnato più volte)
          errs() << "Definizione multipla di " << *Ptr << ": " << *Other << "\n";
          return false;
      }
    }
  }
  // unica
  errs() << "unica definizione: " <<*I<< "\n";
  return true;
}

///////////////////////////////

/// verifico se un blocco  domini tutti gli usi interni del loop
bool dominatesAllUsesInLoop(Instruction *I, Loop *L, DominatorTree &DT) {
  BasicBlock *DefBB = I->getParent();

  for (User *U : I->users()) {
    // salto PHI 
    if (isa<PHINode>(U))
      continue;

    if (auto *UseI = dyn_cast<Instruction>(U)) {
      BasicBlock *UseBB = UseI->getParent();
      // se uso interno loop
      if (L->contains(UseBB)) {
        if (!DT.dominates(DefBB, UseBB)) {
          errs() << "non domina uso interno: " << *I << " vs " << *UseI << "\n";
          return false;
        }
      }
    }
  }

  errs() << "domina tutti gli usi interni: " << *I<< "\n";
  return true;
}

////////////////////////

void MoveBeforeLoop(Instruction *I, Loop *L){

  if (auto *Preheader = L->getLoopPreheader()){
    errs() << "spostamento: " << *I << " --> nel preheader: " << "\n";
    I->moveBefore(Preheader->getTerminator());
  }

}




namespace {

struct LoopPass: PassInfoMixin<LoopPass> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM){
    
    errs() << F.getName() << "\n";
    
    LoopInfo &LI = AM.getResult<LoopAnalysis>(F);
    
    DominatorTree &DF = AM.getResult<DominatorTreeAnalysis>(F);
    

//////////////// Code Motion ///////////////////////
    
    for (Loop *L : LI) {
      // 1. clean set invariant
      errs() << "--- nuovo loop trovato ---"<< "\n";
      InvariantInsts.clear();

      // 2. trovo loop-invariant
      std::vector<Instruction*> Invariants;
      for (BasicBlock *BB : L->blocks())
        for (Instruction &I : *BB)
          if (isLoopInvariant(&I, L))
            Invariants.push_back(&I);

      // 3. prepara exit blocks
      auto ExitBlocks = getLoopExitBlocks(L);

      // 4. filtra i candidate
      std::vector<Instruction*> ToMove;
      for (BasicBlock *BB : L->blocks()) {
        for (Instruction &I : *BB) {
          Instruction *Inst = &I;
          // se è loop-invariant
          if (std::find(Invariants.begin(), Invariants.end(), Inst) == Invariants.end())
            continue;

          if (!canMoveAcrossExits(Inst, L, DF))
            continue;

          if (!isUniqueDefInLoop(Inst, L))
            continue;

          if (!dominatesAllUsesInLoop(Inst, L, DF))
            continue;
          ToMove.push_back(Inst);
        }
      }

      // 5. spostamento a preheader
      if (BasicBlock *PH = L->getLoopPreheader()) {
        for (auto *BB : L->blocks()) {
          for (auto *Inst : ToMove) {
            if (Inst->getParent() == BB) {
              MoveBeforeLoop(Inst, L);
              errs() << "  Moved: " << *Inst << "\n";
            }
          }
        }
      }
    }

    
    return PreservedAnalyses::all();
  }

  static bool isRequired() { return true; }
};
} // namespace

//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getTestPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "LoopPass", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "loop-pass") {
                    FPM.addPass(LoopPass());
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getTestPassPluginInfo();
}
