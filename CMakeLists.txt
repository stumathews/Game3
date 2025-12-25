cmake_minimum_required(VERSION 4.0)

project(Game3 LANGUAGES CXX)

# Use the latest C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Restrict to debug and release builds not things like MinSizeRel and RelWithDebInfo
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

find_package(testlib CONFIG REQUIRED)
find_package(cppgamelib CONFIG REQUIRED)
find_package(unofficial-sodium CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)

# --- SDL2 core ---
set(SDL2_CORE_TARGET SDL2::SDL2)

# --- SDL2_ttf ---
if(TARGET SDL2_ttf::SDL2_ttf)
    set(SDL2_TTF_TARGET SDL2_ttf::SDL2_ttf)
elseif(TARGET SDL2_ttf::SDL2_ttf-static)
    set(SDL2_TTF_TARGET SDL2_ttf::SDL2_ttf-static)
else()
    message(FATAL_ERROR "SDL2_ttf target not found")
endif()

# --- SDL2_image ---
if(TARGET SDL2_image::SDL2_image)
    set(SDL2_IMAGE_TARGET SDL2_image::SDL2_image)
elseif(TARGET SDL2_image::SDL2_image-static)
    set(SDL2_IMAGE_TARGET SDL2_image::SDL2_image-static)
else()
    message(FATAL_ERROR "SDL2_image target not found")
endif()

# --- SDL2_mixer ---
if(TARGET SDL2_mixer::SDL2_mixer)
    set(SDL2_MIXER_TARGET SDL2_mixer::SDL2_mixer)
elseif(TARGET SDL2_mixer::SDL2_mixer-static)
    set(SDL2_MIXER_TARGET SDL2_mixer::SDL2_mixer-static)
else()
    message(FATAL_ERROR "SDL2_mixer target not found")
endif()

# Check if SDL2 targets are static
macro(add_sdl2_static_dependencies target)
    if (${target} MATCHES "-static$")
        if(UNIX)
            target_link_libraries(cppgamelib PRIVATE freetype z png jpeg pthread m dl)
        endif()
    endif()
endmacro()


find_package(mazer REQUIRED)
find_package(llama CONFIG REQUIRED)

add_executable(game3
main.cpp 
GameCommands.cpp 
InputManager.cpp 
LevelManager.cpp 
MoveProbabilityMatrix.cpp 
ExploringNpc.cpp 
LLM.cpp
common.cpp
log.cpp
EmbeddingLLM.cpp
)

target_link_libraries(game3 
PRIVATE 
SDL2::SDL2main
${SDL2_TTF_TARGET}
${SDL2_IMAGE_TARGET}
${SDL2_MIXER_TARGET}
unofficial-sodium::sodium 
cppgamelib::cppgamelib 
mazer::mazer
testlib::testlib
llama
)

if(WIN32)
    target_link_libraries(game3 PRIVATE winmm wsock32 ws2_32)
endif()

# Copy test files in the output folder of the game3 target
add_custom_command(
  TARGET game3 POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          "${CMAKE_CURRENT_SOURCE_DIR}/testdata/"
          "$<TARGET_FILE_DIR:game3>"
)
add_custom_command(
  TARGET game3 POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          "${CMAKE_CURRENT_SOURCE_DIR}/data/"
          "$<TARGET_FILE_DIR:game3>"
)

# Collect all test source files
file(GLOB_RECURSE TEST_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/Tests/*.cpp"
)

if(TEST_SOURCES)
    add_executable(AllTests ${TEST_SOURCES})

    target_link_libraries(AllTests
        PRIVATE
            cppgamelib::cppgamelib
            SDL2::SDL2main
	    ${SDL2_TTF_TARGET}
	    ${SDL2_IMAGE_TARGET}
	    ${SDL2_MIXER_TARGET}
            unofficial-sodium::sodium
            GTest::GTest
            mazer::mazer
            llama
    )

    if(WIN32)
        target_link_libraries(AllTests PRIVATE winmm wsock32 ws2_32)
    endif()
else()
    message(WARNING "No test sources found. Skipping AllTests executable.")
endif()
