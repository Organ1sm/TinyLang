//
// Created by yw.
//

#include "TinyLang/Parser/Parser.h"
#include "TinyLang/Basic/TokenKinds.h"

using namespace tinylang;
namespace
{
    OperatorInfo fromToken(Token token) { return OperatorInfo(token.getLocation(), token.getKind()); }
}    // namespace


namespace tinylang
{

    Parser::Parser(Lexer &Lex, Sema &Actions) : Lex(Lex), Actions(Actions) { advance(); }

    ModuleDeclaration *Parser::parse()
    {
        ModuleDeclaration *ModDecl = nullptr;
        parseCompilationUnit(ModDecl);
        return ModDecl;
    }

    bool Parser::parseCompilationUnit(ModuleDeclaration *&D)
    {
        {
            if (consume(token::kw_module)) goto _error;
            if (expect(token::identifier)) goto _error;

            D = Actions.actOnModuleDeclaration(Tok.getLocation(), Tok.getIdentifier());
            EnterDeclScope S(Actions, D);
            advance();

            if (consume(token::semi)) goto _error;

            while (Tok.isOneOf(token::kw_from, token::kw_import))
            {
                if (parseImport()) goto _error;
            }

            DeclList Decls;
            StmtList Stmts;

            if (parseBlock(Decls, Stmts)) goto _error;
            if (expect(token::identifier)) goto _error;

            Actions.actOnModuleDeclaration(D, Tok.getLocation(), Tok.getIdentifier(), Decls, Stmts);
            advance();

            if (consume(token::period)) goto _error;

            return false;
        }
    _error:
        while (!Tok.is(token::eof)) { advance(); }
        return false;
    }

    bool Parser::parseImport()
    {
        {
            IdentList Ids;
            StringRef ModuleName;
            if (Tok.is(token::kw_from))
            {
                advance();
                if (expect(token::identifier)) goto _error;
                ModuleName = Tok.getIdentifier();
                advance();
            }

            if (consume(token::kw_import)) goto _error;
            if (parseIdentList(Ids)) goto _error;
            if (expect(token::semi)) goto _error;

            Actions.actOnImport(ModuleName, Ids);
            advance();

            return false;
        }
    _error:
        while (!Tok.isOneOf(token::kw_begin, token::kw_const, token::kw_end, token::kw_from, token::kw_import,
                            token::kw_procedure, token::kw_var))
        {
            advance();
            if (Tok.is(token::eof)) return true;
        }

        return false;
    }

    bool Parser::parseBlock(DeclList &Decls, StmtList &Stmts)
    {
        {
            while (Tok.isOneOf(token::kw_const, token::kw_procedure, token::kw_var))
            {
                if (parseDeclaration(Decls)) goto _error;
            }

            if (Tok.is(token::kw_begin))
            {
                advance();
                if (parseStatementSequence(Stmts)) goto _error;
            }

            if (consume(token::kw_end)) goto _error;

            return false;
        }
    _error:
        while (!Tok.is(token::identifier))
        {
            advance();
            if (Tok.is(token::eof)) return true;
        }

        return false;
    }

    bool Parser::parseDeclaration(DeclList &Decls)
    {
        {
            if (Tok.is(token::kw_const))
            {
                advance();
                while (Tok.is(token::identifier))
                {
                    if (parseConstantDeclaration(Decls)) goto _error;
                    if (consume(token::semi)) goto _error;
                }
            }
            else if (Tok.is(token::kw_var))
            {
                advance();
                while (Tok.is(token::identifier))
                {
                    if (parseVariableDeclaration(Decls)) goto _error;
                    if (consume(token::semi)) goto _error;
                }
            }
            else if (Tok.is(token::kw_procedure))
            {
                if (parseProcedureDeclaration(Decls)) goto _error;
                if (consume(token::semi)) goto _error;
            }
            else
            {
                /*ERROR*/
                goto _error;
            }
            return false;
        }
    _error:
        while (!Tok.isOneOf(token::kw_begin, token::kw_const, token::kw_end, token::kw_procedure, token::kw_var))
        {
            advance();
            if (Tok.is(token::eof)) return true;
        }

        return false;
    }

    bool Parser::parseConstantDeclaration(DeclList &Decls)
    {
        {
            if (expect(token::identifier)) goto _error;

            SMLoc Loc      = Tok.getLocation();
            StringRef Name = Tok.getIdentifier();

            advance();
            if (expect(token::equal)) goto _error;
            Expr *E = nullptr;
            advance();

            if (parseExpression(E)) goto _error;
            Actions.actOnConstantDeclaration(Decls, Loc, Name, E);

            return false;
        }
    _error:
        while (!Tok.is(token::semi))
        {
            advance();
            if (Tok.is(token::eof)) return true;
        }
        return false;
    }

