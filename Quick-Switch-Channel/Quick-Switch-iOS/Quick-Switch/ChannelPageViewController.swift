//
//  ChannelPageViewController.swift
//  Quick-Switch
//
//  Created by GongYuhua on 2019/2/25.
//  Copyright © 2019 Agora. All rights reserved.
//

import UIKit

class ChannelPageViewController: UIPageViewController {

    var channels = [ChannelInfo]()
    var currentIndex = 0
    lazy var mediaCenter: MediaCenter = MediaCenter(delegate: self)
    
    override func viewDidLoad() {
        super.viewDidLoad()

        delegate = self
        dataSource = self
        
        if let vc = channelViewController(at: currentIndex) {
            setViewControllers([vc], direction: .forward, animated: false, completion: nil)
        }
    }
    
    override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(animated)
        guard let vc = viewControllers?.first as? ChannelViewController else {
            return
        }
        vc.showLoading(true)
        mediaCenter.joinChannel(vc.channel, renderView: vc.hostRenderView)
    }
}

private extension ChannelPageViewController {
    func channelViewController(at index: Int) -> ChannelViewController? {
        guard channels.count > 0 else {
            return nil
        }
        
        var vcIndex = index
        if vcIndex >= channels.count {
            vcIndex = 0
        } else if vcIndex < 0 {
            vcIndex = channels.count - 1
        }
        
        let channelVC = ChannelViewController(nibName: "ChannelViewController", bundle: nil)
        channelVC.index = vcIndex
        channelVC.channel = channels[vcIndex]
        channelVC.delegate = self
        
        return channelVC
    }
}

extension ChannelPageViewController: UIPageViewControllerDelegate {
    func pageViewController(_ pageViewController: UIPageViewController, didFinishAnimating finished: Bool, previousViewControllers: [UIViewController], transitionCompleted completed: Bool) {
        guard let previousVC = previousViewControllers.last as? ChannelViewController,
            let currentVC = pageViewController.viewControllers?.first as? ChannelViewController,
            previousVC != currentVC else {
            return
        }
        
        previousVC.showCover(true)
        currentVC.showLoading(true)
        mediaCenter.cleanupChannel(previousVC.channel)
        mediaCenter.joinChannel(currentVC.channel, renderView: currentVC.hostRenderView)
    }
}

extension ChannelPageViewController: MediaCenterDelegate {
    func mediaCenter(_ center: MediaCenter, didJoinChannel channel: String) {
        guard let currentVC = viewControllers?.first as? ChannelViewController,
            currentVC.channel.channelName == channel else {
            return
        }
        
        currentVC.showLoading(false)
    }
    
    func mediaCenter(_ center: MediaCenter, didRemoteVideoDecoded channel: String) {
        guard let currentVC = viewControllers?.first as? ChannelViewController,
            currentVC.channel.channelName == channel else {
                return
        }
        currentVC.showCover(false)
    }
}

extension ChannelPageViewController: ChannelViewControllerDelegate {
    func channelVCNeedClose(_ channelVC: ChannelViewController) {
        mediaCenter.cleanupChannel(channelVC.channel)
        mediaCenter.leaveChannel(channelVC.channel)
        navigationController?.popViewController(animated: true)
    }
}

extension ChannelPageViewController: UIPageViewControllerDataSource {
    func pageViewController(_ pageViewController: UIPageViewController, viewControllerAfter viewController: UIViewController) -> UIViewController? {
        guard let channelVC = viewController as? ChannelViewController else {
            return nil
        }
        return channelViewController(at: channelVC.index + 1)
    }
    
    func pageViewController(_ pageViewController: UIPageViewController, viewControllerBefore viewController: UIViewController) -> UIViewController? {
        guard let channelVC = viewController as? ChannelViewController else {
            return nil
        }
        return channelViewController(at: channelVC.index - 1)
    }
}
