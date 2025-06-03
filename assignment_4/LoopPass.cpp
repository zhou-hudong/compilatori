
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
#include "llvm/Analysis/PostDominators.h"
#include "llvm/ADT/BreadthFirstIterator.h"

#include "llvm/IR/CFG.h"
#include "llvm/Analysis/CFGPrinter.h"

#include "llvm/IR/Value.h"
#include "llvm/IR/Constants.h"

#include <unordered_set>

#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/DependenceAnalysis.h"
#include "llvm/IR/GetElementPtrTypeIterator.h"

#include "llvm/Transforms/Utils/SSAUpdater.h"


using namespace llvm;

/*
BasicBlock* IsGuardedBlock(Loop* L0){
  BasicBlock *guardBlock=nullptr;
  BasicBlock *Preheader= L0->getLoopPreheader();
  int icmpflag=0;
  int phinodeflag=0;
  //verify if it is simplified loop, if has preheadedr true
  
  if(!Preheader){
    return guardBlock;
  }
  //errs()<<"preheader: "<<*Preheader<<"\n";

  guardBlock = Preheader->getSinglePredecessor();
  if(guardBlock){
    //errs()<<"guardBlock: "<<*guardBlock<<"\n";
    for(auto& Inst: *guardBlock){
      if(dyn_cast<ICmpInst>(&Inst)){
        icmpflag=1;
      }
      else if(dyn_cast<PHINode>(&Inst)){
        phinodeflag=1;
      }
    }
  }
  
  // con clang -02, con due if consecutivi, il secondo if avrà un phi
  if(icmpflag==1 && phinodeflag==0){
    return guardBlock;
  }
  
  return nullptr;
}

*/


BasicBlock* findGuardingBranch(Loop *L, DominatorTree &DT) {
  BasicBlock *H = L->getHeader();
  // ottengo la funzione(una serie di BasicBlock) a cui appartiene header
  Function *F = H->getParent();

  // guardo tutti i terminator appartenente alla funzione
  for (BasicBlock &BB : *F) {
    auto *BI = dyn_cast<BranchInst>(BB.getTerminator());
    if (!BI || !BI->isConditional()) continue;

    // Successor(0) true branch，(1) false branch
    BasicBlock *T = BI->getSuccessor(0);
    BasicBlock *F0 = BI->getSuccessor(1);

    // se true-edge domina header，false-edge non domina，allora è guard
    if (DT.dominates(T, H) && !DT.dominates(F0, H))
      return &BB;
  }
  return nullptr;
}






bool isEqual(BasicBlock* guard1, BasicBlock* guard2){
  ICmpInst *Icmp1,*Icmp2;
  for(auto& Inst: *guard1){
    if(dyn_cast<ICmpInst>(&Inst)){
      Icmp1=dyn_cast<ICmpInst>(&Inst);
    }
  }
  for(auto& Inst: *guard2){
    if(dyn_cast<ICmpInst>(&Inst)){
      Icmp2=dyn_cast<ICmpInst>(&Inst);
    }
  }
  errs()<<*Icmp1<<"\n"<<*Icmp2<<"\n";
  // controllo se il loro compare instruction sono uguali
  if(Icmp1->getPredicate() == Icmp2->getPredicate() )
  {
  
    // se i primo operandi sono uguali, e hanno tutti il secondo operando, allora equal
    if(Icmp1->getOperand(0) && Icmp2->getOperand(0) && Icmp1->getOperand(1) && Icmp2->getOperand(1)){
      return true;
    }
  
  /*    se attivo questo prova5 il secondo viene come due if diversi, quindi viene segnato come false
    // se ho il primo Operand = 0, e il secondo = %.1 --> non sono uguali
    if(Icmp1->getOperand(0) && Icmp2->getOperand(0) && Icmp1->getOperand(1) && Icmp2->getOperand(1)){
      errs() << "1111111\n";
      return true;
    }
  */
  }
  return false;
}


////////////// punto 1 ////////////////


bool AdjacencyGuard(Loop* L1, Loop* L2, BasicBlock* GuardedBlock, BasicBlock* GuardedBlock1){
  errs()<<"Its guarded loop\n";
  BasicBlock *preheader = L1->getLoopPreheader();
  //Not used
  BasicBlock *preheader1 = L2->getLoopPreheader();
  if((!preheader) || (!preheader1)){
    errs()<<"Two loops may haven't preheaders(One of them or both)\n";
    return false;
  }

  if (BranchInst *BI = dyn_cast<BranchInst>(GuardedBlock->getTerminator())) {
    if (BI->isConditional()) {
        BasicBlock *Succ0 = BI->getSuccessor(0);
        BasicBlock *Succ1 = BI->getSuccessor(1);

        if(preheader != Succ0){
          if(Succ0 != GuardedBlock1){
            return false;
          }
          else{
            return true;
          }        
        }
        if(preheader != Succ1){
          if(Succ1 != GuardedBlock1){
            return false;
          }
          else{
            return true;
          }          
        }
        errs()<<"both two successor not linked to the preheader of first loop, unknow error\n";
        return false;
    }
  }
  errs()<<"Can't find a branch instruction in guarded block\n";
  return false;

}



