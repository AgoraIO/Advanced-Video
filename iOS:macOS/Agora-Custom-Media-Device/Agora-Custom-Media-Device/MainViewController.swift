//
//  MainViewController.swift
//  Agora-Custom-Media-Device
//
//  Created by GongYuhua on 2017/12/19.
//  Copyright © 2017年 Agora.io All rights reserved.
//

import UIKit
import AgoraRtcKit

class MainViewController: UIViewController {

    @IBOutlet weak var roomNameTextField: UITextField!
    @IBOutlet weak var popoverSourceView: UIView!
    
    fileprivate var videoProfile = AgoraVideoProfile.portrait360P
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        guard let segueId = segue.identifier else {
            return
        }
        
        switch segueId {
        case "mainToLive":
            let liveVC = segue.destination as! LiveRoomViewController
            liveVC.roomName = roomNameTextField.text!
            liveVC.videoProfile = videoProfile
            liveVC.delegate = self
        default:
            break
        }
    }
}

private extension MainViewController {
    func joinChannel() {
        performSegue(withIdentifier: "mainToLive", sender: nil)
    }
}

extension MainViewController: LiveRoomVCDelegate {
    func liveVCNeedClose(_ liveVC: LiveRoomViewController) {
        let _ = navigationController?.popViewController(animated: true)
    }
}

extension MainViewController: UITextFieldDelegate {
    func textFieldShouldReturn(_ textField: UITextField) -> Bool {
        if let string = textField.text , !string.isEmpty {
            joinChannel()
        }
        
        return true
    }
}
