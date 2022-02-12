set(LLVM_LINK_COMPONENTS support)

addtinylanglibrary(
  TinyLangLexer
  Lexer.cpp
  LINK_LIBS
  TinyLangBasic
)