bool adjacency(Loop *L0, Loop *L1) {

  // 1) caso istruzione semplice in mezzo viene portato nel preheader del loop
  // raccolgo tutti i exiting block del loop
  SmallVector<BasicBlock*, 4> Exiting;
  L0->getExitingBlocks(Exiting);

  SmallVector<BasicBlock*, 4> ExitSuccs;
  for (BasicBlock *BB : Exiting) {
    // prendo l'ultimo istruzione (come istruzione di salto in caso di if con branch)
    if (auto *BI = dyn_cast<BranchInst>(BB->getTerminator())) {
      // se non è un istruzione di condizione (come if) allora controllo il suo successore
      if (!BI->isConditional()) continue;
      for (unsigned i = 0, e = BI->getNumSuccessors(); i < e; ++i) {
        BasicBlock *Succ = BI->getSuccessor(i);
        
        // se il Succ non fa parte del loop, allora lo salvo nel Exit block del loop
        if (!L0->contains(Succ))
          ExitSuccs.push_back(Succ);
      }
    }
  }

  // se ho solo un exit block (unica direzione), allora seguo questa linea
  if (ExitSuccs.size() == 1) {
  
    BasicBlock *Next = ExitSuccs[0];
    
    // controllo se il pre-Header del secondo loop contiene solamente il br label
    BasicBlock *Pre = L1->getLoopPreheader();
    unsigned BranchCount = 0;
    for (Instruction &I : *Pre){
      if (auto *BI = dyn_cast<BranchInst>(&I)){
        BranchCount++;
        if (BranchCount>1){
          return false;
        }
      }else{
        return false;
      }
    }
    
    // seguo la linea, fino a trovare più vie oppure il Header
    while (true) {
      if (Next == L1->getHeader())
        return true;
      auto *TI = Next->getTerminator();
      // accetto solo con l'unico salto
      if (auto *Br = dyn_cast<BranchInst>(TI)) {
        // se è br senza condizione (br label %..) && ha un solo successore
        // se è un br con condizione && le due loop guard sono uguali
        if (!Br->isConditional() && Br->getNumSuccessors() == 1) {
          Next = Br->getSuccessor(0);
          continue;
        }
      }
      break;
    }
  }

  // 2) Non-guarded： L0 ha solo un exit block，
  //    e può arrivare a L1.preheader
  SmallVector<BasicBlock*, 4> AllExits;
  L0->getExitBlocks(AllExits);
  if (AllExits.size() == 1 && L1->getLoopPreheader()) {
    BasicBlock *Exit = AllExits[0];
    BasicBlock *Pre = L1->getLoopPreheader();
    // stesso procedimento di seguire unica linea
    BasicBlock *Next = Exit;
    while (true) {
      if (Next == Pre)
        return true;
      auto *TI = Next->getTerminator();
      if (auto *Br = dyn_cast<BranchInst>(TI)) {
        if (!Br->isConditional() && Br->getNumSuccessors() == 1) {
          Next = Br->getSuccessor(0);
          continue;
        }
      }
      break;
    }
  }

  return false;
}

//////////////////////// punto 3 /////////////////////

bool controlFlowEquivalentGuard(BasicBlock* g0, BasicBlock* g1, DominatorTree &DT, PostDominatorTree &PDT) {
  return DT.dominates(g0, g1) && PDT.dominates(g1, g0);
}


bool controlFlowEquivalent(Loop *L0, Loop *L1, DominatorTree &DT, PostDominatorTree &PDT) {
  BasicBlock *H0 = L0->getHeader();
  BasicBlock *H1 = L1->getHeader();
  return DT.dominates(H0, H1) && PDT.dominates(H1, H0);
}



////////////////// punto 2 //////////////////

static bool sameTripCount(Loop *L0, Loop *L1, ScalarEvolution &SE) {
  // ottengo il Backedge Count, che è in -1
  const SCEV *BE0 = SE.getBackedgeTakenCount(L0);
  const SCEV *BE1 = SE.getBackedgeTakenCount(L1);

  // se uno dei due non si riesce a calcolare il Backdge Count,allora return false
  if (isa<SCEVCouldNotCompute>(BE0) || isa<SCEVCouldNotCompute>(BE1))
    return false;

  // controllo se il num cicli sono uguali
  if (BE0 == BE1)
    return true;

  // se non sono stesso oggetto SCEV, controllo se sono uguali maticamente
  return SE.isKnownPredicate(CmpInst::ICMP_EQ, BE0, BE1);
}


////////////////////// punto 4 /////////////////

/*
bool hasNegativeDistanceDependence(Loop *L0,
                                   Loop *L1,
                                   DependenceInfo &DI) {
  SmallVector<Instruction*, 8> Mems0, Mems1;

  // 1) raccolgo tutti i load/store delle due loop
  for (BasicBlock *BB : L0->blocks())
    for (Instruction &I : *BB)
      if (isa<LoadInst>(I) || isa<StoreInst>(I))
        Mems0.push_back(&I);

  for (BasicBlock *BB : L1->blocks())
    for (Instruction &I : *BB)
      if (isa<LoadInst>(I) || isa<StoreInst>(I))
        Mems1.push_back(&I);

  // 2) per ogni coppia (I0,I1) cerco la depends
  for (Instruction *I0 : Mems0) {
    for (Instruction *I1 : Mems1) {
      auto DepResult = DI.depends(I0, I1, true);
      if (!DepResult){
        continue;
      }
      // 3a) se è flow‐dependence e la direzione è negative
      if (DepResult->isFlow() && DepResult->isDirectionNegative())
        return true;

      // 3b) o la distanza SCEV è costante e minore di 0
      if (const SCEV *Dist = DepResult->getDistance(0)) {
        if (auto *C = dyn_cast<SCEVConstant>(Dist)) {
          if (C->getValue()->getSExtValue() < 0)
            return true;
        }
      }
    
    }
  }

  return false;
}
*/


