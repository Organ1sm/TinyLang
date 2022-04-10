//
// Created by yw.
//
#include "TinyLang/Lexer/Lexer.h"
#include "TinyLang/Basic/TokenKinds.h"


namespace charinfo
{
    LLVM_READNONE inline bool isASCII(char Ch) { return static_cast<unsigned char>(Ch) <= 127; }

    LLVM_READNONE inline bool isVerticalWhitespace(char Ch) { return isASCII(Ch) && (Ch == '\r' || Ch == '\n'); }

    LLVM_READNONE inline bool isHorizontalWhitespace(char Ch)
    {
        return isASCII(Ch) && (Ch == ' ' || Ch == '\t' || Ch == '\f' || Ch == '\v');
    }

    LLVM_READNONE inline bool isWhitespace(char Ch)
    {
        return isHorizontalWhitespace(Ch) || isVerticalWhitespace(Ch);
    }

    LLVM_READNONE inline bool isDigit(char Ch) { return isASCII(Ch) && Ch >= '0' && Ch <= '9'; }

    LLVM_READNONE inline bool isHexDigit(char Ch)
    {
        return isASCII(Ch) && (isDigit(Ch) || (Ch >= 'A' && Ch <= 'F'));
    }

    LLVM_READNONE inline bool isIdentifierHead(char Ch)
    {
        return isASCII(Ch) && (Ch == '_' || (Ch >= 'A' && Ch <= 'Z') || (Ch >= 'a' && Ch <= 'z'));
    }

    LLVM_READNONE inline bool isIdentifierBody(char Ch) { return isIdentifierHead(Ch) || isDigit(Ch); }
};    // namespace charinfo

namespace tinylang
{
    void KeywordFilter::addKeyword(StringRef Keyword, token::TokenKind TokenCode)
    {
        HashTable.insert(std::make_pair(Keyword, TokenCode));
    }

    void KeywordFilter::addKeywords()
    {
#define KEYWORD(NAME, FLAGS) addKeyword(StringRef(#NAME), token::kw_##NAME);
#include "TinyLang/Basic/TokenKinds.def"
    }

    void Lexer::next(Token &Result)
    {
        while (*CurPtr && charinfo::isWhitespace(*CurPtr)) { ++CurPtr; }
        if (!*CurPtr)
        {
            Result.setKind(token::eof);
            return;
        }
        if (charinfo::isIdentifierHead(*CurPtr))
        {
            identifier(Result);
            return;
        }
        else if (charinfo::isDigit(*CurPtr))
        {
            number(Result);
            return;
        }
        else if (*CurPtr == '"' || *CurPtr == '\'')
        {
            string(Result);
            return;
        }
        else
        {
            switch (*CurPtr)
            {
#define CASE(ch, tok) \
    case ch: formToken(Result, CurPtr + 1, tok); break
                CASE('=', token::equal);
                CASE('#', token::hash);
                CASE('+', token::plus);
                CASE('-', token::minus);
                CASE('*', token::star);
                CASE('/', token::slash);
                CASE(',', token::comma);
                CASE('.', token::period);
                CASE(';', token::semi);
                CASE(')', token::rparen);
#undef CASE
                case '(':
                    if (*(CurPtr + 1) == '*')
                    {
                        comment();
                        next(Result);
                    }
                    else
                        formToken(Result, CurPtr + 1, token::lparen);
                    break;
                case ':':
                    if (*(CurPtr + 1) == '=')
                        formToken(Result, CurPtr + 2, token::colonequal);
                    else
                        formToken(Result, CurPtr + 1, token::colon);
                    break;
                case '<':
                    if (*(CurPtr + 1) == '=')
                        formToken(Result, CurPtr + 2, token::lessequal);
                    else
                        formToken(Result, CurPtr + 1, token::less);
                    break;
                case '>':
                    if (*(CurPtr + 1) == '=')
                        formToken(Result, CurPtr + 2, token::greaterequal);
                    else
                        formToken(Result, CurPtr + 1, token::greater);
                    break;
                default: Result.setKind(token::unknown);
            }
            return;
        }
    }

    void Lexer::identifier(Token &Result)
    {
        const char *Start = CurPtr;
        const char *End   = CurPtr + 1;

        while (charinfo::isIdentifierBody(*End)) ++End;
        StringRef Name(Start, End - Start);
        formToken(Result, End, Keywords.getKeyword(Name, token::identifier));
    }

    void Lexer::number(Token &Result)
    {
        const char *Start     = CurPtr;
        const char *End       = CurPtr + 1;
        token::TokenKind Kind = token::unknown;
        bool IsHex            = false;

        while (*End)
        {
            if (!charinfo::isHexDigit(*End)) break;
            if (!charinfo::isDigit(*End)) IsHex = true;
            ++End;
        }

        switch (*End)
        {
            case 'H': /* hex number */
                Kind = token::IntegerLiteral;
                ++End;
                break;
            default: /* decimal number */
                if (IsHex) Diags.report(getLoc(), diag::err_hex_digit_in_decimal);
                Kind = token::IntegerLiteral;
                break;
        }

        formToken(Result, End, Kind);
    }

    void Lexer::string(Token &Result)
    {
        const char *Start = CurPtr;
        const char *End   = CurPtr + 1;
        while (*End && *End != *Start && !charinfo::isVerticalWhitespace(*End)) ++End;

        if (charinfo::isVerticalWhitespace(*End))
        {
            Diags.report(getLoc(), diag::err_unterminated_char_or_string);
        }

        formToken(Result, End + 1, token::StringLiteral);
    }

    void Lexer::comment()
    {
        const char *End = CurPtr + 2;
        unsigned Level  = 1;

        while (*End && Level)
        {
            // Check for nested comment.
            if (*End == '(' && *(End + 1) == '*')
            {
                End += 2;
                Level++;
            }
            // Check for end of comment
            else if (*End == '*' && *(End + 1) == ')')
            {
                End += 2;
                Level--;
            }
            else
            {
                ++End;
            }
        }

        if (!*End)
        {
            Diags.report(getLoc(), diag::err_unterminated_block_comment);    //
        }

        CurPtr = End;
    }

    void Lexer::formToken(Token &Result, const char *TokEnd, token::TokenKind Kind)
    {
        size_t TokLen = TokEnd - CurPtr;
        Result.Ptr    = CurPtr;
        Result.Length = TokLen;
        Result.Kind   = Kind;
        CurPtr        = TokEnd;
    }
}    // namespace tinylang
