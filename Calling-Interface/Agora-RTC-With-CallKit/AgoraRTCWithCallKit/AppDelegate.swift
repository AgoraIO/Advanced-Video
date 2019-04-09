//
//  AppDelegate.swift
//  AgoraRTCWithCallKit
//
//  Created by GongYuhua on 2018/1/18.
//  Copyright © 2018年 Agora. All rights reserved.
//

import UIKit
import Intents

@UIApplicationMain
class AppDelegate: UIResponder, UIApplicationDelegate {

    var window: UIWindow?
    
    func application(_ application: UIApplication, didFinishLaunchingWithOptions launchOptions: [UIApplication.LaunchOptionsKey: Any]?) -> Bool {
                
        return true
    }
    
    func application(_ application: UIApplication, continue userActivity: NSUserActivity, restorationHandler: @escaping ([UIUserActivityRestoring]?) -> Void) -> Bool {
        guard let interaction = userActivity.interaction else {
            return false
        }
        
        var phoneNumber: String?
        if let callIntent = interaction.intent as? INStartVideoCallIntent {
            phoneNumber = callIntent.contacts?.first?.personHandle?.value
        } else if let callIntent = interaction.intent as? INStartAudioCallIntent {
            phoneNumber = callIntent.contacts?.first?.personHandle?.value
        }
        
        let callVC = window?.rootViewController as? ViewController
        callVC?.applyContinueUserActivity(toCall:phoneNumber)
        
        return true
    }
}