bool hasNegativeDistanceDependence(Loop *L0,
                                               Loop *L1,
                                               DependenceInfo &DI,
                                               ScalarEvolution &SE) {
  // raccolgo tutti i load/store delle due loop
  SmallVector<Instruction*, 8> Mem0, Mem1;

  for (BasicBlock *BB : L0->blocks()) {
    for (Instruction &I : *BB) {
      if (isa<LoadInst>(I) || isa<StoreInst>(I))
        Mem0.push_back(&I);
    }
  }

  for (BasicBlock *BB : L1->blocks()) {
    for (Instruction &I : *BB) {
      if (isa<LoadInst>(I) || isa<StoreInst>(I))
        Mem1.push_back(&I);
    }
  }

  // utilizzare DependenceAnalysis per filtrare le coppie che potrebbero avere alias/dipendenze
  for (Instruction *I0 : Mem0) {
    for (Instruction *I1 : Mem1) {
      auto Dep = DI.depends(I0, I1, true);
      // se Dep = nullptr, significa che non accedono alla stessa memoria
      if (!Dep) 
        continue;

      // estrarre gli operandi puntatori di due operazioni di memoria
      Value *Ptr0 = nullptr;
      if (auto *LI0 = dyn_cast<LoadInst>(I0))
        // ottengo il valore dell'indirizzo
        Ptr0 = LI0->getPointerOperand();
      else if (auto *SI0 = dyn_cast<StoreInst>(I0))
        Ptr0 = SI0->getPointerOperand();

      Value *Ptr1 = nullptr;
      if (auto *LI1 = dyn_cast<LoadInst>(I1))
        Ptr1 = LI1->getPointerOperand();
      else if (auto *SI1 = dyn_cast<StoreInst>(I1))
        Ptr1 = SI1->getPointerOperand();

      // salto se non ottengo valore dell'indirizzo uno dei due
      if (!Ptr0 || !Ptr1)
        continue;

      // utilizzare SCEV per analizzare le espressioni di indice，
      // richiedendo che siano AddRec（riguardando al proprio loop ）
      const SCEV *S0 = SE.getSCEV(Ptr0);
      const SCEV *S1 = SE.getSCEV(Ptr1);
      // addRec -> ottengo {start, step}
      // start : scostamento quando i=0
      // step : valore scostamento da aggiungere ad ogni iterazione
      auto *AR0 = dyn_cast<SCEVAddRecExpr>(S0);
      auto *AR1 = dyn_cast<SCEVAddRecExpr>(S1);
      if (!AR0 || !AR1)           continue;
      if (AR0->getLoop() != L0)   continue;
      if (AR1->getLoop() != L1)   continue;

      // assicurarsi che accedono allo stesso puntatore (indice) base (lo stesso array o oggetto di memoria)
      Value *Base0 = nullptr, *Base1 = nullptr;
      if (auto *G0 = dyn_cast<GetElementPtrInst>(Ptr0))
        Base0 = G0->getPointerOperand();
      if (auto *G1 = dyn_cast<GetElementPtrInst>(Ptr1))
        Base1 = G1->getPointerOperand();
      if (Base0 != Base1) 
        continue;

      // prendo i due start e il primo step
      const SCEV *Start0 = AR0->getStart();
      const SCEV *Start1 = AR1->getStart();
      const SCEV *Step0 = AR0->getStepRecurrence(SE);
      // calcolo la differenza tra i loro offser iniziali Δ = Start1 - Start0
      const SCEV *Diff   = SE.getMinusSCEV(Start1, Start0);
      
      // confronto se sono tutte e due costanti
      auto *Cstep = dyn_cast<SCEVConstant>(Step0);
      auto *Cdiff = dyn_cast<SCEVConstant>(Diff);
      if (!Cstep || !Cdiff) continue;
      
      int64_t step = Cstep->getValue()->getSExtValue();
      // Indica "Quando i=0, quanto è maggiore l'offset dell'indirizzo 
      // a cui si accede nel ciclo successivo rispetto all'indirizzo 
      // a cui si accede nel ciclo precedente"
      int64_t d = Cdiff->getValue()->getSExtValue();
      
      // controllo se è caso indice incremento(distanza positivo) o caso decremento(distanza negativo)
      bool negDep = (step > 0 && d > 0) || (step < 0 && d < 0);

      if (negDep) {
        errs() << " Negative‐distance dependence found: Δ=" << d << ", step=" << step << "\n ('sono sizeof(int) quindi byte')\n";
        
        if(step > 0 && d > 0){
          errs() << " distance positiva \n";
        }
        if(step < 0 && d < 0){
          errs() << " distance negativa \n";
        }
        return true;
      }
    }
  }

  // se tutte le coppie di dipendenza non hanno Δ>0，
  //allora non esiste dipendenza dalla distanza negativa
  return false;
}





////////////////////// fusione ///////////////////


/// Trova la prima PHI (variabile di induzione) dall'intestazione
static PHINode* getInductionVariable(Loop *L) {
  for (Instruction &I : *L->getHeader())
    if (auto *PN = dyn_cast<PHINode>(&I))
      return PN;
  return nullptr;
}

/// Trova il latch del loop (torna al predecessore dell'intestazione)
static BasicBlock* findLatch(Loop *L) {
  BasicBlock *H = L->getHeader();
  for (BasicBlock *P : predecessors(H))
    if (L->contains(P))
      return P;
  return nullptr;
}

/*
/// LoopFusion completo per i casi non protetti
static void LoopFusion(Loop *L1,
                       Loop *L2,
                       SmallVector<Loop*,8> &Worklist,
                       LoopInfo &LI,
                       DominatorTree &DT) {
  Function &F = *L1->getHeader()->getParent();
  errs() << "--- LoopFusion (non-guarded) ---\n";

  // 1) PHI variabile di induzione
  PHINode *Phi1 = getInductionVariable(L1),
           *Phi2 = getInductionVariable(L2);
  assert(Phi1 && Phi2 && "È necessario riassumere la variabile PHI");
  BasicBlock *Pre1   = L1->getLoopPreheader();
  BasicBlock *Latch1 = findLatch(L1);
  BasicBlock *Latch2 = findLatch(L2);

  // 2) SSAUpdater sostituisce in modo sicuro Phi2 → Phi1
  {
    SSAUpdater Up;
    Up.Initialize(Phi1->getType(), "fused");
    Up.AddAvailableValue(Pre1,   Phi1);
    Up.AddAvailableValue(Latch1, Phi1);
    SmallVector<Use*,8> Uses;
    for (Use &U : Phi2->uses()) Uses.push_back(&U);
    for (Use *U : Uses)        Up.RewriteUse(*U);
    Phi2->eraseFromParent();
  }

  // 3) Reindirizza L2Latch a L1Latch
  if (auto *BI = dyn_cast<BranchInst>(Latch2->getTerminator()))
    if (!BI->isConditional() && BI->getNumSuccessors() == 1)
      BI->setSuccessor(0, Latch1);

  // 4) Sposta i blocchi L2 eccetto Header/Latch prima di L1Latch
  SmallVector<BasicBlock*,8> ToMove;
  for (BasicBlock *BB : L2->getBlocks())
    if (BB!=L2->getHeader() && BB!=Latch2)
      ToMove.push_back(BB);

  for (BasicBlock *BB : ToMove) {
    BB->moveBefore(Latch1);
    L1->addBasicBlockToLoop(BB, LI);
  }

  // 5) Spostare il PHI rimanente sull'intestazione su L1Header
  {
    BasicBlock *H1 = L1->getHeader();
    BasicBlock *H2 = L2->getHeader();

    SmallVector<PHINode*,4> OldPhis;
    for (Instruction &I : *H2)
      if (auto *PN = dyn_cast<PHINode>(&I))
        OldPhis.push_back(PN);

    for (PHINode *OldPN : OldPhis) {
      PHINode *NewPN = PHINode::Create(
        OldPN->getType(),
        OldPN->getNumIncomingValues(),
        OldPN->getName() + ".fused",
        &*H1->getFirstInsertionPt()
      );
      for (unsigned i = 0, e = OldPN->getNumIncomingValues(); i < e; ++i) {
        BasicBlock *IncBB = OldPN->getIncomingBlock(i);
        Value      *IncV  = OldPN->getIncomingValue(i);
        // In arrivo da L2 viene mappato su L1Latch, altrimenti su L1Preheader
        BasicBlock *Target = L2->contains(IncBB) ? Latch1 : Pre1;
        NewPN->addIncoming(IncV, Target);
      }
      OldPN->replaceAllUsesWith(NewPN);
      OldPN->eraseFromParent();
    }
  }

  // 6) Rimuovi L2 da LoopInfo
  if (Loop *P = L2->getParentLoop()) P->removeChildLoop(L2);
  else                              LI.erase(L2);

  // 7) Ricalcola DomTree
  DT.recalculate(F);
  errs() << "--- done non-guarded fusion ---\n\n";
}

/// LoopFusion completo con protezione
static void LoopFusionForGuarded(Loop *L1,
                                 Loop *L2,
                                 SmallVector<Loop*,8> &Worklist,
                                 LoopInfo &LI,
                                 DominatorTree &DT,
                                 BasicBlock *G1,
                                 BasicBlock *G2) {
  Function &F = *L1->getHeader()->getParent();
  errs() << "--- LoopFusion (guarded) ---\n";

  // 1) Trova l'uscita 2 non ciclica di G2 e modifica il ramo G1->G2
  BasicBlock *exit2 = nullptr;
  if (auto *BI2 = dyn_cast<BranchInst>(G2->getTerminator()))
    for (BasicBlock *S : successors(G2))
      if (S != L2->getLoopPreheader())
        exit2 = S;

  if (exit2) {
    if (auto *BI1 = dyn_cast<BranchInst>(G1->getTerminator()))
      for (unsigned i = 0, e = BI1->getNumSuccessors(); i < e; ++i)
        if (BI1->getSuccessor(i) == G2)
          BI1->setSuccessor(i, exit2);
  }

  // 2) Chiamare la versione non protetta per completare i passaggi rimanenti
  LoopFusion(L1, L2, Worklist, LI, DT);
  errs() << "--- done guarded fusion ---\n\n";
}
*/

