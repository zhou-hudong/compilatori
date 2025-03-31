#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"


#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"



using namespace llvm;

/////////////////////// es. 1 ///////////////////////

bool algebraicOptsF(BasicBlock &B){
  bool Changed = false;
  for (auto It = B.begin(), End = B.end(); It != End; ) {
    Instruction *I = &*It++;
    //errs() << "istruzione: " << I << "\n";
      
    if (auto *Op = dyn_cast<BinaryOperator>(I)) {
      Value *LHS = Op->getOperand(0);
      Value *RHS = Op->getOperand(1);
          
      IRBuilder<> Builder(I);
          
      //////// 1) Add +0
      if (Op->getOpcode() == Instruction::Add) { //  x + 0 = x
          
          
          
        if (auto *C = dyn_cast<ConstantInt>(RHS)) {
          if (C->isZero()) {
              
            errs() << "istruzione Add x+0 = x: ";
            I->print(errs());
            errs() << "\n";
                
            Value *NewVal = LHS;
                
            I->replaceAllUsesWith(NewVal);
                
            errs() << "istruzione new Add: ";
            NewVal->print(errs());
            errs() << "\n";
                
            //cancello istruzione vecchio////////////
            I->eraseFromParent();
            Changed = true;
            continue;
          }
        }

        else if (auto *C = dyn_cast<ConstantInt>(LHS)) {  //  0 + x = x
          if (C->isZero()) {
          
            errs() << "istruzione Add 0+x = x: ";
            I->print(errs());
            errs() << "\n";
            
            Value *NewVal = RHS;
          
            I->replaceAllUsesWith(NewVal);
            
            errs() << "istruzione new Add: ";
            NewVal->print(errs());
            errs() << "\n";
            
            //////////////////////////
            I->eraseFromParent();
            Changed = true;
            continue;
          }
        }
        

      // 1) MUL x*1 = x

      } else if (Op->getOpcode() == Instruction::Mul) { //  x * 1 = x
      
          
          
        if (auto *C = dyn_cast<ConstantInt>(RHS)) {
          if (C->isOne()) {
          
            errs() << "istruzione Mul x*1 = x: ";
            I->print(errs());
            errs() << "\n";
            
            Value *NewVal = LHS;
            
            I->replaceAllUsesWith(NewVal);
                
            errs() << "istruzione new Mul: ";
            NewVal->print(errs());
            errs() << "\n";
                
            //////////////////////
            I->eraseFromParent();
            Changed = true;
            continue;
          }
        }

        else if (auto *C = dyn_cast<ConstantInt>(LHS)) {  // 1 * x = x
          if (C->isOne()) {
          
            errs() << "istruzione Mul 1*x = x: ";
            I->print(errs());
            errs() << "\n";
            
            Value *NewVal = RHS;
          
            I->replaceAllUsesWith(NewVal);
            
            errs() << "istruzione new Mul: ";
            NewVal->print(errs());
            errs() << "\n";
                
            /////////////////////////
            I->eraseFromParent();
            Changed = true;
            continue;
          }
        }
      }
      
      ///////// estensione x - 0 = x
      else if (Op->getOpcode() == Instruction::Sub) { 
      
          
          
        if (auto *C = dyn_cast<ConstantInt>(RHS)) {
          if (C->isZero()) {
          
            errs() << "istruzione Sub x-0 = x: ";
            I->print(errs());
            errs() << "\n";
            
            Value *NewVal = LHS;
            
            I->replaceAllUsesWith(NewVal);
                
            errs() << "istruzione new Sub: ";
            NewVal->print(errs());
            errs() << "\n";
                
            //////////////////////
            I->eraseFromParent();
            Changed = true;
            continue;
          }
        }

        else if (auto *C = dyn_cast<ConstantInt>(LHS)) {  // 0 - x = - x
          if (C->isZero()) {
          
            errs() << "istruzione Sub 0-x = -x: ";
            I->print(errs());
            errs() << "\n";
            
            Value *NewVal = Builder.CreateNeg(RHS);
          
            I->replaceAllUsesWith(NewVal);
            
            errs() << "istruzione new Sub: ";
            NewVal->print(errs());
            errs() << "\n";
                
            /////////////////////////
            I->eraseFromParent();
            Changed = true;
            continue;
          }
        }
      }
      
      
      ///////////// estensione x / 1 = x
      else if (Op->getOpcode() == Instruction::SDiv) { 
      
          
          
        if (auto *C = dyn_cast<ConstantInt>(RHS)) {
          if (C->isOne()) {
          
            errs() << "istruzione Div x/1 = x: ";
            I->print(errs());
            errs() << "\n";
            
            Value *NewVal = LHS;
            
            I->replaceAllUsesWith(NewVal);
                
            errs() << "istruzione new Div: ";
            NewVal->print(errs());
            errs() << "\n";
                
            //////////////////////
            I->eraseFromParent();
            Changed = true;
            continue;
          }
        }
      }
      
    }
  }
  return Changed;
}



