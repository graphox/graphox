project(graphox)

cmake_minimum_required(VERSION 2.6)

set(INSTALL_DIR ${PROJECT_SOURCE_DIR})
set(INSTALL_LIBRARY_DIR ${INSTALL_DIR}/lib)
set(INSTALL_ARCHIVE_DIR ${INSTALL_DIR}/lib)
set(INSTALL_RUNTIME_DIR ${INSTALL_DIR}/bin)
set(INSTALL_INCLUDE_DIR ${INSTALL_DIR}/src/include)

set(CMAKE_MODULE_PATH 
        ${CMAKE_MODULE_PATH} 
        ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)

add_subdirectory(src)

