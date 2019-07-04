//
//  ViewController.swift
//  Agora-Video-With-ARKit
//
//  Created by GongYuhua on 2017/12/27.
//  Copyright © 2017年 Agora.io All rights reserved.
//

import UIKit
import ARKit
import AgoraRtcEngineKit

class ViewController: UIViewController {

    @IBOutlet weak var sceneView: ARSCNView!
    
    fileprivate var coachingOverlay: UIView?
    
    fileprivate var agoraKit: AgoraRtcEngineKit!
    
    fileprivate let videoSource = ARVideoSource()
    
    fileprivate var unusedScreenNodes = [SCNNode]()
    fileprivate var undisplayedUsers = [UInt]()
    fileprivate var activeScreens = [UInt: SCNNode]()
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        sceneView.delegate = self
        sceneView.session.delegate = self
        sceneView.showsStatistics = true
        
        coachingOverlay = createCoachingOverlay()
        if let coachingOverlay = coachingOverlay {
            sceneView.addSubview(coachingOverlay)
        }
        
        agoraKit = AgoraRtcEngineKit.sharedEngine(withAppId: <#Your App Id#>, delegate: self)
        
        agoraKit.setChannelProfile(.liveBroadcasting)
        agoraKit.setClientRole(.broadcaster)
        
        agoraKit.setVideoEncoderConfiguration(
            AgoraVideoEncoderConfiguration(
                size: AgoraVideoDimension640x360,
                frameRate: .fps15,
                bitrate: AgoraVideoBitrateStandard,
                orientationMode: .adaptative
            )
        )
        agoraKit.enableVideo()
        agoraKit.setVideoSource(videoSource)
        
        agoraKit.enableExternalAudioSource(withSampleRate: 44100, channelsPerFrame: 1)
        
        agoraKit.joinChannel(byToken: nil, channelId: "agoraar", info: nil, uid: 0, joinSuccess: nil)
        
        UIApplication.shared.isIdleTimerDisabled = true
    }

    override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(animated)
        
        guard ARWorldTrackingConfiguration.isSupported else {
            showUnsupportedDeviceError()
            return
        }
        
        coachingOverlay?.center = sceneView.center
        
        let configuration = ARWorldTrackingConfiguration()
        configuration.planeDetection = .horizontal
        configuration.providesAudioData = true
        
        sceneView.session.run(configuration)
    }
    
    override func viewWillDisappear(_ animated: Bool) {
        super.viewWillDisappear(animated)
        sceneView.session.pause()
    }
    
    @IBAction func doSceneViewTapped(_ recognizer: UITapGestureRecognizer) {
        let location = recognizer.location(in: sceneView)
        
        if let node = sceneView.hitTest(location, options: nil).first?.node {
            removeNode(node)
        } else if let result = sceneView.hitTest(location, types: .existingPlane).first {
            addNode(withTransform: result.worldTransform)
        }
    }
}

private extension ViewController {
    func renderRemoteUser(uid: UInt, toNode node: SCNNode) {
        let renderer = ARVideoRenderer()
        renderer.renderNode = node
        activeScreens[uid] = node
        
        agoraKit.setRemoteVideoRenderer(renderer, forUserId: uid)
    }
    
    func addNode(withTransform transform: matrix_float4x4) {
        let scene = SCNScene(named: "art.scnassets/displayer.scn")!
        let rootNode = scene.rootNode
        
        rootNode.position = SCNVector3(
            transform.columns.3.x,
            transform.columns.3.y,
            transform.columns.3.z
        )
        rootNode.rotation = SCNVector4(0, 1, 0, sceneView.session.currentFrame!.camera.eulerAngles.y)
        
        sceneView.scene.rootNode.addChildNode(rootNode)
        
        let displayer = rootNode.childNode(withName: "displayer", recursively: false)!
        let screen = displayer.childNode(withName: "screen", recursively: false)!
        
        if let undisplayedUid = undisplayedUsers.first {
            undisplayedUsers.removeFirst()
            renderRemoteUser(uid: undisplayedUid, toNode: screen)
        } else {
            unusedScreenNodes.append(screen)
        }
    }
    
