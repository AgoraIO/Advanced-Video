//
//  ChannelCell.swift
//  Quick-Switch
//
//  Created by GongYuhua on 2019/2/25.
//  Copyright © 2019 Agora. All rights reserved.
//

import UIKit

class ChannelCell: UITableViewCell {
    
    @IBOutlet weak var nameLabel: UILabel!
    
    func updateCell(with channel: ChannelInfo) {
        nameLabel.text = "\(channel.channelName)"
    }
}
