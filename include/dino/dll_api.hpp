#pragma once

#if defined(_WIN32) || defined(_WIN64)
#if defined(DINO_EXPORT)
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif
#elif defined(__linux__) || defined(__APPLE__)
#define DLL_API
#else
#error "Unsupported platform"
#endif
