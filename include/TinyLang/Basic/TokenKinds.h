//
// Created by yw.
//

#ifndef TINYLANG_BASIC_TOKENKINDS_H
#define TINYLANG_BASIC_TOKENKINDS_H

#include "llvm/Support/Compiler.h"

namespace tinylang::token
{
    enum TokenKind : unsigned short {
#define TOK(ID) ID,
#include "TokenKinds.def"

        NUM_TOKENS
    };

    const char *getTokenName(TokenKind kind) LLVM_READNONE;

    const char *getPunctuatorSpelling(TokenKind kind) LLVM_READNONE;

    const char *getKeywordSpelling(TokenKind kind) LLVM_READNONE;


}    // namespace tinylang::token


#endif    // TINYLANG_BASIC_TOKENKINDS_H
