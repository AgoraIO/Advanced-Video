//
//  LiveRoomViewController.swift
//  OpenLive
//
//  Created by GongYuhua on 6/25/16.
//  Copyright © 2016 Agora. All rights reserved.
//

import UIKit

protocol LiveRoomVCDelegate: NSObjectProtocol {
    func liveVCNeedClose(_ liveVC: LiveRoomViewController)
}

class LiveRoomViewController: UIViewController {
    
    @IBOutlet weak var roomNameLabel: UILabel!
    @IBOutlet weak var remoteContainerView: UIView!
    @IBOutlet weak var broadcastButton: UIButton!
    @IBOutlet weak var audioMuteButton: UIButton!
    @IBOutlet var sessionButtons: [UIButton]!
    
    var currentUid: UInt?
    var currentChannel: String!
    
    var videoConfig: VideoConfig?
    
    var sourceUid: UInt?
    var sourceToken: String?
    var sourceChannel: String?
    
    var destinationList: [Destination]?
   
    weak var delegate: LiveRoomVCDelegate?
    weak var collectionView: UICollectionView?
    
    fileprivate var chatMessageVC: ChatMessageViewController?
    
    var clientRole = AgoraClientRole.audience {
        didSet {
            updateButtonsVisiablity()
        }
    }
    var videoProfile: AgoraVideoProfile!
    var rtcEngine: AgoraRtcEngineKit!
    
    fileprivate var isBroadcaster: Bool {
        return clientRole == .broadcaster
    }
    fileprivate var isMuted = false {
        didSet {
            rtcEngine?.muteLocalAudioStream(isMuted)
            audioMuteButton?.setImage(UIImage(named: isMuted ? "btn_mute_cancel" : "btn_mute"), for: .normal)
        }
    }

    fileprivate var videoSessions = [VideoSession]() {
        didSet {
            guard remoteContainerView != nil else {
                return
            }
            updateInterface(withAnimation: true)
        }
    }
    fileprivate var fullSession: VideoSession? {
        didSet {
            if fullSession != oldValue && remoteContainerView != nil {
                updateInterface(withAnimation: true)
            }
        }
    }
    
    fileprivate let viewLayouter = VideoViewLayouter()
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        roomNameLabel.text = currentChannel
        updateButtonsVisiablity()
        loadAgoraKit()
    }
    
    //MARK: - user action
    @IBAction func doSwitchCameraPressed(_ sender: UIButton) {
        rtcEngine?.switchCamera()
    }
    
    @IBAction func doMutePressed(_ sender: UIButton) {
        isMuted = !isMuted
    }
    
    @IBAction func doBroadcastPressed(_ sender: UIButton) {
        if isBroadcaster {
            clientRole = .audience
            if fullSession?.uid == 0 {
                fullSession = nil
            }
        } else {
            clientRole = .broadcaster
        }

        rtcEngine.setClientRole(clientRole)
        updateInterface(withAnimation :true)
    }
    
    @IBAction func doDoubleTapped(_ sender: UITapGestureRecognizer) {
        if fullSession == nil {
            if let tappedSession = viewLayouter.responseSession(of: sender, inSessions: videoSessions, inContainerView: remoteContainerView) {
                fullSession = tappedSession
            }
        } else {
            fullSession = nil
        }
    }
    
    @IBAction func doLeavePressed(_ sender: UIButton) {
        leaveChannel()
    }
    
    @IBAction func doStartPressed(_ sender: UIButton) {
        if let config = getMediaRelayConfiguration() {
            let value = rtcEngine.startChannelMediaRelay(config)
            info(string: "start channel media relay return value: \(value)")
        }
    }
    
    @IBAction func doUpdatePressed(_ sender: UIButton) {
        if let config = getMediaRelayConfiguration() {
            let value = rtcEngine.updateChannelMediaRelay(config)
            info(string: "update channel media relay return value: \(value)")
        }
    }
    
    @IBAction func doStopPressed(_ sender: UIButton) {
        let value = rtcEngine.stopChannelMediaRelay()
        info(string: "stop channel media relay return value: \(value)")
    }
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        guard let segueId = segue.identifier else {
            return
        }

        switch segueId {
        case "VideoVCEmbedChatMessageVC":
            chatMessageVC = segue.destination as? ChatMessageViewController
        case "liveToDestination":
            let destVC = segue.destination as! DestinationViewController
            destVC.collectionView?.dataSource = self
            destVC.updateLayout(updateCollectionView())
            self.collectionView = destVC.collectionView
        default:
            break
        }
    }
}

