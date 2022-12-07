#ifndef FORMATTING_H
#define FORMATTING_H

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
    std::cout << "[";
    textGreen();
    std::cout << "SUCCESS";
    textReset();
    std::cout << "]\t";
}

void printSuccess(const std::string text) {
    std::cout << "[";
    textGreen();
    std::cout << "SUCCESS";
    textReset();
    std::cout << "]\t";
    std::cout << text;
}

void printError() {
    std::cout << "[";
    textRed();
    std::cout << "ERROR";
    textReset();
    std::cout << "]\t\t";
}

void printError(const std::string text) {
    std::cout << "[";
    textRed();
    std::cout << "ERROR";
    textReset();
    std::cout << "]\t\t";
    std::cout << text;
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
    std::cout << "[";
    textCyan();
    std::cout << "INFO";
    textReset();
    std::cout << "]\t\t";
}

void printInfo(const std::string text) {
    std::cout << "[";
    textCyan();
    std::cout << "INFO";
    textReset();
    std::cout << "]\t\t";
    std::cout << text;
}

#endif