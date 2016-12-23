#pragma once

#include <WinDef.h>
#include <Tlhelp32.h>
#include <io.h>
#include <sstream>
#include <tchar.h>
#include <algorithm>
#include <assert.h>

using namespace std;

#ifdef UNICODE
typedef std::wstring StdString;
#else
typedef std::string StdString;
#endif

class Utility
{
public:
	static void RunMessageLoop()
	{
		MSG msg;
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
	static BOOL IsWinXP()
	{
		OSVERSIONINFO version;
		version.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		::GetVersionEx(&version);
		return version.dwMajorVersion <= 5;
	}

	template<class T>
	static std::string ToString(T i)
	{
		ostringstream s;
		s << i;
		return s.str();
	}
	static int64_t StringToInt(const char* str, int base = 10)
	{
#ifdef _MSC_VER
		return _strtoui64(str, nullptr, base);
#else
		return strtoull(str, nullptr, base);
#endif
	}

	static const StdString & GetExePath()
	{
		static StdString sPath;
		if (sPath == _T(""))
		{
			TCHAR szPath[MAX_PATH] = _T("\0");
			GetModuleFileName(NULL, szPath, MAX_PATH);
			(_tcsrchr(szPath, _T('\\')))[0] = 0;
			sPath = szPath;
		}
		return sPath;
	}

	static unsigned char ToHex(unsigned char x)
	{
		return  x > 9 ? x + 55 : x + 48;
	}

	static unsigned char FromHex(unsigned char x)
	{
		unsigned char y;
		if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
		else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
		else if (x >= '0' && x <= '9') y = x - '0';
		else assert(0);
		return y;
	}

	static std::string UrlEncode(const std::string& str)
	{
		std::string strTemp = "";
		size_t length = str.length();
		for (size_t i = 0; i < length; i++)
		{
			if (isalnum((unsigned char)str[i]) ||
				(str[i] == '-') ||
				(str[i] == '_') ||
				(str[i] == '.') ||
				(str[i] == '~'))
				strTemp += str[i];
			else if (str[i] == ' ')
				strTemp += "+";
			else
			{
				strTemp += '%';
				strTemp += ToHex((unsigned char)str[i] >> 4);
				strTemp += ToHex((unsigned char)str[i] % 16);
			}
		}
		return strTemp;
	}

	static std::string UrlDecode(const std::string& str)
	{
		std::string strTemp = "";
		size_t length = str.length();
		for (size_t i = 0; i < length; i++)
		{
			if (str[i] == '+') strTemp += ' ';
			else if (str[i] == '%')
			{
				assert(i + 2 < length);
				unsigned char high = FromHex((unsigned char)str[++i]);
				unsigned char low = FromHex((unsigned char)str[++i]);
				strTemp += high * 16 + low;
			}
			else strTemp += str[i];
		}
		return strTemp;
	}
	static std::wstring UTF8ToWString(const char* lpcszString)
	{
		int len = strlen(lpcszString);
		int unicodeLen = ::MultiByteToWideChar(CP_UTF8, 0, lpcszString, -1, NULL, 0);
		wchar_t* pUnicode;
		pUnicode = new wchar_t[unicodeLen + 1];
		memset((void*)pUnicode, 0, (unicodeLen + 1) * sizeof(wchar_t));
		::MultiByteToWideChar(CP_UTF8, 0, lpcszString, -1, (LPWSTR)pUnicode, unicodeLen);
		wstring wstrReturn(pUnicode);
		delete[] pUnicode;
		return wstrReturn;
	}

	static std::string WStringToUTF8(const wchar_t* lpwcszWString)
	{
		char* pElementText;
		int iTextLen = ::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)lpwcszWString, -1, NULL, 0, NULL, NULL);
		pElementText = new char[iTextLen + 1];
		memset((void*)pElementText, 0, (iTextLen + 1) * sizeof(char));
		::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)lpwcszWString, -1, pElementText, iTextLen, NULL, NULL);
		std::string strReturn(pElementText);
		delete[] pElementText;
		return strReturn;
	}

	static std::wstring MBytesToWString(const char* lpcszString)
	{
		int len = strlen(lpcszString);
		int unicodeLen = ::MultiByteToWideChar(CP_ACP, 0, lpcszString, -1, NULL, 0);
		wchar_t* pUnicode = new wchar_t[unicodeLen + 1];
		memset(pUnicode, 0, (unicodeLen + 1) * sizeof(wchar_t));
		::MultiByteToWideChar(CP_ACP, 0, lpcszString, -1, (LPWSTR)pUnicode, unicodeLen);
		wstring wString = (wchar_t*)pUnicode;
		delete[] pUnicode;
		return wString;
	}

	static std::string WStringToMBytes(const wchar_t* lpwcszWString)
	{
		char* pElementText;
		int iTextLen;
		// wide char to multi char
		iTextLen = ::WideCharToMultiByte(CP_ACP, 0, lpwcszWString, -1, NULL, 0, NULL, NULL);
		pElementText = new char[iTextLen + 1];
		memset((void*)pElementText, 0, (iTextLen + 1) * sizeof(char));
		::WideCharToMultiByte(CP_ACP, 0, lpwcszWString, 0, pElementText, iTextLen, NULL, NULL);
		std::string strReturn(pElementText);
		delete[] pElementText;
		return strReturn;
	}

