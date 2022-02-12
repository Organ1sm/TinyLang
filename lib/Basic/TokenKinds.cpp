//
// Created by yw.
//
#include "TinyLang/Basic/TokenKinds.h"
#include "llvm/Support/ErrorHandling.h"

namespace tinylang
{
    static const char *const TokenNames[] = {
#define TOK(ID)           #ID,
#define KEYWORD(ID, FLAG) #ID,
#include "TinyLang/Basic/TokenKinds.def"

        nullptr    //
    };

    const char *token::getTokenName(token::TokenKind kind)
    {
        if (kind < token::NUM_TOKENS) { return TokenNames[kind]; }

        llvm_unreachable("unknown TokenKind");
        return nullptr;
    }

    const char *token::getPunctuatorSpelling(token::TokenKind kind)
    {
        switch (kind)
        {
#define PUNCTUATOR(ID, SP) \
    case ID: return SP;
        #include "TinyLang/Basic/TokenKinds.def"

            default: break;
        }
        return nullptr;
    }

    const char *token::getKeywordSpelling(token::TokenKind kind)
    {
        switch (kind)
        {
#define KEYWORD(ID, FLAG)     \
    case kw_##ID: return #ID;
        #include "TinyLang/Basic/TokenKinds.def"

            default: break;
        }
        return nullptr;
    }
}    // namespace tinylang