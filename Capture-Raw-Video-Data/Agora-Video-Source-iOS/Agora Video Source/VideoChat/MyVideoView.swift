//
//  MyVideoView.swift
//  Agora Video Source
//
//  Created by GongYuhua on 2017/4/11.
//  Copyright © 2017年 Agora. All rights reserved.
//

import UIKit
import AVFoundation

class MyVideoView: UIView {
    
    private var previewLayer: AVCaptureVideoPreviewLayer?
    
    func insertCaptureVideoPreviewLayer(previewLayer: AVCaptureVideoPreviewLayer) {
        self.previewLayer?.removeFromSuperlayer()
        
        previewLayer.frame = bounds
        layer.insertSublayer(previewLayer, below: layer.sublayers?.first)
        self.previewLayer = previewLayer
    }
    
    override func layoutSublayers(of layer: CALayer) {
        super.layoutSublayers(of: layer)
        previewLayer?.frame = bounds
    }
}
