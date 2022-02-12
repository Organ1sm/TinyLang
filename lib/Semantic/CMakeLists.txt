set(LLVM_LINK_COMPONENTS support)

addtinylanglibrary(
  TinyLangSema
  Scope.cpp
  Sema.cpp
  LINK_LIBS
  TinyLangBasic
)

