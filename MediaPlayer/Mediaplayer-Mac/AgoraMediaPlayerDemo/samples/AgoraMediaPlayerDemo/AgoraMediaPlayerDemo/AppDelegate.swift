//
//  AppDelegate.swift
//  OpenLive
//
//  Created by GongYuhua on 16/9/2.
//  Copyright © 2016年 Agora. All rights reserved.
//

import Cocoa

@NSApplicationMain
class AppDelegate: NSObject, NSApplicationDelegate {
    
    func applicationShouldTerminateAfterLastWindowClosed(_ sender: NSApplication) -> Bool {
        return true
    }
}

