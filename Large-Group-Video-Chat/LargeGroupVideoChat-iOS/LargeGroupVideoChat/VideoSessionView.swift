//
//  VideoSessionView.swift
//  LargeGroupVideoChat
//
//  Created by ZhangJi on 30/09/2017.
//  Copyright © 2017 Agora. All rights reserved.
//

import UIKit
import AgoraRtcEngineKit

protocol VideoSessionViewDelegate: NSObjectProtocol {
    func videoSessionView(_ view: VideoSessionView, shouldMute: Bool)
}

class VideoSessionView: UIView {

    @IBOutlet weak var hostingView: UIView!
    @IBOutlet weak var unMuteButton: UIButton!
    @IBOutlet weak var muteButton: UIButton!
    @IBOutlet weak var userLabel: UILabel!
    @IBOutlet weak var holdImageView: UIImageView!
    
    var uid: UInt!
    var canvas: AgoraRtcVideoCanvas!
    weak var delegate: VideoSessionViewDelegate?
    
    var isMute: Bool = true {
        didSet {
            if self.uid == 0 {return}
            DispatchQueue.main.async(execute: {
                self.unMuteButton.isHidden = !self.isMute
                self.unMuteButton.isEnabled = self.isMute
                self.muteButton.isHidden = self.isMute
                self.muteButton.isEnabled = !self.isMute
                self.userLabel.isHidden = !self.isMute
                self.holdImageView.isHidden = !self.isMute
            })
        }
    }
    
    static func newInstance(uid: UInt) -> VideoSessionView {
        let nibView = Bundle.main.loadNibNamed("VideoSessionView", owner: nil, options: nil)
        let view = nibView?.first as! VideoSessionView
        
        view.uid = uid
        view.userLabel.text = String(uid)
        view.canvas = AgoraRtcVideoCanvas()
        view.canvas.uid = uid
        view.canvas.view = view.hostingView
        view.canvas.renderMode = .hidden
        return view
    }
    
    init(frame: CGRect, uid: UInt) {
        super.init(frame: frame)
        self.uid = uid
        canvas = AgoraRtcVideoCanvas()
        canvas.uid = uid
        canvas.view = self.hostingView
        canvas.renderMode = .fit
    }
    
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
    }
    
    @IBAction func tapUnMuteButton(_ sender: UIButton) {
        delegate?.videoSessionView(self, shouldMute: false)
    }
    
    @IBAction func tapMuteButton(_ sender: UIButton) {
        delegate?.videoSessionView(self, shouldMute: true)
    }
}
