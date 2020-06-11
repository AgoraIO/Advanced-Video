//
//  ViewController.swift
//  AgoraRTCWithCallKit
//
//  Created by GongYuhua on 2018/1/19.
//  Copyright © 2018年 Agora. All rights reserved.
//

import UIKit
import MediaPlayer
import AgoraRtcKit

class ViewController: UIViewController {

    @IBOutlet weak var phoneNumberTextField: UITextField!
    
    @IBOutlet weak var muteAudioButton: UIButton!
    @IBOutlet weak var outgoingCallButton: UIButton!
    @IBOutlet weak var incomingCallButton: UIButton!
    @IBOutlet weak var endCallButton: UIButton!
    @IBOutlet weak var volumeView: MPVolumeView!
    
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
            updateVolumeView(withSpeakerOn: isSpeakerOn)
        }
    }
    
    private var isSpeakerOn: Bool {
        return audioOutputRouting == .speakerphone
            || audioOutputRouting == .loudspeaker
            || audioOutputRouting == .headsetBluetooth
    }
    
    private var isCallActive = false {
        didSet {
            isAudioMuted = false
            phoneNumberTextField.isEnabled = !isCallActive
            muteAudioButton.isHidden = !isCallActive
            outgoingCallButton.isHidden = isCallActive
            incomingCallButton.isHidden = isCallActive
            endCallButton.isHidden = !isCallActive
            volumeView.isHidden = !isCallActive
        }
    }
    
    private lazy var callCenter = CallCenter(delegate: self)
    private lazy var rtcEngine: AgoraRtcEngineKit = {
        let engine = AgoraRtcEngineKit.sharedEngine(withAppId: <#Your AppId#>, delegate: self)
        engine.setChannelProfile(.liveBroadcasting)
        engine.setClientRole(.broadcaster)
        return engine
    }()
        
    override func viewDidLoad() {
        super.viewDidLoad()
        
        volumeView.showsRouteButton = true
        volumeView.showsVolumeSlider = false
        volumeView.setRouteButtonImage(#imageLiteral(resourceName: "btn_speaker"), for: .normal)
        
        try? AVAudioSession.sharedInstance().setCategory(.playAndRecord, mode: .voiceChat, options: [.allowBluetooth, .allowBluetoothA2DP])
    }
    
    func updateVolumeView(withSpeakerOn isSpeakerOn: Bool) {
        volumeView.setRouteButtonImage(isSpeakerOn ? #imageLiteral(resourceName: "btn_speaker_blue") : #imageLiteral(resourceName: "btn_speaker"), for: .normal)
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
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didAudioRouteChanged routing: AgoraAudioOutputRouting) {
        audioOutputRouting = routing
    }
}

extension ViewController: UITextFieldDelegate {
    func textFieldShouldReturn(_ textField: UITextField) -> Bool {
        textField.resignFirstResponder()
        return true
    }
}