    bool Parser::parseVariableDeclaration(DeclList &Decls)
    {
        {
            Decl *D;
            IdentList Ids;

            if (parseIdentList(Ids)) goto _error;
            if (consume(token::colon)) goto _error;
            if (parseQualident(D)) goto _error;

            Actions.actOnVariableDeclaration(Decls, Ids, D);

            return false;
        }
    _error:
        while (!Tok.is(token::semi))
        {
            advance();
            if (Tok.is(token::eof)) return true;
        }

        return false;
    }

    bool Parser::parseProcedureDeclaration(DeclList &ParentDecls)
    {
        {
            if (consume(token::kw_procedure)) goto _error;
            if (expect(token::identifier)) goto _error;

            ProcedureDeclaration *D = Actions.actOnProcedureDeclaration(Tok.getLocation(), Tok.getIdentifier());
            EnterDeclScope S(Actions, D);

            FormalParamList Params;
            Decl *RetType = nullptr;
            advance();

            if (Tok.is(token::lparen))
            {
                if (parseFormalParameters(Params, RetType)) goto _error;
            }
            Actions.actOnProcedureHeading(D, Params, RetType);

            if (expect(token::semi)) goto _error;

            DeclList Decls;
            StmtList Stmts;

            advance();
            if (parseBlock(Decls, Stmts)) goto _error;
            if (expect(token::identifier)) goto _error;

            Actions.actOnProcedureDeclaration(D, Tok.getLocation(), Tok.getIdentifier(), Decls, Stmts);
            ParentDecls.push_back(D);
            advance();

            return false;
        }
    _error:
        while (!Tok.is(token::semi))
        {
            advance();
            if (Tok.is(token::eof)) return true;
        }

        return false;
    }

    bool Parser::parseFormalParameters(FormalParamList &Params, Decl *&RetType)
    {
        {
            if (consume(token::lparen)) goto _error;
            if (Tok.isOneOf(token::kw_var, token::identifier))
            {
                if (parseFormalParameterList(Params)) goto _error;
            }
            if (consume(token::rparen)) goto _error;
            if (Tok.is(token::colon))
            {
                advance();
                if (parseQualident(RetType)) goto _error;
            }
            return false;
        }
    _error:
        while (!Tok.is(token::semi))
        {
            advance();
            if (Tok.is(token::eof)) return true;
        }
        return false;
    }

    bool Parser::parseFormalParameterList(FormalParamList &Params)
    {
        {
            if (parseFormalParameter(Params)) goto _error;
            while (Tok.is(token::semi))
            {
                advance();
                if (parseFormalParameter(Params)) goto _error;
            }
            return false;
        }
    _error:
        while (!Tok.is(token::rparen))
        {
            advance();
            if (Tok.is(token::eof)) return true;
        }
        return false;
    }

    bool Parser::parseFormalParameter(FormalParamList &Params)
    {
        {
            IdentList Ids;
            Decl *D;
            bool IsVar = false;

            if (Tok.is(token::kw_var))
            {
                IsVar = true;
                advance();
            }

            if (parseIdentList(Ids)) goto _error;
            if (consume(token::colon)) goto _error;
            if (parseQualident(D)) goto _error;

            Actions.actOnFormalParameterDeclaration(Params, Ids, D, IsVar);

            return false;
        }
    _error:
        while (!Tok.isOneOf(token::rparen, token::semi))
        {
            advance();
            if (Tok.is(token::eof)) return true;
        }
        return false;
    }

    bool Parser::parseStatementSequence(StmtList &Stmts)
    {
        {
            if (parseStatement(Stmts)) goto _error;
            while (Tok.is(token::semi))
            {
                advance();
                if (parseStatement(Stmts)) goto _error;
            }

            return false;
        }
    _error:
        while (!Tok.isOneOf(token::kw_else, token::kw_end))
        {
            advance();
            if (Tok.is(token::eof)) return true;
        }
        return false;
    }

