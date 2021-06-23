#include <sstream>
#include <utility>
#include <dino/dino.hpp>

#if defined(_WIN32) || defined(_WIN64)
#define DINO_WINDOWS
#include <Windows.h>
#else
#define DINO_LINUX
#include <dlfcn.h>
#endif

namespace dino {
#if defined(DINO_LINUX)
using handle_type = void*;
constexpr lib::meta_t g_meta = {"lib", "so"};
#elif defined(DINO_WINDOWS)
using handle_type = HMODULE;
constexpr lib::meta_t g_meta = {{}, "dll"};
#endif

namespace {
std::string full_path(std::string_view dir, std::string_view name) {
	if (dir.empty()) { return std::string(name); }
	std::stringstream str;
	str << dir << '/' << name;
	return str.str();
}

handle_type cast(void* ptr) noexcept { return static_cast<handle_type>(ptr); }
} // namespace

handle_t::handle_t(handle_t&& rhs) noexcept : m_ptr(std::exchange(rhs.m_ptr, nullptr)) {}

handle_t& handle_t::operator=(handle_t&& rhs) noexcept {
	if (&rhs != this) {
		release();
		m_ptr = std::exchange(rhs.m_ptr, nullptr);
	}
	return *this;
}

void handle_t::check() const {
	if (active()) {
#if defined(DINO_LINUX)
		if (char const* err = ::dlerror(); err != nullptr && on_err) { (*on_err)(err); }
#elif defined(DINO_WINDOWS)
		LPSTR buf{};
		auto const flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
		if (::FormatMessageA(flags, nullptr, ::GetLastError(), 0, buf, 0, nullptr) != 0 && on_err) {
			(*on_err)(buf);
			LocalFree(buf);
		}
#endif
	}
}

void handle_t::release() noexcept {
	if (active()) {
#if defined(DINO_LINUX)
		::dlclose(cast(m_ptr));
#elif defined(DINO_WINDOWS)
		::FreeLibrary(cast(m_ptr));
#endif
		m_ptr = {};
		check();
	}
}

std::string lib::name_t::full_name() const {
	std::stringstream str;
	str << metadata.prefix << name << '.' << metadata.extension;
	return str.str();
}

lib::lib(std::string id, std::string_view dir) : lib({std::move(id), g_meta}, dir) {}

lib::lib(name_t name, std::string_view dir) : m_name(std::move(name)) {
	auto path = full_path(dir, m_name.full_name());
#if defined(DINO_LINUX)
	m_handle = handle_t(::dlopen(path.data(), RTLD_LAZY));
#elif defined(DINO_WINDOWS)
	m_handle = handle_t(::LoadLibraryA(path.data()));
#endif
	m_handle.check();
}

void* lib::search(std::string_view function) const {
	void* ret = nullptr;
	if (active()) {
#if defined(DINO_LINUX)
		ret = ::dlsym(cast(m_handle.get()), function.data());
#elif defined(DINO_WINDOWS)
		ret = reinterpret_cast<void*>(::GetProcAddress(cast(m_handle.get()), function.data()));
#endif
		m_handle.check();
	}
	return ret;
}
} // namespace dino
