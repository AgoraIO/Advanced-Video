#ifndef __COMMONFUN_H__
#define __COMMONFUN_H__

#include <vector>
//comfun
std::string getAbsoluteDir();
std::string getFilePath();
std::string getCurRunningExeName();
std::string getFileAbsolutePath(const std::string &file);
std::string getPirorDir(const std::string &file);
std::string getPirorDirEx(const std::string &file);
std::string getRootDir(const std::string &file);

std::string int2str(int nNum);
std::string float2str(float fValue);
std::string gbk2utf8(const char *gbk);
std::string utf82gbk(const char *utf8);
std::string gbk2utf8(const std::string &gbk);
std::string utf82gbk(const std::string &utf8);
int str2int(const std::string &str);
int str2long(const std::string &str);
float str2float(const std::string &str);
CString s2cs(const std::string &str);
std::string cs2s(const CString &str);

std::string getTime();
DWORD getProcessID(const std::string &processName);
bool closeProcess(const std::string &processName,int &num);
int getProcessIdMutil(const std::string &processName);
std::vector<DWORD> getProcessMutilVec(const std::string processName);
void closeCurrentProcess();
bool registerStartUp();
DWORD openProcess(const std::string &processName,const std::string &cmdLine);

//Log
std::string getSigSdkLogPath();
std::string getMediaSdkLogPath();
std::string getHQLogPath();
std::string getChannelName();

class CAgoraFormatStr{
public:

	static void PASCAL AgoraMessageBox(LPCTSTR lpFormat, ...){

		TCHAR szBuffer[1024] = { _T("\0") };
		va_list args;
		va_start(args, lpFormat);
		_vsnwprintf(szBuffer + _tcslen(szBuffer), sizeof(szBuffer) / sizeof(TCHAR) - _tcslen(szBuffer), lpFormat, args);
		va_end(args);

		AfxMessageBox(szBuffer);
	}

	static void PASCAL AgoraOutDebugStr(LPCTSTR lpFormat, ...){

		TCHAR szBuffer[1024] = { _T("\0") };
		va_list args;
		_tcsnccat(szBuffer, _T("\n"), 1);
		va_start(args, lpFormat);
		_vsnwprintf(szBuffer + _tcslen(szBuffer), sizeof(szBuffer) / sizeof(TCHAR) - _tcslen(szBuffer), lpFormat, args);
		va_end(args);

		OutputDebugString(szBuffer);
	}

