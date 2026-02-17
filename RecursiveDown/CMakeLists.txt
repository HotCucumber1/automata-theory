file(
        COPY ${CMAKE_CURRENT_SOURCE_DIR}/input/
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/input/
)

add_executable(
        RecursiveDown main.cpp
        ../Lexer/Lexer.cpp
        ../Lexer/Parser.cpp
        SyntaxAnalyzer.cpp)