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

#include <string.h>
#include <tchar.h>
#include <list>
#include <Shlobj.h>

namespace aklib { 
	namespace filesystem {

		namespace details {
			template<typename _STRING>
			_STRING checkbackslash(const _STRING szInput)
			{
				if (szInput.rfind('\\') == (szInput.length() - 1))
					return szInput;
				return (szInput + _T("\\"));
			}

			template<typename _STRING>
			_STRING getmodulepath()
			{
				TCHAR szFileName[_MAX_PATH];
				if (!GetModuleFileName(NULL, szFileName, _MAX_PATH))
					return _T("");

				_STRING filename(szFileName);

				std::size_t pos = filename.find_last_of(_T('\\'));
				if (pos == _STRING::npos)
					return _T("");

				return filename.substr(0, pos + 1);
			}

			template<typename _STRING>
			_STRING getfullmodulename(void)
			{
				TCHAR szFileName[_MAX_PATH];
				if (!GetModuleFileName(NULL, szFileName, _MAX_PATH))
					return _T("");

				return _STRING(szFileName);
			}

		}

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

			template<typename _STRING>
			inline _STRING parse_get_filename(_STRING szFullPath)
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
				return fname + _STRING(ext);
			}
		//@}

			inline BOOL parse_get_folder(LPTSTR szFullPath, LPTSTR szFolderName, const std::size_t nLength)
			{
				// szFolderName must be already allocated of _MAX_PATH size
				TCHAR drive[_MAX_DRIVE];
				TCHAR dir[_MAX_DIR];
				TCHAR fname[_MAX_FNAME];
				TCHAR ext[_MAX_EXT];
				TCHAR szFolderNameTmp[_MAX_PATH];

#ifdef VC2003
				_tsplitpath(szFullPath, drive, dir, fname, ext);
				_tmakepath(szFolderNameTmp, drive, dir, _T(""), _T(""));
#else
				_tsplitpath_s(szFullPath, drive, dir, fname, ext);
				_tmakepath_s(szFolderNameTmp, drive, dir, _T(""), _T(""));
#endif

				std::size_t nLen = _tcslen(szFolderNameTmp);
				if (nLen > nLength + 1)
					return FALSE;

				try {
#ifdef VC2003
					_tcsncpy(szFolderName, szFolderNameTmp, nLen);
#else
					_tcsncpy_s(szFolderName, nLength, szFolderNameTmp, nLen);
#endif
					szFolderName[nLen] = 0;
				}
				catch (...)
				{
					return FALSE;
				}

				return TRUE;
			}

			template<typename _STRING>
			_STRING parse_get_filename_only(_STRING sFileName)
			{
				TCHAR drive[_MAX_DRIVE];
				TCHAR dir[_MAX_DIR];
				TCHAR fname[_MAX_FNAME];
				TCHAR ext[_MAX_EXT];

#ifdef VC2003
				_tsplitpath(sFileName.c_str(), drive, dir, fname, ext);
#else
				_tsplitpath_s(sFileName.c_str(), drive, dir, fname, ext);
#endif
				return fname;
			}

			/*BOOL GetFileNameOnly(LPTSTR szFullPath, LPTSTR name, const unsigned int nLength)
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

				std::size_t nLen = _tcslen(fname);
				if (nLen > nLength + 1)
					return FALSE;

				try {
#ifdef VC2003
					_tcsncpy(name, fname, nLen);
#else
					_tcsncpy_s(name, nLength, fname, nLen);
#endif

					name[nLen] = 0;
				}
				catch (...)
				{
					return FALSE;
				}

				return TRUE;
			}*/

