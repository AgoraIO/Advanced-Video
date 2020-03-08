#include "stdafx.h"
#include "FileIO.h"
#include <assert.h>
#include "commonFun.h"

CFileIO::CFileIO() :
fileHandle_(nullptr)
, filelimitLine_(0)
, isLog_(false)
{
}

CFileIO::~CFileIO()
{
	fileHandle_ = nullptr;
	filelimitLine_ = 0;
	isLog_ = false;
}

void CFileIO::openLog(const std::string &filepath, int fileFlage /*= OPEN_ALWAYS */)
{
	assert(nullptr == fileHandle_);
	fileHandle_ = CreateFile(
		CString(filepath.c_str()),
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		nullptr,
		/*CREATE_NEW | OPEN_ALWAYS | TRUNCATE_EXISTING*/fileFlage,
		FILE_ATTRIBUTE_NORMAL,
		nullptr);
	if (INVALID_HANDLE_VALUE == fileHandle_)
	{
		fileHandle_ = nullptr;
		printf("文件创建失败!!!!!!\n");
		int retCode = GetLastError();
		if (ERROR_ALREADY_EXISTS == retCode)
		{
			printf("文件已经存在,创建文件失败!!!!\n");
		}
	}

	isLog_ = true;
	SetFilePointer(fileHandle_, 0, nullptr, FILE_END);
}

void CFileIO::openMedia(const std::string &filepath, int fileFlage /*= CREATE_ALWAYS*/)
{
	assert(nullptr == fileHandle_);
	fileHandle_ = CreateFile(CString(filepath.c_str()),
		GENERIC_WRITE | GENERIC_READ,
		FILE_SHARE_WRITE | FILE_SHARE_READ,
		nullptr,
		fileFlage,
		FILE_ATTRIBUTE_NORMAL,
		nullptr);
	if (INVALID_HANDLE_VALUE == fileHandle_)
	{
		fileHandle_ = nullptr;
		printf("文件创建失败!!!!!!\n");
		int retCode = GetLastError();
		if (ERROR_ALREADY_EXISTS == retCode)
		{
			printf("文件已经存在,创建文件失败!!!!\n");
		}
	}
}

void CFileIO::openReadFile(const std::string &filePath)
{
	openLog(filePath);
	FlushFileBuffers(fileHandle_);
	SetFilePointer(fileHandle_, 0, nullptr, FILE_BEGIN);
}

void CFileIO::close()
{
	CloseHandle(fileHandle_);
	fileHandle_ = nullptr;
}

int CFileIO::write(char *bufferIn, int bufferLen)
{
	if (fileHandle_)
	{
		DWORD writenLen = 0;
		int res = WriteFile(fileHandle_, bufferIn, (DWORD)bufferLen, &writenLen, nullptr);
		if (0 == res)
		{
			printf("write buffer failed..retCode: %d!!!!!\n", GetLastError());
		}
		return int(writenLen);
	}

	return 0;
}

int CFileIO::write(std::string bufferStr)
{
	filelimitLine_++;
	if (isLog_)
	{
		bufferStr += "\r\n";
	}
	if (isLog_ && 100 == filelimitLine_)
	{//清空操作
		SetFilePointer(fileHandle_, 0, nullptr, FILE_BEGIN);
		SetEndOfFile(fileHandle_);
		filelimitLine_ = 0;
	}

	return write((char*)bufferStr.c_str(), bufferStr.length());
}

int CFileIO::read(char *bufferOut, int bufferLen)
{
	DWORD readLen = 0;
	int res = ReadFile(fileHandle_, bufferOut, bufferLen, &readLen, nullptr);
	if (0 == res)
	{
		printf("read buffer from file failed!!!,retCode: %d\n", GetLastError());
		return int(readLen);
	}
	return readLen;
}

std::string CFileIO::readLine()
{
	std::string strLine;
	char readTxt[2] = { 0 };
	DWORD readLen = 0; DWORD dwValue = 0;
	BOOL res = ReadFile(fileHandle_, readTxt, 1, &readLen, nullptr);
	int nerr = GetLastError();
	std::string chKey = "\r";
	while (true)
	{
		if (res && 1 == readLen && chKey == readTxt)
		{
			res = ReadFile(fileHandle_, readTxt, 1, &readLen, nullptr);
			chKey = "\n";
			if (res && 1 == readLen && chKey == readTxt)
			{
				break;
			}
			else
			{
				printf("read error ,cloud read '\r\n'\n");
				return "";
			}
		}
		else if (res && 1 == readLen &&  chKey != readTxt)
		{
			strLine += readTxt;
			//dwValue = SetFilePointer(fileHandle_, readLen, nullptr, FILE_CURRENT);
			readLen = 0; ZeroMemory(readTxt, 2); dwValue = 0;
			res = ReadFile(fileHandle_, readTxt, 1, &readLen, nullptr);
		}
		if (res && 0 == readLen)
		{
			break;
		}
	}
	return strLine;
}

std::string CFileIO::read()
{
	return "";
}

