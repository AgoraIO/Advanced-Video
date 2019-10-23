#include "stdafx.h"
#include "RenderFactoryImpl.h"
#include "CBufferMgr.h"
#include "libYUV/libyuv.h"

#ifdef _M_IX86
#ifdef DEBUG
#pragma comment(lib, "libYUV/debug_x86/yuv.lib")
#pragma comment(lib, "libYUV/debug_x86/turbojpeg-static.lib")
#else
#pragma comment(lib, "libYUV/release_x86/yuv.lib")
#pragma comment(lib, "libYUV/release_x86/turbojpeg-static.lib")
#endif

#elif defined _M_X64
#pragma comment(lib, "libYUV/release_x64/yuv.lib")
#pragma comment(lib, "libYUV/release_x64/turbojpeg-static.lib")
#endif

using namespace libyuv;

CRenderFactoryImpl::CRenderFactoryImpl()
{
	m_listExternalVideoRender.clear();
}

CRenderFactoryImpl::~CRenderFactoryImpl()
{
	for (auto it = m_listExternalVideoRender.begin(); m_listExternalVideoRender.end() != it; it++)
		delete *it;

	m_listExternalVideoRender.clear();
}

IExternalVideoRender* CRenderFactoryImpl::createRenderInstance(
	const ExternalVideoRenerContext& context) {
	
	IExternalVideoRender* lpExternalVideoRender = new CRenderFactoryVideoFrameInstance(context);
	auto iter = find(m_listExternalVideoRender.begin(), m_listExternalVideoRender.end(), lpExternalVideoRender);
	if (m_listExternalVideoRender.end() != iter)
		m_listExternalVideoRender.push_back(lpExternalVideoRender);

	LPAGE_RENDER_FACTORY lpData = new AGE_RENDER_FACTORY;
	if (lpData) {
		lpData->pViewId = context.view;
	}
	::PostMessage(CAgoraObject::GetAgoraObject()->GetMsgHandlerWnd(), WM_MSGID(EID_RENDER_FACTORY), WPARAM(lpData), NULL);

	return lpExternalVideoRender;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
CRenderFactoryVideoFrameInstance::CRenderFactoryVideoFrameInstance()
{

}

CRenderFactoryVideoFrameInstance::CRenderFactoryVideoFrameInstance(const ExternalVideoRenerContext &externalVideoContext):
m_externalVideoContext(externalVideoContext)
, m_lpVideoRenderBufferSrc(NULL)
, m_lpVideoRenderBufferDest(NULL)
{
	m_externalVideoContext.renderCallback = this;
}

CRenderFactoryVideoFrameInstance::~CRenderFactoryVideoFrameInstance()
{
	delete[] m_lpVideoRenderBufferSrc;
	m_lpVideoRenderBufferSrc = NULL;
	delete[] m_lpVideoRenderBufferDest;
	m_lpVideoRenderBufferDest = NULL;
	m_externalVideoContext.renderCallback = NULL;
}

void CRenderFactoryVideoFrameInstance::release()
{
	if (m_lpVideoFrame)
		m_lpVideoFrame->release();
	m_nRotation = 0;
	m_nMirrored = 0;
	delete this;
}

int CRenderFactoryVideoFrameInstance::initialize()
{
	m_lpVideoFrame = NULL;
	m_nRotation = 0;
	m_nMirrored = 0;
	return 0;
}

int CRenderFactoryVideoFrameInstance::deliverFrame(const IVideoFrame& videoFrame, int rotation, bool mirrored)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_nRotation = rotation;
	m_nMirrored = mirrored;
	videoFrame.copyFrame(&m_lpVideoFrame);

	int nWidth = m_lpVideoFrame->width();
	int nHeight = m_lpVideoFrame->height();
	int nBufferLen = nWidth * nHeight * 3 / 2;
	if (NULL == m_lpVideoRenderBufferSrc)
		m_lpVideoRenderBufferSrc = new BYTE[nBufferLen];
	if (NULL == m_lpVideoRenderBufferSrc)
		return FALSE;
	ZeroMemory(m_lpVideoRenderBufferSrc, nBufferLen);
	if (NULL == m_lpVideoRenderBufferDest)
		m_lpVideoRenderBufferDest = new BYTE[nBufferLen];
	if (NULL == m_lpVideoRenderBufferDest)
		return FALSE;
	ZeroMemory(m_lpVideoRenderBufferDest, nBufferLen);

	const uint8_t* pYBuffer = m_lpVideoFrame->buffer(IVideoFrame::Y_PLANE);
	const uint8_t* pUBuffer = m_lpVideoFrame->buffer(IVideoFrame::U_PLANE);
	const uint8_t* pVBuffer = m_lpVideoFrame->buffer(IVideoFrame::V_PLANE);
	int nYStride = m_lpVideoFrame->stride(IVideoFrame::Y_PLANE);
	int nUStride = m_lpVideoFrame->stride(IVideoFrame::U_PLANE);
	int nVStride = m_lpVideoFrame->stride(IVideoFrame::V_PLANE);
	
	memcpy_s(m_lpVideoRenderBufferSrc, nYStride * nHeight, pYBuffer, nYStride*nHeight);
	memcpy_s(m_lpVideoRenderBufferSrc + nYStride * nHeight, nUStride * nHeight / 2, pUBuffer, nUStride * nHeight / 2);
	memcpy_s(m_lpVideoRenderBufferSrc + nYStride * nHeight + nUStride * nHeight / 2, nVStride * nHeight / 2, pVBuffer, nVStride * nHeight / 2);
	nBufferLen = nYStride * nHeight + nUStride *nHeight / 2 + nVStride *nHeight / 2;

	libyuv::ConvertToI420((uint8_t*)m_lpVideoRenderBufferSrc, nBufferLen,
		m_lpVideoRenderBufferDest, nWidth,
		m_lpVideoRenderBufferDest + nWidth * nHeight, nWidth / 2,
		m_lpVideoRenderBufferDest + nWidth * nHeight * 5 / 4, nWidth / 2,
		0, 0, nYStride, nHeight,
		nWidth, nHeight, libyuv::kRotate0, libyuv::FOURCC_I420);
	nBufferLen = nWidth * nHeight * 3 / 2;

	nYStride = nWidth;
	nUStride = nWidth / 2;
	nVStride = nWidth / 2;
	uint8_t* yBuffer = m_lpVideoRenderBufferDest;
	uint8_t *uBuffer = m_lpVideoRenderBufferDest + nYStride *nHeight;
	uint8_t *vBuffer = m_lpVideoRenderBufferDest + nYStride * nHeight + nUStride * nHeight / 2;
	
	long lViewId = (long)m_externalVideoContext.view;
	CBufferMgr::getInstance()->pushYUVBuffer(lViewId, (uint8_t*)m_lpVideoRenderBufferDest, nBufferLen, nWidth, nHeight);
	char szbuf1[256] = { 0 };
	sprintf_s(szbuf1, "push buffer view: %p,viewId: %ld ,width: %d,height: %d\n", m_externalVideoContext.view,lViewId, nWidth, nHeight);
	OutputDebugStringA(szbuf1);

	return TRUE;
}

void CRenderFactoryVideoFrameInstance::onViewSizeChanged(int width, int height)
{
	OutputDebugString(_T(__FUNCTION__));
	OutputDebugString(_T("\n"));
}

void CRenderFactoryVideoFrameInstance::onViewDestroyed()
{
	OutputDebugString(_T(__FUNCTION__));
	OutputDebugString(_T("\n"));
}