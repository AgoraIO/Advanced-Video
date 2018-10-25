//
//  LiveRoomViewController.swift
//  LargeGroupVideoChat
//
//  Created by ZhangJi on 30/09/2017.
//  Copyright © 2017 Agora. All rights reserved.
//

import Cocoa
import AgoraRtcEngineKit

protocol LiveRoomVCDelegate: NSObjectProtocol {
    func liveRoomVCNeedClose(_ liveVC: LiveRoomViewController)
}

class LiveRoomViewController: NSViewController {
    
    //MARK: IBOutlet
    @IBOutlet weak var roomNameLabel: NSTextField!
    @IBOutlet weak var remoteContainerView: NSView!
    @IBOutlet weak var muteAudioButton: NSButton!
    
    //MARK: public var
    var roomName: String!
    var delegate: LiveRoomVCDelegate?
    
    //MARK: engine & session
    var rtcEngine: AgoraRtcEngineKit!
    fileprivate var isMuted = false {
        didSet {
            rtcEngine.muteLocalAudioStream(isMuted)
            muteAudioButton?.image = NSImage(named: isMuted ? "btn_mute_blue" : "btn_mute")
        }
    }
    fileprivate var videoSessions = [VideoSession]() {
        didSet {
            guard remoteContainerView != nil else {
                return
            }
            updateInterface()
        }
    }
    fileprivate var fullSession: VideoSession? {
        didSet {
            if fullSession != oldValue && remoteContainerView != nil {
                updateInterface()
            }
        }
    }
    fileprivate let viewLayouter = VideoViewLayouter()
    
    //MARK: - life cycle
    override func viewDidLoad() {
        super.viewDidLoad()
        
        roomNameLabel.stringValue = roomName
        viewLayouter.containerView = remoteContainerView
        joinChannel()
    }
    
    override func viewDidAppear() {
        super.viewDidAppear()
        view.window?.delegate = self
    }
    
    //MARK: - user action
    @IBAction func doMuteClicked(_ sender: NSButton) {
        isMuted = !isMuted
    }
    
    override func mouseUp(with theEvent: NSEvent) {
        guard theEvent.clickCount == 2 else {
            return
        }
        
        if fullSession != nil {
            if let tappedSession = viewLayouter.responseSession(of: theEvent, inSessions: videoSessions, inContainerView: remoteContainerView) {
                fullSession = tappedSession
            }
        } else {
            fullSession = videoSessions.first
        }
    }
    
    @IBAction func doLeaveClicked(_ sender: NSButton) {
        leaveChannel()
    }
}

//MARK: - private
private extension LiveRoomViewController {
    func joinChannel() {
        rtcEngine = AgoraRtcEngineKit.sharedEngine(withAppId: KeyCenter.AppId, delegate: self)
        rtcEngine.setChannelProfile(.liveBroadcasting)
        rtcEngine.setClientRole(.broadcaster)
        
        // enable dualStream mode, so others can get my low bitRate stream if they need.
        rtcEngine.enableVideo()
        rtcEngine.enableDualStreamMode(true)
        
        // setup video configuration with AgoraVideoBitrateCompatible
        let videoConfiguration =
            AgoraVideoEncoderConfiguration(size: AgoraVideoDimension640x360,
                                           frameRate: .fps15,
                                           bitrate: AgoraVideoBitrateCompatible,
                                           orientationMode: .adaptative)
        rtcEngine.setVideoEncoderConfiguration(videoConfiguration)
        
        // fallback to audio if network cannot support both video and audio
        rtcEngine.setLocalPublishFallbackOption(.audioOnly)
        rtcEngine.setRemoteSubscribeFallbackOption(.audioOnly)
        
        // setup parameters to support large group chat
        rtcEngine.setParameters("{\"che.audio.live_for_comm\":true}")
        rtcEngine.setParameters("{\"che.video.moreFecSchemeEnable\":true}")
        
        rtcEngine.startPreview()
        addLocalSession()
        
        let code = rtcEngine.joinChannel(byToken: nil, channelId: roomName, info: nil, uid: 0, joinSuccess: nil)
        if code != 0 {
            print("Join channel failed: \(code)")
        }
    }
    
    func leaveChannel() {
        rtcEngine.setupLocalVideo(nil)
        rtcEngine.leaveChannel(nil)
        rtcEngine.stopPreview()
        
        for session in videoSessions {
            session.hostingView.removeFromSuperview()
        }
        videoSessions.removeAll()
        
        delegate?.liveRoomVCNeedClose(self)
    }
}

private extension LiveRoomViewController {
    func updateInterface() {
        let displaySessions = videoSessions
        
        viewLayouter.sessions = displaySessions
        viewLayouter.fullSession = fullSession
        viewLayouter.targetSize = remoteContainerView.frame.size
        
        viewLayouter.layoutVideoViews()
        setStreamType(forSessions: displaySessions, fullSession: fullSession)
    }
    
    func setStreamType(forSessions sessions: [VideoSession], fullSession: VideoSession?) {
        if let fullSession = fullSession {
            for session in sessions {
                rtcEngine.setRemoteVideoStream(UInt(session.uid), type: (session == fullSession ? .high : .low))
            }
        } else {
            for session in sessions {
                rtcEngine.setRemoteVideoStream(UInt(session.uid), type: .high)
            }
        }
    }
    
    func addLocalSession() {
        let localSession = VideoSession.localSession()
        videoSessions.append(localSession)
        fullSession = localSession
        rtcEngine.setupLocalVideo(localSession.canvas)
    }
    
    func fetchSession(ofUid uid: Int64) -> VideoSession? {
        for session in videoSessions {
            if session.uid == uid {
                return session
            }
        }
        
        return nil
    }
    
    func videoSession(ofUid uid: Int64) -> VideoSession {
        if let fetchedSession = fetchSession(ofUid: uid) {
            return fetchedSession
        } else {
            let newSession = VideoSession(uid: uid)
            videoSessions.append(newSession)
            return newSession
        }
    }
}

extension LiveRoomViewController: AgoraRtcEngineDelegate {
    func rtcEngine(_ engine: AgoraRtcEngineKit, firstRemoteVideoDecodedOfUid uid: UInt, size: CGSize, elapsed: Int) {
        let userSession = videoSession(ofUid: Int64(uid))
        rtcEngine.setupRemoteVideo(userSession.canvas)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, firstLocalVideoFrameWith size: CGSize, elapsed: Int) {
        if let _ = videoSessions.first {
            updateInterface()
        }
    }
    
    internal func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        var indexToDelete: Int?
        for (index, session) in videoSessions.enumerated() {
            if session.uid == Int64(uid) {
                indexToDelete = index
            }
        }
        
        if let indexToDelete = indexToDelete {
            let deletedSession = videoSessions.remove(at: indexToDelete)
            deletedSession.hostingView.removeFromSuperview()
            
            if deletedSession == fullSession {
                fullSession = nil
            }
        }
    }
}

//MARK: - window
extension LiveRoomViewController: NSWindowDelegate {
    func windowShouldClose(_ sender: NSWindow) -> Bool {
        leaveChannel()
        return false
    }
}
