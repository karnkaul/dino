# dino

## A simple, multi-platform Dynamic Library Loader using C++17

[![Build status](https://ci.appveyor.com/api/projects/status/hxfv294rbqqv6qxj?svg=true)](https://ci.appveyor.com/project/karnkaul/dino)

`dino`'s central interface is an RAII class `lib`, each instance of which wraps a loaded shared library/module. `dinex` is a simple launcher executable that loads a shared library/module and runs a main-like function on it; set `DINO_BUILD_DINEX=ON` to build.

### Features

- All API types have full RAII support, avoid throwing exceptions, and are moveable and default-constructible
- Ultra lightweight header: only depends on `<string>`
- No platform dependent types (like `HMODULE`) leaked / needed
- Typed function object wrapper and function invocation

### Requirements

- Linux or Windows or MacOSX
- C++17 compiler (and stdlib)
  - Tested with `g++`, `clang++`, `cl.exe`
- CMake 3.14+

### dino usage

1. Copy / clone / submodule [dino](https://github.com/karnkaul/dino) to an appropriate subdirectory in the project
1. Link to `dino` via `target_link_libraries(foo PRIVATE dino::dino)`
1. Use `#include <dino/dino.hpp>` for the loader API
1. (Optional) Use `#include <dino/dll_api.hpp>` for ease of symbol import/export for all supported platforms:
   1. Define `DINO_EXPORT` in the translation units that contain implementations (`#define DINO_EXPORT` in `.cpp` files, or `target_compile_definitions(foo PRIVATE DINO_EXPORT)`)
   1. Prefix `DLL_API` to function signatures in both the interface and implementation

#### CMake build options

- `DINO_BUILD_TESTS`: Set `ON` to build tests (`OFF` by default unless `dino` is the project root)
- `DINO_BUILD_DINEX`: Set `ON` to buld `dinex` (`OFF` by default unless `dino` is the project root)

#### Example

```cpp
// library header
#include <dino/dll_api.hpp>

extern "C" {
DLL_API int dino_sum(int a, int b);
DLL_API void dino_print(char const* str);
}

// library cpp file
#define DINO_EXPORT

#include <iostream>
#include <lib.hpp>

extern "C" {
DLL_API int dino_sum(int a, int b) { return a + b; }
DLL_API void dino_print(char const* str) { std::cout << str << std::endl; }
}

// user code
#include <dino/dino.hpp>

// ...

dino::lib lib("dinolib", "/path/to");
std::cout << "[ " << lib.name().full_name() << " ] active: " << std::boolalpha << lib.active() << '\n';
if (lib) {
  if (auto dino_sum = lib.find<int(int, int)>("dino_sum")) {
    std::cout << "dino_sum(1, 2): " << dino_sum(1, 2) << '\n';
  }
  if (auto dino_print = lib.find<void(char const*)>("dino_print")) {
    dino_print("dino wurx!");
  }
}
```

### dinex usage

`dinex` first attempts to load `<dll_name>` at: `<lib_path>` (empty by default), working directory, and executable directory, in order. It then attempts to call `<entrypoint>` (`run` by default), forwarding `<args>` as a `dino::args` parameter (can be ignored). If unsuccessful or upon `<entrypoint>` throwing an unhanded exception, returns `DINO_ERROR_CODE`, else returns `<entrypoint>`'s return value.

#### CMake build options

- `DINEX_ENTRYPOINT_NAME`: Default entrypoint name that dinex will search for and run (`run`)
- `DINEX_SILENT`: Set to `true` to suppress logging by default (`false`)
- `DINEX_ERROR_CODE` Error return code (`1`)

#### Launcher usage

**Syntax**

```
dinex [-option=value...] <dll_name> [args...]
```

Options:

- `-s[=true]`: suppress logging (runtime option, defaults to `true`, overrides `DINDEX_SILENT`)
  - aliases: `-silent`
- `-d=<lib_path>`: directory where shared library/module is located (empty if not specified)
  - aliases: `-dir`, `-directory`
- `-e=<entrypoint>`: entrypoint function to call (`DINEX_ENTRYPOINT_NAME` if not specified)
  - aliases: `-entry`, `-entrypoint`

#### Library setup

- Define `<entrypoint>` in a translation unit:

```cpp
#define DINO_EXPORT
#include <dino/dinex_args.hpp>

DLL_API int run(dino::args) { /* ... */ }
```

- Verify entrypoint signature:

```cpp
static_assert(dino::is_entrypoint_v<decltype(run)>, "Invalid entrypoint signature");
```

- Build as shared library, install in target working directory / adjacent to dinex (executable directory).
- Run via `/path/to/dinex <dll_name> [args...]`

### FAQ

#### Are the loaded functions actually type-safe?

Unfortunately not, `extern "C"` only exports symbol names and thus the only match is via the function name, the typed wrappers are only for calling convenience (they reinterpret cast `void*` function pointers under the hood).

#### What happens if the names match but signatures don't?

Undefined behaviour.

#### Why is this library not header-only?

So that _the interface_ is lightweight and `dino.hpp` can be included with minimal costs to build-time, and without polluting your codebase with `Windows.h` (or `dlfcn.h` for that matter).

[Original repository](https://github.com/karnkaul/dino)

[LICENCE](LICENSE)
