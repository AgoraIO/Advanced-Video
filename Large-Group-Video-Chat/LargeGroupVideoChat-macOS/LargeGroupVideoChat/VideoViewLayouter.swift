//
//  VideoViewLayouter.swift
//  LargeGroupVideoChat
//
//  Created by ZhangJi on 30/09/2017.
//  Copyright © 2017 Agora. All rights reserved.
//

import Cocoa

class VideoViewLayouter {
    
    var containerView: NSView?
    var sessions = [VideoSession]()
    var fullSession: VideoSession?
    var targetSize = CGSize.zero
    
    fileprivate var layoutConstraints = [NSLayoutConstraint]()
    fileprivate var dummyViews = [VideoDummyView]()
}

extension VideoViewLayouter {
    @discardableResult
    func layoutVideoViews() -> [VideoSession] {
        var actualFullSessions = [VideoSession]()
        
        guard let containerView = containerView else {
            return actualFullSessions
        }
        
        clearConstraints()
        
        for session in sessions {
            session.hostingView.removeFromSuperview()
        }
        
        clearDummyViews()
        
        let sessionsCount = sessions.count
        if sessionsCount == 0 {
        } else if sessionsCount == 1 {
            // just on session
            actualFullSessions = sessions
            
            let view = sessions.first!.hostingView!
            containerView.addSubview(view)
            layoutConstraints.append(contentsOf: layout(fullView: view))
            
        } else {
            let fullView: NSView
            var smallViews = [NSView]()
            
            if let fullSession = fullSession {
                fullView = fullSession.hostingView!
                actualFullSessions = [fullSession]
                
                for session in sessions {
                    if session != fullSession {
                        smallViews.append(session.hostingView)
                    }
                }
            } else {
                fullView = sessions.first!.hostingView!
                actualFullSessions = [sessions.first!]
                
                for (index, session) in sessions.enumerated() {
                    if index != 0 {
                        smallViews.append(session.hostingView)
                    }
                }
            }
            
            let edgePosition: EdgePosition = (targetSize.width >= targetSize.height ? .right : .bottom)
            
            let MaxSmallViewWidth: CGFloat = 120
            let MinLargeViewRatio: CGFloat = (targetSize.width / targetSize.height >= 0.667 || targetSize.width / targetSize.height <= 1.5) ? 0.75 : 1
            var MaxPosibleCountPerLine = smallViews.count
            let largerSide: CGFloat
            let smallerSide: CGFloat
            if targetSize.width >= targetSize.height {
                largerSide = targetSize.width
                smallerSide = targetSize.height
            } else {
                largerSide = targetSize.height
                smallerSide = targetSize.width
            }
            MaxPosibleCountPerLine = max(MaxPosibleCountPerLine, Int(ceil(largerSide / MaxSmallViewWidth)))
            MaxPosibleCountPerLine = max(MaxPosibleCountPerLine, Int(ceil(smallerSide / (largerSide - smallerSide * MinLargeViewRatio))))
            
            var countPerLine = 1
            var lines = 1
            for count in 1..<(MaxPosibleCountPerLine+1) {
                countPerLine = count
                
                let smallWidth = smallerSide / CGFloat(count)
                if smallWidth > MaxSmallViewWidth {
                    continue
                }
                lines = Int(ceil(Double(smallViews.count) / Double(count)))
                if (largerSide - smallWidth * CGFloat(lines)) / smallerSide < MinLargeViewRatio {
                    continue
                }
                
                break
            }
            
            containerView.addSubview(fullView)
            setSmallViews(&smallViews, maxTo: (countPerLine * lines))
            for view in smallViews {
                containerView.addSubview(view)
            }
            
            layoutConstraints.append(contentsOf: layouts(ofSmallViews: smallViews, fullView: fullView, countPerLine: countPerLine, edgePosition: edgePosition))
            
        }
        
        if !layoutConstraints.isEmpty {
            NSLayoutConstraint.activate(layoutConstraints)
        }
        
        return actualFullSessions
    }
    
    func responseSession(of event: NSEvent, inSessions sessions: [VideoSession], inContainerView container: NSView) -> VideoSession? {
        let location = event.locationInWindow
        for session in sessions {
            if let view = session.hostingView , view.frame.contains(location) {
                return session
            }
        }
        return nil
    }
}

private enum EdgePosition {
    case bottom, right
}

private extension VideoViewLayouter {
    func setSmallViews(_ smallViews: inout [NSView], maxTo count: Int) {
        while smallViews.count < count {
            let dummyView = VideoDummyView.reuseDummy()
            smallViews.append(dummyView)
            dummyViews.append(dummyView)
        }
    }
    
