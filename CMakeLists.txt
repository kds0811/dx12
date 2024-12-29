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
include_directories(  
    ${PUBLIC_DIR}  
    ${CMAKE_SOURCE_DIR}/Source  
)  

# Find packages  
list(APPEND CMAKE_PREFIX_PATH "C:/Users/fatba/vcpkg/installed/x64-windows")  

# Windows SDK and DirectX  
if(MSVC)  
    # check path to Windows SDK  
    get_filename_component(WINDOWS_SDK_PATH "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows Kits\\Installed Roots;KitsRoot10]" ABSOLUTE)  
    
    message(STATUS "Windows SDK Path: ${WINDOWS_SDK_PATH}")  
    
    # include Directx headers 
    include_directories(  
        "${WINDOWS_SDK_PATH}/Include/${CMAKE_WINDOWS_KITS_VERSION}/um"  
        "${WINDOWS_SDK_PATH}/Include/${CMAKE_WINDOWS_KITS_VERSION}/shared"  
        "${WINDOWS_SDK_PATH}/Include/${CMAKE_WINDOWS_KITS_VERSION}/winrt"  
    )  
endif()  

# Packages  
find_package(GTest CONFIG REQUIRED)   
find_package(benchmark CONFIG REQUIRED)  
find_package(directx-headers CONFIG REQUIRED)  
find_package(directxtk12 CONFIG REQUIRED)   

# Exec  Main
add_executable(${PROJECT_NAME} WIN32   
    ${PRIVATE_SOURCES}   
    ${PUBLIC_HEADERS}  
)  

# Exec  Test
add_executable(${TestProjectName}   
    Test/Test.cpp  
)   

# Common DirectX libraries  
set(DIRECTX_LIBS  
    Microsoft::DirectX-Headers  
    Microsoft::DirectXTK12  
    d3d12.lib  
    dxgi.lib  
    dxguid.lib  
)  

# link libs main project   
target_link_libraries(${PROJECT_NAME} PRIVATE   
    ${DIRECTX_LIBS}  
)   

# link libs test project   
target_link_libraries(${TestProjectName} PRIVATE   
    GTest::gtest   
    GTest::gtest_main  
    benchmark::benchmark   
    benchmark::benchmark_main  
    ${DIRECTX_LIBS}  
)  

if(WIN32)  
    # Static linking of runtime library  
    set_property(TARGET ${PROJECT_NAME} PROPERTY   
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")  
    set_property(TARGET ${TestProjectName} PROPERTY   
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")  
    
    # Additional compiler warning and compliance settings  
    if(MSVC)  
        target_compile_options(${PROJECT_NAME} PRIVATE   
              
            /W4   
            /WX-  
            /wd4100  
            /wd4458  
            /permissive-  
        )  
        
        # Same settings for test project  
        target_compile_options(${TestProjectName} PRIVATE   
            /W4   
            /WX-   
            /wd4100   
            /wd4458  
            /permissive-  
        )  
    endif()  
endif()  

# Copy Resources  
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Resources   
     DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})  

message ("-------Project Files Generated--------")  