#ifndef COMMON_UTIL_HPP
#define COMMON_UTIL_HPP

#include "Platform.hpp"
#include "Format.hpp"

#define UTIL_FILE_LINE						Helena::Util::GetFileName(__FILE__), __LINE__
#define UTIL_CONSOLE_INFO(format, ...)		Helena::Util::Console(UTIL_FILE_LINE, Helena::Util::ELevelLog::Info, format, ##__VA_ARGS__)
#define UTIL_CONSOLE_WARN(format, ...)		Helena::Util::Console(UTIL_FILE_LINE, Helena::Util::ELevelLog::Warn, format, ##__VA_ARGS__)
#define UTIL_CONSOLE_ERROR(format, ...)		Helena::Util::Console(UTIL_FILE_LINE, Helena::Util::ELevelLog::Error, format, ##__VA_ARGS__)

namespace Helena::Util
{
	namespace Internal {
	#if HF_STANDARD_VER == HF_STANDARD_CPP17
		template<class InputIt, class ForwardIt>
		[[nodiscard]] static constexpr InputIt find_first_of(InputIt first, InputIt last, ForwardIt s_first, ForwardIt s_last) noexcept;
	#endif
	}

	enum class ELevelLog : uint8_t {
		Info,
		Warn,
		Error
	};

	/**
	* @brief	Log message to console
	* 
	* @tparam	Format		Type of format
	* @tparam	Args		Type of arguments
	* @tparam	Char		Type of character
	* 
	* @param	filename	Result of Util::GetFileName(__FILE__)
	* @param	line		Result of __LINE__
	* @param	level		Log level
	* @param	format		Message with formatting support
	* @param	args		Arguments for formatting
	*/
	template <typename Format, typename... Args, typename Char = fmt::char_t<Format>>
	inline void Console(const std::string_view filename, const std::size_t line, const ELevelLog level, const Format& format, Args&&... args);

	/**
	* @brief	Sleep current thread on N milliseconds
	* 
	* @param	milliseconds	Time in milliseconds
	*/
	inline void Sleep(const uint64_t milliseconds);

	/**
		* @brief	Split string using delimeter (support trim) 
		* 
		* @tparam	Type		Type of vector value 
		* 
		* @param	input		Input string data for split 
		* @param	delimeter	Delimeter (default: ",") 
		* @param	trim		Remove space from data 
		* 
		* @details	Split input data using delimiter and return std::vector<Type> 
		* @note	Trim remove space char's from start and end position. 
		*			Example: " Hello World " "Hello World" 
		*/
	template <typename Type>
	[[nodiscard]] inline std::vector<Type> Split(std::string_view input, std::string_view delimeters = ",", bool trim = true);

	/**
		* @brief	Return cutted filename in constexpr
		* 
		* @param	file	Macros of __FILE__
		* 
		* @return	@code{.cpp} const char* @endocde
		*/
	[[nodiscard]] constexpr const char* GetFileName(const std::string_view file);

	//------------[Bit Operations]---------------//
	/**
		* @brief	Assign bit flags from right to left side 
		*
		* @tparam	TypeLeft	Type of data with flags 
		* @tparam	TypeRight	Type of bit flag 
		* @param	left		Data with flags 
		* @param	right		Bit flag 
		*
		* @note	This function assigns new flags to <left>. 
		*			It's remove old data from <left> variable. 
		*			Support multiple flags in <right>. 
		*/
	template <typename TypeLeft, typename TypeRight>
	inline void BitAssign(TypeLeft& left, TypeRight right);
		
	/**
		* @brief	Add bit flag from right to left side 
		* 
		* @tparam	TypeLeft	Type of data with flags 
		* @tparam	TypeRight	Type of bit flag 
		* @param	left		Data with flags 
		* @param	right		Bit flag 
		* 
		* @note	Support multiple flags in <right>
		*/
	template <typename TypeLeft, typename TypeRight>
	inline void BitSet(TypeLeft& left, TypeRight right);

	/**
		* @brief	Get bit flag in left from right 
		*
		* @tparam	TypeLeft	Type of data with flags 
		* @tparam	TypeRight	Type of bit flag 
		* @param	left		Data with flags 
		* @param	right		Bit flag 
		*
		* @return	Return false if right flag disabled 
		* 
		* @note	This function check a <left> on flag. 
		*			Support only single flag in <right>. 
		*/
	template <typename TypeLeft, typename TypeRight>
	[[nodiscard]] inline bool BitGet(TypeLeft left, TypeRight right);

	/**
		* @brief	Remove bit flag from left using right 
		* 
		* @tparam	TypeLeft	Type of data with flags 
		* @tparam	TypeRight	Type of bit flag  
		* @param	left		Data with flags 
		* @param	right		Bit flag 
		* 
		* @note	Support multiple flags in <right>. 
		*/
	template <typename TypeLeft, typename TypeRight>
	inline void BitRemove(TypeLeft& left, TypeRight right);
		
	/**
		* @brief	Xor bit flag in left from right 
		*
		* @tparam	TypeLeft	Type of data with flags 
		* @tparam	TypeRight	Type of bit flag 
		* @param	left		Data with flags 
		* @param	right		Bit flag 
		* 
		* @note	This function invert flags in <left>. \n 
		*			Support multiple flags in <right>.
		*/
	template <typename TypeLeft, typename TypeRight>
	inline void BitXor(TypeLeft& left, TypeRight right);

	/**
		* @brief	Check left on has multiple flags from right 
		*
		* @tparam	TypeLeft	Type of data with flags 
		* @tparam	TypeRight	Type of bit flag 
		* @param	left		Data with flags 
		* @param	right		Bit flag 
		* 
		* @return	Returns false if at least one of the right flags is missing. 
		*
		* @note	Analog BitGet, support multiple flags in <right>. 
		*/
	template <typename TypeLeft, typename TypeRight>
	[[nodiscard]] inline bool BitHas(TypeLeft left, TypeRight right);

	/**
		* @brief	Compare left and right on equality 
		*
		* @tparam	TypeLeft	Type of data with flags 
		* @tparam	TypeRight	Type of bit flag 
		* @param	left		Data with flags 
		* @param	right		Bit flag 
		*
		* @return	Returns false if at least one of the flags is missing. 
		*/ 
	template <typename TypeLeft, typename TypeRight>
	[[nodiscard]] inline bool BitCompare(TypeLeft left, TypeRight right);
}

#include "Util.ipp"

#endif	// COMMON_UTIL_HPP