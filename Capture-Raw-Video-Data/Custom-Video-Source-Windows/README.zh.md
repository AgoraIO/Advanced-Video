# Agora Custom-Video-Source-Windows

*Read this in other languages: [English](README.md)*

*  把官网的sdk放到sln相同目录下
*  目前支持的外部视频格式(2.9.0及以下):
    * yuv420
  
* sdk/dll 文件下的 dll 库需要放在相应的执行路径下。
* 解压360p_I420.zip，把解压的yuv文件拷贝到执行目录下。yuv420格式的数据，分辨率为640*360，15fps。
* 如果想要推其他格式或者分辨率的裸数据，在函数CVideoDlg::pushVideoFrame()CVideoDlg::pushVideoFrame()中修改参数。然后把你要推送的视频裸数据文件放到运行目录下

## 运行示例程序
首先在 [Agora.io 注册](https://dashboard.agora.io/cn/signup/) 注册账号，并创建自己的测试项目，获取到 AppID。将 AppID 内容替换至 APP_ID 宏定义中


```
#define APP_ID _T("Your App ID")
```

## 运行环境
* VC++2013 或更高版本

## 代码许可

The MIT License (MIT).
