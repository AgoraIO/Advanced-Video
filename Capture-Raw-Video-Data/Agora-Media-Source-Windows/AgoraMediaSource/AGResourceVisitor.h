#pragma once

class CAGResourceVisitor
{
public:
	CAGResourceVisitor(void);
	~CAGResourceVisitor(void);

	static BOOL PASCAL SaveResourceToFile(LPCTSTR lpResourceType, WORD wResourceID, LPCTSTR lpFilePath);

	static LPCSTR PASCAL TransWinPathA(LPCSTR lpWinPath, LPSTR lpStandardPath, SIZE_T cchSize);
	static LPCWSTR PASCAL TransWinPathW(LPCWSTR lpWinPath, LPWSTR lpStandardPath, SIZE_T cchSize);
};
