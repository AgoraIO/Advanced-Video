//
//  VideoSessionView.swift
//  LargeGroupVideoChat
//
//  Created by ZhangJi on 30/09/2017.
//  Copyright © 2017 Agora. All rights reserved.
//

import UIKit
import AgoraRtcKit

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
    
    var isMute: Bool = false {
        didSet {
            guard self.isNoNeedSubViews == false else {
                return
            }
            
            self.delegate?.videoSessionView(self, shouldMute: self.isMute)
            
            self.unMuteButton.isHidden = !self.isMute
            self.unMuteButton.isEnabled = self.isMute
            self.muteButton.isHidden = self.isMute
            self.muteButton.isEnabled = !self.isMute
            self.userLabel.isHidden = !self.isMute
            self.holdImageView.isHidden = !self.isMute
        }
    }
    
    var isNoNeedSubViews: Bool = false {
        didSet {
            self.unMuteButton.isHidden = isNoNeedSubViews
            self.muteButton.isHidden = isNoNeedSubViews
            self.userLabel.isHidden = isNoNeedSubViews
            self.holdImageView.isHidden = isNoNeedSubViews
        }
    }
    
    static func newInstance(uid: UInt) -> VideoSessionView {
        let nibView = Bundle.main.loadNibNamed("VideoSessionView", owner: nil, options: nil)
        let view = nibView?.first as! VideoSessionView
        view.isMute = false
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
        self.isMute = false
        canvas = AgoraRtcVideoCanvas()
        canvas.uid = uid
        canvas.view = self.hostingView
        canvas.renderMode = .fit
    }
    
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
    }
    
    @IBAction func tapUnMuteButton(_ sender: UIButton) {
        isMute = false
        
    }
    
    @IBAction func tapMuteButton(_ sender: UIButton) {
        isMute = true
    }
}