///////////////////////////// fusion 2 //////////////

struct PairOfBB{
  BasicBlock *B1;
  BasicBlock *B2;
};

void LoopFusion(Loop* L1,Loop* L2, SmallVector<Loop *, 8> Worklist,LoopInfo &LI,DominatorTree &DT){
  
  // ottengo i Phi di induzione
  errs()<<"Header Loop : \n";
  errs()<<*L1->getHeader()<<"\n"<<*L2->getHeader()<<"\n";
  errs()<<"Getting Induction variable\n";
  PHINode* inductionL1=getInductionVariable(L1);
  PHINode* inductionL2=getInductionVariable(L2);
  errs() << "Induction variable: " << *inductionL1 << "\n";
  errs() << "Induction variable: " << *inductionL2 << "\n";
  errs()<<"End to get Induction variable\n";


  
  // ottengo i blocchi header e latch, assegno bocchi di salto di br di true e false (first, end)
  // BI1 BI2 assegno istruzione che decide di uscire o no dal loop
  // No loop index -> per memorizzare indice di L1 nel BI1 in quale successor esce dal loop
  //errs()<<"if condition\n"<<*L2->getLoopPreheader()->getSinglePredecessor()<<"\n";
  BasicBlock* L1Header= L1->getHeader();
  BasicBlock* L2Header= L2->getHeader();
  BasicBlock* L2Latch= L2->getLoopLatch();
  BasicBlock* L1Latch= L1->getLoopLatch();
  BasicBlock* L2FirstBody= nullptr,*L2EndBody=nullptr,* L1EndBody=nullptr,*L2NoLoopExit=nullptr;
  BranchInst *BI1=nullptr,*BI2=nullptr;
  int L1NoLoopIndex=-1;

  //  trovo e memorizzo isruzione di branch condition
  if(isa<BranchInst>(L2Header->getTerminator())){
    BranchInst* BI=dyn_cast<BranchInst>(L2Header->getTerminator());

    if(BI->isConditional()){
      BI2=BI;

    }
  }
  if(isa<BranchInst>(L2Latch->getTerminator())){
    BranchInst* BI=dyn_cast<BranchInst>(L2Latch->getTerminator());

    if(BI->isConditional()){
      BI2=BI;

    }
  }
  if(isa<BranchInst>(L1Header->getTerminator())){
    BranchInst* BI=dyn_cast<BranchInst>(L1Header->getTerminator());

    if(BI->isConditional()){
      BI1=BI;

    }
  }
  if(isa<BranchInst>(L1Latch->getTerminator())){
    BranchInst* BI=dyn_cast<BranchInst>(L1Latch->getTerminator());

    if(BI->isConditional()){
      BI1=BI;

    }
  }

  // secondo il branch condition, memorizzo i due blocchi che presentato il true e false del branch
  if(BI1 && BI2){
    errs()<<"exit branch1\n"<<*BI1<<"\nexit branch2\n"<<*BI2<<"\n";
    for(int i=0;i<BI2->getNumSuccessors();i++){
      BasicBlock* BB=BI2->getSuccessor(i);
      if(L2->contains(BB)){
        L2FirstBody=BB;
        //errs()<<"FirstBody\n"<<*L2FirstBody<<"\n";
      }
      else if(!L2->contains(BB)){
        L2NoLoopExit=BB;
        //errs()<<"NoLoopExit\n"<<*BB<<"\n";
      }
    }
    // memorizzo indice che salta fuori dal loop
    for(int i=0;i<BI1->getNumSuccessors();i++){
      BasicBlock* BB=BI1->getSuccessor(i);
      if(!L1->contains(BB)){
        L1NoLoopIndex=i;
        errs()<<"NoLoopIndex: "<<L1NoLoopIndex<<"\n";
      }
    }

  }

  // trovo ultimo blocco latch del loop
  L1EndBody=L1Latch->getSinglePredecessor();
  L2EndBody=L2Latch->getSinglePredecessor();

  // redireziono L2 dopo latch di L1
  if(isa<BranchInst>(L2EndBody->getTerminator())){
    BranchInst *L2BI = dyn_cast<BranchInst>(L2EndBody->getTerminator());
    if(!L2BI->isConditional()){
      // branch senza condizione, cambio direttamente
      L2EndBody->getTerminator()->setSuccessor(0,L1Latch);

      // secondo indice, redireziono il branch di uscita loop L1 all'uscita di L2
      if(L1NoLoopIndex!=-1){
        errs()<<"Modifying the non loop exit\nbefore\n"<<*BI1->getSuccessor(L1NoLoopIndex)<<"\n";
        BI1->setSuccessor(L1NoLoopIndex,L2NoLoopExit);
        errs()<<"Modified the non loop exit\nafter\n"<<*BI1->getSuccessor(L1NoLoopIndex)<<"\n";
      }

      // redireziono L1 se salta direttamente al L1 header, lo direziono al header di L2
      if(isa<BranchInst>(L1EndBody->getTerminator())){
        BranchInst *L1BI = dyn_cast<BranchInst>(L1EndBody->getTerminator());
        if(!L1BI->isConditional()){
          L1EndBody->getTerminator()->setSuccessor(0,L2FirstBody);
        }
      }
    }
  }
  errs()<<"End of redirection\n";

  // impiatto induzione di L2 a quello di L1, e controllo se  induction1 domina tutto
  errs()<<"Before replace\n";
  inductionL2->replaceAllUsesWith(inductionL1);
  for (auto &U : inductionL2->uses()) {
    Instruction *userInst = dyn_cast<Instruction>(U.getUser());
    if (!DT.dominates(inductionL1, userInst)) {
      errs() << "Dominance error: inductionL1 does not dominate use: " << *userInst << "\n";
    }
  }
  
  // elimino induzione di L2
  errs()<<"Induction variable moving....\n";
  if (inductionL2->use_empty()){
    inductionL2->eraseFromParent();
  }
  errs()<<"Induction variable moved....\nBasicBlock in L2 after modification of induction variable\n";
  
  // in questo punto abbiamo finito di cambiare control flow
  // adesso spostiamo i blocchi in L1

  int icmpflag=0,removeheader=1;

  //move the body
  for(BasicBlock* BB : L2->getBlocks()){
    if(BB == L2Header){
      // controllo se L2 ha icmp, conservandolo per riscrivere logica di icmp in BI2
      icmpflag=0;
      for(Instruction &Inst : *BB){
        if(isa<ICmpInst>(&Inst)){
          icmpflag++;
        }

      }
      if(icmpflag>0){
        errs()<<"header of L2\n"<<*BB<<"\nContain ICmpInstruction\n";

      }
      // se non ha icmp, lo consideriamo come un normale blocco, spostandolo
      else{
        removeheader=0;
        L1EndBody->getTerminator()->setSuccessor(0,L2Header);
        BB->moveBefore(L1Latch);
        //Per notificare al LI che il contenuto del Loop è cambiato
        L1->addBasicBlockToLoop(BB, LI);
        errs()<<"header of L2\n"<<*BB<<"\nDoesn't contain ICmpInstruction, added to loop1\n";
      }
    }
    else if(BB == L2Latch){
      errs()<<"latch of L2\n"<<*BB<<"\n";
      // lascio latch per ultimo da spostare
    }
    else if(BB != L2Header && BB != L2Latch){
      errs()<<"BasicBlock"<<*BB<<"\n";
      BB->moveBefore(L1Latch);
      //Per notificare al LI che il contenuto del Loop è cambiato
      L1->addBasicBlockToLoop(BB, LI);

    }
  }

  // copy e ricostruzione Phi di L2 header
  SmallVector<PHINode*, 4> InstMoved;
  for (auto &I : *L2Header) {
    if (auto *phi = dyn_cast<PHINode>(&I)) {
      errs()<<"Phi :\n"<<*phi<<"\n";
      // creo un Phi
      PHINode *phiNew = PHINode::Create(phi->getType(), phi->getNumIncomingValues(), "", inductionL1->getNextNode());
      for (unsigned i = 0; i < phi->getNumIncomingValues(); ++i) {
        Value *incomingVal = phi->getIncomingValue(i);
        BasicBlock *incomingBB = phi->getIncomingBlock(i);
        errs()<<"Incoming block:\n"<<*incomingBB<<"\n has value "<<*incomingVal<<"\n";

        // Determina quale blocco ha originariamente passato il valore a questo PHI.
        // Se si tratta di un blocco in L2, utilizzare L1Latch come nuovo BB in arrivo;
        // Altrimenti, utilizzare il preheader di L1.
        
        // Decidere dove impostare il blocco in arrivo
        BasicBlock *newIncomingBB = nullptr;
        // il blocco spostato dal loop ha origine contenuta in L2
        if (L2->contains(incomingBB)) {
          errs()<<"containde by L2\n";
          newIncomingBB = L1Latch;
        } else {
          newIncomingBB = L1->getLoopPreheader();
        }
        phiNew->addIncoming(incomingVal, newIncomingBB);
      }
      phi->replaceAllUsesWith(phiNew);
      InstMoved.push_back(phiNew);
    }
  }

  errs()<<"-------------------------------------\n";
  for(PHINode* PHIN : InstMoved){
    errs()<<"the phinode"<<*PHIN<<"\n";
  }
  
  
  
  // Elimina Preheader, Latch, Header di L2 e aggiorna LoopInfo
  if( L2->getLoopPreheader()){
    //in case guarded, the guarded is loopheader...So this if isn't needed.
    errs()<<"Loop preheader\n"<<*L2->getLoopPreheader()<<"\n";
    BasicBlock* preheader=L2->getLoopPreheader();
    for(Instruction& Inst: *preheader){
      if(!isa<BranchInst> (Inst)){
        Inst.moveBefore(L1->getLoopPreheader()->getTerminator());
      }
    }
    L2->getLoopPreheader()->eraseFromParent();
  }
  if( L2->getLoopLatch()){

    errs()<<"Loop latch\n"<<*L2->getLoopLatch()<<"\n";
    L2->getLoopLatch()->eraseFromParent();
  }
  if(removeheader){
    errs()<<"erasing header\n";
    L2->getHeader()->eraseFromParent();
  }
  
  // Se il ciclo da rimuovere è un ciclo figlio, dobbiamo staccarlo dal suo genitore
  if (Loop *parent = L2->getParentLoop()) {
      parent->removeChildLoop(L2);  // Detaches from parent
  } else {
      LI.erase(L2);  
  }
  
  
  // Pulisci i "branch senza condizione" che potrebbero essere generati dopo l'unione
  SmallVector<PairOfBB,8> BBToReplace;
  BasicBlock* nextBB=nullptr;
  BBToReplace.clear();
  errs()<<"printing the new loop1 (version after motion)\n";
  for(BasicBlock* BB : L1->getBlocks()){
    errs()<<"Block in L1:\n"<<*BB<<"\n";
    if(BB->size()==1 && isa<BranchInst>(BB->getTerminator())){
      BranchInst* BI= dyn_cast<BranchInst>(BB->getTerminator());
      if(BI->isUnconditional()){
        nextBB=BB->getSingleSuccessor();
        if(nextBB->size()==1 && isa<BranchInst>(nextBB->getTerminator())){
          BranchInst* BI1= dyn_cast<BranchInst>(nextBB->getTerminator());
          if(BI1->isUnconditional()){
            BBToReplace.push_back({BB,nextBB});
          }
        }
      }
    }
  }
  if(!BBToReplace.empty()){
    auto BBbegin= BBToReplace.begin();
    auto BBend=BBToReplace.end();
    while(BBbegin != BBend){
      BasicBlock* B1 = BBbegin->B1;
      BasicBlock* B2 = BBbegin->B2;
      errs()<<"redundant br in BB1 \n"<<*B1<<"\nAND\n"<<*B2<<"\n";
      BranchInst *oldBr = dyn_cast<BranchInst>(B1->getTerminator());
      BasicBlock *succ = B2->getSingleSuccessor();
      oldBr->eraseFromParent();
      BranchInst::Create(succ, B1);
      B2->replaceAllUsesWith(B1);

      if (B2->use_empty()) {
          B2->eraseFromParent();
      } else {
          errs() << "Warning: Tried to erase B2 but it's still in use.\n";
      }
      errs()<<"actual B1\n"<<*B1<<"\n";
      ++BBbegin;
    }
  }
  else{
    errs()<<"BB is empty\n";
  }

  

}

