//
//  CallViewController.swift
//  AgoraFacetime
//
//  Created by Qin Cindy on 9/2/16.
//  Copyright © 2016 Qin Cindy. All rights reserved.
//

import UIKit
import AVFoundation

class CallViewController: UIViewController {
    
    //Outlets
    @IBOutlet weak var callButton: UIButton!
    @IBOutlet weak var endCallButton: UIButton!
    
    @IBOutlet weak var callTextField: UITextField!
    @IBOutlet weak var callAlignCenter: NSLayoutConstraint!
    @IBOutlet weak var endcallAlignCenter: NSLayoutConstraint!
    @IBOutlet weak var smallVideoView: UIView!
    @IBOutlet weak var videoView: UIView!
    
    @IBOutlet weak var myNumberLabel: UILabel!
    @IBOutlet weak var informationLabel: UILabel!
    
    @IBOutlet weak var cameraSwitchButton: UIButton!
    @IBOutlet weak var videoVolumeButton: UIButton!
    
    var myPhoneNumber = ""
    var callToNumber: String = ""
    var isSmallViewPeer = true
    
    var audioPlayer: AVAudioPlayer?
    var isAudioStopBeforeJoin = true
    var isAudioStopAfterJoin = true
    
    var isVideo = true
    var isReceiver = false
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        myNumberLabel.text = "Your number: " + myPhoneNumber
        updateUI(status: Agora.sharedInstance.status)
        
        Agora.sharedInstance.updateDelegate = { status in
            self.updateUI(status: status)
        }
        Agora.sharedInstance.updateVideoEnable = { status in
            self.videoAudioSwitch(enable: status)
        }
        initMusicPlayerBeforeJoin()
    }
    
    @IBAction func pressCallButton(_ sender: UIButton) {
        if Agora.sharedInstance.status == .receiveCalling {
            isReceiver = true
            Agora.sharedInstance.acceptCall(callToNumber: myPhoneNumber)
        } else {
            guard let callToNumber = callTextField.text, !callToNumber.isEmpty else {
                alert(string: "Wrong number!")
                return
            }
            
            self.callToNumber = callToNumber
            isReceiver = false
            Agora.sharedInstance.callTo(phoneName: callToNumber)
        }
    }

    @IBAction func pressEndCall(_ sender: UIButton) {
        if Agora.sharedInstance.status == .receiveCalling {
            stopPlayBeforeJoin()
            Agora.sharedInstance.refuseCall(callToNumber: myPhoneNumber)
        } else {
            stopPlayAfterJoin()
            if isReceiver {
                Agora.sharedInstance.endCall(peerPhoneNumber: Agora.sharedInstance.callFrom)
            } else {
                Agora.sharedInstance.endCall(peerPhoneNumber: Agora.sharedInstance.callTo)
            }
        }
    }
    
    @IBAction func pressCameraSwitchButton(_ sender: UIButton) {
        Agora.sharedInstance.switchCamera()
    }
    
    @IBAction func pressVideoAudioSwitchButton(_ sender: UIButton) {
        videoAudioSwitch(enable: !isVideo)
    }
    
    override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
        view.endEditing(true)
        super.touchesBegan(touches, with: event)
    }
}

