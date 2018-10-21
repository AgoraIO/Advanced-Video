//
//  LiveRoomViewController.swift
//  LargeGroupVideoChat
//
//  Created by ZhangJi on 30/09/2017.
//  Copyright © 2017 Agora. All rights reserved.
//

import UIKit
import AgoraRtcEngineKit

enum VideoViewLayoutType {
    case small
    case big
}

protocol LiveRoomVCDelegate: NSObjectProtocol {
    func liveVCNeedClose(_ liveVC: LiveRoomViewController)
}

class LiveRoomViewController: UIViewController {
    
    @IBOutlet weak var remoteContainerView: UIView!
    @IBOutlet weak var audioMuteButton: UIButton!
    
    var roomName: String!
    var layoutType = VideoViewLayoutType.small {
        didSet {
            updateInterface()
        }
    }
    
    weak var delegate: LiveRoomVCDelegate?
    var scrollContainView = UIScrollView()
    
    //MARK: - engine & session view
    var rtcEngine: AgoraRtcEngineKit!
    fileprivate var isMuted = false {
        didSet {
            rtcEngine?.muteLocalAudioStream(isMuted)
            audioMuteButton?.setImage(isMuted ? #imageLiteral(resourceName: "btn_mute_cancel.pdf") : #imageLiteral(resourceName: "btn_mute.pdf"), for: .normal)
        }
    }
    
    fileprivate var videoSessions = [VideoSessionView]() {
        didSet {
            guard remoteContainerView != nil else {
                return
            }
            updateInterface(withAnimation: true)
        }
    }
    fileprivate var fullSession: VideoSessionView? {
        didSet {
            if fullSession == nil {
                layoutType = .big
            } else if fullSession != oldValue {
                layoutType = .small
            }
            
        }
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        self.automaticallyAdjustsScrollViewInsets = false
        scrollContainView.delegate = self
        
        joinChannel()
    }
    
    //MARK: - user action
    @IBAction func doSwitchCameraPressed(_ sender: UIButton) {
        rtcEngine?.switchCamera()
    }
    
    @IBAction func doMutePressed(_ sender: UIButton) {
        isMuted = !isMuted
    }
    
    @IBAction func doLeavePressed(_ sender: UIButton) {
        leaveChannel()
    }
    
    @IBAction func doDoubleTapped(_ sender: UITapGestureRecognizer) {
        if let fullSession = responseSession(of: sender, inSessions: videoSessions, inContainerView: scrollContainView)
        {
            self.fullSession = fullSession
            if self.fullSession?.uid != 0 {
                rtcEngine.setRemoteVideoStream((self.fullSession?.uid)!, type: .high)
            }
        } else {
            fullSession = nil
        }
    }
}

private extension LiveRoomViewController {
    func joinChannel() {
        // init Agora Video Engine with AgoraChannelProfileLiveBroadcasting, and set self as broadcaster
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
        rtcEngine.setParameters("{\"che.video.lowBitRateStreamParameter\":{\"width\":180,\"height\":320,\"frameRate\":15,\"bitRate\":140}}")
        
        rtcEngine.startPreview()
        addLocalSessionView()
        
        let code = rtcEngine.joinChannel(byToken: nil, channelId: roomName, info: nil, uid: 0, joinSuccess: nil)
        if code == 0 {
            setIdleTimerActive(false)
            rtcEngine.setEnableSpeakerphone(true)
        } else {
            print("Join channel failed: \(code)")
        }
    }
    
    func leaveChannel() {
        setIdleTimerActive(true)
        
        rtcEngine.setupLocalVideo(nil)
        rtcEngine.leaveChannel(nil)
        rtcEngine.stopPreview()
        
        for session in videoSessions {
            session.removeFromSuperview()
        }
        videoSessions.removeAll()
        
        delegate?.liveVCNeedClose(self)
    }
}

private extension LiveRoomViewController {
    func updateInterface(withAnimation animation: Bool) {
        if animation {
            UIView.animate(withDuration: 0.15) { [weak self] in
                guard let strongSelf = self else {
                    return
                }
                strongSelf.updateInterface()
                strongSelf.view.layoutIfNeeded()
            }
        } else {
            updateInterface()
        }
    }
    
