//
//  ViewController.swift
//  AgoraRTCWithCallKit
//
//  Created by GongYuhua on 2018/1/19.
//  Copyright © 2018年 Agora. All rights reserved.
//

import UIKit
import AgoraRtcEngineKit

class ViewController: UIViewController {

    @IBOutlet weak var phoneNumberTextField: UITextField!
    
    @IBOutlet weak var muteAudioButton: UIButton!
    @IBOutlet weak var outgoingCallButton: UIButton!
    @IBOutlet weak var incomingCallButton: UIButton!
    @IBOutlet weak var endCallButton: UIButton!
    @IBOutlet weak var speakerButton: UIButton!
    
    @IBOutlet weak var remoteVideoView: UIView!
    @IBOutlet weak var localVideoView: UIView!
    
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
            localVideoView.isHidden = !isCallActive
        }
    }
    
    private lazy var callCenter = CallCenter(delegate: self)
    private lazy var rtcEngine: AgoraRtcEngineKit = AgoraRtcEngineKit.sharedEngine(withAppId: <#Your AppId#>, delegate: self)
    fileprivate var remoteUid: UInt = 0
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        callCenter.delegate = self
        
        rtcEngine.setChannelProfile(.communication)
        
        let canvas = AgoraRtcVideoCanvas()
        canvas.uid = 0
        canvas.view = localVideoView
        canvas.renderMode = .hidden
        rtcEngine.setupLocalVideo(canvas)
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
    func startSession(_ session: String) {
        isCallActive = true
        
        try? AVAudioSession.sharedInstance().setCategory(AVAudioSession.Category.playAndRecord,
                                                         mode: AVAudioSession.Mode.voiceChat,
                                                         options: [.mixWithOthers, .allowBluetooth])
        
        rtcEngine.setAudioSessionOperationRestriction(.all)
        rtcEngine.setParameters("{\"che.audio.use.callkit\":true}")
        
        rtcEngine.startPreview()
        rtcEngine.joinChannel(byToken: nil, channelId: session, info: nil, uid: 0, joinSuccess: nil)
    }
    
    func enableMedia(_ enable: Bool) {
        if enable {
            rtcEngine.enableAudio()
            rtcEngine.enableVideo()
        } else {
            rtcEngine.disableAudio()
            rtcEngine.disableVideo()
        }
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
        remoteVideoView.isHidden = true
        
        rtcEngine.leaveChannel(nil)
        rtcEngine.stopPreview()
    }
}

extension ViewController: CallCenterDelegate {
    func callCenter(_ callCenter: CallCenter, startCall session: String) {
        enableMedia(false)
        startSession(session)
    }
    
    func callCenter(_ callCenter: CallCenter, answerCall session: String) {
        enableMedia(true)
        startSession(session)
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
        enableMedia(true)
    }
}

extension ViewController: AgoraRtcEngineDelegate {
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        let canvas = AgoraRtcVideoCanvas()
        canvas.uid = uid
        canvas.view = remoteVideoView
        canvas.renderMode = .hidden
        engine.setupRemoteVideo(canvas)
        
        remoteUid = uid
        remoteVideoView.isHidden = false
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        if uid == remoteUid {
            remoteVideoView.isHidden = true
        }
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didAudioRouteChanged routing: AgoraAudioOutputRouting) {
        print("didAudioRouteChanged : \(routing.rawValue)")
        if audioOutputRouting == nil && routing == .earpiece {
            rtcEngine.setEnableSpeakerphone(true)
        }
        else {
            audioOutputRouting = routing;
        }
    }
}

extension ViewController: UITextFieldDelegate {
    func textFieldShouldReturn(_ textField: UITextField) -> Bool {
        textField.resignFirstResponder()
        return true
    }
}
