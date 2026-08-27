#include "SayError.h"
#include <iostream>
#include <format>

void sayError(int line, int column, std::string info) {
    sayError(std::format("{},{} {}", line, column, info));
    std::exit(1);
}
void sayError(std::string info) {
    std::cerr << "\033[31mError: " << info << "\033[0m" << std::endl;
    std::exit(1);
}
void sayWarning(std::string info) {
    std::cerr << "\033[33mWarning: " << info << "\033[0m" << std::endl;
}