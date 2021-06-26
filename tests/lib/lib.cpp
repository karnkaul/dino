#include <string_view>
#define DINO_EXPORT

#include <iostream>
#include <lib.hpp>

DLL_API int dino_sum(int a, int b) { return a + b; }
DLL_API void dino_print(std::string_view str) { std::cout << str << std::endl; }
