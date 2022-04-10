//
// Created by yw.
//

#ifndef TINYLANG_DIAGNOSTIC_H
#define TINYLANG_DIAGNOSTIC_H

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/FormatVariadic.h"
#include "llvm/Support/SMLoc.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include <utility>

namespace tinylang
{
    namespace diag
    {
        enum {
#define DIAG(ID, Level, Msg) ID,

#include "Diagnostic.def"
        };
    }    // namespace diag

    class DiagnosticsEngine
    {
      private:
        static const char *getDiagnosticText(unsigned DiagID);
        static llvm::SourceMgr::DiagKind getDiagnosticKind(unsigned DiagID);

        llvm::SourceMgr &sourceMgr;
        unsigned NumErrors;

      public:
        DiagnosticsEngine(llvm::SourceMgr &srcMgr) : sourceMgr(srcMgr), NumErrors(0) {}
        unsigned getNumErrors() { return NumErrors; }

        template <typename... Args>
        void report(llvm::SMLoc Loc, unsigned DiagID, Args &&...Arguments)
        {
            std::string msg = llvm::formatv(getDiagnosticText(DiagID), std::forward<Args>(Arguments)...).str();

            llvm::SourceMgr::DiagKind kind = getDiagnosticKind(DiagID);
            sourceMgr.PrintMessage(Loc, kind, msg);
            NumErrors += (kind == llvm::SourceMgr::DK_Error);
        }
    };
}    // namespace tinylang

#endif