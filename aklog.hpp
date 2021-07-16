#pragma once
#if defined(_WIN32)
#include <mutex>
#include <string>
#include <Windows.h>
#include <shlobj.h>
#include <memory.h>
#include <locale.h>
#include <tchar.h>
#include <wchar.h>

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef STD_STRING
#ifdef _UNICODE
#define STD_STRING std::wstring
#else
#define STD_STRING std::string
#endif
#endif // !STD_STRING

#include "filesystem.hpp"
#include "aktime.hpp"

namespace aklib {

	static const int LOG_FILE_SIZE_MAX = 1048576; // 1 Mb

/////////////////////////////////////////////////////////////////////////////
// Data and constants definition
	using StdMutex = std::recursive_mutex;
	using UniqueMutexLock = std::unique_lock<StdMutex>;
	static StdMutex sGuard;
	#define LOG_LOCK				UniqueMutexLock lock(sGuard);

	#define LOG_INDENT_STEP     2

	const int  cnTagSize = 15;		// Tag field width
	const int  cnTimeSize = 14;		// Time field width
	const int  cnProcIDSize = 5;	// Time field width
	const char cLinePrefix = '|';   // Tag field delimiter

	class log
	{
	public:
		log(LPCTSTR name = _T(""), int nVerbose = 1, LPCTSTR sPath = _T(""))
			: m_sFileName(_T(""))
			, m_sPath(sPath)
			, m_bDebOutEnabled(true)
			, m_nIndentLevel(0)
			, m_nVerboseLevel(nVerbose)
			, m_nLogMaxLength(LOG_FILE_SIZE_MAX)
			, m_bDoNothing(false)
		{
			LOG_LOCK;

			m_sLogNamePrefix = name;
			if (m_sLogNamePrefix.empty()) {
				m_sLogNamePrefix = aklib::filesystem::parse_get_filename_only<STD_STRING>(aklib::filesystem::details::getfullmodulename<STD_STRING>());
			}

			::GetLocalTime(m_timeCurrentLogCreationTime);
			InitFileName();
			PrintStartInfo();
		}
		
		log(bool bDoNothing)
			: m_sFileName(_T(""))
			, m_sPath(_T(""))
			, m_bDebOutEnabled(true)
			, m_nIndentLevel(0)
			, m_nVerboseLevel(0)
			, m_nLogMaxLength(LOG_FILE_SIZE_MAX)
			, m_bDoNothing(bDoNothing)
		{
			LOG_LOCK;

			if (m_sLogNamePrefix.empty()) {
				m_sLogNamePrefix = aklib::filesystem::parse_get_filename_only<STD_STRING>(aklib::filesystem::details::getfullmodulename<STD_STRING>());
			}

			::GetLocalTime(m_timeCurrentLogCreationTime);
			InitFileName();
			if (!m_bDoNothing) PrintStartInfo();
		}

		virtual ~log()
		{
			if (!m_bDoNothing) PrintStop();
		}

		/**
		* Prints a series of characters and values to the output Log file.
		* \param szPrefix applog test marker
		* \param szOutLine applog text (output wstring)
		* \return the number of characters written, not including the terminating null character,
		*         or a negative value if an output error occurs.
		*/
		int WriteLogString(int nVerboseLevel, LPCTSTR szPrefix, LPCTSTR szOutLine)
		{
			int nRet = 0;

			if (nVerboseLevel > m_nVerboseLevel)
				return nRet;

			CheckFileDate();

			LOG_LOCK;
			if (IsBadStringPtr(szOutLine, (UINT_PTR)-1))
				return -1;

			FILE* lpFile = nullptr;
			errno_t err = (!m_sFileName[0]) ? NULL : _tfopen_s(&lpFile, m_sFileName.c_str(), _T("ab")); err;
			if (!lpFile) {
				LPVOID lpMsgBuf;
				if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL) != 0) {
					//SetLibLastError(static_cast<TCHAR*>(lpMsgBuf));
					LocalFree(lpMsgBuf);
				}
				else {
					//SetLibLastError((_T("error in opening file ")+m_sFileName).c_str());
				}
				return -1;
			}

			size_t nIndentPos = cnTagSize + 1 + cnTimeSize + 1 + cnProcIDSize + 1 + (m_nIndentLevel * LOG_INDENT_STEP);

			TCHAR szIndent[0x50];
			TCHAR* pszIndent = szIndent;
			// Fix indent line if it's out of size of the buffer
			if (nIndentPos >= sizeof(szIndent))
				nIndentPos = sizeof(szIndent) - 1;

