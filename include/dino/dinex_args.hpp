#pragma once
#include <string_view>
#include <type_traits>
#include <dino/dll_api.hpp>

namespace dino {
///
/// == QUICKSTART ==
/// launcher usage:
///		dinex [-option=value...] <dll_name> [args...]
///
/// library setup: define <entrypoint> in a translation unit:
///		DLL_API int run(dino::args) { /* ... */ }
///

///
/// \brief Viewing container (specialized `std::span`) for command line arguments
///
struct args {
	///
	/// \brief Number of arguments
	///
	int argc{};
	///
	/// \brief Pointer to first argument
	///
	std::string_view const* argv{};

	using const_iterator = std::string_view const*;
	using iterator = const_iterator;
	using reverse_iterator = std::reverse_iterator<iterator>;

	iterator begin() const noexcept { return argv; }
	iterator end() const noexcept { return argv + argc; }
	reverse_iterator rbegin() const noexcept { return reverse_iterator(end()); }
	reverse_iterator rend() const noexcept { return reverse_iterator(begin()); }

	bool empty() const noexcept { return argc == 0; }
	std::size_t size() const noexcept { return static_cast<std::size_t>(argc); }
};

///
/// \brief Signature for library entrypoint
///
using main_t = int(args);
///
/// \brief Type trait to verify custom entrypoint signature
///
template <typename Fn>
constexpr bool is_entrypoint_v = std::is_same_v<Fn, main_t>;
} // namespace dino
