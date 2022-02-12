//
// Created by yw.
//

#ifndef TINYLANG_SEMA_H
#define TINYLANG_SEMA_H

#include "TinyLang/AST/AST.h"
#include "TinyLang/Semantic/Scope.h"

namespace tinylang
{
    class Sema
    {
        friend class EnterDeclScope;

      private:
        void enterScope(Decl *);
        void leaveScope();
        bool isOperatorForType(token::TokenKind Op, TypeDeclaration *Ty);
        void checkFormalAndActualParameters(llvm::SMLoc Loc, const FormalParamList &Formals, const ExprList &Actuals);

        Scope *CurrentScope;
        Decl *CurrentDecl;
        DiagnosticsEngine &Diags;

        TypeDeclaration *IntegerType;
        TypeDeclaration *BooleanType;
        BooleanLiteral *TrueLiteral;
        BooleanLiteral *FalseLiteral;
        ConstantDeclaration *TrueConst;
        ConstantDeclaration *FalseConst;

      public:
        Sema(DiagnosticsEngine &Diags) : CurrentScope(nullptr), CurrentDecl(nullptr), Diags(Diags) { initialize(); }

        void initialize();

        ModuleDeclaration *actOnModuleDeclaration(llvm::SMLoc Loc, llvm::StringRef Name);
        void actOnModuleDeclaration(ModuleDeclaration *ModDecl, llvm::SMLoc Loc, llvm::StringRef Name, DeclList &Decls,
                                    StmtList &Stmts);
        void actOnImport(llvm::StringRef ModuleName, IdentList &Ids);
        void actOnConstantDeclaration(DeclList &Decls, llvm::SMLoc Loc, llvm::StringRef Name, Expr *E);
        void actOnVariableDeclaration(DeclList &Decls, IdentList &Ids, Decl *D);
        void actOnFormalParameterDeclaration(FormalParamList &Params, IdentList &Ids, Decl *D, bool IsVar);
        ProcedureDeclaration *actOnProcedureDeclaration(llvm::SMLoc Loc, llvm::StringRef Name);
        void actOnProcedureHeading(ProcedureDeclaration *ProcDecl, FormalParamList &Params, Decl *RetType);
        void actOnProcedureDeclaration(ProcedureDeclaration *ProcDecl, llvm::SMLoc Loc, llvm::StringRef Name,
                                       DeclList &Decls, StmtList &Stmts);
        void actOnAssignment(StmtList &Stmts, llvm::SMLoc Loc, Decl *D, Expr *E);
        void actOnProcCall(StmtList &Stmts, llvm::SMLoc Loc, Decl *D, ExprList &Params);
        void actOnIfStatement(StmtList &Stmts, llvm::SMLoc Loc, Expr *Cond, StmtList &IfStmts, StmtList &ElseStmts);
        void actOnWhileStatement(StmtList &Stmts, llvm::SMLoc Loc, Expr *Cond, StmtList &WhileStmts);
        void actOnReturnStatement(StmtList &Stmts, llvm::SMLoc Loc, Expr *RetVal);

        Expr *actOnExpression(Expr *Left, Expr *Right, const OperatorInfo &Op);
        Expr *actOnSimpleExpression(Expr *Left, Expr *Right, const OperatorInfo &Op);
        Expr *actOnTerm(Expr *Left, Expr *Right, const OperatorInfo &Op);
        Expr *actOnPrefixExpression(Expr *E, const OperatorInfo &Op);
        Expr *actOnIntegerLiteral(llvm::SMLoc Loc, llvm::StringRef Literal);
        Expr *actOnVariable(Decl *D);
        Expr *actOnFunctionCall(Decl *D, ExprList &Params);
        Decl *actOnQualIdentPart(Decl *Prev, llvm::SMLoc Loc, llvm::StringRef Name);
    };

    class EnterDeclScope
    {
        Sema &Semantics;

      public:
        EnterDeclScope(Sema &Semantics, Decl *D) : Semantics(Semantics) { Semantics.enterScope(D); }
        ~EnterDeclScope() { Semantics.leaveScope(); }
    };

}    // namespace tinylang
#endif    // TINYLANG_SEMA_H
