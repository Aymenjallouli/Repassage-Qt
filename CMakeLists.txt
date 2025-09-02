cmake_minimum_required(VERSION 3.16)

project(LogisticsManagementSystem VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6 components
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Sql Charts PrintSupport Pdf)

# Enable Qt6 MOC, UIC and RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Include directories
include_directories(src)
include_directories(src/entities)
include_directories(src/services)
include_directories(src/ui)
include_directories(src/utils)
include_directories(src/db)

# Collect all source files
file(GLOB_RECURSE SOURCES
    "src/*.cpp"
    "src/*.h"
    "src/*.ui"
)

# Create executable
add_executable(LogisticsManagementSystem ${SOURCES})

# Link Qt6 libraries
target_link_libraries(LogisticsManagementSystem 
    Qt6::Core 
    Qt6::Widgets 
    Qt6::Sql 
    Qt6::Charts 
    Qt6::PrintSupport
    Qt6::Pdf
)

# Set output directory
set_target_properties(LogisticsManagementSystem PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Copy Qt6 DLLs to output directory on Windows
if(WIN32)
    add_custom_command(TARGET LogisticsManagementSystem POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:Qt6::Core>
        $<TARGET_FILE:Qt6::Widgets>
        $<TARGET_FILE:Qt6::Sql>
        $<TARGET_FILE:Qt6::Charts>
        $<TARGET_FILE:Qt6::PrintSupport>
        $<TARGET_FILE:Qt6::Pdf>
        $<TARGET_FILE_DIR:LogisticsManagementSystem>)
        
    # Copy platform plugins
    add_custom_command(TARGET LogisticsManagementSystem POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
        $<TARGET_FILE_DIR:LogisticsManagementSystem>/platforms
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${Qt6_DIR}/../../../plugins/platforms"
        $<TARGET_FILE_DIR:LogisticsManagementSystem>/platforms)
        
    # Copy SQL plugins to the sqldrivers folder
    add_custom_command(TARGET LogisticsManagementSystem POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
        $<TARGET_FILE_DIR:LogisticsManagementSystem>/sqldrivers
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${Qt6_DIR}/../../../plugins/sqldrivers"
        $<TARGET_FILE_DIR:LogisticsManagementSystem>/sqldrivers)
endif()
