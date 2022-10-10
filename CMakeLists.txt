cmake_minimum_required (VERSION 2.8.12)
project(bb-tanks)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb3 -O0")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -ggdb3 -O0")

find_package(OGRE 13.4.4 HINTS /data/c-code/ogre/build/sdk/bin REQUIRED)
#find_package(OGRE 13.4.4 REQUIRED)
find_package(Freetype QUIET)

include_directories(${OGRE_INCLUDE_DIRS})
link_directories(${OGRE_LIBRARY_DIRS})

set(IMGUI_SRCS 
    ${CMAKE_SOURCE_DIR}/imgui/imgui.cpp 
    ${CMAKE_SOURCE_DIR}/imgui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/imgui/imgui_widgets.cpp)
set(OGRE_IMGUI_SRCS ${CMAKE_SOURCE_DIR}/ImguiManager.cpp HlmsPbsOutline.h)
if(FREETYPE_FOUND)
    include_directories(${CMAKE_SOURCE_DIR}/imgui/misc/freetype/)
    list(APPEND IMGUI_SRCS ${CMAKE_SOURCE_DIR}/imgui/misc/freetype/imgui_freetype.cpp)
endif()

add_definitions(-DIMGUI_DISABLE_OBSOLETE_FUNCTIONS)
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/imgui/)
add_library(bb-tanks ${IMGUI_SRCS} ${OGRE_IMGUI_SRCS})
set_property(TARGET bb-tanks PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(bb-tanks PUBLIC ${OGRE_LIBRARIES})

if(OGRE_Bites_FOUND)
    target_compile_definitions(bb-tanks PRIVATE -DHAVE_OGRE_BITES)
endif()

if(FREETYPE_FOUND)
    target_compile_definitions(bb-tanks PRIVATE -DUSE_FREETYPE)
    target_include_directories(bb-tanks PRIVATE ${FREETYPE_INCLUDE_DIRS})
    target_link_libraries(bb-tanks PRIVATE ${FREETYPE_LIBRARIES})
endif()

option(BUILD_EXAMPLE "build BBTanks app" ON)
if(BUILD_EXAMPLE)
    # copy essential config files next to our binary where OGRE autodiscovers them
    # file(COPY ${OGRE_CONFIG_DIR}/resources.cfg DESTINATION ${CMAKE_BINARY_DIR})
    file(COPY ${CMAKE_SOURCE_DIR}/resources.cfg DESTINATION ${CMAKE_BINARY_DIR})
    file(COPY ${CMAKE_SOURCE_DIR}/plugins.cfg DESTINATION ${CMAKE_BINARY_DIR})

    add_executable(BBTanks BBTanks.cpp ${CMAKE_SOURCE_DIR}/imgui/imgui_demo.cpp)
    target_link_libraries(BBTanks bb-tanks)

    file(COPY ${OGRE_CONFIG_DIR}/resources.cfg DESTINATION ${CMAKE_SOURCE_DIR}/python/)
endif()

if(NOT FREETYPE_FOUND)
    set(FREETYPE_FOUND "FALSE")
endif()
message(STATUS "using freetype: ${FREETYPE_FOUND}")

# Python Bindings
option(BUILD_PYTHON "build python bindings" ON)
if(BUILD_PYTHON)
    add_subdirectory(python)
endif()