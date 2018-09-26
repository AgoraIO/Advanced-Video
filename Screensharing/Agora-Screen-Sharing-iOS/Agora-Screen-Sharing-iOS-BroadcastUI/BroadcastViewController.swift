//
//  BroadcastViewController.swift
//  Agora-Screen-Sharing-iOS-BroadcastUI
//
//  Created by GongYuhua on 2017/8/1.
//  Copyright © 2017年 Agora. All rights reserved.
//

import ReplayKit

class BroadcastViewController: UIViewController {
    
    @IBOutlet weak var channelNameTextField: UITextField!
    
    @IBAction func doCancelPressed(_ sender: UIButton) {
        userDidCancelSetup()
    }
    
    @IBAction func doStartPressed(_ sender: UIButton) {
        userStartBroadcast(withChannel: channelNameTextField.text)
    }
}

private extension BroadcastViewController {
    func userStartBroadcast(withChannel channel: String?) {
        guard let channel = channel, !channel.isEmpty else {
            return
        }
        
        let setupInfo: [String: NSCoding & NSObjectProtocol] =  [ "channelName" : channel as NSString]
        extensionContext?.completeRequest(withBroadcast: URL(string: "http://vid-130451.hls.fastweb.broadcastapp.agoraio.cn/live/\(channel)/index.m3u8")!, setupInfo: setupInfo)
    }
    
    func userDidCancelSetup() {
        let error = NSError(domain: "io.agora.Agora-Screen-Sharing-iOS.Agora-Screen-Sharing-iOS-BroadcastUI", code: -1, userInfo: nil)
        extensionContext?.cancelRequest(withError: error)
    }
}

extension BroadcastViewController: UITextFieldDelegate {
    func textFieldShouldReturn(_ textField: UITextField) -> Bool {
        userStartBroadcast(withChannel: textField.text)
        return true
    }
}