bool CFileIO::generatorFile(const std::string &path)
{
	HANDLE fileHandle = CreateFile(CString(path.c_str()),
		GENERIC_READ | GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		nullptr,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		nullptr);
	if (INVALID_HANDLE_VALUE == fileHandle)
	{
		return false;
	}
	return true;
}

CFileIni::CFileIni() :isValid_(false)
{

}

CFileIni::CFileIni(const std::string &filePath)
{
	iniFile_ = filePath;
	CFileIO::generatorFile(filePath);
}

CFileIni::~CFileIni()
{
	isValid_ = false;
}

bool CFileIni::openFile(const std::string &IniFile)
{
	iniFile_ = IniFile;
	return isValid_ = CFileIO::generatorFile(IniFile);
}

bool CFileIni::write(const std::string &section, const std::string &key, const std::string &Value)
{
	assert(isValid_);
	return (bool)(WritePrivateProfileString(s2cs(section), s2cs(key), s2cs(Value), s2cs(iniFile_)));
}

std::string CFileIni::read(const std::string &section, const std::string &key)
{
	assert(isValid_);
	std::string Value;
	TCHAR returnStr[MAX_DEVICE_ID_LENGTH] = { 0 };
	GetPrivateProfileString(s2cs(section), s2cs(key), _T(""), returnStr, MAX_DEVICE_ID_LENGTH, s2cs(iniFile_));
	Value = cs2s(returnStr);
	return Value;
}

bool CFileIni::getSectionName(std::vector<std::string> &vecSection)
{
	assert(isValid_);
	TCHAR returnStr[MAX_DEVICE_ID_LENGTH] = { 0 }; std::string sectionItem;
	DWORD retNum = GetPrivateProfileSectionNames(returnStr, MAX_DEVICE_ID_LENGTH, s2cs(iniFile_));
	if (0 < retNum)
	{
		int strLen = retNum;
		int nIndex = 0; TCHAR tchTemp = '\0';
		while (nIndex < strLen)
		{
			if ('\0' != (tchTemp = returnStr[nIndex]))
			{
				sectionItem += (tchTemp);
			}
			else
			{
				vecSection.push_back(sectionItem);
				sectionItem.clear();
			}
			nIndex++;
		}
	}
	return retNum > 0;
}

bool CFileIni::getSection(const std::string &section, std::map<std::string, std::string> &mapKeyValue)
{
	assert(isValid_);
	TCHAR returnStr[MAX_DEVICE_ID_LENGTH] = { 0 }; std::string key; std::string value; bool isKey = true;
	DWORD retNum = GetPrivateProfileSection(s2cs(section), returnStr, MAX_DEVICE_ID_LENGTH, s2cs(iniFile_));
	if (0 < retNum)
	{
		int strLen = retNum;
		int nIndex = 0; TCHAR tchTemp = '\0';
		while (nIndex < strLen)
		{
			if ('\0' != (tchTemp = returnStr[nIndex]))
			{
				if (L'=' == tchTemp)
				{
					isKey = false;
					nIndex++;
					continue;
				}
				if (isKey)
				{
					key += (tchTemp);
				}
				else
				{
					value += (tchTemp);
				}
			}
			else
			{
				mapKeyValue.insert(make_pair(key, value));
				key.clear(); value.clear();
				isKey = true;
			}
			nIndex++;
		}
	}
	return retNum > 0;
}

std::string CFileIni::getFilePah()
{
	return iniFile_;
}

CIniBase::CIniBase(const std::string &filePath) :pIniInstance_(nullptr)
{
	pIniInstance_ = new CFileIni(filePath);
	assert(pIniInstance_);
}

CIniBase::~CIniBase()
{
	if (pIniInstance_)
	{
		delete pIniInstance_;
		pIniInstance_ = nullptr;
	}
}

std::string CIniBase::getFilePah()
{
	return pIniInstance_->getFilePah();
}

CConfigAgora::CConfigAgora() :CIniBase("")
{
	std::string path = getAbsoluteDir() + "AgoraHook.ini";
	pIniInstance_->openFile(path);
}

CConfigAgora::~CConfigAgora()
{

}

CConfigAgora::CConfigAgora(const std::string &path) :
CIniBase(path)
{

}

