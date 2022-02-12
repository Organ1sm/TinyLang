//
// Created by yw.
//

#include "TinyLang/Semantic/Scope.h"
#include "TinyLang/AST/AST.h"

namespace tinylang
{

    bool Scope::insert(Decl *Declaration)
    {
        return Symbols.insert(std::pair<llvm::StringRef, Decl *>(Declaration->getName(), Declaration)).second;
    }
    Decl *Scope::lookup(llvm::StringRef Name)
    {
        Scope *S = this;
        while (S)
        {
            auto I = S->Symbols.find(Name);

            if (I != S->Symbols.end()) { return I->second; }

            S = S->getParent();
        }

        return nullptr;
    }
}    // namespace tinylang