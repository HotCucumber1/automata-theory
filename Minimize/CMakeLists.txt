file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/input/
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/input/)

add_executable(
    MinimizeMealy
    ../Model/MealyMachine.cpp
    ../Model/MooreMachine.cpp
    ../Model/Machine.cpp
    MealyMin.cpp)

add_executable(
    MinimizeMoore
    ../Model/MealyMachine.cpp
    ../Model/MooreMachine.cpp
    ../Model/Machine.cpp
    MooreMin.cpp)