void LoopFusionForGuarded(Loop* L1,Loop* L2, SmallVector<Loop *, 8> Worklist,LoopInfo &LI,DominatorTree &DT,BasicBlock* guardedblock1,BasicBlock* guardedblock2){

  // Non dobbiamo confrontare se i due blocchi di guardia hanno la stessa condizione if o meno, perché lo abbiamo già fatto.
  errs()<<"Block Guard : \nGB1:\n"<<*guardedblock1<<"\nGB2:\n"<<*guardedblock2<<"\n";
  // Individuare l'uscita "false branch" del secondo if (guardedblock2) BasicBlock 
  // condExitBB viene utilizzato per memorizzare il BasicBlock a cui si dovrebbe passare quando cond2 è falso
  BasicBlock* condExitBB=nullptr;
  if(BranchInst* BI = dyn_cast<BranchInst>(guardedblock2->getTerminator())){
    for(unsigned i=0;i<BI->getNumSuccessors();i++){
      // Se questo successore non è "il preheader che entra nel ciclo L2", allora è "quello che salta fuori quando cond2==false"
      if(!(BI->getSuccessor(i) == L2->getLoopPreheader())){
        condExitBB=BI->getSuccessor(i);
      }
    }
  }
  // // Se condExitBB viene trovato, stampa ed esegui il passaggio successivo: punta il ramo falso del primo if (guardedblock1) allo stesso condExitBB
  if(condExitBB){
    errs()<<"The exitblock is \n"<<*condExitBB<<"\n";
    int index=-1;
    if(BranchInst* BI = dyn_cast<BranchInst>(guardedblock1->getTerminator())){
      // Attraversa tutti i successori di guardedblock1 e trova il bordo che punta a guardedblock2
      for(unsigned i=0;i<BI->getNumSuccessors();i++){
        if(BI->getSuccessor(i) == guardedblock2){
          index=i;
          errs()<<"GB1's exit before\n"<<*BI->getSuccessor(i)<<"\n";
          // Modifica il bordo "cond1==false jump to guardedblock2" per saltare a condExitBB
          BI->setSuccessor(index,condExitBB);
          errs()<<"GB1's exit after\n"<<*BI->getSuccessor(i)<<"\n";
        }
      }
    }
  }

  // uguale alla funzione senza guard
  
  errs()<<"Header Block Loop :\n";
  errs()<<*L1->getHeader()<<"\n"<<*L2->getHeader()<<"\n";
  errs()<<"Getting Induction variable\n";
  PHINode* inductionL1=getInductionVariable(L1);
  PHINode* inductionL2=getInductionVariable(L2);
  errs() << "Induction variable L1: " << *inductionL1 << "\n";
  errs() << "Induction variable L2: " << *inductionL2 << "\n";
  errs()<<"End to get Induction variable\n";


  BasicBlock* L1Header= L1->getHeader();
  BasicBlock* L2Header= L2->getHeader();
  BasicBlock* L2Latch= L2->getLoopLatch();
  BasicBlock* L1Latch= L1->getLoopLatch();
  errs()<<"L1 header\n"<<*L1Header<<"\nL1 latch\n"<<*L1Latch<<"\n";
  errs()<<"L2 header\n"<<*L2Header<<"\nL2 latch\n"<<*L2Latch<<"\n";
  BasicBlock* L2FirstBody= nullptr,*L2EndBody=nullptr,* L1EndBody=nullptr,*L2NoLoopExit=nullptr;
  BranchInst *BI1=nullptr,*BI2=nullptr;
  int L1NoLoopIndex=-1;
  if(isa<BranchInst>(L2Header->getTerminator())){
    BranchInst* BI=dyn_cast<BranchInst>(L2Header->getTerminator());

    if(BI->isConditional()){
      BI2=BI;

    }
  }
  if(isa<BranchInst>(L2Latch->getTerminator())){
    BranchInst* BI=dyn_cast<BranchInst>(L2Latch->getTerminator());

    if(BI->isConditional()){
      BI2=BI;

    }
  }
  if(isa<BranchInst>(L1Header->getTerminator())){
    BranchInst* BI=dyn_cast<BranchInst>(L1Header->getTerminator());

    if(BI->isConditional()){
      BI1=BI;

    }
  }
  if(isa<BranchInst>(L1Latch->getTerminator())){
    BranchInst* BI=dyn_cast<BranchInst>(L1Latch->getTerminator());

    if(BI->isConditional()){
      BI1=BI;

    }
  }
  if(BI1 && BI2){
    errs()<<"exit branch1\n"<<*BI1<<"\nexit branch2\n"<<*BI2<<"\n";
    for(int i=0;i<BI2->getNumSuccessors();i++){
      BasicBlock* BB=BI2->getSuccessor(i);
      if(L2->contains(BB)){
        L2FirstBody=BB;
        //errs()<<"FirstBody\n"<<*L2FirstBody<<"\n";
      }
      else if(!L2->contains(BB)){
        L2NoLoopExit=BB;
        //errs()<<"NoLoopExit\n"<<*BB<<"\n";
      }
    }
    for(int i=0;i<BI1->getNumSuccessors();i++){
      BasicBlock* BB=BI1->getSuccessor(i);
      if(!L1->contains(BB)){
        L1NoLoopIndex=i;
        errs()<<"NoLoopInde: "<<L1NoLoopIndex<<"\n";
      }
    }

  }

  

  L1EndBody=L1Latch->getSinglePredecessor();
  L2EndBody=L2Latch->getSinglePredecessor();


  errs()<<"Before replace\n";
  inductionL2->replaceAllUsesWith(inductionL1);



  errs()<<"Induction variable moving....\n";
  if (inductionL2->use_empty()){
    inductionL2->eraseFromParent();
  }
  errs()<<"Induction variable moved....\n";


  
  //redirection
  if(isa<BranchInst>(L2EndBody->getTerminator())){
    BranchInst *L2BI = dyn_cast<BranchInst>(L2EndBody->getTerminator());
    if(!L2BI->isConditional()){
      errs()<<"L2's successor(before):\n"<<*L2EndBody->getTerminator()->getSuccessor(0)<<"\n";
      L2EndBody->getTerminator()->setSuccessor(0,L1Latch);
      errs()<<"L2's successor(after):\n"<<*L2EndBody->getTerminator()->getSuccessor(0)<<"\n";
      if(L1NoLoopIndex!=-1){
        errs()<<"Modifying the non loop exit\nbefore\n"<<*BI1->getSuccessor(L1NoLoopIndex)<<"\n";
        BI1->setSuccessor(L1NoLoopIndex,L2NoLoopExit);
        errs()<<"Modified the non loop exit\nafter\n"<<*BI1->getSuccessor(L1NoLoopIndex)<<"\n";
      }

      if(isa<BranchInst>(L1EndBody->getTerminator())){
        BranchInst *L1BI = dyn_cast<BranchInst>(L1EndBody->getTerminator());
        if(!L1BI->isConditional()){
          L1EndBody->getTerminator()->setSuccessor(0,L2FirstBody);
        }
      }
    }
  }
  errs()<<"End of redirection\n";

  errs()<<"-----------------------------------\n";
  for(BasicBlock* BB : L1->getBlocks()){
    errs()<<". Block in L1:\n"<<*BB<<"\n";
  }
  errs()<<"L1 latch"<<*L1->getLoopLatch()<<"\n";
  errs()<<"-----------------------------------\n";

  int icmpflag=0,removeheader=1;
  //move the body
  //errs()<<"the header of l2"<<*L2Header<<"\n";

  for(BasicBlock* BB : L2->getBlocks()){
    //errs()<<"block of L2(including header and latch)\n"<<*BB<<"\n";
    
    if(BB == L2Header){
      //errs()<<"find header\n";
      icmpflag=0;
      for(Instruction &Inst : *BB){
        if(isa<ICmpInst>(&Inst)){
          icmpflag++;
        }

      }
      //int moveflag=0,index;
      if(icmpflag>0){
        errs()<<"header of L2\n"<<*BB<<"\nContain ICmpInstruction\n";
        
      }
      else{
        removeheader=0;
        L1EndBody->getTerminator()->setSuccessor(0,L2Header);
        BB->moveBefore(L1Latch);
        //To notify the LI the Loop content is changed
        L1->addBasicBlockToLoop(BB, LI);
        errs()<<"header of L2\n"<<*BB<<"\nDoesn't contain ICmpInstruction, added to loop1\n";
      }
      
    }
    else if(BB == L2Latch){
      errs()<<"latch of L2\n"<<*BB<<"\n";
    }
    else if(BB != L2Header && BB != L2Latch){
      errs()<<"BasicBlock"<<*BB<<"\n";
      BB->moveBefore(L1Latch);
      //To notify the LI the Loop content is changed
      L1->addBasicBlockToLoop(BB, LI);

    }
  }
  SmallVector<PHINode*, 4> InstMoved;
  for (auto &I : *L2Header) {
    if (auto *phi = dyn_cast<PHINode>(&I)) {
      errs()<<"Phi :\n"<<*phi<<"\n";
      PHINode *phiNew = PHINode::Create(phi->getType(), phi->getNumIncomingValues(), "", inductionL1->getNextNode());
      for (unsigned i = 0; i < phi->getNumIncomingValues(); ++i) {
        Value *incomingVal = phi->getIncomingValue(i);
        BasicBlock *incomingBB = phi->getIncomingBlock(i);
        errs()<<"Incoming block:\n"<<*incomingBB<<"\nhas value "<<*incomingVal<<"\n";
        // Decide where to set the incoming block
        BasicBlock *newIncomingBB = nullptr;
        //the block moved from loop has origin contained by L2
        if (L2->contains(incomingBB)) {
          errs()<<"containde by L2\n";
          newIncomingBB = L1Latch;
        } else {
          newIncomingBB = L1->getLoopPreheader();
        }
        phiNew->addIncoming(incomingVal, newIncomingBB);
      }
      phi->replaceAllUsesWith(phiNew);
      InstMoved.push_back(phiNew);
    }
  }

  errs()<<"-------------------------------------\n";
  for(PHINode* PHIN : InstMoved){
    errs()<<"the phinode"<<*PHIN<<"\n";
  }
  
  
  
  if(guardedblock2->getSinglePredecessor()){
    errs()<<"GB2 predecessor\n"<<*guardedblock2->getSinglePredecessor()<<"\n";
    guardedblock2->getSinglePredecessor()->eraseFromParent();
  }
  errs()<<"guard2\n"<<*guardedblock2<<"\n";
  guardedblock2->eraseFromParent();
  
  
  if( L2->getLoopPreheader()){
    errs()<<"Loop preheader\n"<<*L2->getLoopPreheader()<<"\n";
    //In the case like some assignment, move to the L1's preheader
    BasicBlock* preheader=L2->getLoopPreheader();
    for(Instruction& Inst: *preheader){
      if(!isa<BranchInst> (Inst)){
        Inst.moveBefore(L1->getLoopPreheader()->getTerminator());
      }
    }
    L2->getLoopPreheader()->eraseFromParent();
  }
  if( L2->getLoopLatch()){

    errs()<<"Loop latch\n"<<*L2->getLoopLatch()<<"\n";
    L2->getLoopLatch()->eraseFromParent();
  }

  
  if(removeheader){
    errs()<<"remove l2 header executing....\n";
    L2->getHeader()->eraseFromParent();
  }
  
  //If the loop to be remove is a child loop, we need distach it from its parent!!!
  if (Loop *parent = L2->getParentLoop()) {
      parent->removeChildLoop(L2);  // Detaches from parent
  } else {
      LI.erase(L2);  
  }

  SmallVector<PairOfBB,8> BBToReplace;
  BasicBlock* nextBB=nullptr;
  errs()<<"printing the new loop1 (version after motion)\n";
  for(BasicBlock* BB : L1->getBlocks()){
    errs()<<"Block in L1:\n"<<*BB<<"\n";
    if(BB->size()==1 && isa<BranchInst>(BB->getTerminator())){
      BranchInst* BI= dyn_cast<BranchInst>(BB->getTerminator());
      if(BI->isUnconditional()){
        nextBB=BB->getSingleSuccessor();
        if(nextBB->size()==1 && isa<BranchInst>(nextBB->getTerminator())){
          BranchInst* BI= dyn_cast<BranchInst>(BB->getTerminator());
          if(BI->isUnconditional()){
            BBToReplace.push_back({BB,nextBB});
          }
        }
      }
    }
  }
  if(!BBToReplace.empty()){
    auto BBbegin= BBToReplace.begin();
    auto BBend=BBToReplace.end();
    while(BBbegin != BBend){
      BasicBlock* B1 = BBbegin->B1;
      BasicBlock* B2 = BBbegin->B2;
      errs()<<"redundant br in BB1 \n"<<*B1<<"\nAND\n"<<*B2<<"\n";
      BranchInst *oldBr = dyn_cast<BranchInst>(B1->getTerminator());
      BasicBlock *succ = B2->getSingleSuccessor();
      oldBr->eraseFromParent();
      BranchInst::Create(succ, B1);
      B2->replaceAllUsesWith(B1);

      if (B2->use_empty()) {
          B2->eraseFromParent();
      } else {
          errs() << "Warning: Tried to erase B2 but it's still in use.\n";
      }
      errs()<<"actual B1\n"<<*B1<<"\n";
      ++BBbegin;
    }
  }
  
}






