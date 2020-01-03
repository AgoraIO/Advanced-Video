//
//  LiveRoomViewController.swift
//  17-Multistream
//
//  Created by ZhangJi on 30/09/2017.
//  Copyright © 2017 Agora. All rights reserved.
//

import Cocoa
import AgoraRtcKit
import AGEVideoLayout

protocol LiveRoomVCDelegate: NSObjectProtocol {
    func liveRoomVCNeedClose(_ liveVC: LiveRoomViewController)
}

class LiveRoomViewController: NSViewController {
    
    //MARK: IBOutlet
    @IBOutlet weak var roomNameLabel: NSTextField!
    @IBOutlet weak var videoContainer: AGEVideoContainer!
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
            guard videoContainer != nil,
                videoSessions.count > 0 else {
                return
            }
            
            setupVideoLayout(newCount: videoSessions.count,
                             oldCount: oldValue.count)
        }
    }
    
    private lazy var fullLayout: AGEVideoLayout = AGEVideoLayout(level: 0)
                                                    .startPoint(x: 0, y: 0)
                                                    .size(.scale(CGSize(width: 1, height: 1)))
    
    private lazy var scrollLayout: AGEVideoLayout = {
        var topSafeArea: CGFloat = 20
        
        let width = view.bounds.width - 20
        
        return AGEVideoLayout(level: 1)
                .startPoint(x: 10, y: topSafeArea)
                .size(.constant(CGSize(width: width, height: 100)))
                .itemSize(.constant(CGSize(width: 100, height: 100)))
                .scrollType(.scroll(.horizontal))
                .interitemSpacing(10)
    }()
    
    private let currentUid: UInt = 0
    
    //MARK: - life cycle
    override func viewDidLoad() {
        super.viewDidLoad()
        videoContainer.delegate = self
        videoContainer.dataSource = self
        roomNameLabel.stringValue = roomName
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
        
        rtcEngine.startPreview()
        addLocalSession()
        
        let code = rtcEngine.joinChannel(byToken: nil, channelId: roomName, info: nil, uid: currentUid, joinSuccess: nil)
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
    func setupVideoLayout(newCount: Int, oldCount: Int) {
        if newCount == 1, oldCount == 0 {
            videoContainer.setLayouts([fullLayout])
        } else if newCount == 1, oldCount > 1 {
            videoContainer.removeLayout(level: scrollLayout.level)
            
        // if video session count great than 2, add scroll layout to present new video stream
        } else if newCount > 1, oldCount == 1 {
            videoContainer.setLayouts([fullLayout, scrollLayout])
        }
        
        videoContainer.reload(level: 0)
        videoContainer.reload(level: 1)
    }
    
    func addLocalSession() {
        let localSession = VideoSession.localSession()
        videoSessions.append(localSession)
        rtcEngine.setupLocalVideo(localSession.canvas)
    }
    
    func fetchSession(ofUid uid: UInt) -> VideoSession? {
        for session in videoSessions {
            if session.uid == uid {
                return session
            }
        }
        
        return nil
    }
    
    func videoSession(ofUid uid: UInt) -> VideoSession {
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
        let userSession = videoSession(ofUid: uid)
        rtcEngine.setupRemoteVideo(userSession.canvas)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        var indexToDelete: Int?
        for (index, session) in videoSessions.enumerated() {
            if session.uid == Int64(uid) {
                indexToDelete = index
            }
        }
        
        if let indexToDelete = indexToDelete {
            let deletedSession = videoSessions.remove(at: indexToDelete)
            deletedSession.hostingView.removeFromSuperview()
            deletedSession.canvas.view = nil
        }
    }
}

extension LiveRoomViewController: AGEVideoContainerDataSource {
    func container(_ container: AGEVideoContainer, numberOfItemsIn level: Int) -> Int {
        if level == fullLayout.level {
            return 1
        } else {
            return videoSessions.count - 1
        }
    }
    
    func container(_ container: AGEVideoContainer, viewForItemAt index: AGEIndex) -> AGEView {
        if index.level == fullLayout.level {
            return videoSessions[0].hostingView
        } else {
            return videoSessions[index.item + 1].hostingView
        }
    }
}

extension LiveRoomViewController: AGEVideoContainerDelegate {
    func container(_ container: AGEVideoContainer, didSelected itemView: AGEView, index: AGEIndex) {
        guard index.level == scrollLayout.level else {
            return
        }
        
        let first = 0
        let swap = index.item + 1
        
        let firstSession = videoSessions[first]
        let swapSession = videoSessions[swap]
        
        videoSessions.swapAt(first, swap)
        
        container.reload(level: fullLayout.level)
        container.reload(level: scrollLayout.level)
        
        if currentUid != swapSession.uid {
            rtcEngine.setRemoteVideoStream(swapSession.uid, type: .high)
        }
        
        if currentUid != firstSession.uid {
            rtcEngine.setRemoteVideoStream(firstSession.uid, type: .low)
        }
    }
    
    func container(_ container: AGEVideoContainer, itemDidHidden index: AGEIndex) {
        guard index.level == scrollLayout.level else {
            return
        }
        
        let session = videoSessions[index.item + 1]
        rtcEngine.muteRemoteAudioStream(UInt(session.uid), mute: true)
    }
    
    func container(_ container: AGEVideoContainer, itemWillDisplay index: AGEIndex) {
        guard index.level == scrollLayout.level else {
            return
        }
        
        let session = videoSessions[index.item + 1]
        rtcEngine.muteRemoteAudioStream(UInt(session.uid), mute: false)
    }
}

//MARK: - window
extension LiveRoomViewController: NSWindowDelegate {
    func windowShouldClose(_ sender: NSWindow) -> Bool {
        leaveChannel()
        return false
    }
}
