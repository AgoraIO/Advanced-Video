//
//  LiveRoomViewController.swift
//  MediaPlayerKitQuickstart
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
    @IBOutlet weak var remoteContainerView: MSView!
    @IBOutlet weak var muteAudioButton: NSButton!
    @IBOutlet weak var broadcastButton: NSButton!
    @IBOutlet weak var videotoolsBar: NSView!
    @IBOutlet weak var playBtn: NSButton!
    @IBOutlet weak var openBtnImage: NSImageView!
    //total time label
    @IBOutlet weak var volumeBg: NSImageView!
    @IBOutlet weak var openMediaBtn: NSButton!
    @IBOutlet weak var publishBtn: NSButton!
    @IBOutlet weak var durationLabel: NSTextField!
    var duration:Int = 0
    var isSelected:Bool = false
    var view1: NSView?
    var isOpenFile:Bool = false
    var isPlay:Bool = false
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
            muteAudioButton?.image = NSImage(named: isMuted ? NSImage.Name(rawValue: "btn_mute_blue") : NSImage.Name(rawValue: "btn_mute"))
            
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
        updateButtonsVisiablity()
        loadAgoraKit()
        
    }
    override func viewDidDisappear() {
       
    }
    override func prepare(for segue: NSStoryboardSegue, sender: Any?) {
        
        guard let segueId = segue.identifier else {
            return
        }
        
        if segueId.rawValue == "switchLiveRoom" {
            let switchVC = segue.destinationController as! SwitchLiveController
            switchVC.channelName = roomName
            switchVC.uid = ""
            switchVC.viewClosed = {(name:String,uid:String) in
             print(name,uid)
            }
        }
        
    }
    override func viewDidAppear() {
        super.viewDidAppear()
        view.window?.delegate = self
        initUI()
        remoteContainerView.mouseEventCallBack = {(state:MOUSE_EVENT_STATE) in
            switch state {
            case MOUSE_EVENT_STATE_ENTERED:
                self.videotoolsbarAnimate(isAnimate: false)
                break
            case MOUSE_EVENT_STATE_DOWN:
            self.videotoolsbarAnimate(isAnimate: false)
                break

            case MOUSE_EVENT_STATE_EXIT:
                self.videotoolsbarAnimate(isAnimate: true)
                break
                
            default:
            self.videotoolsbarAnimate(isAnimate: true)
                 break
            }
        }

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
        //updateInterface()
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
    @IBAction func playOrPause(_ sender: NSButton) {
        if isPlay {
            sender.image = NSImage.init(named: NSImage.Name(rawValue: "pause"))
            MediaPlayerKit.shareInstance().play()

        }else{
            sender.image = NSImage.init(named: NSImage.Name(rawValue: "play"))
            MediaPlayerKit.shareInstance().stop()

        }
        isPlay = !isPlay
    }
    //MARK: - change slieder value 0 ~ 1000
    @IBAction func sliderValueChange(_ sender: NSSlider) {
        let duration = self.duration
        let asp = Float(sender.integerValue)/Float(1000)
        let pos =  asp * Float(duration)
        print("seekPos == \(pos)")
        MediaPlayerKit.shareInstance().seek(to: Int(pos))
        
    }
    
    //MARK: - change localVideoVolume
    @IBAction func localVolumeChange(_ sender: NSSlider) {
        MediaPlayerKit.shareInstance().adjustPlaybackSignalVolume(sender.intValue)
    }

    @IBAction func publishVideoStream(_ sender: NSButton) {
        if(!isSelected){
            sender.title = "PUBLISH\nSTREAM"
            if #available(OSX 10.14, *) {
                sender.contentTintColor = NSColor.white
            } else {
                // Fallback on earlier versions
            }
            sender.layer?.borderWidth = 0
            sender.layer?.backgroundColor = NSColor.init(fromHexadecimalValue: "#00adef")?.cgColor
            MediaPlayerKit.shareInstance().unpublishVideo()
            MediaPlayerKit.shareInstance().unpublishAudio()

        }else
        {
            sender.title = "PUBLISH\nSTREAM"
            sender.layer?.backgroundColor = NSColor.clear.cgColor
            if #available(OSX 10.14, *) {
                sender.contentTintColor = NSColor.init(fromHexadecimalValue: "#7c7c7c")
            } else {
                // Fallback on earlier versions
            }
            sender.layer?.borderWidth = 1
            sender.layer?.borderColor = NSColor.init(fromHexadecimalValue: "#7c7c7c")?.cgColor
            MediaPlayerKit.shareInstance().publishAudio()
            MediaPlayerKit.shareInstance().publishVideo()

        }
        isSelected = !isSelected
    }
    //MARK: - open the video
    @IBAction func openVideo(_ sender: Any) {

        let openPanel  = NSOpenPanel.init()
        openPanel.allowsMultipleSelection = false
        openPanel.allowedFileTypes = ["avi","mp4","mkv"]
        openPanel.allowsOtherFileTypes = true
        openPanel.canChooseDirectories = true
        openPanel.canChooseFiles = true
        let finder = openPanel.runModal()
        if finder.rawValue == NSFileHandlingPanelOKButton {
            let path = openPanel.url?.path
            print(path as Any)
            //rtmp://live.hkstv.hk.lxdns.com/live/hks1
            isOpenFile  = true
            videotoolsbarAnimate(isAnimate: true)
            //open video
            MediaPlayerKit.shareInstance().load(path!,isAutoPlay: true)
      }
    }
}

