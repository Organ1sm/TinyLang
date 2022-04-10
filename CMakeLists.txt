cmake_minimum_required(VERSION 3.15)
project(TinyLang)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH ${LLVM_DIR})

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)

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

  message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}, build type ${LLVM_BUILD_TYPE}")
  message(STATUS "CMakeModulePath: " ${CMAKE_MODULE_PATH})
  message(STATUS "LLVM_CMakePath: " ${LLVM_CMAKE_PATH})
  message(STATUS "LLVM_DIR: " ${LLVM_DIR})
  message(STATUS "LLVM_BinaryDir: " ${LLVM_BINARY_DIR})
  message(STATUS "LLVM_LibDir: " ${LLVM_LIBRARY_DIR})

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