///////// es. 2 ///////////////
bool strengthOptsF(BasicBlock &B){
  bool Changed = false;
  for (auto It = B.begin(), End = B.end(); It !=End;) {
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
              Changed = true;
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
              Changed = true;
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
              Changed = true;
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
              Changed = true;
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
              Changed = true;
              continue;
                  
            }
          }
        }
      }
    }
  return Changed;
}






///////////////// es. 3 //////////////
bool multiOptsF(BasicBlock &B){
  bool Changed = false;
  for (auto It = B.begin(), End = B.end(); It != End;) {
    Instruction *I = &*It++;
        
    if (auto *SInst = dyn_cast<BinaryOperator>(I)) {
    // catturo istruzione Sub c = a-1
    
    
      if (SInst->getOpcode() == Instruction::Sub) {
      // controllo se il secondo membro è un numero
        if (auto *ConstSub = dyn_cast<ConstantInt>(SInst->getOperand(1))) {
        // catturo istruzione 'a' dall'istruzione Sub
          if (auto *AddInst = dyn_cast<BinaryOperator>(SInst->getOperand(0))) {
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
                  SInst->replaceAllUsesWith(bValue);
                  errs() << "istruzione new c = b: ";
                  bValue->print(errs());
                  errs() << "\n";
                      
                  /////////////////
                  I->eraseFromParent();
                  Changed = true;
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
                  SInst->replaceAllUsesWith(bValue);
                  errs() << "istruzione new c = b: ";
                  bValue->print(errs());
                  errs() << "\n";
                      
                  //////////////////
                  I->eraseFromParent();
                  Changed = true;
                  continue;
                }
              }
            }
          }
        }
      }
      
      ///////// estensione a = b * 2, c = a / 2 --> c = b
      if (SInst->getOpcode() == Instruction::SDiv) {
      // controllo se il secondo membro è un numero
        if (auto *ConstDiv = dyn_cast<ConstantInt>(SInst->getOperand(1))) {
        // catturo istruzione 'a' dall'istruzione Div
          if (auto *MulInst = dyn_cast<BinaryOperator>(SInst->getOperand(0))) {
          // controllo se istruzione catturato sia un Mul
            if (MulInst->getOpcode() == Instruction::Mul) {
            // controllo se il secondo membro è un numero
              if (auto *ConstMul = dyn_cast<ConstantInt>(MulInst->getOperand(1))) {
              // se i due numeri sono uguali
                if (ConstDiv->equalsInt(ConstMul->getZExtValue())) {
                  errs() << "istruzione (c = (b*2)/2): ";
                  I->print(errs());
                  errs() << "\n";
                  // prendo il b
                  Value *bValue = MulInst->getOperand(0);
                  // c = b
                  SInst->replaceAllUsesWith(bValue);
                  errs() << "istruzione new c = b: ";
                  bValue->print(errs());
                  errs() << "\n";
                      
                  /////////////////
                  I->eraseFromParent();
                  Changed = true;
                  continue;
                }
              }
              // se il numero è primo membro
              else if (auto *ConstMul = dyn_cast<ConstantInt>(MulInst->getOperand(0))) {
                if (ConstDiv->equalsInt(ConstMul->getZExtValue())) {
                  errs() << "istruzione (c = (b*2)/2): ";
                  I->print(errs());
                  errs() << "\n";
                  // prendo il b
                  Value *bValue = MulInst->getOperand(1);
                  // c = b
                  SInst->replaceAllUsesWith(bValue);
                  errs() << "istruzione new c = b: ";
                  bValue->print(errs());
                  errs() << "\n";
                      
                  //////////////////
                  I->eraseFromParent();
                  Changed = true;
                  continue;
                }
              }
            }
          }
        }
      }
      
      
    }
  }
  return Changed;
}







