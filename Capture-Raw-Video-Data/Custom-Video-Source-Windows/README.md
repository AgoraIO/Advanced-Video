# Agora Custom-Video-Source-Windows


*其他语言版本： [简体中文](README.zh.md)*

*  copy sdk folder to the directroy where Opnelive.sln file is located in.
*  currently the external video format supportted(2.9.1 and above):
    * ARGB
    * yuv420
    * yuv422
* copy the dll files in sdk/dll directory to execution dirctory
* unzip the file 360p_i422.zip,then copy the file 360p_i422.yuv in the execution dirctory. This is yuv422 format and the resolution is 640*360, the framterate is 15.
* If you want push other video format file, you can modify the param in the function CVideoDlg::pushVideoFrame().
  Then copy your raw video file to the exectution path.


## Running the App
First, create a developer account at [Agora.io](https://dashboard.agora.io/signin/), and obtain an App ID. define the APP_ID with your App ID.

```
#define APP_ID _T("Your App ID")
```

## Developer Environment Requirements
* VC2013 or higher
## License

The MIT License (MIT).
