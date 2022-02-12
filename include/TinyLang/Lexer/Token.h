//
// Created by yw.
//

#ifndef TINYLANG_LEXER_TOKEN_H
#define TINYLANG_LEXER_TOKEN_H

#include "TinyLang/Basic/TokenKinds.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/SMLoc.h"

namespace tinylang
{
    class Lexer;

    class Token
    {
        friend class Lexer;

      private:
        const char *Ptr;          // The location of token.
        size_t Length;            // The length of the token.
        token::TokenKind Kind;    // Kind - The actual flavor of token this is.

      public:
        token::TokenKind getKind() const { return Kind; }
        void setKind(token::TokenKind k) { Kind = k; }

        /// is/isNot - Predicates to check if this token is a
        /// specific kind, as in "if (Tok.is(tok::lbrace))
        bool is(token::TokenKind K) const { return Kind == K; }
        bool isNot(token::TokenKind K) const { return Kind != K; }
        bool isOneOf(token::TokenKind K1, token::TokenKind K2) const { return is(K1) || is(K2); }

        template <typename... Ts> bool isOneOf(token::TokenKind K1, token::TokenKind K2, Ts... Ks) const
        {
            return is(K1) || isOneOf(K2, Ks...);
        }

        size_t getLength() const { return Length; }
        const char *getName() const { return token::getTokenName(Kind); }
        llvm::SMLoc getLocation() const { return llvm::SMLoc::getFromPointer(Ptr); }

        llvm::StringRef getIdentifier()
        {
            assert(is(token::identifier) && "Cannot get identifier of non-identifier");
            return llvm::StringRef(Ptr, Length);
        }

        llvm::StringRef getLiteralData()
        {
            assert(isOneOf(token::integerLiteral, token::stringLiteral)
                   && "Cannot get literal data of non-literal");
            return llvm::StringRef(Ptr, Length);
        }
    };
}    // namespace tinylang

#endif    // TINYLANG_LEXER_TOKEN_H
