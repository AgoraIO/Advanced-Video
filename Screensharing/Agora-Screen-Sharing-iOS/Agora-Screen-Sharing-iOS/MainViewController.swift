//
//  MainViewController.swift
//  Agora-Screen-Sharing-iOS
//
//  Created by suleyu on 2018/9/29.
//  Copyright © 2018 Agora. All rights reserved.
//

import UIKit

class MainViewController: UIViewController {

    @IBOutlet weak var broadcasterButton: UIButton!
    @IBOutlet weak var audienceButton: UIButton!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        self.broadcasterButton.layer.cornerRadius = 4
        self.broadcasterButton.layer.borderWidth = 0.5
        self.broadcasterButton.layer.borderColor = #colorLiteral(red: 0.8039215803, green: 0.8039215803, blue: 0.8039215803, alpha: 1).cgColor
        
        self.audienceButton.layer.cornerRadius = 4
        self.audienceButton.layer.borderWidth = 0.5
        self.audienceButton.layer.borderColor = #colorLiteral(red: 0.8039215803, green: 0.8039215803, blue: 0.8039215803, alpha: 1).cgColor
    }
    
    @IBAction func backToMain(sender:UIStoryboardSegue) {
        self.navigationController?.popViewController(animated: true)
    }
}