			wmemset(szIndent, ' ', nIndentPos);   // Blank the buffer.
			szIndent[nIndentPos] = '\0';		   // Terminate the buffer at it's length.  
			if (szPrefix != NULL)
				wmemcpy(szIndent, szPrefix, min(cnTagSize, _tcslen(szPrefix)));
			szIndent[cnTagSize] = cLinePrefix;  // fixed position

			STD_STRING sProcID = std::to_wstring(::GetCurrentProcessId());
			std::size_t nLen = min(cnProcIDSize, sProcID.length());
			pszIndent += cnTagSize + 1;
			wmemcpy(pszIndent, sProcID.c_str(), nLen);
			szIndent[cnTagSize + cnProcIDSize + 1] = cLinePrefix;  // fixed position

			aktime tTime;
			STD_STRING sTime = tTime.GetString<STD_STRING>();// return wstring: 2005/05/26 12:26:05
			nLen = min(cnTimeSize, sTime.length() - 5);
			pszIndent += cnProcIDSize + 1;
			wmemcpy(pszIndent, sTime.c_str() + 5, nLen);
			szIndent[cnTagSize + cnProcIDSize + cnTimeSize + 2] = cLinePrefix;  // fixed position

			// Output text line by line preceding each one with indentation wstring
			for (LPTSTR lpszText = (LPTSTR)szOutLine; (*lpszText); )
			{
				// Put the indent of the text we have.
				nRet += PutLine(szIndent, lpFile);

				// Find the (next) "end of line" if any.
				PTCHAR lpszEoL = _tcschr(lpszText, '\n');
				if (lpszEoL == NULL)
				{ // If this is not empty line      
					nRet += PutLine(lpszText, lpFile);
					nRet += PutLine(_T("\r\n"), lpFile);
					break;
				}
				else
				{ // Multiple lines wstring processing
					TCHAR chSave = *lpszEoL;			 // Save this 'end of line' position.
					*lpszEoL = '\0';					 // Temporary replace 'end of line' with zero (cut off the rest of line)      
					nRet += PutLine(lpszText, lpFile); // output the part of wstring we have before the line ends.
					nRet += PutLine(_T("\r\n"), lpFile);

					*lpszEoL = chSave;   // Restore the character's value by it's pointer      
					lpszEoL++;		   // Set up the next char      
					lpszText = lpszEoL;  // Continue with next line
				}
			}

			// Close file at the end of output.
			if (lpFile != NULL)
				fclose(lpFile);

