
set(MULTITHREADING_SRC Job.cpp JobQueue.cpp)

find_package(Threads)

add_library(Multithreading ${MULTITHREADING_SRC})
target_link_libraries(Multithreading ${CMAKE_THREAD_LIBS_INIT})
