
# general info  
cmake_minimum_required(VERSION 3.20)  
set(PROJECT_NAME dx12)  
project(${PROJECT_NAME} VERSION 1.0.0)  
message ("----------------Project File Generation for ${PROJECT_NAME}----------------- ")  


# common options  
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)  
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})  
set(CMAKE_TOOLCHAIN_FILE "C:/Users/fatba/vcpkg/scripts/buildsystems/vcpkg.cmake")  

# cpp options  
set(CMAKE_CXX_STANDARD 20)  
set(CMAKE_CXX_STANDARD_REQUIRED True)  

# Recources  
file(GLOB_RECURSE SOURCES "Source/*.cpp")  
file(GLOB_RECURSE INCLUDES "Source/*.h")  

# Include  
include_directories(Source)  
include_directories(${CMAKE_SOURCE_DIR}/Source) 


# Exec
add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${INCLUDES})  
add_executable(tests Test/Test.cpp)   

# find  
list(APPEND CMAKE_PREFIX_PATH "C:/Users/fatba/vcpkg/installed/x64-windows")  
find_package(GTest CONFIG REQUIRED)   
find_package(benchmark CONFIG REQUIRED)

# link libs  
target_link_libraries(tests PRIVATE GTest::gtest GTest::gtest_main)  
target_include_directories(tests PRIVATE ${CMAKE_SOURCE_DIR}/Source) 
target_link_libraries(tests PRIVATE benchmark::benchmark benchmark::benchmark_main) 
 
message ("-------Project Files Generated--------")  