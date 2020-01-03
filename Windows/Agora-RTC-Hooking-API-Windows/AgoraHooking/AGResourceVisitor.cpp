#include "stdafx.h"
#include "AGResourceVisitor.h"

CAGResourceVisitor::CAGResourceVisitor(void)
{
}

CAGResourceVisitor::~CAGResourceVisitor(void)
{
}

BOOL PASCAL CAGResourceVisitor::SaveResourceToFile(LPCTSTR lpResourceType, WORD wResourceID, LPCTSTR lpFilePath)
{	
	HMODULE hModule = ::GetModuleHandle(NULL);

	if(hModule == NULL)
		return FALSE;

	HRSRC hResrc = ::FindResource(hModule, MAKEINTRESOURCE(wResourceID), lpResourceType);
	if( hResrc == NULL)
		return FALSE;

	HGLOBAL	hGlobal = ::LoadResource(hModule, hResrc);   
	if( hGlobal == NULL )
		return FALSE;

	LPBYTE lpPointer = (LPBYTE)::LockResource( hGlobal );   
	DWORD dwResSize = ::SizeofResource(hModule, hResrc);

	HANDLE hFile = ::CreateFile(lpFilePath, GENERIC_ALL, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	DWORD dwBytesWritten = 0;
	::WriteFile(hFile, lpPointer, dwResSize, &dwBytesWritten, NULL);
	::CloseHandle(hFile);

	return (dwBytesWritten == dwResSize) ? TRUE:FALSE;
}

LPCSTR PASCAL CAGResourceVisitor::TransWinPathA(LPCSTR lpWinPath, LPSTR lpStandardPath, SIZE_T cchSize)
{
	int nPathLen = strlen(lpWinPath) + 1;
	if(cchSize < nPathLen)
		return NULL;

	CHAR	szASIN[MAX_DEVICE_ID_LENGTH];
	WCHAR	szUnicode[MAX_DEVICE_ID_LENGTH];

	memset(szASIN, 0, MAX_DEVICE_ID_LENGTH);

	for(int nIndex = 0; nIndex < nPathLen; nIndex++){

		if(lpWinPath[nIndex] != '\\')
			szASIN[nIndex] = lpWinPath[nIndex];
		else
			szASIN[nIndex] = '/';
	}

	::MultiByteToWideChar(CP_ACP, 0, szASIN, -1, szUnicode, MAX_DEVICE_ID_LENGTH);
	::WideCharToMultiByte(CP_UTF8, 0, szUnicode, -1, lpStandardPath, cchSize, NULL, NULL);
	return lpStandardPath;
}

LPCWSTR PASCAL CAGResourceVisitor::TransWinPathW(LPCWSTR lpWinPath, LPWSTR lpStandardPath, SIZE_T cchSize)
{
	int nPathLen = wcslen(lpWinPath) + 1;

	if(cchSize < nPathLen)
		return NULL;

	for(int nIndex = 0; nIndex < nPathLen; nIndex++){

		if(lpWinPath[nIndex] != L'\\')
			lpStandardPath[nIndex] = lpWinPath[nIndex];
		else
			lpStandardPath[nIndex] = L'/';
	}

	return lpStandardPath;
}

int16_t  PASCAL CAGResourceVisitor::MixerAddS16(int16_t var1, int16_t var2) 
{
	static const int32_t kMaxInt16 = 32767;
	static const int32_t kMinInt16 = -32768;
	int32_t tmp = (int32_t)var1 + (int32_t)var2;
	int16_t out16;

	if (tmp > kMaxInt16) {
		out16 = kMaxInt16;
	}
	else if (tmp < kMinInt16) {
		out16 = kMinInt16;
	}
	else {
		out16 = (int16_t)tmp;
	}

	return out16;
}

void PASCAL CAGResourceVisitor::MixerAddS16(int16_t* src1, const int16_t* src2, size_t size) 
{
	for (size_t i = 0; i < size; ++i) {
		src1[i] = MixerAddS16(src1[i], src2[i]);
	}
}