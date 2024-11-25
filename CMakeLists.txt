cmake_minimum_required(VERSION 3.10)
project(concatenador VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(concatenador main.cpp)

find_package(boost_program_options REQUIRED CONFIG)
find_package(fmt CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::program_options fmt::fmt)

include(InstallRequiredSystemLibraries)
install(TARGETS concatenador DESTINATION bin)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
