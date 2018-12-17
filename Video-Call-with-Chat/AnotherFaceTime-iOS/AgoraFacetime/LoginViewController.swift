//
//  LoginViewController.swift
//  AgoraFacetime
//
//  Created by Qin Cindy on 9/2/16.
//  Copyright © 2016 Qin Cindy. All rights reserved.
//

import UIKit

class LoginViewController: UIViewController {

    @IBOutlet weak var loginButton: UIButton!
    @IBOutlet weak var phoneNumberTextField: UITextField!
    
    var indicator: UIActivityIndicatorView?
    
    @IBAction func pressLoginButton(_ sender: UIButton) {
        if let phoneNumber = phoneNumberTextField.text {
            Agora.sharedInstance.updateDelegate = { [weak self] status in
                self?.indicator?.stopAnimating()
                if status == .login {
                    self?.performSegue(withIdentifier: "showCallView", sender: phoneNumber)
                }
            }
            Agora.sharedInstance.login(phoneNumber: phoneNumber)
            showLoadingView()
        } else {
            alert(string: "Please input your phone number")
        }
    }
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        guard let segueId = segue.identifier, !segueId.isEmpty else {
            return
        }
        
        switch segueId {
        case "showCallView":
            let callVC = segue.destination as! CallViewController
            callVC.myPhoneNumber = sender as! String
        default:
            break
        }
    }
}

fileprivate extension LoginViewController {
    func showLoadingView() {
        let indicator = UIActivityIndicatorView(style: .gray)
        indicator.center = view.center
        view.addSubview(indicator)
        indicator.startAnimating()
        
        self.indicator = indicator
    }
    
    func alert(string: String) {
        let alert = UIAlertController(title: nil, message: string, preferredStyle: .alert)
        alert.addAction(UIAlertAction(title: "Ok", style: .default, handler: nil))
        present(alert, animated: true, completion: nil)
    }
}

extension LoginViewController: UITextFieldDelegate {
    func textFieldShouldReturn(_ textField: UITextField) -> Bool {
        textField.resignFirstResponder()
        return true
    }
    
    override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
        self.view.endEditing(true)
        super.touchesBegan(touches, with: event)
    }
}
