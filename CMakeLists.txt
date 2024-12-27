
# general info  
cmake_minimum_required(VERSION 3.20)  
set(PROJECT_NAME dx12)  
set(TestProjectName Tests)
project(${PROJECT_NAME} VERSION 1.0.0)  
message ("----------------Project File Generation for ${PROJECT_NAME}----------------- ")  

# common options  
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)  
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})  
set(CMAKE_TOOLCHAIN_FILE "C:/Users/fatba/vcpkg/scripts/buildsystems/vcpkg.cmake")  

# cpp options  
set(CMAKE_CXX_STANDARD 20)  
set(CMAKE_CXX_STANDARD_REQUIRED True)  

# Paths  
set(PUBLIC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Source/Public)  
set(PRIVATE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Source/Private)  

# Recources  
file(GLOB_RECURSE PUBLIC_HEADERS   
    "${PUBLIC_DIR}/*.h"  
)  

file(GLOB_RECURSE PRIVATE_SOURCES   
    "${PRIVATE_DIR}/*.cpp"  
)  

# Auto Filtres Header
foreach(HEADER ${PUBLIC_HEADERS})  
    file(RELATIVE_PATH REL_HEADER "${PUBLIC_DIR}" ${HEADER})  
    get_filename_component(HEADER_PATH "${REL_HEADER}" PATH)  
    string(REPLACE "/" "\\" GROUP_PATH "${HEADER_PATH}")  
    source_group("Public\\${GROUP_PATH}" FILES ${HEADER})  
endforeach()  

# Auto Filtres Source  
foreach(SOURCE ${PRIVATE_SOURCES})  
    file(RELATIVE_PATH REL_SOURCE "${PRIVATE_DIR}" ${SOURCE})  
    get_filename_component(SOURCE_PATH "${REL_SOURCE}" PATH)  
    string(REPLACE "/" "\\" GROUP_PATH "${SOURCE_PATH}")  
    source_group("Private\\${GROUP_PATH}" FILES ${SOURCE})  
endforeach()  

# Include directories  
include_directories(${PUBLIC_DIR})  

# Exec  
add_executable(${PROJECT_NAME} WIN32   
    ${PRIVATE_SOURCES}   
    ${PUBLIC_HEADERS}  
)   
add_executable(${TestProjectName} Test/Test.cpp)   

# find  
list(APPEND CMAKE_PREFIX_PATH "C:/Users/fatba/vcpkg/installed/x64-windows")  
find_package(GTest CONFIG REQUIRED)   
find_package(benchmark CONFIG REQUIRED)  
find_package(directx-headers CONFIG REQUIRED)
find_package(directxtk12 CONFIG REQUIRED)

# link libs  
target_link_libraries(${TestProjectName} PRIVATE GTest::gtest GTest::gtest_main)  
target_include_directories(${TestProjectName} PRIVATE ${CMAKE_SOURCE_DIR}/Source)   
target_link_libraries(${TestProjectName} PRIVATE benchmark::benchmark benchmark::benchmark_main)  
target_link_libraries(${PROJECT_NAME} PRIVATE Microsoft::DirectX-Headers) 
target_link_libraries(${PROJECT_NAME} PRIVATE Microsoft::DirectXTK12)
   
message ("-------Project Files Generated--------") 