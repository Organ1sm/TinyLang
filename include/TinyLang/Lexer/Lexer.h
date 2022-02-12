//
// Created by yw.
//

#ifndef TINYLANG_LEXER_H
#define TINYLANG_LEXER_H

#include "TinyLang/Basic/Diagnostic.h"
#include "TinyLang/Lexer/Token.h"
#include "llvm/ADT/StringMap.h"

namespace tinylang
{
    class KeywordFilter
    {
      private:
        llvm::StringMap<token::TokenKind> HashTable;

        void addKeyword(llvm::StringRef Keyword, token::TokenKind TokenCode);

      public:
        void addKeywords();

        token::TokenKind getKeyword(llvm::StringRef Name,
                                    token::TokenKind DefaultTokenCode = token::unknown)
        {
            auto result = HashTable.find(Name);
            if (result != HashTable.end()) { return result->second; }

            return DefaultTokenCode;
        }
    };

    class Lexer
    {
      private:
        llvm::SourceMgr &SrcMgr;
        DiagnosticsEngine &Diags;

        const char *CurPtr;
        llvm::StringRef CurBuf;

        unsigned CurBufferIndex = 0;
        KeywordFilter Keywords;

      public:
        Lexer(llvm::SourceMgr &SrcMgr, DiagnosticsEngine &Diags) : SrcMgr(SrcMgr), Diags(Diags)
        {
            CurBufferIndex = SrcMgr.getMainFileID();
            CurBuf         = SrcMgr.getMemoryBuffer(CurBufferIndex)->getBuffer();
            CurPtr         = CurBuf.begin();
            Keywords.addKeywords();
        }

        DiagnosticsEngine &getDiags() const { return Diags; }

        // Gets source code buffer.
        llvm::StringRef getBuffer() const { return CurBuf; }

        // Returns the next token from the input.
        void next(Token &Result);

      private:
        void makeIdentifier(Token &Result);
        void makeNumber(Token &Result);
        void makeString(Token &Result);
        void makeComment();

        llvm::SMLoc getLoc() { return llvm::SMLoc::getFromPointer(CurPtr); }

        void formToken(Token &Result, const char *TokenEnd, token::TokenKind tokenKind);
    };
};    // namespace tinylang


#endif    // TINYLANG_LEXER_H
