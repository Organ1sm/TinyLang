//
// Created by yw.
//

#ifndef TINYLANG_SCOPE_H
#define TINYLANG_SCOPE_H

#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringMap.h"

namespace tinylang
{
    class Decl;

    class Scope
    {
        Scope *Parent;
        llvm::StringMap<Decl *> Symbols;

      public:
        explicit Scope(Scope *Parent = nullptr) : Parent(Parent) {}

        bool insert(Decl *Declaration);
        Decl *lookup(llvm::StringRef Name);
        Scope *getParent() { return Parent; }
    };
}    // namespace tinylang

#endif    // TINYLANG_SCOPE_H
