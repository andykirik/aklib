#pragma once
#if defined(_WIN32)
#include <string>
#include <tchar.h>//_stprintf_s
#include <windows.h>

namespace aklib {

	class aktime
	{
	public:
		aktime(void)
		{
			::GetLocalTime(&m_timeValue);
		}

		aktime(const int nYear, const int nMonth, const int nDay, const int nHour = 0, const int nMin = 0, const int nSec = 0)
		{
			m_timeValue.wYear = (WORD)nYear;
			m_timeValue.wMonth = (WORD)nMonth;
			m_timeValue.wDay = (WORD)nDay;
			m_timeValue.wHour = (WORD)nHour;
			m_timeValue.wMinute = (WORD)nMin;
			m_timeValue.wSecond = (WORD)nSec;
			m_timeValue.wMilliseconds = 0;
			m_timeValue.wDayOfWeek = 0;
		}

		aktime(const SYSTEMTIME& time)
			: m_timeValue(time)
		{
		}

		virtual ~aktime(void) {}

		aktime(const aktime& rhs)
		{
			*this = rhs;
		}
		aktime& operator = (const aktime& rhs)
		{
			if (this != &rhs) {
				this->m_timeValue = rhs.m_timeValue;
			}
			return *this;
		}

		bool operator == (const aktime& rhs)
		{
			return m_timeValue.wYear == rhs.m_timeValue.wYear && m_timeValue.wMonth == rhs.m_timeValue.wMonth && m_timeValue.wDay == rhs.m_timeValue.wDay &&
				m_timeValue.wHour == rhs.m_timeValue.wHour && m_timeValue.wMinute == rhs.m_timeValue.wMinute && m_timeValue.wSecond == rhs.m_timeValue.wSecond &&
				m_timeValue.wMilliseconds == rhs.m_timeValue.wMilliseconds;
		}

		bool operator > (const aktime& rhs)
		{
			if (m_timeValue.wYear > rhs.m_timeValue.wYear)
				return true;

			if (m_timeValue.wYear < rhs.m_timeValue.wYear)
				return false;

			// same year

			if (m_timeValue.wMonth > rhs.m_timeValue.wMonth)
				return true;

			if (m_timeValue.wMonth < rhs.m_timeValue.wMonth)
				return false;

			// same month

			if (m_timeValue.wDay > rhs.m_timeValue.wDay)
				return true;

			if (m_timeValue.wDay < rhs.m_timeValue.wDay)
				return false;

			// same day

			if (m_timeValue.wHour > rhs.m_timeValue.wHour)
				return true;

			if (m_timeValue.wHour < rhs.m_timeValue.wHour)
				return false;

			// same hour

			if (m_timeValue.wMinute > rhs.m_timeValue.wMinute)
				return true;

			if (m_timeValue.wMinute < rhs.m_timeValue.wMinute)
				return false;

			// same minute

			if (m_timeValue.wSecond > rhs.m_timeValue.wSecond)
				return true;

			if (m_timeValue.wSecond < rhs.m_timeValue.wSecond)
				return false;

			// same second

			return m_timeValue.wMilliseconds > rhs.m_timeValue.wMilliseconds;
		}

		bool operator >= (const aktime& rhs) {
			return operator>(rhs) || operator==(rhs);
		}
		bool operator <	 (const aktime& rhs) {
			return !operator>(rhs) && !operator==(rhs);
		}
		bool operator <= (const aktime& rhs) {
			return operator<(rhs) || operator==(rhs);
		}

		aktime operator + (const aktime& rhs)
		{
			FILETIME ftFileTimeThis, ftFileTimeThem;
			if (!SystemTimeToFileTime(&this->m_timeValue, &ftFileTimeThis)) return aktime(0, 0, 0);
			if (!SystemTimeToFileTime(&rhs.m_timeValue, &ftFileTimeThem)) return aktime(0, 0, 0);

			ULARGE_INTEGER ul1, ul2;
			ul1.HighPart = ftFileTimeThis.dwHighDateTime;
			ul1.LowPart = ftFileTimeThis.dwLowDateTime;
			ul2.HighPart = ftFileTimeThem.dwHighDateTime;
			ul2.LowPart = ftFileTimeThem.dwLowDateTime;

			ul1.QuadPart += ul2.QuadPart;
			//ul1.HighPart += ul2.HighPart;
			//ul1.LowPart += ul2.LowPart;

			ftFileTimeThis.dwHighDateTime = ul1.HighPart;
			ftFileTimeThis.dwLowDateTime = ul1.LowPart;

			SYSTEMTIME stSystemTime;
			if (!FileTimeToSystemTime(&ftFileTimeThis, &stSystemTime)) return aktime(0, 0, 0);

			return aktime(stSystemTime.wYear, stSystemTime.wMonth, stSystemTime.wDay, stSystemTime.wHour, stSystemTime.wMinute, stSystemTime.wSecond);
		}

