//
// Created by yw.
//
#include "TinyLang/Basic/Diagnostic.h"

namespace
{
    const char *DiagnosticText[] = {
#define DIAG(ID, Level, Msg) Msg,
#include "TinyLang/Basic/Diagnostic.def"
    };

    llvm::SourceMgr::DiagKind DiagnosticKind[] = {
#define DIAG(ID, Level, Msg) llvm::SourceMgr::DK_##Level,
#include "TinyLang/Basic/Diagnostic.def"
    };


}    // namespace

namespace tinylang
{
    const char *DiagnosticsEngine::getDiagnosticText(unsigned int DiagID)
    {
        return DiagnosticText[DiagID];
    }

    llvm::SourceMgr::DiagKind DiagnosticsEngine::getDiagnosticKind(unsigned int DiagID)
    {
        return DiagnosticKind[DiagID];
    }
}    // namespace tinylang