#ifdef UNICODE
	static std::wstring ToStdString(const char * lpcszString)
	{
		return UTF8ToWString(lpcszString);
	}
	static std::wstring ToStdString(const wchar_t * lpwcszWString)
	{
		return lpwcszWString;
	}
#else
	static std::string ToStdString(const char * lpcszString)
	{
		return lpcszString;
	}
	static std::string ToStdString(const wchar_t * lpwcszWString)
	{
		return WStringToUTF8(lpwcszWString);
	}
#endif
	/// Try to find in the Haystack the Needle - ignore case
	static bool findStringIC(const StdString & strHaystack, const StdString & strNeedle)
	{
		auto it = std::search(
			strHaystack.begin(), strHaystack.end(),
			strNeedle.begin(), strNeedle.end(),
			[](TCHAR ch1, TCHAR ch2) { return toupper(ch1) == toupper(ch2); }
		);
		if (it != strHaystack.end()) return true;
		return false;
	}

	static void replaceString(StdString & str, const StdString & searchString, const StdString & replaceString)
	{
		string::size_type pos = 0;
		while ((pos = str.find(searchString, pos)) != string::npos) {
			str.replace(pos, searchString.size(), replaceString);
			pos++;
		}
	}

	template<typename dst_type, typename src_type>
	static dst_type Pointer_Cast(src_type src)
	{
		return *static_cast<dst_type*>(static_cast<void*>(&src));
	}

	//-----------------------------------------------------------------------------------------------------------  
	// ����: InjectDll  
	// ����: ��Ŀ�������ע��һ��ָ�� Dll ģ���ļ�.  
	// ����: [in] const TCHAR* ptszDllFile - Dll �ļ�����·��  
	//       [in] DWORD dwProcessId - Ŀ����� ID  
	// ����: bool - ע��ɹ����� true, ע��ʧ���򷵻� false.  
	// ˵��: ����Զ���߳�ע�뼼��ʵ��  
	//-----------------------------------------------------------------------------------------------------------  
	static bool InjectDll(const TCHAR* ptszDllFile, DWORD dwProcessId)
	{
		// ������Ч  
		if (NULL == ptszDllFile || 0 == ::_tcslen(ptszDllFile))
		{
			return false;
		}
		// ָ�� Dll �ļ�������  
		if (-1 == _taccess(ptszDllFile, 0))
		{
			return false;
		}
		HANDLE hProcess = NULL;
		HANDLE hThread = NULL;
		DWORD dwSize = 0;
		TCHAR* ptszRemoteBuf = NULL;
		LPTHREAD_START_ROUTINE lpThreadFun = NULL;
		// ��ȡĿ����̾��  
		hProcess = ::OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, dwProcessId);
		if (NULL == hProcess)
		{
			return false;
		}
		// ��Ŀ������з����ڴ�ռ�  
		dwSize = (DWORD)::_tcslen(ptszDllFile) + 1;
		ptszRemoteBuf = (TCHAR*)::VirtualAllocEx(hProcess, NULL, dwSize * sizeof(TCHAR), MEM_COMMIT, PAGE_READWRITE);
		if (NULL == ptszRemoteBuf)
		{
			::CloseHandle(hProcess);
			return false;
		}
		// ��Ŀ����̵��ڴ�ռ���д���������(ģ����)  
		if (FALSE == ::WriteProcessMemory(hProcess, ptszRemoteBuf, (LPVOID)ptszDllFile, dwSize * sizeof(TCHAR), NULL))
		{
			::VirtualFreeEx(hProcess, ptszRemoteBuf, dwSize, MEM_DECOMMIT);
			::CloseHandle(hProcess);
			return false;
		}
		// �� Kernel32.dll �л�ȡ LoadLibrary ������ַ  
