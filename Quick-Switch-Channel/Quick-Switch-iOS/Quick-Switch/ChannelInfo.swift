//
//  Channel.swift
//  Quick-Switch
//
//  Created by GongYuhua on 2019/2/25.
//  Copyright © 2019 Agora. All rights reserved.
//

import UIKit

struct ChannelInfo {
    let channelName: String
    let cover: UIImage?
}

extension ChannelInfo {
    static func AllChannelList() -> [ChannelInfo] {
        var channels = [ChannelInfo]()
        for index in 1..<5 {
            let channel = ChannelInfo(
                channelName: "channel\(index)",
                cover: UIImage(named: "avatar-\(index)")
            )
            channels.append(channel)
        }
        return channels
    }
}
