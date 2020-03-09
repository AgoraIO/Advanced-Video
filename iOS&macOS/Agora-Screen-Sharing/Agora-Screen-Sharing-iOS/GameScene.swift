//
//  GameScene.swift
//  Agora-Screen-Sharing-iOS
//
//  Created by GongYuhua on 2017/1/3.
//  Copyright © 2016年 Agora. All rights reserved.
//

import SpriteKit
import AVFoundation

func + (left: CGPoint, right: CGPoint) -> CGPoint {
    return CGPoint(x: left.x + right.x, y: left.y + right.y)
}

func - (left: CGPoint, right: CGPoint) -> CGPoint {
    return CGPoint(x: left.x - right.x, y: left.y - right.y)
}

func * (point: CGPoint, scalar: CGFloat) -> CGPoint {
    return CGPoint(x: point.x * scalar, y: point.y * scalar)
}

func / (point: CGPoint, scalar: CGFloat) -> CGPoint {
    return CGPoint(x: point.x / scalar, y: point.y / scalar)
}

#if !(arch(x86_64) || arch(arm64))
    fileprivate func sqrt(a: CGFloat) -> CGFloat {
        return CGFloat(sqrtf(Float(a)))
    }
#endif

extension CGPoint {
    func length() -> CGFloat {
        return sqrt(x*x + y*y)
    }
    
    func normalized() -> CGPoint {
        return self / length()
    }
}

class GameScene: SKScene {
    
    fileprivate let ship = SKSpriteNode(imageNamed: "spaceship")
    fileprivate var backgroundPlayer: AVAudioPlayer?
    
    override func didMove(to view: SKView) {
        backgroundColor = SKColor.white
        
        ship.position = CGPoint(x: size.width * 0.5, y: size.height * 0.1)
        addChild(ship)
        
        run(SKAction.repeatForever(
            SKAction.sequence([
                SKAction.run(addDust),
                SKAction.wait(forDuration: 1.0)
            ])
        ))
    }
    
    override func touchesEnded(_ touches: Set<UITouch>, with event: UIEvent?) {
        if let touchLocation = touches.first?.location(in: self) {
            fireStar(to: touchLocation)
        }
    }
    
    func loadBackGroundMusic() {
        if let fileURL = Bundle.main.url(forResource: "space", withExtension: "mp3"),
            let player = try? AVAudioPlayer(contentsOf: fileURL) {
            player.numberOfLoops = -1
            player.play()
            backgroundPlayer = player
        }
    }
}

private extension GameScene {
    func addDust() {
        let dust = SKSpriteNode(imageNamed: "dust")
        
        let actualX = random(min: dust.size.width / 2, max: size.width - dust.size.width / 2)
        dust.position = CGPoint(x: actualX, y: size.height + dust.size.width / 2)
        dust.zPosition = -2
        addChild(dust)
        
        let actualDuration = random(min: CGFloat(2), max: CGFloat(4))
        
        let actionMove = SKAction.move(to: CGPoint(x: actualX, y: -dust.size.height / 2), duration: TimeInterval(actualDuration))
        let actionMoveDone = SKAction.removeFromParent()
        
        dust.run(SKAction.sequence([actionMove, actionMoveDone]))
    }
    
    func fireStar(to location: CGPoint) {
        let star = SKSpriteNode(imageNamed: "star")
        star.position = ship.position + CGPoint(x: 0, y: 5)
        
        let offset = location - star.position
        if (offset.y < 0) {
            return
        }
        star.zPosition = -1
        addChild(star)
        
        let direction = offset.normalized()
        let shootAmount = direction * 1000
        let realDest = shootAmount + star.position
        
        let actionMove = SKAction.move(to: realDest, duration: 1.5)
        let rotation = SKAction.rotate(byAngle: 18, duration: 1.5)
        let actionMoveDone = SKAction.removeFromParent()
        
        star.run(SKAction.sequence([SKAction.group([actionMove, rotation]), actionMoveDone]))
        run(SKAction.playSoundFileNamed("shoot.mp3", waitForCompletion: false))
    }
}

private extension GameScene {
    func random(min: CGFloat, max: CGFloat) -> CGFloat {
        return CGFloat.random(in: min...max)
    }
}
