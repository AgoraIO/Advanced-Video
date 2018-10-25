//
//  MainViewController.swift
//  LargeGroupVideoChat
//
//  Created by ZhangJi on 30/09/2017.
//  Copyright © 2017 Agora. All rights reserved.
//

import Cocoa
import AgoraRtcEngineKit

class MainViewController: NSViewController {
    
    @IBOutlet weak var roomInputTextField: NSTextField!
    
    var videoProfile = AgoraVideoProfile.landscape360P
    fileprivate var agoraKit: AgoraRtcEngineKit!
    
    override func viewDidAppear() {
        super.viewDidAppear()
        roomInputTextField.becomeFirstResponder()
    }
    
    override func prepare(for segue: NSStoryboardSegue, sender: Any?) {
        guard let segueId = segue.identifier else {
            return
        }
        
        switch segueId {
        case "mainToLive":
            let liveVC = segue.destinationController as! LiveRoomViewController
            let roomName = sender as! String
            liveVC.roomName = roomName
            liveVC.delegate = self
        default:
            break
        }
    }
    
    //MARK: - user actions
    @IBAction func doJoinPressed(_ sender: NSButton) {
        guard let roomName = roomInputTextField?.stringValue, !roomName.isEmpty else {
            return
        }
        enterRoom(roomName)
    }
}

private extension MainViewController {
    func enterRoom(_ roomName: String) {
        performSegue(withIdentifier: "mainToLive", sender: roomName)
    }
}

extension MainViewController: LiveRoomVCDelegate {
    func liveRoomVCNeedClose(_ liveVC: LiveRoomViewController) {
        guard let window = liveVC.view.window else {
            return
        }
        
        window.delegate = nil
        window.contentViewController = self
    }
}
