//
//  AgoraUploader.swift
//  Agora-Screen-Sharing-iOS-Broadcast
//
//  Created by GongYuhua on 2017/1/16.
//  Copyright © 2017年 Agora. All rights reserved.
//

import Foundation
import CoreMedia
import ReplayKit

class AgoraUploader {
    private static let videoDimension : CGSize = {
        let width : CGFloat
        let height : CGFloat
        let screenSize = UIScreen.main.currentMode!.size
        if screenSize.width <= screenSize.height {
            if screenSize.width * 16 <= screenSize.height * 9 {
                width = 640 * screenSize.width / screenSize.height
                height = 640
            }
            else {
                width = 360
                height = 360 * screenSize.height / screenSize.width
            }
        }
        else {
            if screenSize.width * 9 <= screenSize.height * 16 {
                width = 360 * screenSize.width / screenSize.height
                height = 360
            }
            else {
                width = 640
                height = 640 * screenSize.height / screenSize.width
            }
        }
        
        return CGSize(width: width, height: height)
    }()
    
    private static let sharedAgoraEngine: AgoraRtcEngineKit = {
        let kit = AgoraRtcEngineKit.sharedEngine(withAppId: KeyCenter.AppId, delegate: nil)
        kit.setChannelProfile(.liveBroadcasting)
        kit.setClientRole(.broadcaster)
        
        kit.enableVideo()
        kit.setExternalVideoSource(true, useTexture: true, pushMode: true)
        
        let videoConfig = AgoraVideoEncoderConfiguration(size: videoDimension, frameRate: .fps15, bitrate: 400, orientationMode: .adaptative)
        kit.setVideoEncoderConfiguration(videoConfig)
        
        // disable hardware encoding, as extension can not access the hardware
        kit.setParameters("{\"che.hardware_encoding\":0}")
        
        // enable compact memory mode, as extension has a memory limit to 50M
        kit.setParameters("{\"che.video.compact_memory\":true}")
        
        AgoraAudioProcessing.registerAudioPreprocessing(kit)
        kit.setRecordingAudioFrameParametersWithSampleRate(44100, channel: 1, mode: .readWrite, samplesPerCall: 1024)
        kit.setParameters("{\"che.audio.external_device\":true}")
        
        kit.muteAllRemoteVideoStreams(true)
        kit.muteAllRemoteAudioStreams(true)
        
        return kit
    }()
    
    static func startBroadcast(to channel: String) {
        sharedAgoraEngine.joinChannel(byToken: nil, channelId: channel, info: nil, uid: 0, joinSuccess: nil)
    }
    
    static func sendVideoBuffer(_ sampleBuffer: CMSampleBuffer) {
        guard let videoFrame = CMSampleBufferGetImageBuffer(sampleBuffer)
             else {
            return
        }
        
        var rotation : Int32 = 0
        if let orientationAttachment = CMGetAttachment(sampleBuffer, RPVideoSampleOrientationKey as CFString, nil) as? NSNumber {
            if let orientation = CGImagePropertyOrientation(rawValue: orientationAttachment.uint32Value) {
                switch orientation {
                case .up:
                    fallthrough
                case .upMirrored:
                    rotation = 0

                case .down:
                    fallthrough
                case .downMirrored:
                    rotation = 180

                case .left:
                    fallthrough
                case .leftMirrored:
                    rotation = 90

                case .right:
                    fallthrough
                case .rightMirrored:
                    rotation = 270
                }
            }
        }
        
        let time = CMSampleBufferGetPresentationTimeStamp(sampleBuffer)
        
        let frame = AgoraVideoFrame()
        frame.format = 12
        frame.time = time
        frame.textureBuf = videoFrame
        frame.rotation = rotation
        sharedAgoraEngine.pushExternalVideoFrame(frame)
    }
    
    static func sendAudioAppBuffer(_ sampleBuffer: CMSampleBuffer) {
        AgoraAudioProcessing.pushAudioAppBuffer(sampleBuffer)
    }
    
    static func sendAudioMicBuffer(_ sampleBuffer: CMSampleBuffer) {
        AgoraAudioProcessing.pushAudioMicBuffer(sampleBuffer)
    }
    
    static func stopBroadcast() {
        sharedAgoraEngine.leaveChannel(nil)
    }
}