			template<typename _STRING>
			inline _STRING parse_get_folder(_STRING szFullPath)
			{
				// szFolderName must be already allocated of _MAX_PATH size
				TCHAR drive[_MAX_DRIVE];
				TCHAR dir[_MAX_DIR];
				TCHAR fname[_MAX_FNAME];
				TCHAR ext[_MAX_EXT];
				TCHAR szFolderName[_MAX_PATH];

#ifdef VC2003
				_tsplitpath(szFullPath.c_str(), drive, dir, fname, ext);
				_tmakepath(szFolderName, drive, dir, _T(""), _T(""));
#else
				_tsplitpath_s(szFullPath.c_str(), drive, dir, fname, ext);
				_tmakepath_s(szFolderName, drive, dir, _T(""), _T(""));
#endif
				return szFolderName;
			}

			template<typename _STRING>
			inline _STRING parse_get_extension(_STRING szFullPath)
			{
				std::size_t pos = szFullPath.find_last_of(_T('.'));
				if (pos == _STRING::npos)
					return _T("");
				else
					return szFullPath.length() > pos + 1 ? szFullPath.substr(pos + 1) : _T("");
			}

			template<typename _STRING>
			inline _STRING appendpath(LPCTSTR sPathOne, LPCTSTR sPathTwo, LPCTSTR sPathThree = _T(""))
			{
				return appendpath(_STRING(sPathOne), _STRING(sPathTwo), _STRING(sPathThree));
			}

			template<typename _STRING>
			inline _STRING appendpath(const _STRING sPathOne, const _STRING sPathTwo, const _STRING sPathThree = _T(""))
			{
				_STRING sTemp = details::checkbackslash(sPathOne);
				sTemp += sPathTwo;
				if (!sPathThree.empty()) {
					sTemp = details::checkbackslash(sTemp);
					sTemp += sPathThree;
				}
				return sTemp;
			}

			template<typename _STRING>
			inline _STRING appendpath2current(LPCTSTR sPathTwo, LPCTSTR sPathThree = _T(""))
			{
				return appendpath2current(_STRING(sPathTwo), _STRING(sPathThree));
			}

			template<typename _STRING>
			inline _STRING appendpath2current(const _STRING sPathTwo, const _STRING sPathThree = _T(""))
			{
				_STRING sPath = details::getmodulepath<_STRING>();
				return appendpath(sPath, sPathTwo, sPathThree);
			}