    bool Parser::parseStatement(StmtList &Stmts)
    {
        {
            if (Tok.is(token::identifier))
            {
                Decl *D;
                Expr *E   = nullptr;
                SMLoc Loc = Tok.getLocation();

                if (parseQualident(D)) goto _error;
                if (Tok.is(token::colonequal))
                {
                    advance();
                    if (parseExpression(E)) goto _error;
                    Actions.actOnAssignment(Stmts, Loc, D, E);
                }
                else if (Tok.is(token::lparen))
                {
                    ExprList Exprs;
                    if (Tok.is(token::lparen))
                    {
                        advance();
                        if (Tok.isOneOf(token::lparen, token::plus, token::minus, token::kw_not, token::identifier,
                                        token::StringLiteral))
                        {
                            if (parseExpList(Exprs)) goto _error;
                        }
                        if (consume(token::rparen)) goto _error;
                    }
                    Actions.actOnProcCall(Stmts, Loc, D, Exprs);
                }
            }
            else if (Tok.is(token::kw_if))
            {
                if (parseIfStatement(Stmts)) goto _error;
            }
            else if (Tok.is(token::kw_while))
            {
                if (parseWhileStatement(Stmts)) goto _error;
            }
            else if (Tok.is(token::kw_return))
            {
                if (parseReturnStatement(Stmts)) goto _error;
            }
            else
            {
                /*ERROR*/
                goto _error;
            }
            return false;
        }
    _error:
        while (!Tok.isOneOf(token::semi, token::kw_else, token::kw_end))
        {
            advance();
            if (Tok.is(token::eof)) return true;
        }
        return false;
    }

    bool Parser::parseIfStatement(StmtList &Stmts)
    {
        {
            Expr *E = nullptr;
            StmtList IfStmts, ElseStmts;
            SMLoc Loc = Tok.getLocation();

            if (consume(token::kw_if)) goto _error;
            if (parseExpression(E)) goto _error;
            if (consume(token::kw_then)) goto _error;
            if (parseStatementSequence(IfStmts)) goto _error;
            if (Tok.is(token::kw_else))
            {
                advance();
                if (parseStatementSequence(ElseStmts)) goto _error;
            }

            if (expect(token::kw_end)) goto _error;

            Actions.actOnIfStatement(Stmts, Loc, E, IfStmts, ElseStmts);
            advance();

            return false;
        }
    _error:
        while (!Tok.isOneOf(token::semi, token::kw_else, token::kw_end))
        {
            advance();
            if (Tok.is(token::eof)) return true;
        }
        return false;
    }

    bool Parser::parseWhileStatement(StmtList &Stmts)
    {
        {
            Expr *E = nullptr;
            StmtList WhileStmts;
            SMLoc Loc = Tok.getLocation();

            if (consume(token::kw_while)) goto _error;
            if (parseExpression(E)) goto _error;
            if (consume(token::kw_do)) goto _error;
            if (parseStatementSequence(WhileStmts)) goto _error;
            if (expect(token::kw_end)) goto _error;

            Actions.actOnWhileStatement(Stmts, Loc, E, WhileStmts);
            advance();

            return false;
        }
    _error:
        while (!Tok.isOneOf(token::semi, token::kw_else, token::kw_end))
        {
            advance();
            if (Tok.is(token::eof)) return true;
        }
        return false;
    }

    bool Parser::parseReturnStatement(StmtList &Stmts)
    {
        {
            Expr *E   = nullptr;
            SMLoc Loc = Tok.getLocation();

            if (consume(token::kw_return)) goto _error;
            if (Tok.isOneOf(token::lparen, token::plus, token::minus, token::kw_not, token::identifier,
                            token::IntegerLiteral))
            {
                if (parseExpression(E)) goto _error;
            }

            Actions.actOnReturnStatement(Stmts, Loc, E);

            return false;
        }
    _error:
        while (!Tok.isOneOf(token::semi, token::kw_else, token::kw_end))
        {
            advance();
            if (Tok.is(token::eof)) return true;
        }
        return false;
    }

    bool Parser::parseExpList(ExprList &Exprs)
    {
        {
            Expr *E = nullptr;

            if (parseExpression(E)) goto _error;
            if (E) Exprs.push_back(E);

            while (Tok.is(token::comma))
            {
                E = nullptr;
                advance();
                if (parseExpression(E)) goto _error;
                if (E) Exprs.push_back(E);
            }

            return false;
        }
    _error:
        while (!Tok.is(token::rparen))
        {
            advance();
            if (Tok.is(token::eof)) return true;
        }

        return false;
    }

    bool Parser::parseExpression(Expr *&E)
    {
        {
            if (parseSimpleExpression(E)) goto _error;
            if (Tok.isOneOf(token::hash, token::less, token::lessequal, token::equal, token::greater,
                            token::greaterequal))
            {
                OperatorInfo Op;
                Expr *Right = nullptr;

                if (parseRelation(Op)) goto _error;
                if (parseSimpleExpression(Right)) goto _error;

                E = Actions.actOnExpression(E, Right, Op);
            }
            return false;
        }
    _error:
        while (!Tok.isOneOf(token::rparen, token::comma, token::semi, token::kw_do, token::kw_else, token::kw_end,
                            token::kw_then))
        {
            advance();
            if (Tok.is(token::eof)) return true;
        }
        return false;
    }

