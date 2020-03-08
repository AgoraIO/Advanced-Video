#ifndef __FILEIO_H__
#define __FILEIO_H__

#include <vector>
#include <map>
class CFileIO
{
public:
	CFileIO();
	~CFileIO();

	void openLog(const std::string &filepath, int fileFlage = OPEN_ALWAYS);
	void openMedia(const std::string &filepath, int fileFlage = CREATE_ALWAYS);
	void openReadFile(const std::string &filePath);
	void close();
	int write(std::string bufferStr);
	int read(char *bufferOut, int bufferLen);
	std::string readLine();
	std::string read();

public:
	static bool generatorFile(const std::string &path);

protected:
	int write(char *bufferIn, int bufferLen);

private:
	HANDLE fileHandle_;
	int filelimitLine_;
	bool isLog_;
};

class CFileIni
{
public:
	CFileIni();
	CFileIni(const std::string &filePath);
	~CFileIni();

	bool openFile(const std::string &IniFile);
	bool write(const std::string &section, const std::string &key, const std::string &Value);
	std::string read(const std::string &section, const std::string &key);
	bool getSectionName(std::vector<std::string> &vecSection);
	bool getSection(const std::string &section, std::map<std::string, std::string> &mapKeyValue);
	std::string getFilePah();

private:
	std::string iniFile_;
	bool isValid_;
};

class CIniBase
{
public:
	CIniBase(const std::string &filePath);
	virtual ~CIniBase();
	std::string getFilePah();

#define __DECLARE_INICONFIG_FUN(CMDID)\
		std::string get##CMDID();\
		int set##CMDID(const std::string &v);

#define __IMPLEMENT_INICONFIG_FUN(CLASSNAME,CMDID,FIELD,KEY)\
		std::string CLASSNAME::get##CMDID()	{\
		return pIniInstance_->read(FIELD,KEY);}\
		int CLASSNAME::set##CMDID(const std::string &v)	{\
		return pIniInstance_->write(FIELD,KEY,v);}

#define __DECLARE_INICONFIG_SIMILAR_FUN(CMDID)\
		std::string get##CMDID(const std::string &filed);\
		int set##CMDID(const std::string &filed,const std::string &v);

#define __IMPLEMENT_INICONFIG_SIMILAR_FUN(CLASSNAME,CMDID,KEY)\
		std::string CLASSNAME::get##CMDID(const std::string &filed){\
		return pIniInstance_->read(filed,KEY);}\
		int CLASSNAME::set##CMDID(const std::string &filed,const std::string &v){\
		return pIniInstance_->write(filed,KEY,v);}

protected:
	CFileIni *pIniInstance_;
};

#define INI_BaseInfo													"BaseInfo"
#define INI_BaseInfo_APPID									    "AppId"
#define INI_BaseInfo_AppCertEnable							"AppCertEnable"
#define INI_BaseInfo_APPCertificateID							"AppCertificateId"
#define INI_BaseInfo_ChannelName								"ChannelName"
#define INI_BaseInfo_RestartTimeStatus						"RestartTimerStatus"
#define INI_BaseInfo_RestaTimer									"RestartTimer"
#define INI_BaseInfo_VideoPreview								"VideoPreview"
#define INI_BaseInfo_ClearLogInterval							"ClearLogInterval"
#define INI_BaseInfo_Language									"LanguagePack"
#define INI_BaseInfo_VideoProfileIndex						"ResolutionIndex"
#define INI_BaseInfo_VideoProfile								"Resolution"