///////////////////////

void PrintFlow(Loop *L0, Loop *L1){
  errs() << "due loop : \n";
  L0->getHeader()->printAsOperand(errs(),false);
  errs() << "\n";
  errs()<<"   stampo i blocchi appartenenti a questo loop : \n";
  for (BasicBlock *BB : L0->blocks()){
    errs()<<"       ";
    BB->printAsOperand(errs(),false);
    errs()<<"\n";
  }
  
  L1->getHeader()->printAsOperand(errs(),false);
  errs()<<"\n";
  
  errs()<<"   stampo i blocchi appartenenti a questo loop : \n";
  for (BasicBlock *BB : L1->blocks()){
    errs()<<"       ";
    BB->printAsOperand(errs(),false);
    errs()<<"\n";
  }
}

/////////////////

void ifguard(BasicBlock* g0, BasicBlock* g1){
  if(g0){
    errs()<<"L0 guarded\n";
  }
  else{
    errs()<<"L0 NON guraded\n";
  }
  
  if(g1){
    errs()<<"L1 guarded\n";
  }
  else{
    errs()<<"L1 NON guraded\n";
  }
}


namespace {

struct LoopPass: PassInfoMixin<LoopPass> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM){
    
    errs() << "\n///////////////" << F.getName() << "////////////// \n \n";

