#include "stdafx.h"
#include "DShowPinHelper.h"

CDShowPinHelper::CDShowPinHelper()
	: m_ptrFilter(NULL)
{

}

CDShowPinHelper::~CDShowPinHelper()
{
	Close();
}

BOOL CDShowPinHelper::Open(IBaseFilter* ptrFilter)
{
	ATLASSERT(ptrFilter != NULL);

	HRESULT				hResult = S_OK;
	CComPtr<IEnumPins>	ptrEnumPins = NULL;
	CComPtr<IPin>		ptrPin = NULL;

	m_ptrFilter = ptrFilter;
	return TRUE;
}

void CDShowPinHelper::Close()
{
	m_ptrFilter = NULL;
}

int CDShowPinHelper::GetPinCount()
{
	ATLASSERT(m_ptrFilter != NULL);

	if (m_ptrFilter == NULL)
		return 0;

	HRESULT				hResult = S_OK;
	CComPtr<IEnumPins>	ptrEnumPins = NULL;
	CComPtr<IPin>		ptrPin = NULL;
	int					nPinCount = 0;

	hResult = m_ptrFilter->EnumPins(&ptrEnumPins);
	if (hResult != S_OK)
		return 0;

	while (TRUE) {
		ptrPin = NULL;
		hResult = ptrEnumPins->Next(1, &ptrPin, NULL);
		if (hResult != S_OK)
			break;

		nPinCount++;
	}

	return nPinCount;
}

BOOL CDShowPinHelper::GetPinDirection(int nIndex, PIN_DIRECTION *pDirection)
{
	ATLASSERT(pDirection != NULL);
	ATLASSERT(m_ptrFilter != NULL);

	if (pDirection == NULL || m_ptrFilter == NULL)
		return FALSE;

	HRESULT				hResult = S_OK;
	CComPtr<IEnumPins>	ptrEnumPins = NULL;
	CComPtr<IPin>		ptrPin = NULL;
	int					nCount = 0;

	hResult = m_ptrFilter->EnumPins(&ptrEnumPins);
	if (hResult != S_OK)
		return FALSE;

	while (TRUE) {
		ptrPin = NULL;
		hResult = ptrEnumPins->Next(1, &ptrPin, NULL);
		if (hResult != S_OK)
			break;

		if (nIndex == nCount) {
			hResult = ptrPin->QueryDirection(pDirection);
			return hResult == S_OK ? TRUE : FALSE;
		}

		nCount++;
	}

	return FALSE;
}

BOOL CDShowPinHelper::GetPin(int nIndex, IPin **pptrPin)
{
	ATLASSERT(m_ptrFilter != NULL);

	if (m_ptrFilter == NULL)
		return FALSE;

	HRESULT				hResult = S_OK;
	CComPtr<IEnumPins>	ptrEnumPins = NULL;
	CComPtr<IPin>		ptrPin = NULL;
	int					nCount = 0;

	hResult = m_ptrFilter->EnumPins(&ptrEnumPins);
	if (hResult != S_OK)
		return FALSE;

	while (TRUE) {
		ptrPin = NULL;
		hResult = ptrEnumPins->Next(1, &ptrPin, NULL);
		if (hResult != S_OK)
			break;

		if (nIndex == nCount) {
			hResult = ptrPin.CopyTo(pptrPin);
			return hResult == S_OK ? TRUE : FALSE;
		}

		nCount++;
	}

	return FALSE;
}

BOOL CDShowPinHelper::FindUnconnectedPin(PIN_DIRECTION PinDir, IPin **pptrPin)
{
	HRESULT				hResult = S_OK;
	CComPtr<IEnumPins>	ptrEnumPins = NULL;
	CComPtr<IPin>		ptrPin = NULL;
	BOOL				bMatch = FALSE;

	hResult = m_ptrFilter->EnumPins(&ptrEnumPins);
	if (hResult != S_OK)
		return FALSE;

	while (TRUE) {
		ptrPin = NULL;
		hResult = ptrEnumPins->Next(1, &ptrPin, NULL);
		if (hResult != S_OK)
			return FALSE;

		if (!CDShowPinHelper::MatchPin(ptrPin, PinDir, FALSE, &bMatch))
			return FALSE;

		if (bMatch) {
			hResult = ptrPin.CopyTo(pptrPin);
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CDShowPinHelper::IsPinConnected(IPin *pPin, BOOL *pIsConnected)
{
	CComPtr<IPin> ptrTmpPin = NULL;
	BOOL bConnected = FALSE;

	HRESULT hResult = pPin->ConnectedTo(&ptrTmpPin);
	if (SUCCEEDED(hResult)) {
		*pIsConnected = TRUE;
		return TRUE;
	}
	else if (hResult == VFW_E_NOT_CONNECTED) {
		*pIsConnected = FALSE;
		return TRUE;
	}

	return FALSE;
}

BOOL CDShowPinHelper::MatchPin(IPin *pPin, PIN_DIRECTION direction, BOOL bIsConnected, BOOL *pMatch)
{
	ATLASSERT(pPin != NULL);

	BOOL bMatch = FALSE;
	
	BOOL bSuccess = CDShowPinHelper::IsPinConnected(pPin, &bIsConnected);
	if (!bSuccess)
		return FALSE;

	if (bIsConnected == bIsConnected)
		bSuccess = IsPinDirection(pPin, direction, &bMatch);

	if (bSuccess)
		*pMatch = bMatch;
	
	return bSuccess;
}

BOOL CDShowPinHelper::IsPinDirection(IPin *pPin, PIN_DIRECTION dir, BOOL *pMatch)
{
	PIN_DIRECTION pinDir;
	HRESULT hResult = pPin->QueryDirection(&pinDir);

	if (SUCCEEDED(hResult)) {
		*pMatch = (pinDir == dir);
		return TRUE;
	}
	
	return FALSE;
}