#ifdef _UNICODE  
		lpThreadFun = (PTHREAD_START_ROUTINE)::GetProcAddress(::GetModuleHandle(_T("Kernel32")), "LoadLibraryW");
#else  
		lpThreadFun = (PTHREAD_START_ROUTINE)::GetProcAddress(::GetModuleHandle(_T("Kernel32")), "LoadLibraryA");
#endif  
		if (NULL == lpThreadFun)
		{
			::VirtualFreeEx(hProcess, ptszRemoteBuf, dwSize, MEM_DECOMMIT);
			::CloseHandle(hProcess);
			return false;
		}
		// ����Զ���̵߳��� LoadLibrary  
		hThread = ::CreateRemoteThread(hProcess, NULL, 0, lpThreadFun, ptszRemoteBuf, 0, NULL);
		if (NULL == hThread)
		{
			::VirtualFreeEx(hProcess, ptszRemoteBuf, dwSize, MEM_DECOMMIT);
			::CloseHandle(hProcess);
			return false;
		}
		// �ȴ�Զ���߳̽���  
		::WaitForSingleObject(hThread, INFINITE);
		// ����  
		::VirtualFreeEx(hProcess, ptszRemoteBuf, dwSize, MEM_DECOMMIT);
		::CloseHandle(hThread);
		::CloseHandle(hProcess);
		return true;
	}
	//-----------------------------------------------------------------------------------------------------------  
	// ����: UnInjectDll  
	// ����: ��Ŀ�������ж��һ��ָ�� Dll ģ���ļ�.  
	// ����: [in] const TCHAR* ptszDllFile - Dll �ļ�����·��  
	//       [in] DWORD dwProcessId - Ŀ����� ID  
	// ����: bool - ж�سɹ����� true, ж��ʧ���򷵻� false.  
	// ˵��: ����Զ���߳�ע�뼼��ʵ��  
	//-----------------------------------------------------------------------------------------------------------  
	static bool UnInjectDll(const TCHAR* ptszDllFile, DWORD dwProcessId)
	{
		// ������Ч  
		if (NULL == ptszDllFile || 0 == ::_tcslen(ptszDllFile))
		{
			return false;
		}
		HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
		HANDLE hProcess = NULL;
		HANDLE hThread = NULL;
		LPTHREAD_START_ROUTINE lpThreadFun = NULL;
		// ��ȡģ�����  
		hModuleSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId);
		if (INVALID_HANDLE_VALUE == hModuleSnap)
		{
			return false;
		}
		MODULEENTRY32 me32;
		memset(&me32, 0, sizeof(MODULEENTRY32));
		me32.dwSize = sizeof(MODULEENTRY32);
		// ��ʼ����  
		if (FALSE == ::Module32First(hModuleSnap, &me32))
		{
			::CloseHandle(hModuleSnap);
			return false;
		}
		// ��������ָ��ģ��  
		bool isFound = false;
		do
		{
			isFound = (0 == ::_tcsicmp(me32.szModule, ptszDllFile) || 0 == ::_tcsicmp(me32.szExePath, ptszDllFile));
			if (isFound) // �ҵ�ָ��ģ��  
			{
				break;
			}
		} while (TRUE == ::Module32Next(hModuleSnap, &me32));
		::CloseHandle(hModuleSnap);
		if (false == isFound)
		{
			return false;
		}
		// ��ȡĿ����̾��  
		hProcess = ::OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION, FALSE, dwProcessId);
		if (NULL == hProcess)
		{
			return false;
		}
		// �� Kernel32.dll �л�ȡ FreeLibrary ������ַ  
		lpThreadFun = (PTHREAD_START_ROUTINE)::GetProcAddress(::GetModuleHandle(_T("Kernel32")), "FreeLibrary");
		if (NULL == lpThreadFun)
		{
			::CloseHandle(hProcess);
			return false;
		}
		// ����Զ���̵߳��� FreeLibrary  
		hThread = ::CreateRemoteThread(hProcess, NULL, 0, lpThreadFun, me32.modBaseAddr /* ģ���ַ */, 0, NULL);
		if (NULL == hThread)
		{
			::CloseHandle(hProcess);
			return false;
		}
		// �ȴ�Զ���߳̽���  
		::WaitForSingleObject(hThread, INFINITE);
		// ����  
		::CloseHandle(hThread);
		::CloseHandle(hProcess);
		return true;
	}
};