/* 
   Copyright (c) Andrew Kirik 2017-2018.

   Distributed under the GPU
*/

/*
    My most common string utils
*/

/// \file  string.hpp
/// \brief My most common string utils
/// \author Andrew Kirik

#pragma once
#ifndef AKLIB_STRING_HPP
#define AKLIB_STRING_HPP

#include <windows.h>

namespace aklib { 
	namespace string {

		//! function ansi2unicode
		/*!
		\param sIn - the character string to convert.
		\param sOut - reference to a string that receives the converted string.
		\return true if convert was successful
		*/
		//@{
		/*inline bool ansi2unicode(const std::string& sIn, std::wstring& sOut, UINT uiCodepage = CP_ACP)
		{
			return ansi2unicode(sIn.c_str(), sOut, uiCodepage);
		}*/

		inline bool ansi2unicode(IN LPCSTR sIn, OUT std::wstring& sOut, UINT uiCodepage = CP_ACP)
		{
			bool	bresult = false;

			if (sIn != NULL)
			{
				// Get length of the multibyte
				int length = ::MultiByteToWideChar(uiCodepage, 0, sIn, -1, NULL, 0);

				// Make sure the internal buffer of the string object is large enough. 
				sOut.resize(length);

				if (sOut.size() == size_t(length))
				{
					// Convert to wide char string
					bresult = (length == ::MultiByteToWideChar(uiCodepage, 0, sIn, -1, (LPWSTR)sOut.c_str(), length));

					// Make sure the size of the string is correct.
					sOut.resize(wcslen(sOut.c_str()));
				}
			}
			else
				sOut.clear();

			return bresult;
		}
		//@}

		void wstring2utf8(const std::wstring& input, std::string& output)
		{
			std::locale ru_RU("ru_RU.utf8");  // Any UTF-8 locale would do
			typedef std::codecvt<wchar_t, char, std::mbstate_t> utf8_codecvt_t;
			const utf8_codecvt_t& utf8_codecvt = std::use_facet<utf8_codecvt_t>(ru_RU);
			std::locale utf8_locale(ru_RU, &utf8_codecvt);

			const size_t inlen = input.length() + 1;  // for terminating 0
			const wchar_t* in{ input.c_str() };
			size_t outlen{ inlen };
			char* out{ 0 };
			std::mbstate_t mbstate{ 0 };

			output.clear();

			for (;;)
			{
				if (out != 0)
					delete[] out;

				if ((out = new char[outlen]) == 0)
					break;

				const wchar_t* wp = in;
				char* p = out;
				utf8_codecvt_t::result res;

				// This is the
				res = utf8_codecvt.out(mbstate,
					in, in + inlen, wp,
					out, out + outlen, p);

				if (res == std::codecvt<wchar_t, char, mbstate_t>::ok)
					break;  // All ok
				else if (res == std::codecvt<wchar_t, char, mbstate_t>::noconv)
					break;  // No conversion made, straight copy
				else if (res == std::codecvt<wchar_t, char, mbstate_t>::partial)
					outlen *= 2;  // Not all converted, increase size and try again
				else // error
				{					
					if (out != 0)
					{
						delete[] out;
						out = 0;
					}
					break;
				}
			}

			if (out != 0)
			{
				output = out;
				delete[] out;
			}
		}


	} // namespace string

} //namespace aklib

#endif //AKLIB_STRING_HPP