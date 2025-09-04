cmake_minimum_required(VERSION 3.23)
project(StarletEngine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_SUPPRESS_REGENERATION ON)

add_library(StarletEngine STATIC)

file(GLOB_RECURSE ENGINE_SRC     CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE ENGINE_HEADERS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/inc/*.hpp)

target_sources(StarletEngine
  PRIVATE ${ENGINE_SRC}
  PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/inc
    FILES ${ENGINE_HEADERS}
)

target_include_directories(StarletEngine
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
    $<INSTALL_INTERFACE:include>
)

include(FetchContent)
add_library(glad STATIC ${CMAKE_CURRENT_SOURCE_DIR}/external/src/glad/glad.c)
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/inc)
set_target_properties(glad PROPERTIES FOLDER "External")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG 3.4)
FetchContent_MakeAvailable(glfw)
set_target_properties(glfw PROPERTIES FOLDER "External")

FetchContent_Declare(StarletMath GIT_REPOSITORY https://github.com/masonlet/starletmath.git GIT_TAG main)
FetchContent_Declare(StarletParsers GIT_REPOSITORY https://github.com/masonlet/starletparsers.git GIT_TAG main)
FetchContent_Declare(StarletScene GIT_REPOSITORY https://github.com/masonlet/starletscene.git GIT_TAG main)
FetchContent_Declare(StarletGraphics GIT_REPOSITORY https://github.com/masonlet/starletgraphics.git GIT_TAG main)
FetchContent_Declare(StarletControls GIT_REPOSITORY https://github.com/masonlet/starletcontrols.git GIT_TAG main)
FetchContent_MakeAvailable(StarletMath StarletParsers StarletScene StarletGraphics StarletControls)
target_link_libraries(StarletParsers PUBLIC glad)
target_link_libraries(StarletGraphics PUBLIC glad)

find_package(OpenGL REQUIRED)
target_link_libraries(StarletEngine 
  PUBLIC
  glad glfw OpenGL::GL 
  StarletMath StarletParsers StarletScene StarletGraphics StarletControls
)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "External")
set_target_properties(glfw PROPERTIES FOLDER "External")
if(TARGET update_mappings)
  set_target_properties(update_mappings PROPERTIES FOLDER "External")
endif()
if(TARGET uninstall)
  set_target_properties(uninstall PROPERTIES FOLDER "External")
endif()
