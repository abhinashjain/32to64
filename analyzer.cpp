// Need to add support to recognize the unsigned data type

#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/User.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/InstIterator.h"
#include "set"
#include "fstream"
using namespace llvm;
using namespace std;

namespace
{
    struct analyzer : public ModulePass
    {
        static char ID;
        LLVMContext* Context;
        set<unsigned int> line;
        set<unsigned int>::iterator iter;
        analyzer() : ModulePass(ID) {}

        bool fooexpr(Instruction *vi)
        {
            if(!isa<PHINode>(*vi)) // && !isa<CallInst>(*vi))
            {
//                errs() << *vi << "\n";
                if(isa<CastInst>(*vi) && (vi->getOperand(0)->getType()) != (vi->getType()))// && !isa<BitCastInst>(*vi)  (vi->getOperand(0)->getType()) > (vi->getType())
                {
                    const DebugLoc &location = vi->getDebugLoc();
                    if (location)
                    {
//                      errs() << "\n" << (vi->getOperand(0)->getType());
//                      errs() << "\n" << (vi->getType());
//                      errs() << "\n" << *vi;
                        line.insert(location.getLine());
//                        errs() << "\nLine:  " << location.getLine();// << "\t" << location.getCol();
                    }
                    return 1;
                }
                for(User::op_iterator i = vi->op_begin(); i != vi->op_end(); ++i)
                {
                    if(Instruction *ti = dyn_cast<Instruction>(*i))
                    {
//                      errs() << *vi << "     \t    " << *ti << "\n";
                        if(analyzer::fooexpr(ti))
                        {
                            break;
                        }
                    }
                }
            }
            return 0;
        }
        
        virtual bool runOnModule(Module &M)
        {
            bool ret;
            Context = &M.getContext();

            for(Module::iterator F = M.begin(); F != M.end(); ++F)
            {
                for(Function::iterator BB = F->begin(); BB != F->end(); ++BB)
                {
                    for(BasicBlock::iterator BI = BB->begin(); BI != BB->end(); ++BI)
                    {
                        if(isa<StoreInst>(&(*BI)))
                        {
                            StoreInst *instr = dyn_cast<StoreInst>(BI);
                            if(Instruction *vi = dyn_cast<Instruction>(instr->getOperand(0)))
                            {
                                if(!isa<AllocaInst>(*vi))
                                {
                                    ret=analyzer::fooexpr(vi);
                                }
                            }
                        }
                        else
                        {
                            if(isa<ICmpInst>(&(*BI)))
                            {
                                ICmpInst *instr = dyn_cast<ICmpInst>(BI);
                                if(Instruction *vi = dyn_cast<Instruction>(instr->getOperand(0)))
                                {
                                    if(!isa<AllocaInst>(*vi))
                                    {
                                        ret=analyzer::fooexpr(vi);
                                    }
                                }
                                else
                                {
                                    if (ConstantInt* CI = dyn_cast<ConstantInt>(instr->getOperand(0)))
                                    {
                                        if(CI->isNegative())
                                        {
                                            const DebugLoc &location = instr->getDebugLoc();
                                            line.insert(location.getLine());
                                        }
                                    }
                                }

                                if(Instruction *vi = dyn_cast<Instruction>(instr->getOperand(1)))
                                {
                                    if(!isa<AllocaInst>(*vi))
                                    {
                                        ret=analyzer::fooexpr(vi);
                                    }
                                }
                                else
                                {
                                    if (ConstantInt* CI = dyn_cast<ConstantInt>(instr->getOperand(1)))
                                    {
                                        if(CI->isNegative())
                                        {
                                            const DebugLoc &location = instr->getDebugLoc();
                                            line.insert(location.getLine());
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            ofstream filewrite;
            filewrite.open("~/irfiles/LoC.txt");
            for(iter=line.begin(); iter!=line.end(); iter++)
                filewrite << "Line:" << *iter << " \n";
            filewrite.close();
            return true;
        }
    };
}
char analyzer::ID = 0;
static RegisterPass<analyzer> X("analyzer", "test function exist", false, false);
