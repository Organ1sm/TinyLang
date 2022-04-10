set(LLVM_LINK_COMPONENTS support)

addtinylanglibrary(
  TinyLangCodeGen
  CGProcedure.cpp
  LINK_LIBS
  TinyLangSema
)
