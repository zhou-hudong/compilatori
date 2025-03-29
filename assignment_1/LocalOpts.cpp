
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"


#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"



using namespace llvm;




//-----------------------------------------------------------------------------
// TestPass implementation
//-----------------------------------------------------------------------------
// No need to expose the internals of the pass to the outside world - keep
// everything in an anonymous namespace.
namespace {


// New PM implementation
struct LocalOpts: PassInfoMixin<LocalOpts> {
  // Main entry point, takes IR unit to run the pass on (&F) and the
  // corresponding pass manager (to be queried if need be)
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &){
    
    errs() << F.getName() << "\n";
    
      // 1) ADD
      
    for (auto &BB : F) {
      for (auto It = BB.begin(), End = BB.end(); It != End; ) {
        Instruction *I = &*It++;
      //errs() << "istruzione: " << I << "\n";
      
        if (auto *Op = dyn_cast<BinaryOperator>(I)) {
          Value *LHS = Op->getOperand(0);
          Value *RHS = Op->getOperand(1);
          
          IRBuilder<> Builder(I);
          
          
          if (Op->getOpcode() == Instruction::Add) { //  x + 0 = x
          
          
          
            if (auto *C = dyn_cast<ConstantInt>(RHS)) {
              if (C->isZero()) {
              
                errs() << "istruzione Add: ";
                I->print(errs());
                errs() << "\n";
                
                Value *NewVal = LHS;
                
                I->replaceAllUsesWith(NewVal);
                
                errs() << "istruzione new Add: ";
                NewVal->print(errs());
                errs() << "\n";
                
                //cancello istruzione vecchio////////////
                I->eraseFromParent();
                continue;
              }
            }

            else if (auto *C = dyn_cast<ConstantInt>(LHS)) {  //  0 + x = x
              if (C->isZero()) {
              
                errs() << "istruzione Add: ";
                I->print(errs());
                errs() << "\n";
                
                Value *NewVal = RHS;
              
                I->replaceAllUsesWith(NewVal);
                
                errs() << "istruzione new Add: ";
                NewVal->print(errs());
                errs() << "\n";
                
                //////////////////////////
                I->eraseFromParent();
                continue;
              }
            }
            


          // 1) MUL

          } else if (Op->getOpcode() == Instruction::Mul) { //  x * 1 = x
          
          
          
            if (auto *C = dyn_cast<ConstantInt>(RHS)) {
              if (C->isOne()) {
              
                errs() << "istruzione Mul: ";
                I->print(errs());
                errs() << "\n";
                
                Value *NewVal = LHS;
                
                I->replaceAllUsesWith(NewVal);
                
                errs() << "istruzione new Mul: ";
                NewVal->print(errs());
                errs() << "\n";
                
                //////////////////////
                I->eraseFromParent();
                continue;
              }
            }

            else if (auto *C = dyn_cast<ConstantInt>(LHS)) {  // 1 * x = x
              if (C->isOne()) {
              
                errs() << "istruzione Mul: ";
                I->print(errs());
                errs() << "\n";
                
                Value *NewVal = RHS;
              
                I->replaceAllUsesWith(NewVal);
                
                errs() << "istruzione new Mul: ";
                NewVal->print(errs());
                errs() << "\n";
                
                /////////////////////////
                I->eraseFromParent();
                continue;
              }
            }
          }
        }
      }
    }

      // 2)
      
      for (auto &BB : F) {
        for (auto It = BB.begin(), End = BB.end(); It !=End;) {
          Instruction *I = &*It++;
        
        
          if (auto *Op = dyn_cast<BinaryOperator>(I)) {
            IRBuilder<> Builder(I);
            Value *LHS = Op->getOperand(0);
            Value *RHS = Op->getOperand(1);
            if (Op->getOpcode() == Instruction::Mul) {
            
            //errs() << "istruzione Mul 2: " << Op << "\n";
            
            // controllo se destra è un num
              if (auto *C = dyn_cast<ConstantInt>(RHS)) {
                APInt ConstVal = C->getValue();
                
                // se num è 2^k
                if (ConstVal.isPowerOf2()){
                
                  unsigned k = ConstVal.logBase2();
                  
                  errs() << "istruzione Mul R 2^k: ";
                  I->print(errs());
                  errs() << "\n";
                  
                  Value *NewVal = Builder.CreateShl(LHS, k);
                  
                  errs() << "istruzione New shift: ";
                  NewVal->print(errs());
                  errs() << "\n";
                  
                  I->replaceAllUsesWith(NewVal);
                  
                  /////////////////
                  I->eraseFromParent();
                  continue;
                  
                  }
                
                  // se num+1 è 2^k
                else if ((ConstVal+1).isPowerOf2()){
                
                  unsigned k = (ConstVal+1).logBase2();
                  
                  errs() << "istruzione Mul R+1 2^k: ";
                  I->print(errs());
                  errs() << "\n";
                  
                  Value *Shifted = Builder.CreateShl(LHS, k);
                  
                  errs() << "istruzione New shift: ";
                  Shifted->print(errs());
                  errs() << "\n";
                  
                  Value *NewVal = Builder.CreateSub(Shifted,LHS);
                  
                  errs() << "istruzione New sub: ";
                  NewVal->print(errs());
                  errs() << "\n";
                  
                  I->replaceAllUsesWith(NewVal);
                  
                  /////////////////
                  I->eraseFromParent();
                  continue;
                  
                  }
                }
                
                
                // controllo se sinistra è un num
                
              else if (auto *C = dyn_cast<ConstantInt>(LHS)) {
                APInt ConstVal = C->getValue();
                
                // se num è 2^k
                if (ConstVal.isPowerOf2()){
                
                  unsigned k = ConstVal.logBase2();
                  
                  errs() << "istruzione Mul L 2^k: ";
                  I->print(errs());
                  errs() << "\n";
                  
                  Value *NewVal = Builder.CreateShl(RHS, k);
                  
                  errs() << "istruzione New shift: ";
                  NewVal->print(errs());
                  errs() << "\n";
                  
                  I->replaceAllUsesWith(NewVal);
                  
                  /////////////////
                  I->eraseFromParent();
                  continue;
                  }
                
                  // se num+1 è 2^k
                else if ((ConstVal+1).isPowerOf2()){
                
                  unsigned k = (ConstVal+1).logBase2();
                  
                  errs() << "istruzione Mul L+1 2^k: ";
                  I->print(errs());
                  errs() << "\n";
                  
                  Value *Shifted = Builder.CreateShl(RHS, k);
                  
                  errs() << "istruzione New shift: ";
                  Shifted->print(errs());
                  errs() << "\n";
                  
                  Value *NewVal = Builder.CreateSub(Shifted,RHS);
                  
                  errs() << "istruzione New sub: ";
                  NewVal->print(errs());
                  errs() << "\n";
                  
                  I->replaceAllUsesWith(NewVal);
                  
                  /////////////////
                  I->eraseFromParent();
                  continue;
                  
                  }
                }
              } 
              
              
              
              
              
            
            
            
            
            // ottimizzazione divisore -> gestisco solo il caso in cui il divisore è 2^k
            
            else if (Op->getOpcode() == Instruction::SDiv) {
            // controllo num destra
              if (auto *C = dyn_cast<ConstantInt>(RHS)) {
              
                APInt ConstVal = C->getValue();
                if (ConstVal.isPowerOf2()) { 
                  unsigned k = ConstVal.logBase2();
                  errs() << "istruzione Div: ";
                  I->print(errs());
                  errs() << "\n";
                
                  Value *NewVal = Builder.CreateAShr(LHS, k);
                  
                  errs() << "istruzione shift div: ";
                  NewVal->print(errs());
                  errs() << "\n";
                  
                  I->replaceAllUsesWith(NewVal);
                  
                  /////////////////
                  I->eraseFromParent();
                  continue;
                  
                }
              }
            }
          }
        }
      }



    // Multi-istruzioni  a = b+1, c = a-1  -->  a = b+1, c = b
    
    for (auto &BB : F) {
      for (auto It = BB.begin(), End = BB.end(); It != End;) {
        Instruction *I = &*It++;
        
        if (auto *SubInst = dyn_cast<BinaryOperator>(I)) {
        // catturo istruzione Sub c = a-1
          if (SubInst->getOpcode() == Instruction::Sub) {
          // controllo se il secondo membro è un numero
            if (auto *ConstSub = dyn_cast<ConstantInt>(SubInst->getOperand(1))) {
            // catturo istruzione 'a' dall'istruzione Sub
              if (auto *AddInst = dyn_cast<BinaryOperator>(SubInst->getOperand(0))) {
              // controllo se istruzione catturato sia un Add
                if (AddInst->getOpcode() == Instruction::Add) {
                // controllo se il secondo membro è un numero
                  if (auto *ConstAdd = dyn_cast<ConstantInt>(AddInst->getOperand(1))) {
                  // se i due numeri sono uguali
                    if (ConstSub->equalsInt(ConstAdd->getZExtValue())) {
                      errs() << "istruzione (c = (b+1)-1): ";
                      I->print(errs());
                      errs() << "\n";
                      // prendo il b
                      Value *bValue = AddInst->getOperand(0);
                      // c = b
                      SubInst->replaceAllUsesWith(bValue);
                      errs() << "istruzione new c = b: ";
                      bValue->print(errs());
                      errs() << "\n";
                      
                      /////////////////
                      I->eraseFromParent();
                      continue;
                    }
                  }
                  // se il numero è primo membro
                  else if (auto *ConstAdd = dyn_cast<ConstantInt>(AddInst->getOperand(0))) {
                    if (ConstSub->equalsInt(ConstAdd->getZExtValue())) {
                      errs() << "istruzione (c = (b+1)-1): ";
                      I->print(errs());
                      errs() << "\n";
                      // prendo il b
                      Value *bValue = AddInst->getOperand(1);
                      // c = b
                      SubInst->replaceAllUsesWith(bValue);
                      errs() << "istruzione new c = b: ";
                      bValue->print(errs());
                      errs() << "\n";
                      
                      //////////////////
                      I->eraseFromParent();
                      continue;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }



    return PreservedAnalyses::all();
}


  // Without isRequired returning true, this pass will be skipped for functions
  // decorated with the optnone LLVM attribute. Note that clang -O0 decorates
  // all functions with optnone.
  static bool isRequired() { return true; }
};
} // namespace

//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getTestPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "LocalOpts", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "local-opts") {
                    FPM.addPass(LocalOpts());
                    return true;
                  }
                  return false;
                });
          }};
}

// This is the core interface for pass plugins. It guarantees that 'opt' will
// be able to recognize TestPass when added to the pass pipeline on the
// command line, i.e. via '-passes=test-pass'
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getTestPassPluginInfo();
}