    func removeNode(_ node: SCNNode) {
        let rootNode: SCNNode
        let screen: SCNNode
        
        if node.name == "screen", let parent = node.parent?.parent {
            rootNode = parent
            screen = node
        } else if node.name == "displayer", let parent = node.parent {
            rootNode = parent
            screen = parent.childNode(withName: "screen", recursively: false)!
        } else {
            rootNode = node
            screen = node
        }
        
        rootNode.removeFromParentNode()
        
        if let index = unusedScreenNodes.index(where: {$0 == screen}) {
            unusedScreenNodes.remove(at: index)
        }
        
        if let (uid, _) = activeScreens.first(where: {$1 == screen}) {
            activeScreens.removeValue(forKey: uid)
            if let screenNode = unusedScreenNodes.first {
                unusedScreenNodes.removeFirst()
                renderRemoteUser(uid: uid, toNode: screenNode)
            } else {
                undisplayedUsers.insert(uid, at: 0)
            }
        }
    }
    
    func createCoachingOverlay() -> UIView? {
        guard #available(iOS 13.0, *) else {
            return nil
        }
        
        let coachingOverlay = ARCoachingOverlayView()
        coachingOverlay.session = sceneView.session
        coachingOverlay.goal = .horizontalPlane
        
        return coachingOverlay
    }
    
    func showUnsupportedDeviceError() {
        // This device does not support 6DOF world tracking.
        let alertController = UIAlertController(
            title: "ARKit is not available on this device.",
            message: "This app requires world tracking, which is available only on iOS devices with the A9 processor or later.",
            preferredStyle: .alert
        )
        
        alertController.addAction(UIAlertAction(title: "OK", style: .default, handler: nil))
        
        present(alertController, animated: true, completion: nil)
    }
}

extension ViewController: ARSCNViewDelegate {
    func renderer(_ renderer: SCNSceneRenderer, didAdd node: SCNNode, for anchor: ARAnchor) {
        guard let planeAnchor = anchor as? ARPlaneAnchor else {
            return
        }
        
        let plane = SCNBox(width: CGFloat(planeAnchor.extent.x),
                           height: CGFloat(planeAnchor.extent.y),
                           length: CGFloat(planeAnchor.extent.z),
                           chamferRadius: 0)
        plane.firstMaterial?.diffuse.contents = UIColor.red
        
        let planeNode = SCNNode(geometry: plane)
        node.addChildNode(planeNode)
        planeNode.runAction(SCNAction.fadeOut(duration: 1))
    }
}

extension ViewController: ARSessionDelegate {
    func session(_ session: ARSession, didUpdate frame: ARFrame) {
        videoSource.sendBuffer(frame.capturedImage, timestamp: frame.timestamp)
    }
    
    func session(_ session: ARSession, didOutputAudioSampleBuffer audioSampleBuffer: CMSampleBuffer) {
        agoraKit.pushExternalAudioFrameSampleBuffer(audioSampleBuffer)
    }
}

extension ViewController: AgoraRtcEngineDelegate {
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        print("error: \(errorCode.rawValue)")
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurWarning warningCode: AgoraWarningCode) {
        print("warning: \(warningCode.rawValue)")
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        print("did join channel with uid:\(uid)")
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didRejoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        print("did rejoin channel")
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        print("did joined of uid: \(uid)")
        
        if let screenNode = unusedScreenNodes.first {
            unusedScreenNodes.removeFirst()
            renderRemoteUser(uid: uid, toNode: screenNode)
        } else {
            undisplayedUsers.append(uid)
        }
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        print("did offline of uid: \(uid)")
        
        if let screenNode = activeScreens[uid] {
            agoraKit.setRemoteVideoRenderer(nil, forUserId: uid)
            unusedScreenNodes.insert(screenNode, at: 0)
            activeScreens[uid] = nil
        } else if let index = undisplayedUsers.index(of: uid) {
            undisplayedUsers.remove(at: index)
        }
    }
}
