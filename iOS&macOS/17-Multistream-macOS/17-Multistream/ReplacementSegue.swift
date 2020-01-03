//
//  ReplacementSegue.swift
//  17-Multistream
//
//  Created by ZhangJi on 30/09/2017.
//  Copyright © 2017 Agora. All rights reserved.
//

import Cocoa

class ReplacementSegue: NSStoryboardSegue {
    override func perform() {
        (sourceController as AnyObject).view.window?.contentViewController = destinationController as? NSViewController
    }
}
