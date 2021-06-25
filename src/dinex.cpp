#include <chrono>
#include <iostream>
#include <sstream>
#include <string_view>
#include <vector>
#include <dinex_constants.hpp>
#include <dino/dinex_args.hpp>
#include <dino/dino.hpp>

namespace {
struct options_t {
	std::string_view dir;
	std::string_view entrypoint = dinex::entrypoint_name;
	bool silent = dinex::silent;
} g_options;

struct launchee_t {
	std::string_view dll_name;
	std::string_view entrypoint;
	std::vector<std::string_view> args;
};

struct path_t {
	std::string_view dir;
	std::string_view file;
};

template <typename... T>
bool in(std::string_view match, T... ts) noexcept {
	return (... || (match == ts));
}

void set_option(std::string_view key, std::string_view value) noexcept {
	if (in(key, "s", "silent")) {
		g_options.silent = value == "false" ? false : true;
	} else if (in(key, "d", "dir", "directory")) {
		g_options.dir = value;
	} else if (in(key, "e", "entry", "entrypoint")) {
		g_options.entrypoint = value;
	}
}

int set_options(int argc, char const* argv[]) {
	int ret = 1;
	for (; ret < argc; ++ret) {
		std::string_view arg = argv[ret];
		if (arg[0] != '-') { break; }
		arg = arg.substr(arg.find_first_not_of('-'));
		std::size_t const eq = arg.find('=');
		set_option(arg.substr(0, eq), arg.substr(eq + 1));
	}
	return ret;
}

launchee_t launchee(int argc, char const* argv[]) {
	launchee_t ret;
	int const start = set_options(argc, argv);
	ret.entrypoint = g_options.entrypoint;
	if (argc > start) { ret.dll_name = argv[start]; }
	if (int args = argc - start - 1; args > 0) {
		ret.args.reserve(std::size_t(args));
		for (int i = 0; i < args; ++i) { ret.args.push_back(argv[std::size_t(i + start + 1)]); }
	}
	return ret;
}

path_t path(std::string_view path) {
	auto i = path.find_last_of('/');
	if (i > path.size()) { i = path.find_last_of('\\'); }
	if (i < path.size()) { return {path.substr(0, i), path.substr(i + 1)}; }
	return {{}, path};
}

using sec_t = std::chrono::duration<float>;

std::string prettify(sec_t secs) {
	std::stringstream str;
	float const count = secs.count();
	if (count > 60.0f) {
		int const c = static_cast<int>(count);
		str << c / 60 << 'm' << c % 60 << 's';
		return str.str();
	}
	if (count > 1.0f) {
		int const c = static_cast<int>(count * 1000.0f);
		str << c / 1000 << 's' << c % 1000 << "ms";
		return str.str();
	} else {
		int const c = static_cast<int>(count * 1000.0f * 1000.0f);
		if (c > 1000) { str << c / 1000 << "ms"; }
		str << c % 1000 << "us";
		return str.str();
	}
}

struct dir_t {
	std::string_view dir;

	friend std::ostream& operator<<(std::ostream& out, dir_t const& dir) { return dir.dir.empty() ? out : out << dir.dir << '/'; }
};

struct xout {
	std::ostream& out;

	template <typename T>
	xout& operator<<(T const& t) {
		if (!g_options.silent) { out << t; }
		return *this;
	}
};

xout g_cout{std::cout};
xout g_cerr{std::cerr};

std::pair<dino::lib, dir_t> load(std::string_view str, std::string_view exe_dir) {
	dir_t dir{g_options.dir};
	std::string const id(str);
	dino::lib lib(id, dir.dir);
	if (!lib) { lib = dino::lib(id, dir.dir = "."); }
	if (!lib) { lib = dino::lib(id, dir.dir = exe_dir); }
	return {std::move(lib), dir};
}
} // namespace

int main(int argc, char const* argv[]) {
	if (argc < 1) { return dinex::error_code; }
	path_t const exe_path = path(argv[0]);
	if (argc < 2) {
		g_cout << "Usage: " << exe_path.file << " [-option=value...] <dll_name> [args...]\n";
		return 0;
	}
	launchee_t const launch = launchee(argc, argv);
	if (launch.dll_name.empty()) {
		g_cout << "Usage: " << exe_path.file << " [-option=value...] <dll_name> [args...]\n";
		return 0;
	}
	if (auto const [lib, dir] = load(launch.dll_name, exe_path.dir); lib) {
		if (auto run = lib.find<dino::main_t>(launch.entrypoint)) {
			auto const dll_name = lib.name().full_name();
			g_cout << "Launching [int " << launch.entrypoint << "()] from [" << dir << dll_name << "]\n\n";
			try {
				auto const start = std::chrono::steady_clock::now();
				int const ret = run({static_cast<int>(launch.args.size()), launch.args.data()});
				auto const dt = prettify(std::chrono::steady_clock::now() - start);
				g_cout << "\nFinished running [" << dir << dll_name << "] in " << dt << "\n";
				return ret;
			} catch (std::exception const& e) {
				g_cerr << "\nError: Unhandled exception in [" << dll_name << "]:\n\t" << e.what() << '\n';
				return dinex::error_code;
			}
		} else {
			g_cerr << "Error: entrypoint [" << launch.entrypoint << "] not found in dll [" << dir << lib.name().full_name() << "]\n";
		}
	} else {
		g_cerr << "Error: dll [" << lib.name().full_name() << "] not found in ";
		if (!g_options.dir.empty()) { g_cerr << "[" << g_options.dir << "], "; }
		g_cerr << "working directory or [" << dir << "]\n";
	}
	return dinex::error_code;
}
