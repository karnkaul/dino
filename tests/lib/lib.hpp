#pragma once
#include <dino/dll_api.hpp>

extern "C" {
DLL_API int dino_sum(int a, int b);
DLL_API void dino_print(char const* str);
}
