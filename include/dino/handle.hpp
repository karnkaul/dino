#pragma once

namespace dino {
///
/// \brief RAII wrapper around a native handle to a (loaded) shared library
///
class handle_t final {
  public:
	handle_t() = default;
	explicit handle_t(void* ptr) noexcept : m_ptr(ptr) {}
	handle_t(handle_t&& rhs) noexcept;
	handle_t& operator=(handle_t&& rhs) noexcept;
	~handle_t() noexcept { release(); }

	///
	/// \brief Obtain a pointer to the native library/module
	///
	void* get() const noexcept { return m_ptr; }
	///
	/// \brief Check if this instance points to a valid native library/module
	///
	bool active() const noexcept { return m_ptr != nullptr; }

  private:
	void check() const;
	void release() noexcept;

	void* m_ptr{};

	friend class lib;
};
} // namespace dino
