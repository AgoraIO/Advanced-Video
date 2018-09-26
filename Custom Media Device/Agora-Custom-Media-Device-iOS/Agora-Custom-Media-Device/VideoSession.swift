//
//  VideoSession.swift
//  Agora-Custom-Media-Device
//
//  Created by GongYuhua on 2017/12/19.
//  Copyright © 2017年 Agora.io All rights reserved.
//

import UIKit
import AgoraRtcEngineKit

class VideoSession: NSObject {
    var uid: Int64 = 0
    var hostingView: AgoraRenderView!
    
    init(uid: Int64) {
        self.uid = uid
        
        hostingView = AgoraRenderView(frame: CGRect(x: 0, y: 0, width: 100, height: 100))
        hostingView.translatesAutoresizingMaskIntoConstraints = false
    }
}

extension VideoSession {
    static func localSession() -> VideoSession {
        return VideoSession(uid: 0)
    }
}
