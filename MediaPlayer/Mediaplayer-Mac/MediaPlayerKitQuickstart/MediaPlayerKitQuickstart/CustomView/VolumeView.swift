//
//  VolumeView.swift
//  MediaPlayerKitQuickstart
//
//  Created by zhanxiaochao on 2019/8/14.
//  Copyright © 2019 Agora. All rights reserved.
//

import Cocoa

class VolumeView: NSView {
    @IBOutlet weak var vol1: NSButton!
    @IBOutlet weak var vol2: NSButton!
    @IBOutlet weak var vol3: NSButton!
    @IBOutlet weak var vol4: NSButton!
    @IBOutlet weak var vol5: NSButton!
    @IBOutlet weak var vol6: NSButton!
    @IBOutlet weak var vol7: NSButton!
    @IBOutlet weak var vol8: NSButton!
    @IBOutlet weak var vol9: NSButton!
    @IBOutlet weak var vol10: NSButton!
    var volumeLevelCallback:((_ volLevel:Float ) -> ())?
    override func draw(_ dirtyRect: NSRect) {
        super.draw(dirtyRect)
        
        // Drawing code here.
    }
    
    @IBAction func level1Click(_ sender: NSButton) {
        print(sender.tag)
        switch sender.tag {
        case 1:
            vol1.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol2.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            vol3.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            vol4.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            vol5.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            vol6.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            vol7.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            vol8.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            vol9.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            vol10.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            if (volumeLevelCallback != nil){
                volumeLevelCallback!(0.1)
            }
            break
        case 2:
            vol1.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol2.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol3.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            vol4.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            vol5.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            vol6.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            vol7.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            vol8.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            vol9.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            vol10.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            if (volumeLevelCallback != nil){
                volumeLevelCallback!(0.2)
            }
            break
        case 3:
            vol1.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol2.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol3.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol4.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            vol5.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            vol6.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            vol7.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            vol8.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            vol9.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            vol10.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            if (volumeLevelCallback != nil){
                volumeLevelCallback!(0.3)
            }
            break
        case 4:
            vol1.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol2.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol3.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol4.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol5.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            vol6.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            vol7.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            vol8.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            vol9.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            vol10.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            if (volumeLevelCallback != nil){
                volumeLevelCallback!(0.4)
            }
            break
        case 5:
            vol1.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol2.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol3.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol4.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol5.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol6.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            vol7.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            vol8.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            vol9.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            vol10.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            if (volumeLevelCallback != nil){
                volumeLevelCallback!(0.5)
            }
            break
        case 6:
            vol1.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol2.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol3.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol4.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol5.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol6.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol7.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            vol8.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            vol9.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            vol10.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            if (volumeLevelCallback != nil){
                volumeLevelCallback!(0.6)
            }
            break
        case 7:
            vol1.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol2.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol3.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol4.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol5.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol6.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol7.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol8.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            vol9.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            vol10.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            if (volumeLevelCallback != nil){
                volumeLevelCallback!(0.7)
            }
            break
        case 8:
            vol1.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol2.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol3.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol4.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol5.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol6.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol7.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol8.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol9.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            vol10.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            if (volumeLevelCallback != nil){
                volumeLevelCallback!(0.8)
            }
            break
        case 9:
            vol1.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol2.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol3.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol4.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol5.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol6.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol7.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol8.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol9.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol10.image = NSImage.init(named: NSImage.Name(rawValue: "volume_not_filled"))
            if (volumeLevelCallback != nil){
                volumeLevelCallback!(0.9)
            }
            break
        case 10:
            vol1.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol2.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol3.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol4.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol5.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol6.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol7.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol8.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol9.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            vol10.image = NSImage.init(named: NSImage.Name(rawValue: "volume_filled"))
            if (volumeLevelCallback != nil){
                volumeLevelCallback!(1.0)
            }
            break
        default:
            break
        }
    }
    
}