	static void  PASCAL AgoraWriteLog(LPSTR lpFormat, ...){

		char szBuffer[1024] = { '\0' };
		va_list args;
		va_start(args, lpFormat);

		SYSTEMTIME st;
		GetLocalTime(&st);
		sprintf_s(szBuffer, "%04d%02d%02d:%02d%02d%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

		_vsnprintf(szBuffer + strlen(szBuffer), sizeof(szBuffer) / sizeof(char) - strlen(szBuffer), lpFormat, args);
		va_end(args);

		gFileHook.write(szBuffer);
	}

	static int PASCAL CS2int(const CString &csStr)
	{
		std::string str = cs2s(csStr);
		return str2int(str);
	}

	static bool PASCAL IsDebugMode(HINSTANCE HModule)
	{
		TCHAR szFilePath[MAX_PATH];
		CString DebugMode(_T(""));

		::GetModuleFileName(HModule, szFilePath, MAX_PATH);
		LPTSTR lpLastSlash = _tcsrchr(szFilePath, _T('\\'));

		if (lpLastSlash == NULL)
			return FALSE;

		SIZE_T nNameLen = MAX_PATH - (lpLastSlash - szFilePath + 1);
		_tcscpy_s(lpLastSlash + 1, nNameLen, _T("DebugMode.ini"));

		if (::GetFileAttributes(szFilePath) == INVALID_FILE_ATTRIBUTES) {
			CreateFile(szFilePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
				CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

			::WritePrivateProfileString(_T("DebugMode"), _T("DebugMode"), _T("1"), szFilePath);
			::WritePrivateProfileString(_T("DebugMode"), _T("SaveDumpPcm"), _T("0"), szFilePath);

			return FALSE;
		}

		CString strResolution;
		//Default 1
		::GetPrivateProfileString(_T("DebugMode"), _T("DebugMode"), NULL, DebugMode.GetBuffer(MAX_PATH), MAX_PATH, szFilePath);
		if (_T("") == DebugMode) {
			DebugMode = _T("1");
			::WritePrivateProfileString(_T("DebugMode"), _T("DebugMode"), DebugMode, szFilePath);
		}

		DebugMode.ReleaseBuffer();

		return CS2int(DebugMode);
	}

	static bool PASCAL IsSaveDumpPcm(HINSTANCE HModule)
	{
		TCHAR szFilePath[MAX_PATH];
		CString DebugMode(_T(""));

		::GetModuleFileName(HModule, szFilePath, MAX_PATH);
		LPTSTR lpLastSlash = _tcsrchr(szFilePath, _T('\\'));

		if (lpLastSlash == NULL)
			return FALSE;

		SIZE_T nNameLen = MAX_PATH - (lpLastSlash - szFilePath + 1);
		_tcscpy_s(lpLastSlash + 1, nNameLen, _T("DebugMode.ini"));

		if (::GetFileAttributes(szFilePath) == INVALID_FILE_ATTRIBUTES) {
			CreateFile(szFilePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
				CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

			::WritePrivateProfileString(_T("DebugMode"), _T("DebugMode"), _T("1"), szFilePath);
			::WritePrivateProfileString(_T("DebugMode"), _T("SaveDumpPcm"), _T("0"), szFilePath);

			return FALSE;
		}

		CString strResolution;
		//Default 0
		::GetPrivateProfileString(_T("DebugMode"), _T("SaveDumpPcm"), NULL, DebugMode.GetBuffer(MAX_PATH), MAX_PATH, szFilePath);
		if (_T("") == DebugMode) {
			DebugMode = _T("0");
			::WritePrivateProfileString(_T("DebugMode"), _T("SaveDumpPcm"), DebugMode, szFilePath);
		}

		DebugMode.ReleaseBuffer();

		return CS2int(DebugMode);
	}

	enum eHookPlayerType{

		ePlayer_NULL = -1,
		ePlayer_KuGou,
		ePlayer_CloudMusic,
		ePlayer_QQMusic,
		ePlayer_KwMusic7,

		ePlayer_UNKNOWN = 0xff
	};

#if 0
	static BOOL PASCAL QueryDir(eHookPlayerType hookType, CString &strDir)
	{
		HKEY hKeyOpen;
		std::string strAppName = "";
		TCHAR szBuffer[MAX_PATH] = { _T("\0") };
		TCHAR szBufferTemp[MAX_PATH] = { _T("\0") };
		CString strFormat;

		switch (hookType)
		{
		case eHookPlayerType::ePlayer_KuGou: {

			hKeyOpen = HKEY_CURRENT_USER;
			strAppName = "KuGou";
			strFormat = _T("software\\%s");
		}
			break;
		case eHookPlayerType::ePlayer_CloudMusic: {

			hKeyOpen = HKEY_LOCAL_MACHINE;
			strAppName = "CloudMusic";
			strFormat = _T("")
		}
			break;
		default:break;
		}

		MultiByteToWideChar(CP_UTF8, 0, strAppName.data(), strAppName.length(), szBufferTemp, MAX_PATH);
		swprintf_s(szBuffer,strFormat, szBufferTemp);

		HKEY hKey;
		LONG lRet = RegOpenKeyEx(hKeyOpen, szBuffer, 0, KEY_QUERY_VALUE, &hKey);
		if (ERROR_SUCCESS != lRet)
			return FALSE;

		ZeroMemory(szBufferTemp, sizeof(szBufferTemp) / sizeof(TCHAR));
		DWORD dwBufferLen = MAX_PATH;
		lRet = RegQueryValueEx(hKey, L"AppPath", NULL, NULL, (LPBYTE)szBufferTemp,&dwBufferLen);
		if (ERROR_SUCCESS != lRet)
			return FALSE;

		RegCloseKey(hKey);
		hKey = nullptr;

		strDir.Format(_T("%s\\%s.exe"), szBufferTemp, s2cs(strAppName));

		return TRUE;
	}
#endif

	static BOOL PASCAL QueryDir(eHookPlayerType hookType, CString &strDir)
	{
		CString strFormat;
		switch (hookType)
		{
		case eHookPlayerType::ePlayer_KuGou:
			strFormat = _T("SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\ø·π∑“Ù¿÷"); break;
		case eHookPlayerType::ePlayer_CloudMusic:
			strFormat = _T("SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Õ¯“◊‘∆“Ù¿÷"); break;
		case eHookPlayerType::ePlayer_KwMusic7:
			strFormat = _T("SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\KwMusic7"); break;
		case eHookPlayerType::ePlayer_QQMusic:
			strFormat = _T("SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\QQMusic"); break;
		default:break;
		}
		
		TCHAR szBuffer[MAX_PATH] = { _T("\0") };
		HKEY hKey;
		LONG lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, strFormat, 0, KEY_QUERY_VALUE, &hKey);
		if (ERROR_SUCCESS != lRet)
			return FALSE;

		DWORD dwBufferLen = MAX_PATH;
		lRet = RegQueryValueEx(hKey, L"DisplayIcon", NULL, NULL, (LPBYTE)szBuffer, &dwBufferLen);
		if (ERROR_SUCCESS != lRet)
			return FALSE;

		RegCloseKey(hKey);
		hKey = nullptr;

		strDir = szBuffer;

		return TRUE;
	}


};

#endif