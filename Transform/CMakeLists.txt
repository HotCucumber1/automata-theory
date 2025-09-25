file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/input/
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/input/)

add_executable(
        Transform
        ../Model/MealyMachine.cpp
        ../Model/MooreMachine.cpp
        ../Transform/main.cpp)