			template<typename _STRING>
			_STRING getallusersfolder()
			{
				TCHAR szPath[MAX_PATH];

				if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, szPath))) {
					return szPath;
				}
				return _T("");
			}

			inline bool createdirectory(LPCTSTR  szPathName)
			{
				if (!szPathName)
					return false;

				std::wstring sPathName(szPathName);
				std::size_t slash_pos = 0;
				bool bCreated;
				do {
					if (!aklib::filesystem::isdirexists(szPathName)) {
						if (!::CreateDirectory(szPathName, NULL)) {
							if (GetLastError() == 3) {//The system cannot find the path specified.
								if ((slash_pos = sPathName.rfind(_T('\\'))) == std::wstring::npos) {
									if ((slash_pos = sPathName.rfind(_T('/'))) == std::wstring::npos)
										return false;
								}
								if (-1 == slash_pos)
									return false;
								sPathName = sPathName.substr(0, slash_pos);
								if (!createdirectory(sPathName.c_str())) {
									return false;
								}
								else {
									bCreated = false;
									continue;
								}
							}
							else {
								return false;
							}
						}
						else {
							bCreated = true;
						}
					}
					else {
						bCreated = true;
					}
				} while (!bCreated);

				return true;
			}

			inline long getfilesize(LPCTSTR  szFileName)
			{
				HANDLE hFile = ::CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				if (hFile == INVALID_HANDLE_VALUE) {
					return -1;
				}
				DWORD dwSize = ::GetFileSize(hFile, NULL);
				if (dwSize == INVALID_FILE_SIZE) {
					::CloseHandle(hFile);
					return -1;
				}
				::CloseHandle(hFile);
				return dwSize;
			}

			template<typename _STRING>
			inline std::list<_STRING> getfileslist(const _STRING sPath, const _STRING sExtention /*= _T("*")*/)
			{
				WIN32_FIND_DATA fileData;
				HANDLE hFindFile;
				BOOL bFileFound(TRUE);

				std::list<_STRING> m_fileArray;
				_STRING sSearch = aklib::filesystem::appendpath(sPath, _T("*.") + sExtention);
				hFindFile = FindFirstFile(sSearch.c_str(), &fileData);
				if (hFindFile != INVALID_HANDLE_VALUE)
				{
					while (bFileFound)
					{
						if (((fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY) && (fileData.cFileName[0] != _T('.')))
						{
							m_fileArray.push_back(fileData.cFileName);
						}

						bFileFound = FindNextFile(hFindFile, &fileData);
					}
					FindClose(hFindFile);
				}
				return m_fileArray;
			}

			template<typename _STRING>
			inline std::list<_STRING> getfileslistrecursive(const _STRING sPath, const _STRING sExtention /*= _T("*")*/)
			{
				std::list<_STRING> ar;
				getfileslistrecursive(ar, sPath, sExtention);
				return ar;
			}

			template<typename _STRING>
			inline void getfileslistrecursive(std::list<_STRING> &ar, const _STRING sPath, const _STRING sExtention /*= _T("*")*/)
			{
				WIN32_FIND_DATA fileData;
				HANDLE hFindFile;
				BOOL bFileFound(TRUE);

				_STRING sSearch = aklib::filesystem::appendpath<_STRING>(sPath, _T("*.*"));
				hFindFile = FindFirstFile(sSearch.c_str(), &fileData);
				if (hFindFile != INVALID_HANDLE_VALUE)
				{
					while (bFileFound)
					{
						if (((fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY) && (fileData.cFileName[0] != '.') && (fileData.cFileName[0] != '..'))
						{
							_STRING sFull = aklib::filesystem::appendpath<_STRING>(sPath, fileData.cFileName);
							getfileslistrecursive(ar, sFull, sExtention);
						}

						bFileFound = FindNextFile(hFindFile, &fileData);
					}
					FindClose(hFindFile);
				}

				bFileFound = TRUE;

				sSearch = aklib::filesystem::appendpath<_STRING>(sPath, _T("*.") + sExtention);
				hFindFile = FindFirstFile(sSearch.c_str(), &fileData);
				if (hFindFile != INVALID_HANDLE_VALUE)
				{
					while (bFileFound)
					{
						if (((fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY) && (fileData.cFileName[0] != _T('.')))
						{
							_STRING sFull = aklib::filesystem::appendpath<_STRING>(sPath, fileData.cFileName);
							ar.push_back(sFull);
						}

						bFileFound = FindNextFile(hFindFile, &fileData);
					}
					FindClose(hFindFile);
				}
			}

			template<typename _STRING>
			inline std::list<_STRING> getfolderslist(const _STRING sPath)
			{
				WIN32_FIND_DATA fileData;
				HANDLE hFindFile;
				BOOL bFileFound(TRUE);

				std::list<_STRING> m_fileArray;
				_STRING sSearch = aklib::filesystem::appendpath<_STRING>(sPath, _T("*.*"));
				hFindFile = FindFirstFile(sSearch.c_str(), &fileData);
				if (hFindFile != INVALID_HANDLE_VALUE)
				{
					while (bFileFound)
					{
						if (((fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY) && (fileData.cFileName[0] != _T('.')))
						{
							m_fileArray.push_back(fileData.cFileName);
						}

						bFileFound = FindNextFile(hFindFile, &fileData);
					}
					FindClose(hFindFile);
				}
				return m_fileArray;
			}

			template<typename _STRING>
			inline std::list<_STRING> getfolderslistrecursive(const _STRING sPath)
			{
				std::list<_STRING> ar;
				getfolderslistrecursive(ar, sPath);
				return ar;
			}

			template<typename _STRING>
			inline void getfolderslistrecursive(std::list<_STRING> &ar, const _STRING sPath)
			{
				WIN32_FIND_DATA fileData;
				HANDLE hFindFile;
				BOOL bFileFound(TRUE);

				_STRING sSearch = aklib::filesystem::appendpath<_STRING>(sPath, _T("*.*"));
				hFindFile = FindFirstFile(sSearch.c_str(), &fileData);
				if (hFindFile != INVALID_HANDLE_VALUE)
				{
					while (bFileFound)
					{
						if (((fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY) && (fileData.cFileName[0] != _T('.'))
							/*&& (fileData.cFileName[0] != _T('..'))*/)
						{
							_STRING sFull = aklib::filesystem::appendpath<_STRING>(sPath, fileData.cFileName);
							ar.push_back(sFull);
							getfolderslistrecursive(ar, sFull);
						}

						bFileFound = FindNextFile(hFindFile, &fileData);
					}
					FindClose(hFindFile);
				}
			}

			template<typename _NAME>
			bool setfilecontent(_NAME filename, const std::list<std::string>& array)
			{
				try {
					HANDLE hFile = ::CreateFile(filename.c_str(), GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
					if (hFile == INVALID_HANDLE_VALUE) {
						DWORD dw = GetLastError();
						return false;
					}
					DWORD dwWritten;
					for (auto it = array.begin(); it != array.end(); it++) 
					{
						const std::string& str = *it;
						::WriteFile(hFile, str.c_str(), (DWORD)str.size(), &dwWritten, NULL);
						assert(dwWritten == str.size());
						::WriteFile(hFile, "\r\n", 2, &dwWritten, NULL);
						assert(dwWritten == 2);
					}

					CloseHandle(hFile);

				}
				catch (...) {
				}
				return true;
			}

			template<typename _NAME>
			bool setfilecontent(_NAME filename, const std::list<std::wstring>& array)
			{
				try {
					HANDLE hFile = ::CreateFile(filename.c_str(), GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
					if (hFile == INVALID_HANDLE_VALUE) {
						DWORD dw = GetLastError();
						return false;
					}
					DWORD dwWritten;
					for (auto it = array.begin(); it != array.end(); it++)
					{
						const std::wstring& str = *it;
						::WriteFile(hFile, str.c_str(), (DWORD)str.size() * sizeof(wchar_t), &dwWritten, NULL);
						assert(dwWritten == str.size());
						::WriteFile(hFile, L"\r\n", 2, &dwWritten, NULL);
						assert(dwWritten == 2);
					}

					CloseHandle(hFile);

				}
				catch (...) {
				}
				return true;
			}

			bool setfilecontent(std::wstring filename, std::wstring str, const std::locale loc = std::locale(""))
			{
				std::locale::global(loc);//En_US, Fr_CH, De_DE

				std::wofstream output(filename.c_str(), std::wios::out | std::wios::trunc | std::wios::binary);
				if (!output) return false;

				auto dwToWrite = str.length();
				output.write(str.c_str(), dwToWrite);
				output.flush();
				output.close();

				return true;
			}

			bool setfilecontent(std::string filename, std::string str, const std::locale loc = std::locale(""))
			{
				std::locale::global(loc);//En_US, Fr_CH, De_DE

				std::ofstream output(filename.c_str(), std::wios::out | std::wios::trunc | std::wios::binary);
				if (!output) return false;

				auto dwToWrite = str.length();
				output.write(str.c_str(), dwToWrite);
				output.flush();
				output.close();

				return true;
			}

			template<typename _STRING>
			bool setfilecontent(_STRING filename, LPCTSTR buf, int nLen)
			{
				try {
					HANDLE hFile = ::CreateFile(filename.c_str(), GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
					if (hFile == INVALID_HANDLE_VALUE) {
						DWORD dw = GetLastError();
						return false;
					}
					DWORD dwWritten;
					::WriteFile(hFile, buf, nLen, &dwWritten, NULL);
					assert(dwWritten == nLen);

					CloseHandle(hFile);

				}
				catch (...) {
				}
				return true;
			}

			template<typename _STRING>
			bool setfilecontent(_STRING filename, _STRING str)
			{
				return setfilecontent(filename, str.c_str(), str.length());
			}

			template<typename _STRING>
			std::string getfilecontent(_STRING filename)
			{
				std::string out;
				try {
					HANDLE hFile = ::CreateFile(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
					if (hFile == INVALID_HANDLE_VALUE) {
						//DWORD dw = GetLastError();
						return "";
					}

					// attempt an asynchronous read operation
					const int nBytesToRead = 10;
					char buf[nBytesToRead];
					DWORD nBytesRead = 0;
					while (::ReadFile(hFile, &buf, nBytesToRead, &nBytesRead, NULL) && nBytesRead > 0) {
						for (unsigned int i = 0, j = 0; i < nBytesRead; i++, j++)
							out += buf[i];
					}
					::CloseHandle(hFile);

				}
				catch (...) {
				}
				return out;
			}

			template<typename _STRING>
			bool getfilecontent(_STRING filename, std::list<std::string> &array)
			{
				array.clear();
				try {
					HANDLE hFile = ::CreateFile(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
					if (hFile == INVALID_HANDLE_VALUE) {
						//DWORD dw = GetLastError();
						return false;
					}

					// attempt an asynchronous read operation
					char buf[512];
					DWORD nBytesRead = 0;
					std::string sPreviousRead = "";
					while (::ReadFile(hFile, &buf, 512, &nBytesRead, NULL) && nBytesRead > 0) {
						char* buf2 = new char[nBytesRead + 1];
						unsigned int i = 0, j = 0;
						for (; i < nBytesRead; i++, j++) {
							if (buf[i] == 0x0D || buf[i] == 0x0A) {
								buf2[j] = 0;
								std::string sLine(buf2);
								if (!sPreviousRead.empty()) {
									sLine = sPreviousRead + sLine;
									sPreviousRead = "";
								}
								if (NULL != buf2) {
									delete[] buf2;
									buf2 = NULL;
								}
								buf2 = new char[nBytesRead + 1];
								j = -1;
								array.push_back(sLine);
								if (buf[i + 1] == 0x0A || buf[i + 1] == 0x0D)
									i++;
							}
							else {
								buf2[j] = buf[i];
							}
						}
						buf2[j] = 0;
						sPreviousRead += buf2;//beginning of new line, the rest would be at next ReadFile call
						if (NULL != buf2) {
							delete[] buf2;
							buf2 = NULL;
						}
					}
					::CloseHandle(hFile);

					if (!sPreviousRead.empty()) {
						array.push_back(sPreviousRead);
						sPreviousRead = "";
					}

				}
				catch (...) {
				}
				return true;
			}

			// Read a file in UTF-8, and convert for internal storage in UCS-4 (wchar_t)
			bool getUTF8filecontent(const char* file_name, std::vector<std::wstring>& input)
			{
				std::locale ru_RU("ru_RU.utf8");  // Any UTF-8 locale would do
				typedef std::codecvt<wchar_t, char, std::mbstate_t> utf8_codecvt_t;
				const utf8_codecvt_t& utf8_codecvt = std::use_facet<utf8_codecvt_t>(ru_RU);
				std::locale utf8_locale(ru_RU, &utf8_codecvt);
				const int max_input_line_length = 512;

				size_t total_length = 0;

				std::wifstream ifs(file_name);
				if (ifs.is_open())
				{
					// This line is the magic that converts UTF-8 to UCS-4
					ifs.imbue(utf8_locale);

					wchar_t line[max_input_line_length];

					// -1 to have place for string terminator
					while (ifs.getline(line, max_input_line_length - 1))
					{
						input.push_back(line);
						total_length += std::wcslen(line);
					}

					//if (!ifs.eof())
					//	std::cout << "Error reading input file: " << std::strerror(errno) << "\n";

					ifs.close();
				}
				else
					return false;

				return true;
			}

	} // namespace filesystem

} //namespace aklib

#endif //AKLIB_FILESYSTEM_HPP