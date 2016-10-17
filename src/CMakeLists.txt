cmake_minimum_required(VERSION 2.8.12.2 FATAL_ERROR)
project(ggrep)

include_directories(. curses)

set(SOURCES src/application_states.cc
            src/browser_model.cc
            src/buffer_model.cc
            src/context.cc
            src/controller.cc
            src/debug.cc
            src/fbar_model.cc
            src/filter_set.cc
            src/main.cc
            src/processor.cc
            src/prompt_model.cc
            src/state.cc
            src/state_model.cc
            src/terminal_view.cc,
            src/curses/utils.cc)

#-----------------------------------------------------------------------
# Setup executable
#-----------------------------------------------------------------------

add_definitions(-std=c++11)

set(EXE_NAME ggrep)
add_executable(${EXE_NAME} ${SOURCES})
target_link_libraries(${EXE_NAME} ${CMAKE_THREAD_LIBS_INIT}
                                  ${CMAKE_CURSES_LIBS_INIT}
                                  -lstdc++)
