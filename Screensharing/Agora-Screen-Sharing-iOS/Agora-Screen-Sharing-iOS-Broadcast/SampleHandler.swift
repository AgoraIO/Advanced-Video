//
//  SampleHandler.swift
//  Agora-Screen-Sharing-iOS-Broadcast
//
//  Created by GongYuhua on 2017/8/1.
//  Copyright © 2017年 Agora. All rights reserved.
//

import ReplayKit

class SampleHandler: RPBroadcastSampleHandler {

    override func broadcastStarted(withSetupInfo setupInfo: [String : NSObject]?) {
        
        if let setupInfo = setupInfo, let channel = setupInfo["channelName"] as? String {
            //In-App Screen Capture
            AgoraUploader.startBroadcast(to: channel)
        } else {
            //iOS Screen Record and Broadcast
            AgoraUploader.startBroadcast(to: "channel")
        }
    }
    
    override func broadcastPaused() {
        // User has requested to pause the broadcast. Samples will stop being delivered.
    }
    
    override func broadcastResumed() {
        // User has requested to resume the broadcast. Samples delivery will resume.
    }
    
    override func broadcastFinished() {
        AgoraUploader.stopBroadcast()
    }
    
    override func processSampleBuffer(_ sampleBuffer: CMSampleBuffer, with sampleBufferType: RPSampleBufferType) {
        DispatchQueue.main.async {
            switch sampleBufferType {
            case .video:
                AgoraUploader.sendVideoBuffer(sampleBuffer)
            case .audioApp:
                AgoraUploader.sendAudioAppBuffer(sampleBuffer)
            case .audioMic:
                AgoraUploader.sendAudioMicBuffer(sampleBuffer)
            @unknown default:
                break
            }
        }
    }
}
