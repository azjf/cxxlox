#include <filesystem>
#include <fstream>
#include <iostream>
#include <new>
#include <string>

#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "vm.h"

static VM vm;

static void repl()
{
    std::string line;
    line.reserve(1024);
    for (;;) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) {
            printf("\n");
            break;
        }

        vm.interpret(line);
    }
}

static std::string readFile(const char *path)
{
    std::ifstream file(path);
    if (!file) {
        fprintf(stderr, "Could not open file \"%s\".\n", path);
        exit(74);
    }

    auto fileSize = std::filesystem::file_size(path);
    std::string buffer;
    try {
        buffer.resize(fileSize + static_cast<std::ifstream::pos_type>(1), '\0');
    } catch (std::bad_alloc &e) {
        fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
        exit(74);
    }

    try {
        buffer.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
        file.read(&buffer[0], fileSize);
    } catch (std::exception &e) {
        fprintf(stderr, "Could not open file \"%s\".\n", path);
        exit(74);
    }
    return buffer;
}

static void runFile(const char *path)
{
    std::string source = readFile(path);
    InterpretResult result = vm.interpret(source);

    if (result == INTERPRET_COMPILE_ERROR) { exit(65); }
    if (result == INTERPRET_RUNTIME_ERROR) { exit(70); }
}

int main(int argc, const char *argv[])
{
    if (argc == 1) {
        repl();
    } else if (argc == 2) {
        runFile(argv[1]);
    } else {
        std::cerr << "Usage: clox [path]" << std::endl;
        exit(64);
    }

    return 0;
}
