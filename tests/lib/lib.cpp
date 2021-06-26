#define DINO_EXPORT

#include <iostream>
#include <lib.hpp>

DLL_API int dino_sum(int a, int b) { return a + b; }
DLL_API void dino_print(char const* str) { std::cout << str << std::endl; }
