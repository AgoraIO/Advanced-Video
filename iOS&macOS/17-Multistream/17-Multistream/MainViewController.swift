//
//  MainViewController.swift
//  LargeGroupVideoChat
//
//  Created by ZhangJi on 30/09/2017.
//  Copyright © 2017 Agora. All rights reserved.
//

import UIKit
import AgoraRtcKit

class MainViewController: UIViewController {

    @IBOutlet weak var roomNameTextField: UITextField!
    @IBOutlet weak var popoverSourceView: UIView!
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        guard let segueId = segue.identifier else {
            return
        }
        
        switch segueId {
        case "mainToLive":
            let liveVC = segue.destination as! LiveRoomViewController
            let roomName = sender as! String
            liveVC.roomName = roomName
            liveVC.delegate = self
        default:
            break
        }
    }
    
    @IBAction func doJoinPressed(_ sender: UIButton) {
        guard let roomName = roomNameTextField.text, !roomName.isEmpty else {
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
    func liveVCNeedClose(_ liveVC: LiveRoomViewController) {
        let _ = navigationController?.popViewController(animated: true)
    }
}

extension MainViewController: UITextFieldDelegate {
    func textFieldShouldReturn(_ textField: UITextField) -> Bool {
        if let roomName = textField.text, !roomName.isEmpty {
            enterRoom(roomName)
        }
        
        return true
    }
}
