#ifndef TINYLANG_CGMODULE_H
#define TINYLANG_CGMODULE_H

#include "TinyLang/AST/AST.h"
#include <llvm/ADT/DenseMap.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/GlobalObject.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>

namespace tinylang
{

    class CGModule
    {
        llvm::Module *M;
        ModuleDeclaration *Mod;

        llvm::DenseMap<Decl *, llvm::GlobalObject *> Globals;

      public:
        llvm::Type *VoidType;
        llvm::Type *Int1Type;
        llvm::Type *Int32Type;
        llvm::Type *Int64Type;
        llvm::Constant *Int32Zero;

        CGModule(llvm::Module *M) : M(M) { initialize(); }
        void initialize();

        llvm::LLVMContext &getLLVMCtx() { return M->getContext(); };
        llvm::Module *getModule() { return M; }
        ModuleDeclaration *getModuleDeclaration() { return Mod; }

        llvm::Type *convertType(TypeDeclaration *Ty);
        std::string mangleName(Decl *D);

        llvm::GlobalObject *getGlobal(Decl *);


        void run(ModuleDeclaration *Mod);
    };


}    // namespace tinylang

#endif