    func layout(fullView: NSView) -> [NSLayoutConstraint] {
        var layoutConstraints = [NSLayoutConstraint]()
        
        let constraintsH = NSLayoutConstraint.constraints(withVisualFormat: "H:|[view]|", options: [], metrics: nil, views: ["view": fullView])
        let constraintsV = NSLayoutConstraint.constraints(withVisualFormat: "V:|[view]|", options: [], metrics: nil, views: ["view": fullView])
        layoutConstraints.append(contentsOf: constraintsH)
        layoutConstraints.append(contentsOf: constraintsV)
        
        return layoutConstraints
    }
    
    func layouts(ofSmallViews smallViews: [NSView], fullView: NSView, countPerLine: Int, edgePosition: EdgePosition) -> [NSLayoutConstraint] {
        
        var layoutConstraints = [NSLayoutConstraint]()
        
        let smallViewLineCounts: Int
        if countPerLine > 0 {
            smallViewLineCounts = Int(ceil(Double(smallViews.count) / Double(countPerLine)))
        } else {
            smallViewLineCounts = 0
        }
        
        for view in smallViews {
            let squar = NSLayoutConstraint(item: view, attribute: .width, relatedBy: .equal, toItem: view, attribute: .height, multiplier: 1, constant: 0)
            layoutConstraints.append(squar)
        }
        
        let constraintsFullViewA: [NSLayoutConstraint]
        switch edgePosition {
        case .bottom:
            constraintsFullViewA = NSLayoutConstraint.constraints(withVisualFormat: "H:|[view]|", options: [], metrics: nil, views: ["view": fullView])
        case .right:
            constraintsFullViewA = NSLayoutConstraint.constraints(withVisualFormat: "V:|[view]|", options: [], metrics: nil, views: ["view": fullView])
        }
        layoutConstraints.append(contentsOf: constraintsFullViewA)
        
        var constraintsFullViewBString: String
        switch edgePosition {
        case .bottom:   constraintsFullViewBString = "V:|[view]"
        case .right:    constraintsFullViewBString = "H:|[view]"
        }
        
        var constraintsFullViewBViewDic = ["view": fullView]
        for lineNumber in 0..<smallViewLineCounts {
            let viewKey = "view\(lineNumber)"
            constraintsFullViewBString.append("-1-[\(viewKey)]")
            constraintsFullViewBViewDic[viewKey] = smallViews[lineNumber * countPerLine]
        }
        constraintsFullViewBString.append("|")
        
        let constraintsFullViewB = NSLayoutConstraint.constraints(withVisualFormat: constraintsFullViewBString, options: [], metrics: nil, views: constraintsFullViewBViewDic)
        layoutConstraints.append(contentsOf: constraintsFullViewB)
        
        
        for lineNumber in 0..<smallViewLineCounts {
            
            let rulerIndex = lineNumber * countPerLine
            let rulerView = smallViews[rulerIndex]
            
            var constraintsSmallViewString: String
            switch edgePosition {
            case .bottom:   constraintsSmallViewString = "H:|[view]"
            case .right:    constraintsSmallViewString = "V:|[view]"
            }
            
            var constraintsSmallViewDic = ["view": smallViews[rulerIndex]]
            
            for index in (rulerIndex + 1)..<(rulerIndex + countPerLine) {
                if index >= smallViews.count {
                    break
                }
                
                let view = smallViews[index]
                
                let equalSize: NSLayoutConstraint
                let equalSide: NSLayoutConstraint
                switch edgePosition {
                case .bottom:
                    equalSize = NSLayoutConstraint(item: view, attribute: .width, relatedBy: .equal, toItem: rulerView, attribute: .width, multiplier: 1, constant: 0)
                    equalSide = NSLayoutConstraint(item: view, attribute: .bottom, relatedBy: .equal, toItem: rulerView, attribute: .bottom, multiplier: 1, constant: 0)
                default:
                    equalSize = NSLayoutConstraint(item: view, attribute: .height, relatedBy: .equal, toItem: rulerView, attribute: .height, multiplier: 1, constant: 0)
                    equalSide = NSLayoutConstraint(item: view, attribute: .trailing, relatedBy: .equal, toItem: rulerView, attribute: .trailing, multiplier: 1, constant: 0)
                }
                layoutConstraints.append(equalSize)
                layoutConstraints.append(equalSide)
                
                let viewKey = "view\(index)"
                constraintsSmallViewString.append("-1-[\(viewKey)]")
                constraintsSmallViewDic[viewKey] = smallViews[index]
            }
            
            constraintsSmallViewString.append("|")
            let constraintsSmallViewV = NSLayoutConstraint.constraints(withVisualFormat: constraintsSmallViewString, options: [], metrics: nil, views: constraintsSmallViewDic)
            layoutConstraints.append(contentsOf: constraintsSmallViewV)
        }
        
        return layoutConstraints
    }
    
    func clearDummyViews() {
        for view in dummyViews {
            view.removeFromSuperview()
            view.backToPool()
        }
        dummyViews.removeAll()
    }
    
    func clearConstraints() {
        NSLayoutConstraint.deactivate(layoutConstraints)
        layoutConstraints.removeAll()
    }
}
