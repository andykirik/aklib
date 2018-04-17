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

namespace aklib { 
	namespace string {

		//! function ansi2unicode
		/*!
		\param sIn - the character string to convert.
		\param sOut - reference to a string that receives the converted string.
		\return true if convert was successful
		*/
		//@{
		inline bool ansi2unicode(const std::string& sIn, std::wstring& sOut, UINT uiCodepage = CP_ACP)
		{
			return ansi2unicode(sIn.c_str(), sOut, uiCodepage);
		}

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

	} // namespace string

} //namespace aklib

#endif //AKLIB_STRING_HPP