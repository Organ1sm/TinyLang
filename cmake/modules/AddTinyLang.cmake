macro(AddTinyLangSubdirectory name)
    add_llvm_subdirectory(TINYLANG TOOL ${name})
endmacro()

macro(AddTinyLangLibrary name)
    if (BULID_SHARED_LIBS)
        set(LIBTYPE SHARED)
    else ()
        set(LIBTYPE STATIC)
    endif ()

    llvm_add_library(${name} ${LIBTYPE} ${ARGN})

    if (TARGET ${name})
        target_link_libraries(${name} INTERFACE ${LLVM_COMMON_LIBS})
        install(TARGETS ${name}
                COMPONENT ${name}
                LIBRARY DESTINATION lib${LLVM_LIBDIR_SUFFIX}
                ARCHIVE DESTINATION lib${LLVM_LIBDIR_SUFFIX}
                RUNTIME DESTINATION bin
        )
    else ()
        add_custom_target(${name})
    endif ()
endmacro()

macro(AddTinyLangExecutable name)
  add_executable(${name} ${ARGN})
endmacro()

macro(AddTinyLangTool name)
    AddTinyLangExecutable(${name} ${ARGN})
    install(TARGETS ${name}
            RUNTIME DESTINATION bin
            COMPONENT ${name}
    )
endmacro()