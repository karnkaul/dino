#define DINO_EXPORT

#include <iostream>
#include <string>
#include <dino/dinex_args.hpp>

namespace {
int run_app(dino::args args) {
	std::cout << "app launched successfully!\n";
	if (!args.empty()) {
		std::cout << "args:\n";
		for (auto arg : args) { std::cout << arg << '\n'; }
	}
	return 0;
}
} // namespace

extern "C" {
DLL_API int run_test(dino::args args) { return run_app(args); }
}

static_assert(dino::is_entrypoint_v<decltype(run_test)>, "fubar");
