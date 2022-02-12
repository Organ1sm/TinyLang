//
// Created by yw.
//

#ifndef TINYLANG_PARSER_H
#define TINYLANG_PARSER_H

#include "TinyLang/Lexer/Lexer.h"
#include "TinyLang/Semantic/Sema.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/raw_ostream.h"

namespace tinylang
{
    class Parser
    {
        Lexer &Lex;
        Sema &Actions;
        Token Tok;

        DiagnosticsEngine &getDiagnostics() const { return Lex.getDiags(); }

        void advance() { Lex.next(Tok); }

        bool expect(token::TokenKind ExpectedTok)
        {
            if (Tok.is(ExpectedTok)) { return false; }

            const char *Expected = token::getPunctuatorSpelling(ExpectedTok);
            if (!Expected) Expected = token::getKeywordSpelling(ExpectedTok);

            llvm::StringRef Actual(Tok.getLocation().getPointer(), Tok.getLength());
            getDiagnostics().report(Tok.getLocation(), diag::err_expected, Expected, Actual);

            return true;
        }

        bool consume(token::TokenKind ExpectedTok)
        {
            if (Tok.is(ExpectedTok))
            {
                advance();
                return false;
            }

            return true;
        }
        bool parseCompilationUnit(ModuleDeclaration *&D);
        bool parseImport();
        bool parseBlock(DeclList &Decls, StmtList &Stmts);
        bool parseDeclaration(DeclList &Decls);
        bool parseConstantDeclaration(DeclList &Decls);
        bool parseVariableDeclaration(DeclList &Decls);
        bool parseProcedureDeclaration(DeclList &ParentDecls);
        bool parseFormalParameters(FormalParamList &Params, Decl *&RetType);
        bool parseFormalParameterList(FormalParamList &Params);
        bool parseFormalParameter(FormalParamList &Params);
        bool parseStatementSequence(StmtList &Stmts);
        bool parseStatement(StmtList &Stmts);
        bool parseIfStatement(StmtList &Stmts);
        bool parseWhileStatement(StmtList &Stmts);
        bool parseReturnStatement(StmtList &Stmts);
        bool parseExpList(ExprList &Exprs);
        bool parseExpression(Expr *&E);
        bool parseRelation(OperatorInfo &Op);
        bool parseSimpleExpression(Expr *&E);
        bool parseAddOperator(OperatorInfo &Op);
        bool parseTerm(Expr *&E);
        bool parseMulOperator(OperatorInfo &Op);
        bool parseFactor(Expr *&E);
        bool parseQualident(Decl *&D);
        bool parseIdentList(IdentList &Ids);

      public:
        Parser(Lexer &Lex, Sema &Actions);
        ModuleDeclaration *parse();
    };
}    // namespace tinylang

#endif    // TINYLANG_PARSER_H