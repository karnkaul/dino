#pragma once
#include <cstddef>
#include <iterator>
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
	/// \brief Alias for C sting
	///
	using str_t = char const*;
	///
	/// \brief Number of arguments
	///
	int argc{};
	///
	/// \brief Pointer to first argument
	///
	str_t const* argv{};

	struct iterator {
		using iterator_category = std::bidirectional_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = str_t;
		using reference = value_type;
		using pointer = value_type;

		str_t const* arg{};

		reference operator*() const noexcept { return arg ? *arg : nullptr; }
		pointer operator->() const noexcept { return arg ? *arg : nullptr; }
		iterator& operator++() noexcept { return (++arg, *this); }
		iterator& operator--() noexcept { return (--arg, *this); }
		iterator operator++(int) noexcept { return {arg++}; }
		iterator operator--(int) noexcept { return {arg--}; }
		friend bool operator==(iterator l, iterator r) noexcept { return l.arg == r.arg; }
		friend bool operator!=(iterator l, iterator r) noexcept { return !(l == r); }
	};

	using const_iterator = iterator;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	iterator begin() const noexcept { return {argv}; }
	iterator end() const noexcept { return {argv + argc}; }
	const_iterator cbegin() const noexcept { return {argv}; }
	const_iterator cend() const noexcept { return {argv + argc}; }
	reverse_iterator rbegin() const noexcept { return reverse_iterator(end()); }
	reverse_iterator rend() const noexcept { return reverse_iterator(begin()); }
	const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(cend()); }
	const_reverse_iterator crend() const noexcept { return const_reverse_iterator(cbegin()); }

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
