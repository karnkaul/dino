#pragma once
#include <string>
#include <dino/handle.hpp>

namespace dino {
///
/// \brief Alias for callback on errors
///
using on_err_t = void (*)(std::string_view);
///
/// \brief Customization point for (all) error callbacks
///
inline on_err_t on_err{};

///
/// \brief RAII wrapper around a (loaded) shared library
///
class lib {
  public:
	///
	/// \brief Shared library metadata (platform dependent)
	///
	struct meta_t {
		std::string_view prefix;
		std::string_view extension;
	};
	///
	/// \brief Shared library filename
	///
	struct name_t {
		std::string name;
		meta_t metadata;

		std::string full_name() const;
	};
	///
	/// \brief Wrapper around a loaded function pointer
	///
	template <typename Fn>
	class func_t;

	///
	/// \brief Load a shared library by its (platform independent) id
	/// \param id name of the library (without prefix/extension)
	/// \param dir path to parent directory (if needed)
	///
	explicit lib(std::string id, std::string_view dir = {});
	///
	/// \brief Load a shared library by its (platform dependent) filename
	/// \param name full filename of the library (with prefix/extension)
	/// \param dir path to parent directory (if needed)
	///
	explicit lib(name_t name, std::string_view dir = {});

	lib(lib&& rhs) = default;
	lib& operator=(lib&&) = default;
	lib(lib const& rhs) = delete;
	lib& operator=(lib const&) = delete;

	///
	/// \brief Obtain a const reference to this instance's wrapper handle
	///
	handle_t const& handle() const noexcept { return m_handle; }
	///
	/// \brief Check if this instance points to a valid native library/module
	///
	bool active() const noexcept { return m_handle.active(); }
	///
	/// \brief Obtain the full filename of the library/module this instance is configured to load
	///
	name_t const& name() const noexcept { return m_name; }
	///
	/// \brief Check if this instance points to a valid native library/module
	///
	explicit operator bool() const noexcept { return active(); }
	///
	/// \brief Search for a function in the loaded library/module
	///
	void* search(std::string_view function) const;
	///
	/// \brief Typed wrapper for a searched function in the loaded library/module
	///
	template <typename Fn>
	func_t<Fn> find(std::string_view function) const {
		return func_t<Fn>(reinterpret_cast<Fn*>(search(function)));
	}

  private:
	name_t m_name;
	handle_t m_handle;
};

template <typename Ret, typename... Args>
class lib::func_t<Ret(Args...)> {
  public:
	using type = Ret (*)(Args...);

	func_t() = default;
	explicit func_t(type ptr) noexcept : m_ptr(ptr) {}

	///
	/// \brief Check if this instance points to a valid functiom
	///
	bool valid() const noexcept { return m_ptr != nullptr; }
	///
	/// \brief Check if this instance points to a valid functiom
	///
	explicit operator bool() const noexcept { return valid(); }
	///
	/// \brief Obtain a pointer to the wrapped function
	///
	type get() const noexcept { return m_ptr; }
	///
	/// \brief Execute wrapped function with passed args
	/// Precondition: instance must be valid
	///
	template <typename... Ar>
	Ret operator()(Ar&&... args) const {
		return (*m_ptr)(std::forward<Ar>(args)...);
	}

  private:
	type m_ptr{};
};
} // namespace dino