private extension LiveRoomViewController {
    func updateCollectionView() -> UICollectionViewFlowLayout {
        let layout = UICollectionViewFlowLayout()
        let width = 240 * 0.25
        layout.itemSize = CGSize(width: width, height: 180)
        layout.minimumLineSpacing = 8
        layout.minimumInteritemSpacing = 8
        layout.sectionInset = UIEdgeInsetsMake(0, 10, 0, 10)
        return layout
    }
    
    func updateButtonsVisiablity() {
        guard let sessionButtons = sessionButtons else {
            return
        }

        broadcastButton?.setImage(UIImage(named: isBroadcaster ? "btn_join_cancel" : "btn_join"), for: UIControlState())

        for button in sessionButtons {
            button.isHidden = !isBroadcaster
        }
    }
    
    func setIdleTimerActive(_ active: Bool) {
        UIApplication.shared.isIdleTimerDisabled = !active
    }
    
    func alert(string: String) {
        guard !string.isEmpty else {
            return
        }
        chatMessageVC?.append(alert: string)
    }
    
    func info(string: String) {
        guard !string.isEmpty else {
            return
        }
        chatMessageVC?.append(chat: string)
    }
}

private extension LiveRoomViewController {
    func updateInterface(withAnimation animation: Bool) {
        if animation {
            UIView.animate(withDuration: 0.3, animations: { [weak self] in
                self?.updateInterface()
                self?.view.layoutIfNeeded()
            })
        } else {
            updateInterface()
        }
    }
    
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

//MARK: - Agora Media SDK
private extension LiveRoomViewController {
    func loadAgoraKit() {
        rtcEngine = AgoraRtcEngineKit.sharedEngine(withAppId: KeyCenter.AppId, delegate: self)
        rtcEngine.enableVideo()

        let file = NSHomeDirectory() + "/Documents/sdk.log"
        rtcEngine.setLogFile(file)
        rtcEngine.enableVideo()
        rtcEngine.setChannelProfile(.liveBroadcasting)
        
        if let config = videoConfig {
            rtcEngine.setVideoResolution(CGSize(width: config.width, height: config.height),
                                                andFrameRate: config.frameRate, bitrate: config.bitrate)
            
            info(string: "video resolution: width: \(config.width), height: \(config.height), frameRate: \(config.frameRate), bitrate: \(config.bitrate)")
        } else {
            rtcEngine.setVideoProfile(self.videoProfile, swapWidthAndHeight: true)
        }
        
        rtcEngine.setClientRole(clientRole)
        if isBroadcaster {
            rtcEngine.startPreview()
        }
        
        addLocalSession()
        
        let code = rtcEngine.joinChannel(byToken: nil, channelId: currentChannel, info: nil, uid: currentUid ?? 0, joinSuccess: nil)
        
        info(string: "join channel uid: \(currentUid ?? 0)")
        
        if code == 0 {
            setIdleTimerActive(false)
            rtcEngine.setEnableSpeakerphone(true)
        } else {
            DispatchQueue.main.async(execute: {
                self.alert(string: "Join channel failed: \(code)")
            })
        }
    }
    
