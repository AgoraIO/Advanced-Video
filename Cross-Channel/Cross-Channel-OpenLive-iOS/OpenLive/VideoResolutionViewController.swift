//
//  VideoResolutionViewController.swift
//  OpenLive
//
//  Created by CavanSu on 2019/5/10.
//  Copyright © 2019 Agora. All rights reserved.
//

import UIKit

struct VideoConfig {
    var width: Double
    var height: Double
    var frameRate: Int
    var bitrate: Int
    
    init(width: Double, height: Double, frameRate: Int, bitrate: Int) {
        self.width = width
        self.height = height
        self.bitrate = bitrate
        self.frameRate = frameRate
    }
}

protocol VideoResolutionVCDelegate: NSObjectProtocol {
    func videoResolutionSelected(_ config: VideoConfig)
    func vdeoResolutionCancelSelected()
}

class VideoResolutionViewController: UIViewController {
    @IBOutlet weak var widthTextField: UITextField!
    @IBOutlet weak var heightTextField: UITextField!
    @IBOutlet weak var frameRateTextField: UITextField!
    @IBOutlet weak var bitrateTextField: UITextField!
    
    weak var delegate: VideoResolutionVCDelegate?
    
    override func viewDidLoad() {
        super.viewDidLoad()
        read()
    }

    @IBAction func doSurePressed(_ sender: UIButton) {
        let width = Double(widthTextField.text ?? "0")!
        let height = Double(heightTextField.text ?? "0")!
        let frameRate = Int(frameRateTextField.text ?? "0")!
        let bitrate = Int(bitrateTextField.text ?? "0")!
        
        let config = VideoConfig(width: width, height: height, frameRate: frameRate, bitrate: bitrate)
        
        delegate?.videoResolutionSelected(config)
        write()
        dismiss(animated: true, completion: nil)
    }
    
    @IBAction func doCancelPressed(_ sender: UIButton) {
        delegate?.vdeoResolutionCancelSelected()
        
        dismiss(animated: true, completion: nil)
    }
    
    func read() {
        widthTextField.text = DefaultValue.getStringValueFor(key: "widthTextField")
        heightTextField.text = DefaultValue.getStringValueFor(key: "heightTextField")
        frameRateTextField.text = DefaultValue.getStringValueFor(key: "frameRateTextField")
        bitrateTextField.text = DefaultValue.getStringValueFor(key: "bitrateTextField")
    }
    
    func write() {
        if let text = widthTextField.text, text.count > 0 {
            DefaultValue.setValue(text, key: "widthTextField")
        }
        
        if let text = heightTextField.text, text.count > 0 {
            DefaultValue.setValue(text, key: "heightTextField")
        }
        
        if let text = frameRateTextField.text, text.count > 0 {
            DefaultValue.setValue(text, key: "frameRateTextField")
        }
        
        if let text = bitrateTextField.text, text.count > 0 {
            DefaultValue.setValue(text, key: "bitrateTextField")
        }
    }
}
