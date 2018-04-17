/* 
   Copyright (c) Andrew Kirik 2017-2018.

   Distributed under the GPU
*/

/*
    My most common file/folder access tasks
*/

/// \file  filesystem.hpp
/// \brief My most common file/folder access tasks
/// \author Andrew Kirik

#pragma once
#ifndef AKLIB_FILESYSTEM_HPP
#define AKLIB_FILESYSTEM_HPP

namespace aklib { 
	namespace filesystem {

		#define FILE_ATTRIBUTE_FAIL	0xffffffff

		//! function FileExists
		/*!
		\param szFileName - fully qualified path or file name
		\return true if file exist
		*/
		//@{
			inline bool isfileexists(LPCTSTR szFileName)
			{
				if (!szFileName || (_tcslen(szFileName) == 0))
					return false;

				DWORD dwFA = ::GetFileAttributes(szFileName);
				if (dwFA == FILE_ATTRIBUTE_FAIL)
					return false;
				else
					return ((dwFA & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY);
			}
		//@}

		//! function DirExists
		/*!
		\param szFileName - fully qualified path or file name
		\return true if directory exist
		*/
		//@{
			inline bool isdirexists(LPCTSTR szDirName)
			{
				if (!szDirName || (_tcslen(szDirName) == 0))
					return false;

				DWORD dwFA = ::GetFileAttributes(szDirName);
				if (dwFA == FILE_ATTRIBUTE_FAIL)
					return false;
				else
					return ((dwFA & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);
			}
		//@}

		//! function parse_get_filename
		/*!	Retrieve file name only (name + extension) from fully qualified path
		\param szFullPath - fully qualified path
		\return file name, or empty string if path is empty
		*/
		//@{
			inline BOOL parse_get_filename(LPCTSTR szFullPath, LPTSTR name, const unsigned int nLength)
			{
				TCHAR drive[_MAX_DRIVE];
				TCHAR dir[_MAX_DIR];
				TCHAR fname[_MAX_FNAME];
				TCHAR ext[_MAX_EXT];

#ifdef VC2003
				_tsplitpath(szFullPath, drive, dir, fname, ext);
#else
				_tsplitpath_s(szFullPath, drive, dir, fname, ext);
#endif

				std::size_t nLen = _tcslen(fname) + _tcslen(ext);
				if (nLen > nLength + 1)
					return FALSE;

				try {
#ifdef VC2003
					_tcsncpy(name, fname, _tcslen(fname));
					_tcsncpy(name+ _tcslen(fname), ext, _tcslen(ext));
#else
					_tcsncpy_s(name, nLength, fname, _tcslen(fname));
					_tcsncpy_s(name + _tcslen(fname), nLength, ext, _tcslen(ext));
#endif
					name[nLen] = 0;
				}
				catch (...)
				{
					return FALSE;
				}

				return TRUE;
			}

			template<typename STD_STRING>
			inline STD_STRING parse_get_filename(STD_STRING szFullPath)
			{
				TCHAR drive[_MAX_DRIVE];
				TCHAR dir[_MAX_DIR];
				TCHAR fname[_MAX_FNAME];
				TCHAR ext[_MAX_EXT];

#ifdef VC2003
				_tsplitpath(szFullPath.c_str(), drive, dir, fname, ext);
#else
				_tsplitpath_s(szFullPath.c_str(), drive, dir, fname, ext);
#endif
				return fname + STD_STRING(ext);
			}
		//@}


	} // namespace filesystem

} //namespace aklib

#endif //AKLIB_FILESYSTEM_HPP