#define INI_PROCESSID												"Instance"
#define INI_PROCESSID_Enable									"ProcessEnable"
#define INI_LoginInfo_UID											"LoginUid"
#define INI_LoginInfo_ChannelName								"ChannelName"
#define INI_DeviceInfo_AudioInName							"AudioInName"
#define INI_DeviceInfo_AudioInCOMID							"AudioInComID"
#define INI_DeviceInfo_PlayOutName							"PlayOutName"
#define INI_DeviceInfo_PlayOutCOMID							"PlayOutComID"
#define INI_DeviceInfo_CameraName							"CameraName"
#define INI_DeviceInfo_CameraCOMID							"CameraComID"
#define INI_DeviceInfo_State										"DeviceState"
#define INI_DeviceInfo_Choose									"DeviceChoose"
#define INI_DeviceInfo_ResolutionSave							"ResolutionSave"
#define INI_DeviceInfo_ResolutionIndex						"ResolutionIndex"
#define INI_DeviceInfo_ResolutionWidth						"ResolutionWidth"
#define INI_DeviceInfo_ResolutionHeight						"ResolutionHeight"
#define INI_DeviceInfo_SwitchWHEnable						"SwitchWHEnable"
#define INI_DeviceInfo_ResolutionFps							"ResolutionFps"
#define INI_DeviceInfo_ResolutionBitrate						"ResolutionBitrate"
#define INI_DeviceInfo_RtmpSave									"RtmpSave"
#define INI_DeviceInfo_RtmpUrl									"RtmpUrl"
#define INI_DeviceInfo_RtmpWidth								"RtmpWidth"
#define INI_DeviceInfo_RtmpHeight								"RtmpHeight"
#define INI_DeviceInfo_RtmpFps									"RtmpFps"
#define INI_DeviceInfo_RtmpBitrate								"RtmpBitrate"
#define INI_DeviceInfo_LeftRotate90								"LeftRotate90"

class CConfigAgora:public CIniBase
{
public:
	CConfigAgora();
	CConfigAgora(const std::string &path);
	~CConfigAgora();

public:
	__DECLARE_INICONFIG_FUN(AppId)
	__DECLARE_INICONFIG_FUN(AppCertEnable)
	__DECLARE_INICONFIG_FUN(AppCertificateId)
	__DECLARE_INICONFIG_FUN(ChannelName)
	__DECLARE_INICONFIG_FUN(RestartTimerStatus)
	__DECLARE_INICONFIG_FUN(RestartTimer)
	__DECLARE_INICONFIG_FUN(VideoPreview)
	__DECLARE_INICONFIG_FUN(ClearLogInterval)
	__DECLARE_INICONFIG_FUN(LanguagePack)
	__DECLARE_INICONFIG_FUN(ResolutionIndex)
	__DECLARE_INICONFIG_FUN(Resolution)

	__DECLARE_INICONFIG_SIMILAR_FUN(ProcessEnable)
	__DECLARE_INICONFIG_SIMILAR_FUN(LoginUid)
	__DECLARE_INICONFIG_SIMILAR_FUN(ChannelName)
	__DECLARE_INICONFIG_SIMILAR_FUN(AudioInName)
	__DECLARE_INICONFIG_SIMILAR_FUN(AudioInComID)
	__DECLARE_INICONFIG_SIMILAR_FUN(PlayOutName)
	__DECLARE_INICONFIG_SIMILAR_FUN(PlayOutComID)
	__DECLARE_INICONFIG_SIMILAR_FUN(CameraName)
	__DECLARE_INICONFIG_SIMILAR_FUN(CameraComID)
	__DECLARE_INICONFIG_SIMILAR_FUN(DeviceState)
	__DECLARE_INICONFIG_SIMILAR_FUN(DeviceChoose)
	__DECLARE_INICONFIG_SIMILAR_FUN(LeftRotate90)

	__DECLARE_INICONFIG_SIMILAR_FUN(ResolutionSave)
	__DECLARE_INICONFIG_SIMILAR_FUN(ResolutionIndex)
	__DECLARE_INICONFIG_SIMILAR_FUN(ResolutionWidth)
	__DECLARE_INICONFIG_SIMILAR_FUN(ResolutionHeight)
	__DECLARE_INICONFIG_SIMILAR_FUN(SwitchWHEnable)
	__DECLARE_INICONFIG_SIMILAR_FUN(ResolutionFps)
	__DECLARE_INICONFIG_SIMILAR_FUN(ResolutionBitrate)

	__DECLARE_INICONFIG_SIMILAR_FUN(RtmpSave)
	__DECLARE_INICONFIG_SIMILAR_FUN(RtmpUrl)
	__DECLARE_INICONFIG_SIMILAR_FUN(RtmpWidth)
	__DECLARE_INICONFIG_SIMILAR_FUN(RtmpHeight)
	__DECLARE_INICONFIG_SIMILAR_FUN(RtmpFps)
	__DECLARE_INICONFIG_SIMILAR_FUN(RtmpBitrate)

private:

};

#endif
