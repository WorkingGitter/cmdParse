/* ===============================================
*	Author: Richard Chin
*	Date: August 2021
*
*  ===============================================*/

#pragma once
#include <string>
#include <cassert>
#include <algorithm>
#include <cwctype>
#include <windows.h>

namespace WGT
{

	/*!
	*	@brief common string routines for std::wstring
	*/
	class string_utils
	{
	public:
		// Used to convert between std::string and std::wstring
		// see: https://codingtidbit.com/2020/02/09/c17-codecvt_utf8-is-deprecated/

		///<! Convert a wide Unicode string to an UTF8 string
		static std::string wide2ascii_convert(std::wstring& wstr)
		{
			if (wstr.empty()) return std::string();
			int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
			std::string strTo(size_needed, 0);
			WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
			return strTo;
		}

		///<! Convert an UTF8 string to a wide Unicode String
		static std::wstring ascii2wide_convert(std::string& str)
		{
			if (str.empty()) return std::wstring();
			int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
			std::wstring wstrTo(size_needed, 0);
			MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
			return wstrTo;
		}

		/*! Lowercases given string
		*/
		static std::wstring make_lower(const std::wstring& str) {
			std::wstring out_str = str;
			std::transform(out_str.begin(), out_str.end(), out_str.begin(),
				[](wchar_t c) { return std::towlower(c); });
			return out_str;
		}

		static std::string make_lower(const std::string& str)
		{
			std::string out_str = str;
			std::transform(out_str.begin(), out_str.end(), out_str.begin(), [](char c) { return static_cast<char>(std::tolower(static_cast<int>(c))); });
			return out_str;
		}

		/*! Uppercase
		*/
		static std::wstring make_upper(const std::wstring& str) {
			std::wstring out_str = str;
			std::transform(out_str.begin(), out_str.end(), out_str.begin(),
				[](wchar_t c) { return std::towupper(c); });
			return out_str;
		}

		static std::string make_upper(const std::string& str)
		{
			std::string out_str = str;
			std::transform(out_str.begin(), out_str.end(), out_str.begin(), [](char c) { return std::toupper(c); });
			return out_str;
		}

		// trim from start (in place)
		static inline void ltrim(std::wstring& s) {
			s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](wchar_t ch) {
				return !std::iswspace(ch);
				}));
		}

		static inline void ltrim(std::string& s) {
			s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](char ch) {
				return !std::isspace(ch);
				}));
		}

		/*! left trim using given character
		*/
		static inline void ltrim(std::wstring& s, TCHAR c) {
			s.erase(s.begin(), std::find_if(s.begin(), s.end(), [&c](wchar_t ch) {
				return !(ch == c);
				}));
		}

		static inline void ltrim(std::string& s, char c) {
			s.erase(s.begin(), std::find_if(s.begin(), s.end(), [&c](char ch) {
				return !(ch == c);
				}));
		}


		// trim from end (in place)
		static inline void rtrim(std::wstring& s) {
			s.erase(std::find_if(s.rbegin(), s.rend(), [](wchar_t ch) {
				return !std::iswspace(ch);
				}).base(), s.end());
		}

		static inline void rtrim(std::wstring& s, TCHAR c) {
			s.erase(std::find_if(s.rbegin(), s.rend(), [&c](wchar_t ch) {
				return !(ch == c);
				}).base(), s.end());
		}


		// trim from both ends (in place)
		static inline void trim(std::wstring& s, TCHAR c = (TCHAR)' ') {
			if (c == (TCHAR)' ') {
				ltrim(s);
				rtrim(s);
			}
			else {
				ltrim(s, c);
				rtrim(s, c);
			}
		}

		static inline void trim(std::string& s, char c = ' ') {
			if (c == (TCHAR)' ') {
				ltrim(s);

				s.erase(std::find_if(s.rbegin(), s.rend(), [](char ch) {
					return !std::isspace(ch);
					}).base(), s.end());
			}
			else
			{
				ltrim(s, c);

				s.erase(std::find_if(s.rbegin(), s.rend(), [&c](char ch) {
					return !(ch == c);
					}).base(), s.end());

			}
		}


		/*! Test if given string has only spaces
		*/
		static bool is_blank(const std::wstring& s)
		{
			return std::all_of(s.cbegin(), s.cend(), [](wchar_t c) { return std::iswspace(c); });
		}

		static bool is_blank(const std::string& s)
		{
			return std::all_of(s.cbegin(), s.cend(), [](char c) { return std::isspace(c); });
		}

		// Test given string to see if it can be converted to a boolean type
		static bool is_boolean(std::wstring_view sv, bool& converted_value)
		{
			if (sv.empty())
				return false;

			std::wstring test_str = sv.data();
			string_utils::trim(test_str);
			auto lower_str = string_utils::make_lower(test_str);

			// test for positive
			if ((lower_str == L"true") || (lower_str == L"yes") || (lower_str == L"1"))
			{
				converted_value = true;
				return true;
			}

			if ((lower_str == L"false") || (lower_str == L"no") || (lower_str == L"0"))
			{
				converted_value = false;
				return true;
			}

			// no conversion possible
			return false;
		}
	};


}