    func updateInterface() {
        updateInterface(withContainerSize: remoteContainerView!.frame.size)
    }
    
    func updateInterface(withContainerSize containerSize: CGSize) {
        let containerWidth = containerSize.width
        let containerHeight = containerSize.height
        
        switch layoutType {
        case .small:
            scrollContainView.frame = CGRect(x: 0.0, y: 20.0, width: containerWidth, height: 128.0)
            scrollContainView.contentSize = CGSize(width: 128 * (videoSessions.count - 1), height: 0)
            var count = 0
            for view in videoSessions {
                if view != fullSession {
                    view.frame = CGRect(x: 129 * count, y: 0, width: Int(scrollContainView.frame.height), height: Int(scrollContainView.frame.height))
                    count += 1
                    scrollContainView.addSubview(view)
                } else {
                    view.frame = remoteContainerView.frame
                    remoteContainerView.addSubview(view)
                }
            }
            remoteContainerView.addSubview(scrollContainView)
            scrollContainView.contentOffset = CGPoint(x: 0, y: 0)
        case .big:
            scrollContainView.removeFromSuperview()
            scrollContainView.frame = remoteContainerView.frame
            scrollContainView.alwaysBounceHorizontal = true
            scrollContainView.alwaysBounceVertical = false
            
            scrollContainView.contentSize = CGSize(width: containerWidth / 2.0 * CGFloat((videoSessions.count - 1) / 2 + 1) , height: 0)
            for (index, view) in videoSessions.enumerated() {
                view.frame = CGRect(x: CGFloat(index / 2) * (containerWidth / 2.0),
                                    y: CGFloat(index % 2) * (containerHeight / 2.0),
                                    width: self.view.frame.width / 2.0,
                                    height: self.view.frame.height / 2.0)
                scrollContainView.addSubview(view)
            }
            remoteContainerView.addSubview(scrollContainView)
            scrollContainView.contentOffset = CGPoint(x: 0, y: 0)
            
        }
        caculateDisplaySessions(withContainerSize: containerSize)
    }
    
