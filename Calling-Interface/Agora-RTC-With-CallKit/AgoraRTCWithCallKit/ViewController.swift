//
//  ViewController.swift
//  AgoraRTCWithCallKit
//
//  Created by GongYuhua on 2018/1/19.
//  Copyright © 2018年 Agora. All rights reserved.
//

import UIKit
import AgoraRtcKit

class ViewController: UIViewController {

    @IBOutlet weak var phoneNumberTextField: UITextField!
    
    @IBOutlet weak var muteAudioButton: UIButton!
    @IBOutlet weak var outgoingCallButton: UIButton!
    @IBOutlet weak var incomingCallButton: UIButton!
    @IBOutlet weak var endCallButton: UIButton!
    @IBOutlet weak var speakerButton: UIButton!
    
    private var session: String {
        get {
            return phoneNumberTextField?.text ?? ""
        }
    }
    
    private var isAudioMuted = false {
        didSet {
            muteAudioButton?.setImage(isAudioMuted ? #imageLiteral(resourceName: "btn_mute_blue") : #imageLiteral(resourceName: "btn_mute"), for: .normal)
        }
    }
    
    private var audioOutputRouting: AgoraAudioOutputRouting? {
        didSet {
            speakerButton?.setImage(isSpeakerEnabled ? #imageLiteral(resourceName: "btn_speaker_blue") : #imageLiteral(resourceName: "btn_speaker"), for: .normal)
            speakerButton?.setImage(isSpeakerEnabled ? #imageLiteral(resourceName: "btn_speaker") : #imageLiteral(resourceName: "btn_speaker_blue"), for: .highlighted)
        }
    }
    
    private var isSpeakerEnabled: Bool {
        return audioOutputRouting == .speakerphone
    }
    
    private var isCallActive = false {
        didSet {
            isAudioMuted = false
            phoneNumberTextField.isEnabled = !isCallActive
            muteAudioButton.isHidden = !isCallActive
            outgoingCallButton.isHidden = isCallActive
            incomingCallButton.isHidden = isCallActive
            endCallButton.isHidden = !isCallActive
            speakerButton.isHidden = !isCallActive
        }
    }
    
    private lazy var callCenter = CallCenter(delegate: self)
    private lazy var rtcEngine: AgoraRtcEngineKit = AgoraRtcEngineKit.sharedEngine(withAppId: <#Your AppId#>, delegate: self)
    fileprivate var remoteUid: UInt = 0
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        try? AVAudioSession.sharedInstance().setCategory(.playAndRecord, mode: .voiceChat, options: [.allowBluetooth])
        
        callCenter.delegate = self
        
        rtcEngine.setChannelProfile(.liveBroadcasting)
        rtcEngine.setClientRole(.broadcaster)
    }
    
    @IBAction func doMutePressed(_ sender: UIButton) {
        callCenter.muteAudio(of: session, muted: !isAudioMuted)
    }
    
    @IBAction func doCallOutPressed(_ sender: UIButton) {
        callCenter.startOutgoingCall(of: session)
    }
    
    @IBAction func doCallInPressed(_ sender: UIButton) {
        callCenter.showIncomingCall(of: session)
    }
    
    @IBAction func doEndCallPressed(_ sender: UIButton) {
        callCenter.endCall(of: session)
    }
    
    @IBAction func doSpeakerPressed(_ sender: UIButton) {
        setEnableSpeakerphone(!isSpeakerEnabled)
    }
}

extension ViewController {
    func applyContinueUserActivity(toCall phoneNumber: String?) {
        guard let phoneNumber = phoneNumber, !phoneNumber.isEmpty, !isCallActive else {
            return
        }
        phoneNumberTextField.text = phoneNumber
        callCenter.startOutgoingCall(of: session)
    }
}

private extension ViewController {
    func joinSession() {
        rtcEngine.setAudioSessionOperationRestriction(.all)
        rtcEngine.joinChannel(byToken: nil, channelId: session, info: nil, uid: 0, joinSuccess: nil)
    }
    
    func muteAudio(_ mute: Bool) {
        isAudioMuted = mute
        rtcEngine.muteLocalAudioStream(mute)
    }
    
    func setEnableSpeakerphone(_ enable: Bool) {
        rtcEngine.setEnableSpeakerphone(enable)
    }
    
    func stopSession() {
        isCallActive = false
        audioOutputRouting = nil
        rtcEngine.leaveChannel(nil)
    }
}

extension ViewController: CallCenterDelegate {
    func callCenter(_ callCenter: CallCenter, startCall session: String) {
        isCallActive = true
    }
    
    func callCenter(_ callCenter: CallCenter, answerCall session: String) {
        isCallActive = true
        callCenter.setCallConnected(of: session)
    }
    
    func callCenter(_ callCenter: CallCenter, declineCall session: String) {
        print("call declined")
    }
    
    func callCenter(_ callCenter: CallCenter, muteCall muted: Bool, session: String) {
        muteAudio(muted)
    }
    
    func callCenter(_ callCenter: CallCenter, endCall session: String) {
        stopSession()
    }
    
    func callCenterDidActiveAudioSession(_ callCenter: CallCenter) {
        print("Audiosession did active")
        joinSession()
    }
}

extension ViewController: AgoraRtcEngineDelegate {
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        print("remote user \(uid) did join room")
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        print("remote user \(uid) did left")
    }
}

extension ViewController: UITextFieldDelegate {
    func textFieldShouldReturn(_ textField: UITextField) -> Bool {
        textField.resignFirstResponder()
        return true
    }
}
