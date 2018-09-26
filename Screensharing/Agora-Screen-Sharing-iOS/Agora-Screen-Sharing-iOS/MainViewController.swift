//
//  MainViewController.swift
//  Agora-Screen-Sharing-iOS
//
//  Created by GongYuhua on 2017/8/1.
//  Copyright © 2017年 Agora. All rights reserved.
//

import UIKit
import SpriteKit
import ReplayKit

class MainViewController: UIViewController {

    @IBOutlet weak var sceneView: SKView!
    @IBOutlet weak var broadcastButton: UIButton!
    
    fileprivate weak var broadcastActivityVC: RPBroadcastActivityViewController?
    fileprivate weak var broadcastController: RPBroadcastController?
    fileprivate weak var cameraPreview: UIView?
    
    private var isBroadcasting = false {
        didSet {
            broadcastButton?.setImage(isBroadcasting ? #imageLiteral(resourceName: "btn_broadcasting") : #imageLiteral(resourceName: "btn_join"), for: .normal)
        }
    }
    
    override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)
        
        // Used to trigger the allowing network dialog when first run, only for cellphones sold in China
        let url = URL(string: "https://www.agora.io")
        let session = URLSession(configuration: .default, delegate: nil, delegateQueue: nil)
        let dataTask = session.dataTask(with: url!)
        dataTask.resume()
    }
    
    override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(animated)
        
        let scene = GameScene(size: sceneView.bounds.size)
        scene.scaleMode = .resizeFill
        sceneView.presentScene(scene)
    }
    
    @IBAction func doBroadcastPressed(_ sender: UIButton) {
        isBroadcasting = !isBroadcasting
        
        if isBroadcasting {
            startReplayKitBroadcasting()
        } else {
            stopReplayKitBroadcasting()
        }
    }
}

private extension MainViewController {
    func startReplayKitBroadcasting() {
        guard RPScreenRecorder.shared().isAvailable else {
            return
        }
        
        RPScreenRecorder.shared().isCameraEnabled = true
        RPScreenRecorder.shared().isMicrophoneEnabled = true
        
        // Broadcast Pairing
        let bundleID = Bundle.main.bundleIdentifier!
        RPBroadcastActivityViewController.load(withPreferredExtension: bundleID + ".BroadcastUI") { (broadcastActivityViewController, _) in
            self.presentBroadcastActivityVC(broadcastActivityVC: broadcastActivityViewController)
        }
    }
    
    func presentBroadcastActivityVC(broadcastActivityVC: RPBroadcastActivityViewController?) {
        guard let broadcastActivityVC = broadcastActivityVC else {
            return
        }
        broadcastActivityVC.delegate = self
        if UIDevice.current.userInterfaceIdiom == .pad {
            broadcastActivityVC.modalPresentationStyle = .popover
            broadcastActivityVC.popoverPresentationController?.sourceView = broadcastButton
            broadcastActivityVC.popoverPresentationController?.sourceRect = broadcastButton.frame
            broadcastActivityVC.popoverPresentationController?.permittedArrowDirections = .down
        }
        present(broadcastActivityVC, animated: true, completion: nil)
        
        self.broadcastActivityVC = broadcastActivityVC
    }
    
    func stopReplayKitBroadcasting() {
        if let broadcastController = broadcastController {
            broadcastController.finishBroadcast(handler: { (error) in
                
            })
        }
        
        if let cameraPreview = cameraPreview {
            cameraPreview.removeFromSuperview()
        }
    }
}

extension MainViewController: RPBroadcastActivityViewControllerDelegate {
    func broadcastActivityViewController(_ broadcastActivityViewController: RPBroadcastActivityViewController, didFinishWith broadcastController: RPBroadcastController?, error: Error?) {
        DispatchQueue.main.async { [unowned self] in
            if let broadcastActivityVC = self.broadcastActivityVC {
                broadcastActivityVC.dismiss(animated: true, completion: nil)
            }
            
            self.broadcastController = broadcastController
            
            if let broadcastController = broadcastController {
                broadcastController.startBroadcast(handler: { (error) in
                    if let error = error {
                        print("startBroadcastWithHandler error: \(error.localizedDescription)")
                    } else {
                        DispatchQueue.main.async {
                            if let cameraPreview = RPScreenRecorder.shared().cameraPreviewView {
                                cameraPreview.frame = CGRect(x: 8, y: 28, width: 120, height: 180)
                                self.view.addSubview(cameraPreview)
                                self.cameraPreview = cameraPreview
                            }
                        }
                    }
                })
            }
        }
    }
}