    func setStreamType(forSessions sessions: [VideoSessionView], fullSession: VideoSessionView?) {
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
    
    func addLocalSessionView() {
        let localSession = videoSessionView(ofUid: 0)
        localSession.frame = remoteContainerView.frame
        localSession.muteButton.isHidden = true
        localSession.unMuteButton.isHidden = true
        localSession.holdImageView.isHidden = true
        localSession.userLabel.isHidden = true
        fullSession = localSession
        remoteContainerView.addSubview(localSession)
        rtcEngine.setupLocalVideo(localSession.canvas)
    }
    
    func fetchSessionView(ofUid uid: UInt) -> VideoSessionView? {
        for session in videoSessions {
            if session.uid == uid {
                return session
            }
        }
        return nil
    }
    
    func videoSessionView(ofUid uid: UInt) -> VideoSessionView {
        if let fetchedSession = fetchSessionView(ofUid: uid) {
            return fetchedSession
        } else {
            let newSession = VideoSessionView.newInstance(uid: uid)
            newSession.delegate = self
            videoSessions.insert(newSession, at: 0)
            return newSession
        }
    }
    
    func responseSession(of gesture: UIGestureRecognizer, inSessions sessions: [VideoSessionView], inContainerView container: UIScrollView) -> VideoSessionView? {
        let location = gesture.location(in: container)
        for session in sessions {
            if session != fullSession, session.frame.contains(location) {
                print(session.uid)
                return session
            }
        }
        return nil
    }
    
    func caculateDisplaySessions(withContainerSize containerSize: CGSize? = nil) {
        for session in videoSessions {
            if session.uid != 0 {
                session.tapMuteButton(session.muteButton)
            }
        }
        
        let pageWidth: CGFloat
        if let containerSize = containerSize {
            pageWidth = containerSize.width
        } else {
            pageWidth = remoteContainerView.frame.size.width
        }
        
        let offset = scrollContainView.contentOffset.x
        let pageIndex = Int(offset / pageWidth)
        var displayIndexs = [Int]()
        
        switch layoutType {
        case .big:
            let sessionIndex = pageIndex * 4
            let pageOffset = offset - pageWidth * CGFloat(pageIndex)
            
            displayIndexs.append(sessionIndex + 2)
            displayIndexs.append(sessionIndex + 3)
            
            if pageOffset > 0 {
                displayIndexs.append(sessionIndex + 4)
                displayIndexs.append(sessionIndex + 5)
            }
            if pageOffset < pageWidth / 2 {
                displayIndexs.append(sessionIndex)
                displayIndexs.append(sessionIndex + 1)
            }
            if pageOffset > pageWidth / 2 {
                displayIndexs.append(sessionIndex + 6)
                displayIndexs.append(sessionIndex + 7)
            }
            for index in displayIndexs {
                if index >= (videoSessions.count - 1) || index < 0 {
                    continue
                }
                videoSessions[index].tapUnMuteButton(videoSessions[index].unMuteButton)
                rtcEngine.setRemoteVideoStream(videoSessions[index].uid, type: .high)
            }
        case .small:
            fullSession?.tapUnMuteButton((fullSession?.unMuteButton)!)
            let beginIndex = Int(offset / 129)
            let showNum = ceil((pageWidth - (129 - offset.truncatingRemainder(dividingBy: 129))) / 129 + 1)
            var includeFull = false
            var lastIndex = 0
            for index in 0 ..< beginIndex + Int(showNum) {
                lastIndex = index
                if index >= (videoSessions.count - 1) {
                    continue
                }
                if videoSessions[index] == fullSession! {
                    includeFull = true
                    continue
                }
                if index < beginIndex {
                    continue
                }
                videoSessions[index].tapUnMuteButton(videoSessions[index].unMuteButton)
                rtcEngine.setRemoteVideoStream(videoSessions[index].uid, type: .low)
            }
            if includeFull && lastIndex < videoSessions.count - 2 {
                videoSessions[lastIndex + 1].tapUnMuteButton(videoSessions[lastIndex + 1].unMuteButton)
                rtcEngine.setRemoteVideoStream(videoSessions[lastIndex + 1].uid, type: .low)
            }
        }
    }
}

//MARK: - Agora Media SDK
extension LiveRoomViewController: AgoraRtcEngineDelegate {
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        let remoteSession = videoSessionView(ofUid: uid)
        rtcEngine.setupRemoteVideo(remoteSession.canvas)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        var indexToDelete: Int?
        for (index, session) in videoSessions.enumerated() {
            if session.uid == uid {
                indexToDelete = index
            }
        }
        
        if let indexToDelete = indexToDelete {
            let deletedSession = videoSessions.remove(at: indexToDelete)
            deletedSession.removeFromSuperview()
            
            if deletedSession == fullSession {
                fullSession = fetchSessionView(ofUid: 0)
            }
        }
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurWarning warningCode: AgoraWarningCode) {
        print("didOccurWarning: \(warningCode.rawValue)")
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        print("didOccurError: \(errorCode.rawValue)")
    }
}

extension LiveRoomViewController {
    override func viewWillTransition(to size: CGSize, with coordinator: UIViewControllerTransitionCoordinator) {
        updateInterface(withContainerSize: size)
    }
    
    func setIdleTimerActive(_ active: Bool) {
        UIApplication.shared.isIdleTimerDisabled = !active
    }
}

extension LiveRoomViewController: VideoSessionViewDelegate {
    func videoSessionView(_ view: VideoSessionView, shouldMute: Bool) {
        let uid: UInt = view.uid
        rtcEngine.muteRemoteAudioStream(uid, mute: shouldMute)
        rtcEngine.muteRemoteVideoStream(uid, mute: shouldMute)
        view.isMute = shouldMute
    }
}

extension LiveRoomViewController: UIScrollViewDelegate {
    func scrollViewDidEndDecelerating(_ scrollView: UIScrollView) {
        caculateDisplaySessions()
    }
    
    func scrollViewDidEndDragging(_ scrollView: UIScrollView, willDecelerate decelerate: Bool) {
        if !decelerate {
            caculateDisplaySessions()
        }
    }
}
