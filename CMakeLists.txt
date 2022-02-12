cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
endif()

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(TinyLang)
  list(APPEND CMAKE_MODULE_PATH ${LLVM_DIR})
  message(STATUS ${CMAKE_MODULE_PATH})

  # HINTS 指定搜索路径
  find_package(LLVM REQUIRED CONFIG)
  find_package(
    LLVM
    REQUIRED
    HINTS
    ${CMAKE_MODULE_PATH}
  )

  include(AddLLVM)
  include(HandleLLVMOptions)

  message(STATUS "CMakeModulePath: " ${CMAKE_MODULE_PATH})
  message(STATUS "LLvmCMakePath: " ${LLVM_CMAKE_PATH})
  message(STATUS "LLvmDir: " ${LLVM_DIR})
  message(STATUS "LLvmBinaryDir: " ${LLVM_BINARY_DIR})
  message(STATUS "LLvmLibraryDir: " ${LLVM_LIBRARY_DIR})

  include_directories("${LLVM_BINARY_DIR}/include" "${LLVM_INCLUDE_DIR}")
  link_directories("${LLVM_LIBRARY_DIR}")

  set(TINYLANG_BUILT_STANDALONG 1)

endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR AND NOT MSVC_IDE)
  message(FATAL_ERROR "In-source builds are not allowed.")
endif()

set(TINYLANG_VERSION_STRING "0.1")
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/include/TinyLang/Basic/Version.inc.in
  ${CMAKE_CURRENT_BINARY_DIR}/include/TinyLang/Basic/Version.inc
)

include(AddTinyLang)

include_directories(BEFORE ${CMAKE_CURRENT_BINARY_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(lib)
add_subdirectory(tools)
