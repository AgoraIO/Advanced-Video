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
    
    @IBAction func backToMain(sender: UIStoryboardSegue) {
        self.navigationController?.popViewController(animated: true)
    }
}
