//
//  LiveRoomViewController.swift
//  OpenLive
//
//  Created by GongYuhua on 2/20/16.
//  Copyright © 2016 Agora. All rights reserved.
//

import Cocoa
protocol LiveRoomVCDelegate: NSObjectProtocol {
    func liveRoomVCNeedClose(_ liveVC: LiveRoomViewController)
}

class LiveRoomViewController: NSViewController {
    
    //MARK: IBOutlet
    @IBOutlet weak var roomNameLabel: NSTextField!
    @IBOutlet weak var remoteContainerView: NSView!
    @IBOutlet weak var muteAudioButton: NSButton!
    @IBOutlet weak var broadcastButton: NSButton!
    //total time label
    @IBOutlet weak var durationLabel: NSTextField!
    var duration:Int = 0
    var isSelected:Bool = true
    @IBOutlet weak var videoSlider: NSSlider!
    //MARK: public var
    var roomName: String!
    var clientRole = AgoraClientRole.audience {
        didSet {
            updateButtonsVisiablity()
        }
    }
    var videoProfile: AgoraVideoProfile!
    var delegate: LiveRoomVCDelegate?
    
    //MARK: engine & session
    var rtcEngine: AgoraRtcEngineKit!
    fileprivate var isBroadcaster: Bool {
        return clientRole == .broadcaster
    }
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
        updateButtonsVisiablity()
        
        loadAgoraKit()
    }
    
    override func viewDidAppear() {
        super.viewDidAppear()
        view.window?.delegate = self
    }
    
    //MARK: - user action
    @IBAction func doMuteClicked(_ sender: NSButton) {
        isMuted = !isMuted
    }
    
    @IBAction func doBroadcastClicked(_ sender: NSButton) {
        if isBroadcaster {
            clientRole = .audience
            if fullSession?.uid == 0 {
                fullSession = nil
            }
        } else {
            clientRole = .broadcaster
        }
        
        rtcEngine.setClientRole(clientRole)
        updateInterface()
    }
    
    override func mouseUp(with theEvent: NSEvent) {
        if theEvent.clickCount == 2 {
            if fullSession == nil {
                if let tappedSession = viewLayouter.responseSession(of: theEvent, inSessions: videoSessions, inContainerView: remoteContainerView) {
                    fullSession = tappedSession
                }
            } else {
                fullSession = nil
            }
        }
    }
    
    @IBAction func doLeaveClicked(_ sender: NSButton) {
        leaveChannel()
    }
    //MARK: - play or pause
    @IBAction func playOrPause(_ sender: Any) {
        
        MediaPlayerKit.shareInstance().play()
    }
    //MARK: - change slieder value 0 ~ 1000
    @IBAction func sliderValueChange(_ sender: NSSlider) {
        let duration = self.duration * 1000
        let asp = Float(sender.integerValue)/Float(1000)
        let pos =  asp * Float(duration)
        MediaPlayerKit.shareInstance().seek(to: Int32(pos))
        
    }
    
    //MARK: - change localVideoVolume
    @IBAction func localVolumeChange(_ sender: NSSlider) {
        MediaPlayerKit.shareInstance().adjustPlaybackSignalVolume(sender.intValue)
    }

    @IBAction func publishVideoStream(_ sender: NSButton) {
        if(isSelected){
            sender.title = "unPublish"
            MediaPlayerKit.shareInstance().unpublishAudio()

        }else
        {
            sender.title = "publish"
            MediaPlayerKit.shareInstance().publishAudio()
        }
        isSelected = !isSelected
    }
    //MARK: - open the video
    @IBAction func openVideo(_ sender: Any) {
        let openPanel  = NSOpenPanel.init()
        openPanel.allowsMultipleSelection = false
        openPanel.allowedFileTypes = ["avi","mp4","mkv","rmvb"]
        openPanel.allowsOtherFileTypes = true
        openPanel.canChooseDirectories = true
        openPanel.canChooseFiles = true
        let finder = openPanel.runModal()
        if finder == NSFileHandlingPanelOKButton {
            let path = openPanel.url?.path
            print(path as Any)
            //rtmp://live.hkstv.hk.lxdns.com/live/hks1
            MediaPlayerKit.shareInstance().load(path ?? "", isAutoPlay: true)
        }
    }
}

