#ifndef FORMATTING_H
#define FORMATTING_H

const std::string ERROR_HINT =  "[\e[0;31m FAIL \033[0m]\t";
const std::string SUCCESS_HINT = "[\e[0;32m  OK  \033[0m]\t";
const std::string INFO_HINT = "[\e[0;36m INFO \033[0m]\t";
const std::string PIPELINE_HINT = INFO_HINT;

void textRed() {
    std::cout << "\033[1;31m";
}

void textGreen() {
    std::cout << "\033[1;32m";
}

void textCyan() {
    std::cout << "\033[1;36m";
}

void textYellow() {
    std::cout << "\033[1;33m";	
}

void textReset() {
    std::cout << "\033[0m";
}

void printSuccess() {
    std::cout << SUCCESS_HINT;
}

void printSuccess(const std::string text) {
    std::cout << SUCCESS_HINT << text;
}

void printError() {
    std::cout << ERROR_HINT;
}

void printError(const std::string text) {
    std::cout << ERROR_HINT << text;
}

void printGLError(const std::string text) {
    std::cout << "[";
    textYellow();
    std::cout << "GL_INFO";
    textReset();
    std::cout << "]\t";
    std::cout << text;
}

void printInfo() {
    std::cout << INFO_HINT;
}

void printInfo(const std::string text) {
    std::cout << INFO_HINT << text;
}

#endif