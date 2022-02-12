set(LLVM_LINK_COMPONENTS support)

addtinylanglibrary(
  TinyLangParser
  Parser.cpp
  LINK_LIBS
  TinyLangBasic
  TinyLangLexer
)