		aktime operator - (const aktime& rhs) 
		{
			FILETIME ftFileTimeThis, ftFileTimeThem;
			if (!SystemTimeToFileTime(&this->m_timeValue, &ftFileTimeThis)) return aktime(0, 0, 0);
			if (!SystemTimeToFileTime(&rhs.m_timeValue, &ftFileTimeThem)) return aktime(0, 0, 0);

			ULARGE_INTEGER ul1, ul2;
			ul1.HighPart = ftFileTimeThis.dwHighDateTime;
			ul1.LowPart = ftFileTimeThis.dwLowDateTime;
			ul2.HighPart = ftFileTimeThem.dwHighDateTime;
			ul2.LowPart = ftFileTimeThem.dwLowDateTime;

			ul1.QuadPart -= ul2.QuadPart;
			//ul1.HighPart -= ul2.HighPart;
			//ul1.LowPart -= ul2.LowPart;

			ftFileTimeThis.dwHighDateTime = ul1.HighPart;
			ftFileTimeThis.dwLowDateTime = ul1.LowPart;

			SYSTEMTIME stSystemTime;
			if (!FileTimeToSystemTime(&ftFileTimeThis, &stSystemTime)) return aktime(0, 0, 0);

			return aktime(stSystemTime.wYear, stSystemTime.wMonth, stSystemTime.wDay, stSystemTime.wHour, stSystemTime.wMinute, stSystemTime.wSecond);
		}


		operator LPSYSTEMTIME() {
			return &m_timeValue;
		}

		SYSTEMTIME m_timeValue;

		static SYSTEMTIME GetCurrentTime()
		{
			SYSTEMTIME time;
			::GetLocalTime(&time);

			return time;
		}

		int GetYear()	const {
			return m_timeValue.wYear;
		}

		int GetMonth()	const {
			return m_timeValue.wMonth;
		}

		int GetDay()	const {
			return m_timeValue.wDay;
		}

		int GetHour()	const {
			return m_timeValue.wHour;
		}

		int GetMinute()	const {
			return m_timeValue.wMinute;
		}

		int GetSecond()	const {
			return m_timeValue.wSecond;
		}

		// return wstring: 20050526
		template<typename _STRING>
		_STRING GetStringDateWithoutDelim() {
			TCHAR buf[48];
			_stprintf_s(buf, 48, _T("%04d%02d%02d"), m_timeValue.wYear, m_timeValue.wMonth, m_timeValue.wDay);
			_STRING sTime = buf;
			return sTime;
		}

		// return wstring: 2005/05/26
		template<typename _STRING>
		_STRING GetStringDate() {
			TCHAR buf[48];
			_stprintf_s(buf, 48, _T("%04d/%02d/%02d"), m_timeValue.wYear, m_timeValue.wMonth, m_timeValue.wDay);
			_STRING sTime = buf;
			return sTime;
		}

		// return wstring: 12:26:05
		template<typename _STRING>
		_STRING GetStringTime() {
			TCHAR buf[48];
			_stprintf_s(buf, 48, _T("%02d:%02d:%02d"), m_timeValue.wHour, m_timeValue.wMinute, m_timeValue.wSecond);
			_STRING sTime = buf;
			return sTime;
		}

		// return wstring: 2005/05/26 12:26:05
		template<typename _STRING>
		_STRING GetString() {
			TCHAR buf[48];
			_stprintf_s(buf, 48, _T("%04d/%02d/%02d %02d:%02d:%02d"), m_timeValue.wYear, m_timeValue.wMonth, m_timeValue.wDay, m_timeValue.wHour, m_timeValue.wMinute, m_timeValue.wSecond);
			_STRING sTime = buf;
			return sTime;
		}

		// return wstring: 2005052612260533
		template<typename _STRING>
		static _STRING GetUniqueString() {
			SYSTEMTIME time;
			::GetLocalTime(&time);
			TCHAR buf[48];
			_stprintf_s(buf, 48, _T("%04d%02d%02d%02d%02d%02d%02d"), time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
			_STRING sTime = buf;
			return sTime;
		}

		// return wstring: 12260533
		template<typename _STRING>
		static _STRING GetUniqueStringShort() {
			SYSTEMTIME time;
			::GetLocalTime(&time);
			TCHAR buf[48];
			_stprintf_s(buf, 48, _T("%02d%02d%02d%02d"), time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
			_STRING sTime = buf;
			return sTime;
		}

		// return wstring: 2005/05/26 12:26:05
		template<typename _STRING>
		static _STRING GetCurrentTimeString() {
			SYSTEMTIME time;
			::GetLocalTime(&time);
			TCHAR buf[48];
			_stprintf_s(buf, 48, _T("%04d/%02d/%02d %02d:%02d:%02d"), time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
			_STRING sTime = buf;
			return sTime;
		}

		template<typename _STRING>
		_STRING GetDayOfWeek() {
			_STRING day;
			switch (m_timeValue.wDayOfWeek)
			{
			case 0:			day = _T("Sunday");		break;
			case 1:			day = _T("Monday");		break;
			case 2:			day = _T("Tuesday");		break;
			case 3:			day = _T("Wednesday");		break;
			case 4:			day = _T("Thursday");		break;
			case 5:			day = _T("Friday");		break;
			case 6:			day = _T("Saturday");		break;
			default:		day = _T("");
			}
			return day;
		}

	};

}
#endif