    LoopInfo          &LI  = AM.getResult<LoopAnalysis>(F);
    DominatorTree     &DT  = AM.getResult<DominatorTreeAnalysis>(F);
    PostDominatorTree &PDT = AM.getResult<PostDominatorTreeAnalysis>(F);
    ScalarEvolution   &SE  = AM.getResult<ScalarEvolutionAnalysis>(F);
    DependenceInfo    &DI  = AM.getResult<DependenceAnalysis>(F);

    // creazione Worklist
    SmallVector<Loop*, 8> Worklist;
    for (Loop *Top : LI)
      Worklist.push_back(Top);

    for (Loop *L0 : LI) {
      for (Loop *L1 : LI) {
        if (L0 == L1) continue;

        // controllo guarded
        ///////////// punto 1 ////////////////
        BasicBlock *guard0 = findGuardingBranch(L0, DT);
        BasicBlock *guard1 = findGuardingBranch(L1, DT);
        ifguard(guard0, guard1);

        if (guard0 && guard1 && guard0 != guard1) {
          if (isEqual(guard0, guard1)) {
            bool CanFuse = AdjacencyGuard(L0, L1, guard0, guard1);
            if (CanFuse) {
              PrintFlow(L0, L1);
              errs() << " loop sono adiacenti\n";
              // Punto 3
              bool IsCFEq = controlFlowEquivalentGuard(guard0, guard1, DT, PDT);
              if (IsCFEq) {
                errs() << " control flow equivalente\n";
                // Punto 2
                if (sameTripCount(L0, L1, SE)) {
                  errs() << " count equivalente\n";
                  // Punto 4
                  if (!hasNegativeDistanceDependence(L0, L1, DI, SE)) {
                    errs() << " Negative-distance dependence NON Trovato\n";
                    ///////////////
                    LoopFusionForGuarded(L0, L1, Worklist, LI, DT, guard0, guard1);
                  } else {
                    errs() << " Negative-distance dependence Trovato, Salto Fusion\n";
                  }
                } else {
                  errs() << " count NON equivalente\n";
                }
              } else {
                errs() << " NON control flow equivalente\n";
              }
            } else {
              errs() << " loop NON sono adiacenti\n";
            }
          }
        } else {
          // no guarded
          bool CanFuse = adjacency(L0, L1);
          if (CanFuse) {
            PrintFlow(L0, L1);
            errs() << " loop sono adiacenti\n";
            // Punto 3
            bool IsCFEq = controlFlowEquivalent(L0, L1, DT, PDT);
            if (IsCFEq) {
              errs() << " control flow equivalente\n";
              // Punto 2
              if (sameTripCount(L0, L1, SE)) {
                errs() << " count equivalente\n";
                // Punto 4
                if (!hasNegativeDistanceDependence(L0, L1, DI, SE)) {
                  errs() << " Negative-distance dependence NON Trovato\n";
                  ////////////////
                  LoopFusion(L0, L1, Worklist, LI, DT);
                } else {
                  errs() << " Negative-distance dependence Trovato, Salto Fusion\n";
                }
              } else {
                errs() << " count NON equivalente\n";
              }
            } else {
              errs() << " NON control flow equivalente\n";
            }
          } else {
            errs() << " loop NON sono adiacenti\n";
          }
        }

        errs() << "------------\n";
      }
    }

    // ricostruzione
    DT.recalculate(F);
    PDT.recalculate(F);
    LI.releaseMemory();
    LI.analyze(DT);
    errs() << "------------\n";


    
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
