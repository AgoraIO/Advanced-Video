#pragma once

#include <stdint.h>
class CAGResourceVisitor
{
public:
	CAGResourceVisitor(void);
	~CAGResourceVisitor(void);

	static BOOL PASCAL SaveResourceToFile(LPCTSTR lpResourceType, WORD wResourceID, LPCTSTR lpFilePath);

	// 因为WINDOWS的路径中目录分隔符与标准格式不一致，不转换会发生错误
	static LPCSTR PASCAL TransWinPathA(LPCSTR lpWinPath, LPSTR lpStandardPath, SIZE_T cchSize);
	static LPCWSTR PASCAL TransWinPathW(LPCWSTR lpWinPath, LPWSTR lpStandardPath, SIZE_T cchSize);

	static int16_t  PASCAL MixerAddS16(int16_t var1, int16_t var2);
	static void PASCAL MixerAddS16(int16_t* src1, const int16_t* src2, size_t size);
};
