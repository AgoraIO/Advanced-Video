//
//  ChannelViewController.swift
//  Quick-Switch
//
//  Created by GongYuhua on 2019/2/25.
//  Copyright © 2019 Agora. All rights reserved.
//

import UIKit

protocol ChannelViewControllerDelegate: NSObjectProtocol {
    func channelVCNeedClose(_ channelVC: ChannelViewController)
}

class ChannelViewController: UIViewController {

    @IBOutlet weak var hostLabel: UILabel!
    @IBOutlet weak var channelLabel: UILabel!
    @IBOutlet weak var coverView: UIView!
    @IBOutlet weak var coverImageView: UIImageView!
    @IBOutlet weak var spinner: UIActivityIndicatorView!
    @IBOutlet weak var hostRenderView: UIView!
    
    var channel: ChannelInfo!
    var index = 0
    weak var delegate: ChannelViewControllerDelegate?
    
    override func viewDidLoad() {
        super.viewDidLoad()
        updateView(with: channel)
    }
    
    @IBAction func doClosePressed(_ sender: UIButton) {
        delegate?.channelVCNeedClose(self)
    }
    
    func updateView(with channel: ChannelInfo) {
        channelLabel.text = "Channel: \(channel.channelName)"
        coverImageView.image = channel.cover
    }
    
    func showCover(_ shouldShow: Bool) {
        coverView.isHidden = !shouldShow
    }
    
    func showLoading(_ shouldShow: Bool) {
        if shouldShow {
            spinner.startAnimating()
        } else {
            spinner.stopAnimating()
        }
    }
}