    func leaveChannel() {
        setIdleTimerActive(true)
        
        rtcEngine.setupLocalVideo(nil)
        rtcEngine.leaveChannel(nil)
        if isBroadcaster {
            rtcEngine.stopPreview()
        }
        
        for session in videoSessions {
            session.hostingView.removeFromSuperview()
        }
        videoSessions.removeAll()
        
        delegate?.liveVCNeedClose(self)
    }
    
    func getMediaRelayConfiguration() -> AgoraChannelMediaRelayConfiguration? {
        guard let list = destinationList else {
            alert(string: "destination list nil")
            return nil
        }
        
        let config = AgoraChannelMediaRelayConfiguration()
        
        // Source
        if let uid = sourceUid {
            config.sourceInfo.uid = uid
        } else {
            config.sourceInfo.uid = currentUid ?? 0
        }
        
        if let channel = sourceChannel {
            config.sourceInfo.channelName = channel
        }
        
        if let token = sourceToken {
            config.sourceInfo.token = token
        }
        
        info(string: "Source: channel: \(config.sourceInfo.channelName ?? "nil"), uid: \(config.sourceInfo.uid), token: \(config.sourceInfo.token ?? "nil")")
        
        // Destination
        for item in list where item.isPrepareForMediaRelay() {
            let info = getRelayInfoFromDestination(item)
            config.setDestinationInfo(info, forChannelName: item.channel)
        }
        
        guard let destinationInfos = config.destinationInfos else {
            alert(string: "destinationInfos nil")
            return nil
        }
        
        for (key, value) in destinationInfos {
            info(string: "Destination: channel: \(key), uid: \(value.uid), token: \(value.token ?? "nil")")
        }
        
        return config
    }
    
    func getRelayInfoFromDestination(_ destination: Destination) -> AgoraChannelMediaRelayInfo {
        let info = AgoraChannelMediaRelayInfo(token: destination.token.count > 0 ? destination.token : nil)
        info.uid = UInt(destination.uid) ?? 0
        return info
    }
}

extension LiveRoomViewController: AgoraRtcEngineDelegate {
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        let userSession = videoSession(ofUid: Int64(uid))
        rtcEngine.setupRemoteVideo(userSession.canvas)
        info(string: "didJoinedOfUid: \(uid)")
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, firstLocalVideoFrameWith size: CGSize, elapsed: Int) {
        if let _ = videoSessions.first {
            updateInterface(withAnimation: false)
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
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, channelMediaRelayStateDidChange state: AgoraChannelMediaRelayState, error: AgoraChannelMediaRelayError) {
        if error == .none {
            info(string: "channelMediaRelayStateDidChange, state: \(state.rawValue), error: \(error.rawValue))")
        } else {
            alert(string: "channelMediaRelayStateDidChange, state: \(state.rawValue), error: \(error.rawValue))")
        }
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didReceive event: AgoraChannelMediaRelayEvent) {
        info(string: "didReceivedChannelMediaRelayEvent: \(event.rawValue)")
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        alert(string: "didOccurError: \(errorCode.rawValue)")
    }

    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurWarning warningCode: AgoraWarningCode) {
        alert(string: "didOccurWarning: \(warningCode.rawValue)")
    }
}

extension LiveRoomViewController: UICollectionViewDataSource {
    func collectionView(_ collectionView: UICollectionView, numberOfItemsInSection section: Int) -> Int {
        return destinationList!.count
    }
    
    func collectionView(_ collectionView: UICollectionView, cellForItemAt indexPath: IndexPath) -> UICollectionViewCell {
        let cell = collectionView.dequeueReusableCell(withReuseIdentifier: "DestinationView", for: indexPath) as! DestinationView
        cell.delegate = self
        let info = destinationList![indexPath.row]
        cell.updateFrom(info)
        return cell
    }
}

extension LiveRoomViewController: DestinationViewDelegate {
    func destinationView(_ cell: DestinationView, willUpdate info: Destination) {
        if let index = collectionView?.indexPath(for: cell) {
            destinationList![index.item] = info
        }
    }
}
