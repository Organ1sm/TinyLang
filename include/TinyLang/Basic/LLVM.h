//
// Created by yw.
//

#ifndef TINYLANG_BASIC_LLVM_H
#define TINYLANG_BASIC_LLVM_H

#include <llvm/ADT/Optional.h>
#include <llvm/ADT/StringMap.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/SMLoc.h>
#include <llvm/Support/SourceMgr.h>
namespace tinylang
{
    using llvm::raw_ostream;
    using llvm::SMLoc;
    using llvm::SourceMgr;
    using llvm::StringMap;
    using llvm::StringRef;
}    // namespace tinylang

#endif    // TINYLANG_LLVM_H
