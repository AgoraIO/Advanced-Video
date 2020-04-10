#pragma once
#include <atlbase.h>
#include <atlcoll.h>

#include <DShow.h>

class CDShowPinHelper
{
public:
	CDShowPinHelper();
	~CDShowPinHelper();

	BOOL Open(IBaseFilter* ptrFilter);
	void Close();

	int GetPinCount();

	BOOL GetPinDirection(int nIndex, PIN_DIRECTION *pDirection);
	BOOL GetPin(int nIndex, IPin **pptrPin);
	BOOL FindUnconnectedPin(PIN_DIRECTION PinDir, IPin **pptrPin);

	static BOOL IsPinConnected(IPin *pPin, BOOL *pIsConnected);
	static BOOL MatchPin(IPin *pPin, PIN_DIRECTION direction, BOOL bIsConnected, BOOL *pMatch);
	static BOOL IsPinDirection(IPin *pPin, PIN_DIRECTION dir, BOOL *pMatch);
private:
	CComPtr<IBaseFilter>	m_ptrFilter;
};
