//
//  VideoSession.swift
//  17-Multistream
//
//  Created by ZhangJi on 30/09/2017.
//  Copyright © 2017 Agora. All rights reserved.
//

import Cocoa
import AgoraRtcKit

class VideoSession: NSObject {
    var uid: UInt = 0
    var hostingView: NSView!
    var canvas: AgoraRtcVideoCanvas!
    
    init(uid: UInt) {
        self.uid = uid
        
        hostingView = NSView()
        hostingView.translatesAutoresizingMaskIntoConstraints = false
        
        canvas = AgoraRtcVideoCanvas()
        canvas.uid = UInt(uid)
        canvas.view = hostingView
        canvas.renderMode = .hidden
    }
}

extension VideoSession {
    static func localSession() -> VideoSession {
        return VideoSession(uid: 0)
    }
}

//MARK: - DummyView
class VideoDummyView: NSView {
    private static var Pool = [VideoDummyView]()
    
    class func reuseDummy() -> VideoDummyView {
        if Pool.count > 0 {
            return Pool.removeFirst()
        } else {
            return VideoDummyView(frame: CGRect(x: 0, y: 0, width: 1, height: 1))
        }
    }
    
    func backToPool() {
        VideoDummyView.Pool.append(self)
    }
    
    private override init(frame: CGRect) {
        super.init(frame: frame)
        loadDefaultSets()
    }
    
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
        loadDefaultSets()
    }
    
    private func loadDefaultSets() {
        translatesAutoresizingMaskIntoConstraints = false
    }
}
