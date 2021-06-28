cmake_minimum_required(VERSION 3.0.0)
project(AIForGames)

set(CMAKE_CXX_STANDARD 17) 			# Minimum C++ version
set(CMAKE_CXX_STANDARD_REQUIRED ON) # Enforce minimum C++ on all projects

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Tell IDE to separate files into folders, instead of one large list
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Add libraries
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/raylib)
add_subdirectory(GameEngine)

set(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty)

# Add Applications
add_subdirectory(Game)

# Solution filters
set_target_properties(GameEngine PROPERTIES FOLDER "Engine")
set_target_properties(GamePhysics PROPERTIES FOLDER "Engine/Modules")
set_target_properties(GameUtilities PROPERTIES FOLDER "Engine/Modules")
set_target_properties(EntityComponentSystem PROPERTIES FOLDER "Engine/Modules")

set_target_properties(glfw PROPERTIES FOLDER "Third Party")
set_target_properties(raylib PROPERTIES FOLDER "Third Party")

if(TARGET glfw_objlib)
	set_target_properties(glfw_objlib PROPERTIES FOLDER "Third Party")
endif()

# Disable warnings for certain projects (third party targets)
if(MSVC)
	target_compile_options(raylib PRIVATE /w)
else()
	target_compile_options(raylib PRIVATE -w)
endif()