bool runOnBasicBlock(BasicBlock &B) {
  bool Changed = false;
  Changed |= algebraicOptsF(B);
  Changed |= strengthOptsF(B);
  Changed |= multiOptsF(B);
  return Changed;
  }


bool runOnFunction(Function &F) {
  bool Transformed = false;

  for (auto Iter = F.begin(); Iter != F.end(); ++Iter) {
    if (runOnBasicBlock(*Iter)) {
      Transformed = true;
    }
  }
  return Transformed;
}


bool runAlgebraic(Function &F) {
  bool Transformed = false;

  for (auto Iter = F.begin(); Iter != F.end(); ++Iter) {
    if (algebraicOptsF(*Iter)) {
      Transformed = true;
    }
  }
  return Transformed;
}

bool runStrength(Function &F) {
  bool Transformed = false;

  for (auto Iter = F.begin(); Iter != F.end(); ++Iter) {
    if (strengthOptsF(*Iter)) {
      Transformed = true;
    }
  }
  return Transformed;
}

bool runMulti(Function &F) {
  bool Transformed = false;

  for (auto Iter = F.begin(); Iter != F.end(); ++Iter) {
    if (multiOptsF(*Iter)) {
      Transformed = true;
    }
  }
  return Transformed;
}




namespace {

struct LocalOpts: PassInfoMixin<LocalOpts> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &){
    
    errs() << F.getName() << "\n";
    bool Changed = runOnFunction(F);
    if (Changed){
      errs() << "Changed by localOpts" << "\n";  
    }
    return PreservedAnalyses::all();
  }
  static bool isRequired() { return true; }
};

struct algebraicOpts: PassInfoMixin<algebraicOpts> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &){
    
    errs() << F.getName() << "\n";
    bool Changed = runAlgebraic(F);
    if (Changed){
      errs() << "Changed by algebraicOpts" << "\n";  
    }
    return PreservedAnalyses::all();
  }
  static bool isRequired() { return true; }
};

struct strengthOpts: PassInfoMixin<strengthOpts> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &){
    
    errs() << F.getName() << "\n";
    bool Changed = runStrength(F);
    if (Changed){
      errs() << "Changed by strengthOpts" << "\n";  
    }
    return PreservedAnalyses::all();
  }
  static bool isRequired() { return true; }
};

struct multiOpts: PassInfoMixin<multiOpts> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &){
    
    errs() << F.getName() << "\n";
    bool Changed = runMulti(F);
    if (Changed){
      errs() << "Changed by multiOpts" << "\n";  
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
                  else if (Name == "algebraic-opts") {
                    FPM.addPass(algebraicOpts());
                    return true;
                  }
                  else if (Name == "strength-opts") {
                    FPM.addPass(strengthOpts());
                    return true;
                  }
                  else if (Name == "multi-opts") {
                    FPM.addPass(multiOpts());
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
