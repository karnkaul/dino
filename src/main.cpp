#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>

namespace {
void format(std::ostream& out_str, std::string_view fmt) {
	out_str << fmt;
}

template <typename Arg0, typename... Args>
void format(std::ostream& out_str, std::string_view fmt, Arg0&& arg0, Args&&... args) {
	if (!fmt.empty()) {
		constexpr static std::string_view token = "{}";
		auto const search = fmt.find(token);
		if (search < fmt.size()) {
			auto const lhs = fmt.substr(0, search);
			auto const rhs = fmt.substr(search + token.size());
			out_str << lhs << std::forward<Arg0>(arg0);
			format(out_str, rhs, std::forward<Args>(args)...);
		} else {
			format(out_str, fmt);
		}
	}
}

template <typename... Args>
std::string format(std::string_view fmt, Args&&... args) {
	std::stringstream str;
	format(str, fmt, std::forward<Args>(args)...);
	return str.str();
}
} // namespace

int main(int argc, char** argv) {
	if (argc > 0) {
		std::string_view const argv0 = argv[0];
		auto const full_path = std::filesystem::absolute(argv0);
		if (std::filesystem::is_regular_file(full_path)) {
			std::cout << format("Executable located at: {}\n", full_path.generic_string());
		} else {
			std::cerr << "Executable not found!\n";
		}
	}
}
