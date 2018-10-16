//
//  VideoChatViewController.swift
//  Agora Video Source
//
//  Created by GongYuhua on 2017/4/11.
//  Copyright © 2017 Agora.io All rights reserved.
//

import UIKit

class VideoChatViewController: UIViewController {
    @IBOutlet weak var localVideo: MyVideoView!
    @IBOutlet weak var remoteVideo: UIView!
    @IBOutlet weak var controlButtons: UIView!
    @IBOutlet weak var remoteVideoMutedIndicator: UIImageView!
    @IBOutlet weak var localVideoMutedBg: UIImageView!
    @IBOutlet weak var localVideoMutedIndicator: UIImageView!

    var agoraKit: AgoraRtcEngineKit!
    var videoCapture: MyVideoCapture!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        hideVideoMuted()
        initializeAgoraEngine()
        setupVideo()
        startVideoCapture()
        joinChannel()
    }
    
    func initializeAgoraEngine() {
        agoraKit = AgoraRtcEngineKit.sharedEngine(withAppId: AppID, delegate: self)
    }

    func setupVideo() {
        agoraKit.setExternalVideoSource(true, useTexture: true, pushMode: true)
        agoraKit.enableVideo()
        agoraKit.setVideoProfile(.landscape360P, swapWidthAndHeight: false)
    }
    
    func startVideoCapture() {
        videoCapture = MyVideoCapture(delegate: self, videoView: localVideo)
        videoCapture.startCapture(ofCamera: Camera.defaultCamera())
    }
    
    func joinChannel() {
        agoraKit.joinChannel(byToken: nil, channelId: "demoChannel", info: nil, uid: 0) { [unowned self] _ in
            self.agoraKit.setEnableSpeakerphone(true)
            UIApplication.shared.isIdleTimerDisabled = true
        }
    }
    
    @IBAction func didClickHangUpButton(_ sender: UIButton) {
        leaveChannel()
    }
    
    func leaveChannel() {
        videoCapture.stopCapture()
        agoraKit.leaveChannel(nil)
        
        hideControlButtons()
        UIApplication.shared.isIdleTimerDisabled = false
        remoteVideo.removeFromSuperview()
        localVideo.removeFromSuperview()
        
        agoraKit = nil
    }

    func hideControlButtons() {
        controlButtons.isHidden = true
    }
    
    @IBAction func didClickMuteButton(_ sender: UIButton) {
        sender.isSelected = !sender.isSelected
        agoraKit.muteLocalAudioStream(sender.isSelected)
    }
    
    @IBAction func didClickVideoMuteButton(_ sender: UIButton) {
        sender.isSelected = !sender.isSelected
        agoraKit.muteLocalVideoStream(sender.isSelected)
        localVideo.isHidden = sender.isSelected
        localVideoMutedBg.isHidden = !sender.isSelected
        localVideoMutedIndicator.isHidden = !sender.isSelected
    }
    
    func hideVideoMuted() {
        remoteVideoMutedIndicator.isHidden = true
        localVideoMutedBg.isHidden = true
        localVideoMutedIndicator.isHidden = true
    }
    
    @IBAction func didClickSwitchCameraButton(_ sender: UIButton) {
        sender.isSelected = !sender.isSelected
        videoCapture.switchCamera()
    }
}

extension VideoChatViewController: AgoraRtcEngineDelegate {
    func rtcEngine(_ engine: AgoraRtcEngineKit, firstRemoteVideoDecodedOfUid uid:UInt, size:CGSize, elapsed:Int) {
        if (remoteVideo.isHidden) {
            remoteVideo.isHidden = false
        }
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        videoCanvas.view = remoteVideo
        videoCanvas.renderMode = .adaptive
        agoraKit.setupRemoteVideo(videoCanvas)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid:UInt, reason:AgoraUserOfflineReason) {
        self.remoteVideo.isHidden = true
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didVideoMuted muted:Bool, byUid:UInt) {
        remoteVideo.isHidden = muted
        remoteVideoMutedIndicator.isHidden = !muted
    }
}

extension VideoChatViewController: MyVideoCaptureDelegate {
    func myVideoCapture(_ capture: MyVideoCapture, didOutputSampleBuffer pixelBuffer: CVPixelBuffer, rotation: Int, timeStamp: CMTime) {
        let videoFrame = AgoraVideoFrame()
        videoFrame.format = 12
        videoFrame.textureBuf = pixelBuffer
        videoFrame.time = timeStamp
        videoFrame.rotation = Int32(rotation)
        
        agoraKit?.pushExternalVideoFrame(videoFrame)
    }
}
