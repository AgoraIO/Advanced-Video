#!/usr/bin/python
# -*- coding: UTF-8 -*-
import re
import os

def main():
    SDK_URL = ""
    if "SDK_URL" in os.environ:
        SDK_URL = os.environ["SDK_URL"]
    
    TARGET_LIBS_ZIP = "agora_sdk.zip"
    TARGET_INTERNAL_FOLDER = "agora_sdk"
    ZIP_STRUCTURE_FOLDER = "Agora_Native_SDK_for_Mac_VOICE/libs"
    FRAMEWORK_NAME = "AgoraRtcKit.framework"
    LIBS_FOLDER = "libs/macOS"
    
    rm = "rm -r " + TARGET_LIBS_ZIP
    os.system(rm)
    
    rm = "rm -r " + TARGET_INTERNAL_FOLDER
    os.system(rm)
    
    rm = "rm -r " + LIBS_FOLDER + "/" + FRAMEWORK_NAME
    os.system(rm)
    
    wget = "wget -q " + SDK_URL + " -O " + TARGET_LIBS_ZIP
    os.system(wget)
    
    unzip = "unzip -q " + TARGET_LIBS_ZIP + " -d " + TARGET_INTERNAL_FOLDER
    os.system(unzip)
    
    mv = "mv -f " + TARGET_INTERNAL_FOLDER + "/" + ZIP_STRUCTURE_FOLDER + "/" + FRAMEWORK_NAME + " \"" + LIBS_FOLDER +"\""
    os.system(mv)

    appId = ""
    if "AGORA_APP_ID" in os.environ:
        appId = os.environ["AGORA_APP_ID"]
    token = ""

    # KeyCenter.swift
    f = open("./shared/Swift/KeyCenter.swift", 'r+')
    content = f.read()
    appString = "\"" + appId + "\""
    tokenString = "\"" + token + "\""
    contentNew = re.sub(r'<#Your App Id#>', appString, content)
    contentNew = re.sub(r'<#Temp Access Token#>', tokenString, contentNew)
    f.seek(0)
    f.write(contentNew)
    f.truncate()
    
    # KeyCenter.m
    f = open("./shared/Objective-C/KeyCenter.m", 'r+')
    content = f.read()
    appString = "@\"" + appId + "\""
    tokenString = "@\"" + token + "\""
    contentNew = re.sub(r'<#Your App Id#>', appString, content)
    contentNew = re.sub(r'<#Temp Access Token#>', tokenString, contentNew)
    f.seek(0)
    f.write(contentNew)
    f.truncate()

if __name__ == "__main__":
    main()