    bool Parser::parseRelation(OperatorInfo &Op)
    {
        {
            if (Tok.is(token::equal))
            {
                Op = fromToken(Tok);
                advance();
            }
            else if (Tok.is(token::hash))
            {
                Op = fromToken(Tok);
                advance();
            }
            else if (Tok.is(token::less))
            {
                Op = fromToken(Tok);
                advance();
            }
            else if (Tok.is(token::lessequal))
            {
                Op = fromToken(Tok);
                advance();
            }
            else if (Tok.is(token::greater))
            {
                Op = fromToken(Tok);
                advance();
            }
            else if (Tok.is(token::greaterequal))
            {
                Op = fromToken(Tok);
                advance();
            }
            else
            {
                /*ERROR*/
                goto _error;
            }

            return false;
        }
    _error:
        while (!Tok.isOneOf(token::lparen, token::plus, token::minus, token::kw_not, token::identifier,
                            token::IntegerLiteral))
        {
            advance();
            if (Tok.is(token::eof)) return true;
        }

        return false;
    }

    bool Parser::parseSimpleExpression(Expr *&E)
    {
        {
            OperatorInfo PrefixOp;
            if (Tok.isOneOf(token::plus, token::minus))
            {
                if (Tok.is(token::plus))
                {
                    PrefixOp = fromToken(Tok);
                    advance();
                }
                else if (Tok.is(token::minus))
                {
                    PrefixOp = fromToken(Tok);
                    advance();
                }
            }
            if (parseTerm(E)) goto _error;
            while (Tok.isOneOf(token::plus, token::minus, token::kw_or))
            {
                OperatorInfo Op;
                Expr *Right = nullptr;

                if (parseAddOperator(Op)) goto _error;
                if (parseTerm(Right)) goto _error;

                E = Actions.actOnSimpleExpression(E, Right, Op);
            }
            if (!PrefixOp.isUnspecified()) E = Actions.actOnPrefixExpression(E, PrefixOp);

            return false;
        }
    _error:
        while (!Tok.isOneOf(token::hash, token::rparen, token::comma, token::semi, token::less, token::lessequal,
                            token::equal, token::greater, token::greaterequal, token::kw_do, token::kw_else,
                            token::kw_end, token::kw_then))
        {
            advance();
            if (Tok.is(token::eof)) return true;
        }
        return false;
    }

    bool Parser::parseAddOperator(OperatorInfo &Op)
    {
        {
            if (Tok.is(token::plus))
            {
                Op = fromToken(Tok);
                advance();
            }
            else if (Tok.is(token::minus))
            {
                Op = fromToken(Tok);
                advance();
            }
            else if (Tok.is(token::kw_or))
            {
                Op = fromToken(Tok);
                advance();
            }
            else
            {
                /*ERROR*/
                goto _error;
            }
            return false;
        }
    _error:
        while (!Tok.isOneOf(token::lparen, token::kw_not, token::identifier, token::IntegerLiteral))
        {
            advance();
            if (Tok.is(token::eof)) return true;
        }
        return false;
    }

    bool Parser::parseTerm(Expr *&E)
    {
        {
            if (parseFactor(E)) goto _error;

            while (Tok.isOneOf(token::star, token::slash, token::kw_and, token::kw_div, token::kw_mod))
            {
                OperatorInfo Op;
                Expr *Right = nullptr;

                if (parseMulOperator(Op)) goto _error;
                if (parseFactor(Right)) goto _error;

                E = Actions.actOnTerm(E, Right, Op);
            }

            return false;
        }
    _error:
        while (!Tok.isOneOf(token::hash, token::rparen, token::plus, token::comma, token::minus, token::semi,
                            token::less, token::lessequal, token::equal, token::greater, token::greaterequal,
                            token::kw_do, token::kw_else, token::kw_end, token::kw_or, token::kw_then))
        {
            advance();
            if (Tok.is(token::eof)) return true;
        }

        return false;
    }

