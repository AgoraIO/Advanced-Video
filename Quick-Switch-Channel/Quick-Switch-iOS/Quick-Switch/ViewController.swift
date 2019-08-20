//
//  ViewController.swift
//  Quick-Switch
//
//  Created by GongYuhua on 2019/2/25.
//  Copyright © 2019 Agora. All rights reserved.
//

import UIKit

class ViewController: UIViewController {

    @IBOutlet weak var channelsTableView: UITableView!
    
    lazy var channels: [ChannelInfo] = ChannelInfo.AllChannelList()
    
    override func viewDidDisappear(_ animated: Bool) {
        super.viewDidDisappear(animated)
        if let selectedIndexPath = channelsTableView.indexPathForSelectedRow {
            channelsTableView.deselectRow(at: selectedIndexPath, animated: false)
        }
    }
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        guard let pageVC = segue.destination as? ChannelPageViewController,
            let index = sender as? Int else {
            return
        }
        pageVC.channels = channels
        pageVC.currentIndex = index
    }
    
    func enterChannel(at index: Int) {
        performSegue(withIdentifier: "enterPageVC", sender: index)
    }
}

extension ViewController: UITableViewDataSource {
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return channels.count
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: "channelCell", for: indexPath) as! ChannelCell
        cell.updateCell(with: channels[indexPath.row])
        return cell
    }
}

extension ViewController: UITableViewDelegate {
    func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        enterChannel(at: indexPath.row)
    }
}