__IMPLEMENT_INICONFIG_FUN(CConfigAgora, AppId, INI_BaseInfo, INI_BaseInfo_APPID)
__IMPLEMENT_INICONFIG_FUN(CConfigAgora, AppCertEnable, INI_BaseInfo, INI_BaseInfo_AppCertEnable)
__IMPLEMENT_INICONFIG_FUN(CConfigAgora, AppCertificateId, INI_BaseInfo, INI_BaseInfo_APPCertificateID)
__IMPLEMENT_INICONFIG_FUN(CConfigAgora, ChannelName, INI_BaseInfo, INI_BaseInfo_ChannelName)
__IMPLEMENT_INICONFIG_FUN(CConfigAgora, RestartTimerStatus, INI_BaseInfo, INI_BaseInfo_RestartTimeStatus)
__IMPLEMENT_INICONFIG_FUN(CConfigAgora, RestartTimer, INI_BaseInfo, INI_BaseInfo_RestaTimer)
__IMPLEMENT_INICONFIG_FUN(CConfigAgora, VideoPreview, INI_BaseInfo, INI_BaseInfo_VideoPreview)
__IMPLEMENT_INICONFIG_FUN(CConfigAgora, ClearLogInterval, INI_BaseInfo, INI_BaseInfo_ClearLogInterval)
__IMPLEMENT_INICONFIG_FUN(CConfigAgora, LanguagePack, INI_BaseInfo, INI_BaseInfo_Language)
__IMPLEMENT_INICONFIG_FUN(CConfigAgora, ResolutionIndex, INI_BaseInfo, INI_BaseInfo_VideoProfileIndex)
__IMPLEMENT_INICONFIG_FUN(CConfigAgora, Resolution, INI_BaseInfo, INI_BaseInfo_VideoProfile)

__IMPLEMENT_INICONFIG_SIMILAR_FUN(CConfigAgora, ProcessEnable, INI_PROCESSID_Enable)
__IMPLEMENT_INICONFIG_SIMILAR_FUN(CConfigAgora, LoginUid, INI_LoginInfo_UID)
__IMPLEMENT_INICONFIG_SIMILAR_FUN(CConfigAgora, ChannelName, INI_LoginInfo_ChannelName)
__IMPLEMENT_INICONFIG_SIMILAR_FUN(CConfigAgora, AudioInName, INI_DeviceInfo_AudioInName)
__IMPLEMENT_INICONFIG_SIMILAR_FUN(CConfigAgora, AudioInComID, INI_DeviceInfo_AudioInCOMID)
__IMPLEMENT_INICONFIG_SIMILAR_FUN(CConfigAgora, PlayOutName, INI_DeviceInfo_PlayOutName)
__IMPLEMENT_INICONFIG_SIMILAR_FUN(CConfigAgora, PlayOutComID, INI_DeviceInfo_PlayOutCOMID)
__IMPLEMENT_INICONFIG_SIMILAR_FUN(CConfigAgora, CameraName, INI_DeviceInfo_CameraName)
__IMPLEMENT_INICONFIG_SIMILAR_FUN(CConfigAgora, CameraComID, INI_DeviceInfo_CameraCOMID)
__IMPLEMENT_INICONFIG_SIMILAR_FUN(CConfigAgora, DeviceState, INI_DeviceInfo_State)
__IMPLEMENT_INICONFIG_SIMILAR_FUN(CConfigAgora, DeviceChoose, INI_DeviceInfo_Choose)
__IMPLEMENT_INICONFIG_SIMILAR_FUN(CConfigAgora, LeftRotate90, INI_DeviceInfo_LeftRotate90)

__IMPLEMENT_INICONFIG_SIMILAR_FUN(CConfigAgora, ResolutionSave, INI_DeviceInfo_ResolutionSave)
__IMPLEMENT_INICONFIG_SIMILAR_FUN(CConfigAgora, ResolutionIndex, INI_DeviceInfo_ResolutionIndex)
__IMPLEMENT_INICONFIG_SIMILAR_FUN(CConfigAgora, ResolutionWidth, INI_DeviceInfo_ResolutionWidth)
__IMPLEMENT_INICONFIG_SIMILAR_FUN(CConfigAgora, ResolutionHeight, INI_DeviceInfo_ResolutionHeight)
__IMPLEMENT_INICONFIG_SIMILAR_FUN(CConfigAgora, SwitchWHEnable, INI_DeviceInfo_SwitchWHEnable)
__IMPLEMENT_INICONFIG_SIMILAR_FUN(CConfigAgora, ResolutionFps, INI_DeviceInfo_ResolutionFps)
__IMPLEMENT_INICONFIG_SIMILAR_FUN(CConfigAgora, ResolutionBitrate, INI_DeviceInfo_ResolutionBitrate)

__IMPLEMENT_INICONFIG_SIMILAR_FUN(CConfigAgora, RtmpSave, INI_DeviceInfo_RtmpSave)
__IMPLEMENT_INICONFIG_SIMILAR_FUN(CConfigAgora, RtmpUrl, INI_DeviceInfo_RtmpUrl)
__IMPLEMENT_INICONFIG_SIMILAR_FUN(CConfigAgora, RtmpWidth, INI_DeviceInfo_RtmpWidth)
__IMPLEMENT_INICONFIG_SIMILAR_FUN(CConfigAgora, RtmpHeight, INI_DeviceInfo_RtmpHeight)
__IMPLEMENT_INICONFIG_SIMILAR_FUN(CConfigAgora, RtmpFps, INI_DeviceInfo_RtmpFps)
__IMPLEMENT_INICONFIG_SIMILAR_FUN(CConfigAgora, RtmpBitrate, INI_DeviceInfo_RtmpBitrate)