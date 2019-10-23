#pragma  once

#include <list>
#include <mutex>

class CRenderFactoryImpl : public IExternalVideoRenderFactory{
public:
	CRenderFactoryImpl();
	~CRenderFactoryImpl();

protected:
	virtual IExternalVideoRender* createRenderInstance(
		const ExternalVideoRenerContext& context);

private:
	std::list<IExternalVideoRender*> m_listExternalVideoRender;
};

class CRenderFactoryVideoFrameInstance :public IExternalVideoRender,public IExternalVideoRenderCallback {
public:
	CRenderFactoryVideoFrameInstance();
	CRenderFactoryVideoFrameInstance(const ExternalVideoRenerContext &externalVideoContext);
	~CRenderFactoryVideoFrameInstance();

protected:

	virtual void release();
	virtual int initialize();
	virtual int deliverFrame(const IVideoFrame& videoFrame, int rotation,
		bool mirrored);

	virtual void onViewSizeChanged(int width, int height);
	virtual void onViewDestroyed();

private:
	UINT m_uID;
	ExternalVideoRenerContext m_externalVideoContext;
	IVideoFrame* m_lpVideoFrame;
	int					m_nRotation;
	int					m_nMirrored;
	LPBYTE		m_lpVideoRenderBufferSrc;
	LPBYTE		m_lpVideoRenderBufferDest;
	std::mutex		m_mutex;
};