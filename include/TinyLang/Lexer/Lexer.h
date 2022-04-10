//
// Created by yw.
//

#ifndef TINYLANG_LEXER_H
#define TINYLANG_LEXER_H

#include "TinyLang/Basic/LLVM.h"
#include "TinyLang/Basic/Diagnostic.h"
#include "TinyLang/Lexer/Token.h"
#include "llvm/ADT/StringMap.h"

namespace tinylang
{
    class KeywordFilter
    {
        llvm::StringMap<token::TokenKind> HashTable;

        void addKeyword(StringRef Keyword, token::TokenKind TokenCode);

      public:
        void addKeywords();

        token::TokenKind getKeyword(StringRef Name, token::TokenKind DefaultTokenCode = token::unknown)
        {
            auto Result = HashTable.find(Name);
            if (Result != HashTable.end()) return Result->second;
            return DefaultTokenCode;
        }
    };

    class Lexer
    {
        SourceMgr &SrcMgr;
        DiagnosticsEngine &Diags;

        const char *CurPtr;
        StringRef CurBuf;

        /// CurBuffer - This is the current buffer index we're
        /// lexing from as managed by the SourceMgr object.
        unsigned CurBuffer = 0;

        KeywordFilter Keywords;

      public:
        Lexer(SourceMgr &SrcMgr, DiagnosticsEngine &Diags) : SrcMgr(SrcMgr), Diags(Diags)
        {
            CurBuffer = SrcMgr.getMainFileID();
            CurBuf    = SrcMgr.getMemoryBuffer(CurBuffer)->getBuffer();
            CurPtr    = CurBuf.begin();
            Keywords.addKeywords();
        }

        DiagnosticsEngine &getDiagnostics() const { return Diags; }

        /// Returns the next tokenen from the input.
        void next(Token &Result);

        /// Gets source code buffer.
        StringRef getBuffer() const { return CurBuf; }

      private:
        void identifier(Token &Result);
        void number(Token &Result);
        void string(Token &Result);
        void comment();

        SMLoc getLoc() { return SMLoc::getFromPointer(CurPtr); }

        void formToken(Token &Result, const char *TokEnd, token::TokenKind Kind);
    };
}    // namespace tinylang


#endif    // TINYLANG_LEXER_H
