cmake_minimum_required(VERSION 2.8.12.2 FATAL_ERROR)
project(ggrep)

include_directories(. curses)

set(SOURCES application_states.cc
            browser_model.cc
            buffer_model.cc
            context.cc
            controller.cc
            debug.cc
            fbar_model.cc
            filter_set.cc
            main.cc
            processor.cc
            prompt_model.cc
            state.cc
            state_model.cc
            terminal_view.cc
            curses/utils.cc
            application_states.h
            browser_model.h
            buffer.h
            buffer_factory.h
            buffer_model.h
            command.h
            constants.h
            consumer.h
            context.h
            controller.h
            debug.h
            event.h
            event_factory.h
            fbar_model.h
            filter_set.h
            input.h
            logmacros.h
            model.h
            observable.h
            processor.h
            producer.h
            prompt_model.h
            range.h
            state.h
            state_model.h
            terminal_view.h
            text.h
            types.h
            view.h
            work_queue.h
            )

#-----------------------------------------------------------------------
# Setup executable
#-----------------------------------------------------------------------

set(EXE_NAME ggrep)
add_executable(${EXE_NAME} ${SOURCES})
target_link_libraries(${EXE_NAME} ${CMAKE_THREAD_LIBS_INIT}
                                  ${CURSES_LIBRARIES}
                                  -lstdc++)
