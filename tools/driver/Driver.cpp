//
// Created by yw.
//
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/InitLLVM.h"
#include "TinyLang/Basic/Version.h"
#include "TinyLang/Parser/Parser.h"

using namespace tinylang;

int main(int argc_, const char **argv_)
{
    llvm::InitLLVM X(argc_, argv_);
    llvm::SmallVector<const char *, 256> argv(argv_ + 1, argv_ + argc_);

    llvm::outs() << "Hello I am TinyLang " << tinylang::getTinyLangVersion() << "\n";


    for (const char *F : argv)
    {
        llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> FileOrErr = llvm::MemoryBuffer::getFile(F);
        if (std::error_code BufferError = FileOrErr.getError())
        {
            llvm::errs() << "Error reading " << F << ": " << BufferError.message() << "\n";
            continue;
        }

        llvm::SourceMgr SrcMgr;
        tinylang::DiagnosticsEngine Diags(SrcMgr);

        // Tell SrcMgr about this buffer, which is what the
        // parser will pick up.

        SrcMgr.AddNewSourceBuffer(std::move(*FileOrErr), llvm::SMLoc());

        auto lexer  = tinylang::Lexer(SrcMgr, Diags);
        auto sema   = tinylang::Sema(Diags);
        auto parser = Parser(lexer, sema);
        parser.parse();
    }
    return 0;
}