//MARK: - private
private extension LiveRoomViewController {
    func updateButtonsVisiablity() {
        broadcastButton?.image = NSImage(named: isBroadcaster ? "btn_join_cancel" : "btn_join")
        muteAudioButton?.isHidden = !isBroadcaster
    }
    
    func leaveChannel() {
        
        MediaPlayerKit.shareInstance().unload()
        MediaPlayerKit.shareInstance().destroy()
        rtcEngine.setupLocalVideo(nil)
        rtcEngine.leaveChannel(nil)
        if isBroadcaster {
            rtcEngine.stopPreview()
        }
        
        for session in videoSessions {
            session.hostingView.removeFromSuperview()
        }
        videoSessions.removeAll()
        
        delegate?.liveRoomVCNeedClose(self)
    }
    
    func alert(string: String) {
        guard !string.isEmpty else {
            return
        }
        
        let alert = NSAlert()
        alert.messageText = string
        alert.addButton(withTitle: "OK")
        alert.beginSheetModal(for: view.window!, completionHandler: nil)
    }
}

private extension LiveRoomViewController {
    func updateInterface() {
        var displaySessions = videoSessions
        if !isBroadcaster && !displaySessions.isEmpty {
            displaySessions.removeFirst()
        }
        viewLayouter.layout(sessions: displaySessions, fullSession: fullSession, inContainer: remoteContainerView)
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

//MARK: - Agora SDK
private extension LiveRoomViewController {
    func loadAgoraKit() {
        rtcEngine = AgoraRtcEngineKit.sharedEngine(withAppId: KeyCenter.AppId, delegate: self)
        rtcEngine.setChannelProfile(.liveBroadcasting)
        rtcEngine.enableVideo()
        rtcEngine.enableAudio()
        rtcEngine.enableDualStreamMode(true)
        rtcEngine.setVideoProfile(videoProfile, swapWidthAndHeight: true)
        rtcEngine.setClientRole(clientRole)
        if isBroadcaster {
            rtcEngine.startPreview()
        }
        //MARK: -  add VideoKit
        MediaPlayerKit.shareInstance().createMediaPlayerKit(withRtcEngine: rtcEngine, withSampleRate: 44100)
        //MARK: - add duration call back
//       MediaPlayerKit.shareInstance().getduation = { (duration:Int) -> Void in
//            self.duration = duration/1000;
//            //caculate the duation
//            let thh:Int = (duration/1000)/3600
//            let thm:Int = ((duration/1000) % 3600) / 60
//            let ths:Int = ((duration/1000) % 60)
//            let timeStr = NSString.init(format: "%02d:%02d:%02d", thh,thm,ths)
//            DispatchQueue.main.async(execute: {
//                self.durationLabel.stringValue = timeStr as String
//            })
//        }
        //add timer
        if #available(OSX 10.12, *) {
            Timer.scheduledTimer(withTimeInterval: 1, repeats: true) { (Timer) in
                print("getCurrentPosition === \(MediaPlayerKit.shareInstance().getCurrentPosition())")
                //receive time
                let pos = MediaPlayerKit.shareInstance().getCurrentPosition()
                if self.duration == 0{
                    return
                }
                var progress:float_t = float_t(pos/1000)
                progress = progress/Float(self.duration)
                progress = progress * 1000    
                self.videoSlider.intValue = Int32(progress)
                
            }
        } else {
            // Fallback on earlier versions
            
        }
        
        
        //addLocalSession()
        MediaPlayerKit.shareInstance().setVideoView(self.view)
        let code = rtcEngine.joinChannel(byToken: nil, channelId: roomName, info: nil, uid: 0, joinSuccess:nil)
        if code != 0 {
            DispatchQueue.main.async(execute: {
                self.alert(string: "Join channel failed: \(code)")
            })
        }

    }

}

extension LiveRoomViewController: AgoraRtcEngineDelegate {
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        
        
        print(uid)
    }
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        let userSession = videoSession(ofUid: Int64(uid))
        
        rtcEngine.setupRemoteVideo(userSession.canvas)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, firstLocalVideoFrameWith size: CGSize, elapsed: Int) {
        if let _ = videoSessions.first {
            updateInterface()
        }
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
            
            if deletedSession == fullSession {
                fullSession = nil
            }
        }
        
        
    }
}

//MARK: - window
extension LiveRoomViewController: NSWindowDelegate {
    func windowShouldClose(_ sender: Any) -> Bool {
        leaveChannel()
        return false
    }
}
