//
//  SwitchLiveController.swift
//  MediaPlayerKitQuickstart
//
//  Created by zhanxiaochao on 2019/8/14.
//  Copyright © 2019 Agora. All rights reserved.
//

import Cocoa

class SwitchLiveController: NSViewController {
    
    @IBOutlet weak var channelTField: NSTextField!
    @IBOutlet weak var uidTField: NSTextField!
    var channelName:String?
    var uid:String?
    var viewClosed:((_ channelName: String ,_ uid:String) -> ())?
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do view setup here.
    }
    override func viewDidAppear() {
        channelTField.stringValue = channelName!
        uidTField.stringValue = uid!
    }
    override func viewDidDisappear() {
        print("view didDisappear")
        viewClosed!(channelTField.stringValue,uidTField.stringValue)
    }
}
