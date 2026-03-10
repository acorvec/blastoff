cmake_minimum_required(VERSION 4.0)
project(BlastOffpp CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POLICY_VERSION_MINIMUM 4.0)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # don't build the supplied example games

FetchContent_Declare(
    raylib
    GIT_REPOSITORY "https://github.com/raysan5/raylib.git"
    GIT_TAG "master"
    GIT_PROGRESS TRUE
)
FetchContent_Declare(
    rapidjson
    GIT_REPOSITORY https://github.com/Tencent/rapidjson/
    GIT_TAG master
    SOURCE_SUBDIR "none"
)

FetchContent_MakeAvailable(raylib)
FetchContent_MakeAvailable(rapidjson)

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/source/*.cpp")
set(PROJECT_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/sources/")

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_INCLUDE})
target_include_directories(${PROJECT_NAME} PRIVATE ${X11_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/_deps/rapidjson-src/include")
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE stdc++exp)
    target_link_options(${PROJECT_NAME} PRIVATE -static-libstdc++ -static-libgcc)
    target_link_options(${PROJECT_NAME} PRIVATE -static)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE _DEBUG=1)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(${PROJECT_NAME} PRIVATE _DEBUG=0)
endif()

# emscripten is the reincarnation of the devil in software form.
# this is a counter to remind you of how many afternoons this bullshit has wasted: 
# 4

if (EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")

    # required due to bullshit
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FAGS} -s ASYNCIFY")
    
    target_compile_definitions(${PROJECT_NAME} PRIVATE COMPILE_TARGET_EMSCRIPTEN=1)

    file(GLOB_RECURSE PNG_RESOURCES CONFIGURE "${CMAKE_CURRENT_LIST_DIR}/resource/png/*")
    file(GLOB_RECURSE TTF_RESOURCES CONFIGURE "${CMAKE_CURRENT_LIST_DIR}/resource/ttf/*")
    file(GLOB_RECURSE WAV_RESOURCES CONFIGURE "${CMAKE_CURRENT_LIST_DIR}/resource/wav/*")
    file(GLOB_RECURSE OGG_RESOURCES CONFIGURE "${CMAKE_CURRENT_LIST_DIR}/resource/ogg/*")
    
    set(PRELOAD_FILES ${PNG_RESOURCES} ${WAV_RESOURCES} ${OGG_RESOURCES} ${TTF_RESOURCES})
    foreach (FILE ${PRELOAD_FILES})
        target_link_libraries(${PROJECT_NAME} PRIVATE "--preload-file \"${FILE}\"")
    endforeach ()
    message("${ASSET_BUNDLE_CMD}")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -gsource-map")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")
    target_link_options(${PROJECT_NAME} PRIVATE -fexceptions)
endif ()

# fixes builds in vscode
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}")
