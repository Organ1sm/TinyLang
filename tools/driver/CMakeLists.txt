set(LLVM_LINK_COMPONENTS Support)

addtinylangtool(TinyLang Driver.cpp)

target_link_libraries(
  TinyLang
  PRIVATE TinyLangBasic
          TinyLangLexer
          TinyLangParser
          TinyLangSema
)
