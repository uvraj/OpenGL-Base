#ifndef FORMATTING_H
#define FORMATTING_H

void textRed() {
    std::printf("\033[1;31m");
}

void textGreen() {
    std::printf("\033[1;32m");
}

void textCyan() {
    std::printf("\033[1;36m");
}

void textYellow() {
    std::printf("\033[1;33m");	
}

void textReset() {
    std::printf("\033[0m");
}

void printSuccess() {
    std::printf("[");
    textGreen();
    std::printf("SUCCESS");
    textReset();
    std::printf("]\t");
}

void printSuccess(const char *text) {
    std::printf("[");
    textGreen();
    std::printf("SUCCESS");
    textReset();
    std::printf("]\t");
    std::printf(text);
}

void printError() {
    std::printf("[");
    textRed();
    std::printf("ERROR");
    textReset();
    std::printf("]\t\t");
}

void printError(const char *text) {
    std::printf("[");
    textRed();
    std::printf("ERROR");
    textReset();
    std::printf("]\t\t");
    std::printf(text);
}

void printGLError(const char *text) {
    std::printf("[");
    textYellow();
    std::printf("GL_INFO");
    textReset();
    std::printf("]\t");
    std::printf(text);
}

void printInfo() {
    std::printf("[");
    textCyan();
    std::printf("INFO");
    textReset();
    std::printf("]\t\t");
}

void printInfo(const char *text) {
    std::printf("[");
    textCyan();
    std::printf("INFO");
    textReset();
    std::printf("]\t\t");
    std::printf(text);
}

#endif