			return nRet;
		}

		/**
		* Formats and prints a series of characters and values to the output Log file.
		* \param lpszTag Marker of test
		* \param szOutLine Output wstring
		* \param ... - all other params
		* \return the number of characters written, not including the terminating null character,
		*         or a negative value if an output error occurs.
		*/
		int Log(int nVerboseLevel, LPCTSTR szPrefix, LPCTSTR lpszFormat, ...)
		{
			LOG_LOCK;

			if (lpszFormat == NULL)
				return WriteLogString(nVerboseLevel, szPrefix, _T(""));

			int nRet = 0;
			va_list argsList;
			va_start(argsList, lpszFormat);
			// Getting the length of text output.
			int nLength = _vsctprintf(lpszFormat, argsList);
			// Allocate the required text buffer
			TCHAR* szOutString = new TCHAR[nLength + 1];
			if (szOutString != NULL)
			{	// Prepare the wstring for output.
				int nLen2 = _vsntprintf_s(szOutString, nLength + 1, nLength, lpszFormat, argsList);

				if (nLen2 == nLength)
					szOutString[nLength] = '\0'; // Terminate the allocated buffer.

				nRet = WriteLogString(nVerboseLevel, szPrefix, szOutString);	// Sent the sting to Log file.

				delete[] szOutString;// Free used memory.
			}
			va_end(argsList);

			return nRet;
		}

		/**
		* Changes Log text indent level.
		* \param nIndent new value of indent to use
		* \return old indent value
		*/
		int SetLogIndent(int nIndent)
		{
			LOG_LOCK;
			int nOld = m_nIndentLevel;

			m_nIndentLevel += nIndent; // Adjust current indentation level

			if (m_nIndentLevel < 0)
				m_nIndentLevel = 0;

			return nOld;
		}

		// Overrides
		//! friend class CAKMutexLocker 
		friend class CAKMutexLocker;

		void SetVerboseLevel(unsigned int nNewVal) {
			m_nVerboseLevel = nNewVal;
		}
		unsigned int GetVerboseLevel() {
			return m_nVerboseLevel;
		}

		STD_STRING GetFileName() {
			return m_sFileName;
		}

		void SetFileName(STD_STRING str) {
			m_sFileName = str;
		}

		STD_STRING GetFilePrefix() {
			return m_sLogNamePrefix;
		}

		void SetFilePath(STD_STRING path, bool bPrintStartupInfo = false) {
			m_sPath = path;
			InitFileName();

			Log(1, _T("LOG"), _T("Log filename is changes to "), m_sFileName.c_str());
		}

		void PrintStartInfo()
		{
			TCHAR szDate[0x10];
			TCHAR szTime[0x10];
			STD_STRING path = aklib::filesystem::details::getmodulepath<STD_STRING>();

			TCHAR s[256];
			_tstrdate_s(szDate);
			_tstrtime_s(szTime);
			_stprintf_s(s, 256, _T("Time: %s %s, Location: %s"), szDate, szTime, path.c_str());
			Log(1, _T("Start"), s);
		}

		/**
		* Writes to applog output end label (end of applog process)
		*/
		void PrintStop()
		{
			TCHAR szDate[0x10];
			TCHAR szTime[0x10];
			TCHAR s[256];
			_tstrdate_s(szDate);
			_tstrtime_s(szTime);
			_stprintf_s(s, 256, _T("Time: %s %s"), szDate, szTime);
			Log(1, _T("Stop"), s);
		}

		static STD_STRING CreateFileName(STD_STRING prefix, STD_STRING path, aktime timestamp)
		{
			if (path.empty()) {
				path = aklib::filesystem::appendpath(aklib::filesystem::getallusersfolder<STD_STRING>(), aklib::filesystem::parse_get_filename_only<STD_STRING>(aklib::filesystem::details::getfullmodulename<STD_STRING>()));
				aklib::filesystem::createdirectory(path.c_str());
			}

			STD_STRING sPath(path);
			STD_STRING sPrefix(prefix);
			STD_STRING sFileName = aklib::filesystem::details::checkbackslash(sPath) + sPrefix;
			//STD_STRING sTime = timestamp.Format(L("%Y%m%d"));
			STD_STRING sTime = timestamp.GetStringDateWithoutDelim<STD_STRING>();
			sFileName += _T("_") + STD_STRING(sTime);
			sFileName += _T(".log");

			return sFileName.c_str();
		}

	protected:
		/**
		* Writes wstring to the output stream (applog file name) at the current position.
		* \param szLine Output wstring
		* \param lpFile Pointer to FILE structure.
		* \return the number of characters written, not including the terminating null character.
		*/
		int PutLine(LPCTSTR szLine, FILE* lpFile/*= NULL*/)
		{
			if (!szLine || (szLine[0] == 0))
				return 0;

			if (m_bDebOutEnabled)
				OutputDebugString(szLine);

			if (lpFile != NULL)
			{
				LOG_LOCK;
				return _fputts(szLine, lpFile);
			}
			return 0;
		}

		void InitFileName()
		{
			//CTime time = CTime::GetCurrentTime();
			aktime time;
			m_sFileName = log::CreateFileName(m_sLogNamePrefix, m_sPath, time);
		}

		void CheckFileDate()
		{
			aktime current;
			aktime timeReportSent(m_timeCurrentLogCreationTime.GetYear(), m_timeCurrentLogCreationTime.GetMonth(), m_timeCurrentLogCreationTime.GetDay());
			aktime timeCurrent(current.GetYear(), current.GetMonth(), current.GetDay());
			if (timeCurrent > timeReportSent) {
				m_timeCurrentLogCreationTime = current;
				InitFileName();
			}
		}

		void SetLogMaxLength(unsigned int nNewVal) {
			m_nLogMaxLength = nNewVal;
		}

	private:
		//! Enables to write copy os output to the DEBUG stream
		bool		m_bDebOutEnabled;
		//! Current applog text indentation level
		int			m_nIndentLevel;
		//! Log file name.
		STD_STRING		m_sFileName;
		STD_STRING		m_sLogNamePrefix;
		STD_STRING		m_sPath;
		//! Verbose mode
		int			m_nVerboseLevel;

		unsigned int m_nLogMaxLength;

		aktime m_timeCurrentLogCreationTime;

	protected:
		bool m_bDoNothing;

		DWORD m_dwProcessID;

		//StdMutex sGuard;
	};

}
#endif