fileprivate extension CallViewController {
    func updateUI(status: AgoraStatusMachine) {
        switch (status) {
        case .login:
            callTextField.isHidden = false
            showCallHideStopcall()
            informationLabel.text = ""
            isSmallViewPeer = true
            updateVideoView()
        case .refuseCall:
            callTextField.isHidden = false
            showCallHideStopcall()
            informationLabel.text = "Call rejected"
            isSmallViewPeer = true
            updateVideoView()
            stopPlayAfterJoin()
        case .endCall:
            callTextField.isHidden = false
            showCallHideStopcall()
            informationLabel.text = "Call ended"
            isSmallViewPeer = true
            updateVideoView()
            stopPlayBeforeJoin()
        case .calling:
            callTextField.isHidden = true
            showStopcallHideCall()
            informationLabel.text = "Calling" + callToNumber
            isSmallViewPeer = true
            updateVideoView()
            startPlayAfterJoin()
        case .inCall:
            informationLabel.text = "In call"
            callTextField.isHidden = true
            showStopcallHideCall()
            isSmallViewPeer = false
            updateVideoView()
            stopPlayBeforeJoin()
            stopPlayAfterJoin()
        case .receiveCalling:
            callTextField.isHidden = true
            informationLabel.text = "Received calling:" + Agora.sharedInstance.callFrom
            showCallShowStopCall()
            isSmallViewPeer = true
            updateVideoView()
            startPlayBeforeJoin()
        default:
            break;
        }
    }
    
    func showCallShowStopCall() {
        callButton.isEnabled = true
        endCallButton.isEnabled = true
        
        if callAlignCenter.constant == 0 {
            callAlignCenter.constant = self.view.frame.width/4
        }
        
        if endcallAlignCenter.constant == 0 {
            endcallAlignCenter.constant = -self.view.frame.width/4
        }
        
        UIView.animate(withDuration: 0.3, animations: {
            self.view.setNeedsLayout()
            self.callButton.alpha = 1.0
            self.endCallButton.alpha = 1.0
        })
    }
    
    func showCallHideStopcall() {
        
        callButton.isEnabled = true
        endCallButton.isEnabled = false
        
        if callAlignCenter.constant != 0 {
            callAlignCenter.constant = 0
        }
        
        if endcallAlignCenter.constant != 0 {
            endcallAlignCenter.constant = 0
        }
        
        UIView.animate(withDuration: 0.3, animations: {
            self.view.setNeedsLayout()
            self.callButton.alpha = 1.0
            self.endCallButton.alpha = 0.0
        })
    }
    
    func showStopcallHideCall() {
        callButton.isEnabled = false
        endCallButton.isEnabled = true
        
        if callAlignCenter.constant != 0 {
            callAlignCenter.constant = 0
        }
        
        if endcallAlignCenter.constant != 0 {
            endcallAlignCenter.constant = 0
        }
        
        UIView.animate(withDuration: 0.3, animations: {
            self.view.setNeedsLayout()
            self.callButton.alpha = 0.0
            self.endCallButton.alpha = 1.0
        })
    }
    
    func updateVideoView() {
        // if small view is peer and hide, main view is local
        if isSmallViewPeer {
            if !smallVideoView.isHidden {
                Agora.sharedInstance.setUpVideo(videoView: videoView)
                smallVideoView.isHidden = true
            }
            
        } else {
            if smallVideoView.isHidden {
                smallVideoView.isHidden = false
                Agora.sharedInstance.setUpVideo(videoView: smallVideoView)
                Agora.sharedInstance.peerViewHolder = videoView
            }
            
        }
    }
    
    func videoAudioSwitch(enable: Bool) {
        guard isVideo != enable else {
            return
        }
        
        isVideo = !isVideo
        
        if enable {
            self.videoVolumeButton.setTitle("Audio", for: .normal)
            Agora.sharedInstance.enableVideoMode()
        } else {
            self.videoVolumeButton.setTitle("Video", for: .normal)
            Agora.sharedInstance.disableVideoMode()
        }
        
        self.videoView.isHidden = !enable
        self.smallVideoView.isHidden = !enable
        self.cameraSwitchButton.isHidden = !enable
    }
    
    func alert(string: String) {
        let alert = UIAlertController(title: nil, message: string, preferredStyle: .alert)
        alert.addAction(UIAlertAction(title: "Ok", style: .default, handler: nil))
        present(alert, animated: true, completion: nil)
    }
}

extension CallViewController: UITextFieldDelegate {
    func textFieldDidEndEditing(_ textField: UITextField) {
        if let number = textField.text {
            callToNumber = number
        }
    }
    
    func textFieldShouldReturn(_ textField: UITextField) -> Bool {
        textField.resignFirstResponder()
        return true
    }
}

// MusicPlayer
extension CallViewController: AVAudioPlayerDelegate {
    func startPlayAfterJoin() {
        guard isAudioStopAfterJoin else {
            return
        }
        
        isAudioStopAfterJoin = false
        let file = Bundle.main.path(forResource: "videoCall", ofType: "m4a")!
        Agora.sharedInstance.startPlay(file: file)
    }
    
    func stopPlayAfterJoin() {
        guard !isAudioStopAfterJoin else {
            return
        }
        
        isAudioStopAfterJoin = true
        Agora.sharedInstance.stopPlay()
    }
    
    func initMusicPlayerBeforeJoin() {
        guard let path = Bundle.main.path(forResource: "videoCall", ofType: "m4a") else {
            return
        }
        
        let alertSound = URL(fileURLWithPath: path)
        
        do {
            try AVAudioSession.sharedInstance().setCategory(.playback, mode: AVAudioSession.Mode.videoChat, options: [.defaultToSpeaker, .allowBluetooth])
            try AVAudioSession.sharedInstance().setActive(true)
            audioPlayer =  try AVAudioPlayer(contentsOf: alertSound)
        } catch let error as NSError {
            print("\(error) \(error.userInfo)")
        }
        
        audioPlayer?.delegate = self
    }
    
    func startPlayBeforeJoin() {
        guard isAudioStopBeforeJoin else {
            return
        }
        
        isAudioStopBeforeJoin = false
        audioPlayer?.numberOfLoops = -1
        audioPlayer?.prepareToPlay()
        audioPlayer?.play()
    }
    
    func stopPlayBeforeJoin() {
        guard !isAudioStopBeforeJoin else {
            return
        }
        
        isAudioStopBeforeJoin = true
        audioPlayer?.stop()
    }
}