    bool Parser::parseMulOperator(OperatorInfo &Op)
    {
        {
            if (Tok.is(token::star))
            {
                Op = fromToken(Tok);
                advance();
            }
            else if (Tok.is(token::slash))
            {
                Op = fromToken(Tok);
                advance();
            }
            else if (Tok.is(token::kw_div))
            {
                Op = fromToken(Tok);
                advance();
            }
            else if (Tok.is(token::kw_mod))
            {
                Op = fromToken(Tok);
                advance();
            }
            else if (Tok.is(token::kw_and))
            {
                Op = fromToken(Tok);
                advance();
            }
            else
            {
                /*ERROR*/
                goto _error;
            }
            return false;
        }
    _error:
        while (!Tok.isOneOf(token::lparen, token::kw_not, token::identifier, token::IntegerLiteral))
        {
            advance();
            if (Tok.is(token::eof)) return true;
        }
        return false;
    }

    bool Parser::parseFactor(Expr *&E)
    {
        {
            if (Tok.is(token::IntegerLiteral))
            {
                E = Actions.actOnIntegerLiteral(Tok.getLocation(), Tok.getLiteralData());
                advance();
            }
            else if (Tok.is(token::identifier))
            {
                Decl *D;
                ExprList Exprs;

                if (parseQualident(D)) goto _error;

                if (Tok.is(token::lparen))
                {
                    advance();
                    if (Tok.isOneOf(token::lparen, token::plus, token::minus, token::kw_not, token::identifier,
                                    token::IntegerLiteral))
                    {
                        if (parseExpList(Exprs)) goto _error;
                    }
                    if (expect(token::rparen)) goto _error;

                    E = Actions.actOnFunctionCall(D, Exprs);
                    advance();
                }
                else if (Tok.isOneOf(token::hash, token::rparen, token::star, token::plus, token::comma,
                                     token::minus, token::slash, token::semi, token::less, token::lessequal,
                                     token::equal, token::greater, token::greaterequal, token::kw_and,
                                     token::kw_div, token::kw_do, token::kw_else, token::kw_end, token::kw_mod,
                                     token::kw_or, token::kw_then))
                {
                    E = Actions.actOnVariable(D);
                }
            }
            else if (Tok.is(token::lparen))
            {
                advance();
                if (parseExpression(E)) goto _error;
                if (consume(token::rparen)) goto _error;
            }
            else if (Tok.is(token::kw_not))
            {
                OperatorInfo Op = fromToken(Tok);
                advance();
                if (parseFactor(E)) goto _error;
                E = Actions.actOnPrefixExpression(E, Op);
            }
            else
            {
                /*ERROR*/
                goto _error;
            }
            return false;
        }
    _error:
        while (!Tok.isOneOf(token::hash, token::rparen, token::star, token::plus, token::comma, token::minus,
                            token::slash, token::semi, token::less, token::lessequal, token::equal, token::greater,
                            token::greaterequal, token::kw_and, token::kw_div, token::kw_do, token::kw_else,
                            token::kw_end, token::kw_mod, token::kw_or, token::kw_then))
        {
            advance();
            if (Tok.is(token::eof)) return true;
        }
        return false;
    }

    bool Parser::parseQualident(Decl *&D)
    {
        {
            D = nullptr;

            if (expect(token::identifier)) goto _error;

            D = Actions.actOnQualIdentPart(D, Tok.getLocation(), Tok.getIdentifier());
            advance();

            while (Tok.is(token::period) && (llvm::isa<ModuleDeclaration>(D)))
            {
                advance();
                if (expect(token::identifier)) goto _error;
                D = Actions.actOnQualIdentPart(D, Tok.getLocation(), Tok.getIdentifier());
                advance();
            }
            return false;
        }
    _error:
        while (!Tok.isOneOf(token::hash, token::lparen, token::rparen, token::star, token::plus, token::comma,
                            token::minus, token::slash, token::colonequal, token::semi, token::less,
                            token::lessequal, token::equal, token::greater, token::greaterequal, token::kw_and,
                            token::kw_div, token::kw_do, token::kw_else, token::kw_end, token::kw_mod,
                            token::kw_or, token::kw_then))
        {
            advance();
            if (Tok.is(token::eof)) return true;
        }
        return false;
    }

    bool Parser::parseIdentList(IdentList &Ids)
    {
        {
            if (expect(token::identifier)) goto _error;

            Ids.push_back(std::pair<SMLoc, StringRef>(Tok.getLocation(), Tok.getIdentifier()));
            advance();

            while (Tok.is(token::comma))
            {
                advance();
                if (expect(token::identifier)) goto _error;

                Ids.push_back(std::pair<SMLoc, StringRef>(Tok.getLocation(), Tok.getIdentifier()));
                advance();
            }
            return false;
        }
    _error:
        while (!Tok.isOneOf(token::colon, token::semi))
        {
            advance();
            if (Tok.is(token::eof)) return true;
        }
        return false;
    }
}    // namespace tinylang
