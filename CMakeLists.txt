cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_TOOLCHAIN_FILE $ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake
  CACHE STRING "Vcpkg toolchain file")
project(leetcode VERSION 0.1.0)

find_package(range-v3 CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(Threads REQUIRED)
find_package(unifex CONFIG REQUIRED)

include(CTest)
enable_testing()

function(add_exe exe)
  add_executable(${exe} ${exe}.cpp)
  target_compile_features(${exe} PRIVATE cxx_std_20)
  target_compile_options(${exe} PRIVATE -fcoroutines-ts -Wall -pedantic)
  target_link_libraries(${exe} PRIVATE
    fmt::fmt
    range-v3
    range-v3-meta
    range-v3::meta
    range-v3-concepts
    Threads::Threads
    unifex::unifex)
endfunction()

add_exe(coFuture)
add_exe(compareVersion)
add_exe(shellSubstitute)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
