file(
        COPY ${CMAKE_CURRENT_SOURCE_DIR}/input/
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/input/
)


add_executable(
        NFA
        ../Model/Machine.cpp
        ../Model/MooreMachine.cpp
        ../Model/MealyMachine.cpp
        NFA.cpp)