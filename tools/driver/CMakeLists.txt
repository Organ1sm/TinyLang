set(LLVM_LINK_COMPONENTS Support)

AddTinyLangTool(TinyLang Driver.cpp)

target_link_libraries(TinyLang PRIVATE
        TinyLangBasic
        TinyLangLexer
        TinyLangParser
        TinyLangSema
)