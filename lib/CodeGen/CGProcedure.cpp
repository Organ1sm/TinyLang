#include "TinyLang/CodeGen/CGProcedure.h"
#include "TinyLang/AST/AST.h"
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Value.h>

namespace tinylang
{

    void CGProcedure::emitStmt(WhileStatement *Stmt)
    {
        llvm::BasicBlock *WhileCondBB  = llvm::BasicBlock::Create(CGM.getLLVMCtx(), "while.cond", Fn);
        llvm::BasicBlock *WhileBodyBB  = llvm::BasicBlock::Create(CGM.getLLVMCtx(), "while.body", Fn);
        llvm::BasicBlock *AfterWhileBB = llvm::BasicBlock::Create(CGM.getLLVMCtx(), "after.while", Fn);

        Builder.CreateBr(WhileCondBB);
        sealBlock(Curr);
        setCurr(WhileCondBB);

        llvm::Value *Cond = emitExpr(Stmt->getCond());

        Builder.CreateCondBr(Cond, WhileBodyBB, AfterWhileBB);
    }

}    // namespace tinylang