//MARK: - private
extension LiveRoomViewController : NSAnimationDelegate {
    func updateButtonsVisiablity() {
        broadcastButton?.image = NSImage(named: isBroadcaster ? NSImage.Name(rawValue: "btn_join_cancel") : NSImage.Name(rawValue: "btn_join"))
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
    func initUI(){
        
        let view = NSView.init()
        view.frame = NSRect.init(x: 0, y: 0, width: videotoolsBar.frame.width, height: videotoolsBar.frame.height)
        let layer  = CALayer.init();
        layer.backgroundColor = NSColor.init(red: 23/255.0, green: 35/255.0, blue: 34/255.0, alpha: 0.75).cgColor;
        layer.cornerRadius = 8
        view.wantsLayer = true
        view.layer = layer
        view.needsDisplay = true
        videotoolsBar.addSubview(view, positioned: .below, relativeTo: videotoolsBar)

        view1 = NSView.init()
        view1?.frame = playBtn.frame
        let layer1  = CALayer.init();
        layer1.backgroundColor = NSColor.init(red: 23/255.0, green: 35/255.0, blue: 34/255.0, alpha: 0.75).cgColor;
        layer1.cornerRadius = 8
        view1?.wantsLayer = true
        view1?.layer = layer1
        view1?.needsDisplay = true
        remoteContainerView.addSubview(view1!, positioned: .below, relativeTo: remoteContainerView)
        
        let viewOpMedia = NSView.init()
        viewOpMedia.frame = NSRect.init(x: 0, y: 0, width: openMediaBtn.frame.width, height: openMediaBtn.frame.height)
        let layerOpMedia  = CALayer.init();
        layerOpMedia.backgroundColor = NSColor.white.cgColor;
        layerOpMedia.cornerRadius = 10
        layerOpMedia.borderWidth = 3
        layerOpMedia.borderColor = NSColor.init(fromHexadecimalValue: "#ffffff").cgColor
        viewOpMedia.wantsLayer = true
        viewOpMedia.layer = layerOpMedia
        viewOpMedia.needsDisplay = true
        openMediaBtn.needsDisplay =  true
        openMediaBtn.addSubview(viewOpMedia, positioned: .below, relativeTo: openMediaBtn)
        
        publishBtn.layer?.cornerRadius = 8
        publishBtn.layer?.backgroundColor = NSColor.init(fromHexadecimalValue: "#00adef")?.cgColor
        
        var viewsArr:NSArray?
        Bundle.main.loadNibNamed(NSNib.Name(rawValue: "VolumeView"), owner: nil, topLevelObjects: &viewsArr)
        for item in viewsArr ?? [] {
            if item is NSView
            {
                let volumeView = item as! VolumeView
                volumeView.frame = volumeBg.frame
                volumeView.volumeLevelCallback = { (volLevel:Float) in
                    //调节音量
                    MediaPlayerKit.shareInstance().adjustPlaybackSignalVolume(Int32(volLevel * 100))
                }
                volumeBg.removeFromSuperview()
                videotoolsBar.addSubview(volumeView)
            }
        }
        
        publishBtn.title = "PUBLISH\nSTREAM"
        publishBtn.layer?.backgroundColor = NSColor.clear.cgColor
        if #available(OSX 10.14, *) {
            publishBtn.contentTintColor = NSColor.init(fromHexadecimalValue: "#7c7c7c")
        } else {
            // Fallback on earlier versions
        }
        publishBtn.layer?.borderWidth = 1
        publishBtn.layer?.borderColor = NSColor.init(fromHexadecimalValue: "#7c7c7c")?.cgColor
        

        
    }
    func startAnimation(animationTarget:NSView,endPoint:NSPoint){
        
        let startFrame = animationTarget.frame
        let endFrame = NSMakeRect(endPoint.x, endPoint.y, startFrame.size.width, startFrame.size.height)
        let dictionary =  NSDictionary.init(objects: [animationTarget,NSViewAnimation.EffectName.fadeOut,NSValue.init(rect: startFrame),NSValue.init(rect: endFrame)], forKeys:  [NSViewAnimation.Key.target as NSCopying,NSViewAnimation.Key.effect as NSCopying,NSViewAnimation.Key.startFrame as NSCopying,NSViewAnimation.Key.endFrame as NSCopying])
        let animation = NSViewAnimation.init(viewAnimations: NSArray.init(object: dictionary) as! [[NSViewAnimation.Key : Any]])
        animation.delegate = self as! NSAnimationDelegate
        animation.duration = 2.0
        animation.animationBlockingMode = .nonblocking
        animation.start()
        
    }
    @objc func videotoolsbarAnimate(isAnimate:Bool){
        if !isOpenFile {
            openMediaBtn.isHidden = false
            openBtnImage.isHidden = false

        }else{
            openMediaBtn.isHidden = true
            openBtnImage.isHidden = true
        }
        videotoolsBar.isHidden  = isAnimate
        playBtn.isHidden = isAnimate
        view1?.isHidden = isAnimate
    }
    func playBtnAnimate(isAnimate:Bool){

        
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
        //MARK: -  add MediaPlayerKit
        MediaPlayerKit.shareInstance().createMediaPlayerKit(withRtcEngine: rtcEngine, withSampleRate: 44100)
        MediaPlayerKit.shareInstance().delegate = self
        //add timer
        if #available(OSX 10.12, *) {
            Timer.scheduledTimer(withTimeInterval: 1, repeats: true) { (Timer) in
                print("getCurrentPosition === \(MediaPlayerKit.shareInstance().getCurrentPosition())")
                //receive time
                let pos = MediaPlayerKit.shareInstance().getCurrentPosition()
                if self.duration == 0{
                    return
                }
                var progress:float_t = float_t(pos)
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
extension LiveRoomViewController: MediaPlayerKitDelegate {
    func onAudioTrackInfoCallback(_ index: Int32, samplerate: Int32, channels: Int32, duration: Int) {
        self.duration = duration;
        //caculate the duation
        let thh:Int = (duration / 1000)/3600
        let thm:Int = ((duration / 1000) % 3600) / 60
        let ths:Int = ((duration / 1000) % 60)
        let timeStr = NSString.init(format: "%02d:%02d:%02d", thh,thm,ths)
        DispatchQueue.main.async(execute: {
            self.durationLabel.stringValue = timeStr as String
        })
    }
    
    func onPlayerStateChanged(_ state: AG_MEDIA_PLAYER_STATE) {
        // state callback
        print(state)

    }
    func onVideoTrackInfoCallback(_ index: Int32, width: Int32, height: Int32) {
        //video info callback
    }
    
    func onKitError(_ error: AG_KIT_ERROR) {
        //kit callback
        print(error)
    }

    
}
