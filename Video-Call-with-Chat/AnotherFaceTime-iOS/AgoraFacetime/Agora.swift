//
//  Agora.swift
//  AgoraFacetime
//
//  Created by Qin Cindy on 9/2/16.
//  Copyright © 2016 Qin Cindy. All rights reserved.
//

import Foundation
import AgoraSigKit
import AgoraRtcEngineKit

enum AgoraStatusMachine: Int {
    case logout
    case login
    case calling
    case inCall
    case receiveCalling
    case refuseCall
    case endCall
    case callFail
}

class Agora: NSObject {
    /// shared instance of Configuration (singleton)
    static let sharedInstance = Agora()
    
    var agoraInstance: AgoraAPI!
    var rtcEngineInstance: AgoraRtcEngineKit!
    var uid: UInt32 = 0
    var remoteUid: UInt = 0
    var status = AgoraStatusMachine.logout {
        didSet {
            updateDelegate?(status)
        }
    }
    var updateDelegate: ((_ status: AgoraStatusMachine)->())?
    var updateVideoEnable:((_ isEnable: Bool)->())?
    
    var myPhoneNumber = ""
    var callFrom = ""
    var callTo = ""
    
    var peerViewHolder: UIView?
    
    override init() {
        super.init()
        agoraInstance = AgoraAPI.getInstanceWithoutMedia(KeyCenter.appId)
        rtcEngineInstance = AgoraRtcEngineKit.sharedEngine(withAppId: KeyCenter.appId, delegate: self)
        rtcEngineInstance.setParameters("{\"rtc.log_filter\":32783}")
        rtcEngineInstance.setEnableSpeakerphone(true)
        initCallbackHandler()
    }
    
    func initCallbackHandler() {
        loginCallback()
        inviteCallback()
    }
    
    func loginCallback() {
        agoraInstance.onLoginSuccess = { (uid, fd) in
            DispatchQueue.main.async {
                self.uid = uid
                self.status = .login
            }
        }
        
        agoraInstance.onLoginFailed = { ecode in
            DispatchQueue.main.async {
                print("Login failed:" + "\(ecode.rawValue)")
                self.status = .logout
            }
        }
    }
    
    func startPlay(file: String) {
        rtcEngineInstance.startAudioMixing(file, loopback: false, replace: false, cycle: -1)
    }
    
    func stopPlay() {
        rtcEngineInstance.stopAudioMixing()
    }
    
    func inviteCallback() {
        agoraInstance.onInviteFailed = {(channelID, account, uid, ecode, extra) in
            guard let channelID = channelID else {
                return
            }
            DispatchQueue.main.async {
                self.leaveChannel(callToNumber: channelID)
                print("Call failed:" + "\(ecode.rawValue)")
                self.status = .refuseCall
            }
        }
        
        agoraInstance.onInviteAcceptedByPeer = { (channel, name, uid, extra) in
            DispatchQueue.main.async {
                self.status = .inCall
            }
        }
        
        agoraInstance.onInviteReceived = { (channel, name, uid, extra) in
            guard let name = name else {
                return
            }
            DispatchQueue.main.async {
                self.callFrom = name
                self.status = .receiveCalling
            }
        }
        
        agoraInstance.onInviteRefusedByPeer = { (channel, name, uid, extra) in
            guard let channel = channel else {
                return
            }
            DispatchQueue.main.async {
                self.leaveChannel(callToNumber: channel)
                self.status = .refuseCall
            }
        }
        
        agoraInstance.onInviteEndByPeer = { (channel, name, uid, extra) in
            guard let channel = channel else {
                return
            }
            DispatchQueue.main.async {
                self.leaveChannel(callToNumber: channel)
                self.status = .endCall
            }
        }
        
        agoraInstance.onInviteEndByMyself = { (channel, name, uid) in
            DispatchQueue.main.async {
                self.status = .endCall
            }
        }
    }
    
    func login(phoneNumber: String) {
        self.myPhoneNumber = phoneNumber
        agoraInstance.login(KeyCenter.appId, account: phoneNumber, token: "_no_need_token", uid: uid, deviceID: "")
    }
    
    func setUpVideo(videoView: UIView) {
        rtcEngineInstance.enableVideo()
        let canvas = AgoraRtcVideoCanvas()
        canvas.uid = 0
        canvas.view = videoView
        canvas.renderMode = .hidden
        rtcEngineInstance.setupLocalVideo(canvas)

        rtcEngineInstance.startPreview()
    }
    
    func setUpPeerVideo(videoView: UIView) {
        let canvas = AgoraRtcVideoCanvas()
        canvas.uid = remoteUid

        canvas.view = videoView
        canvas.renderMode = .hidden
        rtcEngineInstance.setupRemoteVideo(canvas)
    }
    
    func enableVideoMode() {
        rtcEngineInstance.enableVideo()
        rtcEngineInstance.startPreview()
    }

    func disableVideoMode() {
        rtcEngineInstance.disableVideo()
        rtcEngineInstance.stopPreview()
    }
    
    func switchCamera() {
        rtcEngineInstance.switchCamera()
    }
    
    func joinChannel(callToNumber: String) {
        agoraInstance.channelJoin(callToNumber)
        
        rtcEngineInstance.joinChannel(byToken: nil, channelId: callToNumber, info: callToNumber, uid: UInt(uid), joinSuccess: nil)
    }
    
    func leaveChannel(callToNumber: String) {
        agoraInstance.channelLeave(callToNumber)
        rtcEngineInstance.leaveChannel(nil)
    }
    
    func callTo(phoneName: String) {
        callTo = phoneName
        callFrom = self.myPhoneNumber
        status = .calling
        joinChannel(callToNumber: phoneName)
        agoraInstance.channelInviteUser(phoneName, account: phoneName, uid: 0)
    }
    
    func endCall(peerPhoneNumber: String) {
        leaveChannel(callToNumber: callTo)
        agoraInstance.channelInviteEnd(callTo, account: peerPhoneNumber, uid: 0)
    }
    
    func acceptCall(callToNumber: String) {
        guard callFrom != "" else {
            return
        }
        callTo = callToNumber
        joinChannel(callToNumber: callToNumber)
        agoraInstance.channelInviteAccept(callToNumber, account: callFrom, uid: 0, extra: nil)
        status = .inCall
    }
    
    func refuseCall(callToNumber: String){
        status = .refuseCall
        print("callToNumber \(callToNumber) callFrom \(callFrom)")
        agoraInstance.channelInviteRefuse(callToNumber, account: callFrom, uid: 0, extra: nil)
    }
}


extension Agora: AgoraRtcEngineDelegate {
    func rtcEngine(_ engine: AgoraRtcEngineKit, firstRemoteVideoDecodedOfUid uid: UInt, size: CGSize, elapsed: Int) {
        remoteUid = uid
        guard let peerView = peerViewHolder else {
            return
        }
        
        setUpPeerVideo(videoView: peerView)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didVideoEnabled enabled: Bool, byUid uid: UInt) {
        updateVideoEnable?(enabled)
    }
}
