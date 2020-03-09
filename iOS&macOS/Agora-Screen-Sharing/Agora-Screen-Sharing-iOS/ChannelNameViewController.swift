//
//  ChannelNameViewController.swift
//  Agora-Screen-Sharing-iOS
//
//  Created by suleyu on 2018/9/29.
//  Copyright © 2018 Agora. All rights reserved.
//

import UIKit

class ChannelNameViewController: UIViewController {
    
    @IBOutlet weak var channelNameTextField: UITextField!
    
    override func shouldPerformSegue(withIdentifier identifier: String, sender: Any?) -> Bool {
        if identifier == "channelNameToAudience" {
            if let string = channelNameTextField.text , !string.isEmpty {
                return true
            }
            else {
                return false
            }
        }
        
        return true
    }
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        if segue.identifier == "channelNameToAudience" {
            let audienceVC = segue.destination as! AudienceViewController
            audienceVC.channelName = channelNameTextField.text!
        }
    }
    
    @IBAction func backToChannelName(sender:UIStoryboardSegue) {
        self.navigationController?.popViewController(animated: true)
    }
}

extension ChannelNameViewController: UITextFieldDelegate {
    func textFieldShouldReturn(_ textField: UITextField) -> Bool {
        textField.resignFirstResponder()
        return true
    }
}
