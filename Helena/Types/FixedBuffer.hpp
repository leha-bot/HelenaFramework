#ifndef HELENA_TYPES_FIXEDBUFFER_HPP
#define HELENA_TYPES_FIXEDBUFFER_HPP

#include <Helena/Dependencies/FixedString.hpp>
#include <Helena/Debug/Assert.hpp>
#include <Helena/Types/Format.hpp>
#include <Helena/Util/Length.hpp>

namespace Helena::Types
{
	template <std::size_t Capacity, typename Char = char, Util::ELengthPolicy Policy = Util::ELengthPolicy::Fixed>
	class FixedBuffer
	{
		static constexpr std::size_t max_size = std::numeric_limits<std::size_t>::max();

		constexpr void FillBuffer(const Char* data, std::size_t size = max_size) noexcept 
		{
			if(!data && size) {
				Clear();
				return;
			}

			if(size == max_size) {
				size = Util::Length(Policy, data, Capacity);
			} 
			else if(size > Capacity) 
			{
				if constexpr(Policy == Util::ELengthPolicy::Truncate) {
					size = Capacity;
				} else if constexpr(Policy == Util::ELengthPolicy::Fixed) {
					Clear();
					return;
				}
			}

			std::memcpy(m_Buffer.data(), data, size);
			m_Buffer[size] = '\0';
			//if(static_cast<std::size_t>(m_Size) > size) {
			//	// Fill all ?
			//	//std::memset(m_Buffer.data() + static_cast<std::ptrdiff_t>(size), '\0', static_cast<std::size_t>(m_Size) - size);

			//	// Just add one null terminator
			//	m_Buffer[size] = '\0';
			//}

			m_Size = static_cast<size_type>(size);
		}

	public:
		using value_type =	fixstr::basic_fixed_string<Char, Capacity>;
		using size_type	 =	std::conditional_t<Capacity <= std::numeric_limits<std::uint8_t>::max(), std::uint8_t,
							std::conditional_t<Capacity <= std::numeric_limits<std::uint16_t>::max(), std::uint16_t,
							std::conditional_t<Capacity <= std::numeric_limits<std::uint32_t>::max(), std::uint32_t, std::uint64_t>>>;

		constexpr FixedBuffer() = default;
		constexpr ~FixedBuffer() = default;

		constexpr FixedBuffer(const FixedBuffer& other) noexcept {
			FillBuffer(other.m_Buffer.data(), other.m_Size);
		}

		constexpr FixedBuffer(FixedBuffer&& other) noexcept = delete;

		constexpr FixedBuffer(const Char* data) noexcept {
			FillBuffer(data);
		}

		template <std::size_t Capacity>
		constexpr FixedBuffer(const Format<Capacity>& other) noexcept {
			FillBuffer(other.GetData(), other.GetSize());
		}

		constexpr void SetData(const Char* data) noexcept {
			FillBuffer(data);
		}

		[[nodiscard]] constexpr const Char* GetData() const noexcept {
			return m_Buffer.data();
		}

		[[nodiscard]] constexpr std::size_t GetSize() const noexcept {
			return m_Size;
		}

		[[nodiscard]] static constexpr std::size_t GetCapacity() noexcept {
			return Capacity;
		}

		template <std::size_t Capacity>
		[[nodiscard]] constexpr bool IsEqual(const FixedBuffer<Capacity>& other) const noexcept {
			if(m_Size != other.m_Size) return false;
			return std::equal(m_Buffer.cbegin(), m_Buffer.cbegin() + m_Size, other.m_Buffer.cbegin(), other.m_Buffer.cbegin() + other.m_Size);
		}

		template <std::size_t Capacity>
		[[nodiscard]] constexpr bool IsEqual(const Format<Capacity>& other) const noexcept {
			if(m_Size != other.GetSize()) return false;
			return std::equal(m_Buffer.cbegin(), m_Buffer.cbegin() + m_Size, other.GetData(), other.m_Buffer.GetData() + other.GetSize());
		}

		[[nodiscard]] constexpr bool IsEmpty() const noexcept {
			return !m_Size;
		}

		[[nodiscard]] constexpr void Clear() noexcept {
			std::fill_n(m_Buffer.data(), m_Size, '\0');
			m_Size = 0;
		}

		constexpr FixedBuffer& operator=(const FixedBuffer& other) noexcept {
			FillBuffer(other.m_Buffer.data(), other.m_Size);
			return *this;
		}

		constexpr FixedBuffer& operator=(FixedBuffer&& other) noexcept = delete;

		constexpr FixedBuffer& operator=(const Char* data) noexcept {
			FillBuffer(data);
			return *this;
		}

		template <std::size_t Capacity>
		constexpr FixedBuffer& operator=(const Format<Capacity>& other) noexcept  {
			FillBuffer(other.GetData(), other.GetSize());
			return *this;
		}

		[[nodiscard]] constexpr operator std::basic_string_view<Char>() const noexcept {
			return {m_Buffer.data(), m_Size};
		}

		value_type m_Buffer;
		size_type m_Size {};
	};

	template <std::size_t Capacity>
	FixedBuffer(Format<Capacity>&&) -> FixedBuffer<Capacity, char>;

	template <std::size_t Capacity, typename Char = char>
	FixedBuffer(const Char (&array)[Capacity]) -> FixedBuffer<Capacity, Char>;
}

#endif // HELENA_TYPES_FIXEDBUFFER_HPP
