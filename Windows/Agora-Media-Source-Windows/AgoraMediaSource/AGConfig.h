#pragma once
class CAGConfig
{
public:
	CAGConfig();
	~CAGConfig();

	int GetCodec();
	BOOL SetCodec(int nCodec);

/*	int GetResolution();
	BOOL SetResolution(int nResolution);

	int GetFPS();
	BOOL SetFPS(int nPFS);

	int GetMaxRate();
	BOOL SetMaxRate(int nMaxRate);
*/
	BOOL EnableAutoSave(BOOL bEnable);
	BOOL IsAutoSaveEnabled();

    BOOL SetSolution(int nIndex);
    int GetSolution();

    CString GetAppid();
    void SetAppid(CString appid);

    CString GetFilePath() {return m_szConfigFile;}
private:
	TCHAR m_szConfigFile[MAX_PATH];
};

