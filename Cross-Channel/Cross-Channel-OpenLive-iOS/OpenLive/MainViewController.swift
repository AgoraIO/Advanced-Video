//
//  MainViewController.swift
//  OpenLive
//
//  Created by GongYuhua on 6/25/16.
//  Copyright © 2016 Agora. All rights reserved.
//

import UIKit

let maxDestination = 4

class MainViewController: UIViewController {
    @IBOutlet weak var currentUidTextField: UITextField!
    @IBOutlet weak var currenChannelTextField: UITextField!
   
    @IBOutlet weak var sourceUidTextField: UITextField!
    @IBOutlet weak var sourceTokenTextField: UITextField!
    @IBOutlet weak var sourceChannelTextField: UITextField!
    
    fileprivate var videoProfile = AgoraVideoProfile.landscape360P
    var videoConfig: VideoConfig?
    
    weak var collectionView: UICollectionView?
    
    lazy var destinationList: [Destination] = {
        var list = [Destination]()
        
        for i in 0..<maxDestination {
            let item = Destination(title: "Destination \(i+1)")
            list.append(item)
        }
        
        return list
    }()
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        guard let segueId = segue.identifier else {
            return
        }
        
        switch segueId {
        case "mainToLive":
            let liveVC = segue.destination as! LiveRoomViewController
            
            // Current
            liveVC.videoProfile = videoProfile
            liveVC.videoConfig = videoConfig
            
            if let currentUid = currentUidTextField.text {
                liveVC.currentUid = UInt(currentUid)
                DefaultValue.setValue(currentUid, key: "currentUidTextField")
            } else {
                liveVC.currentUid = 0
            }
            
            if let value = sender as? NSNumber, let role = AgoraClientRole(rawValue: value.intValue) {
                liveVC.clientRole = role
            }
            
            liveVC.currentChannel = currenChannelTextField.text!
            DefaultValue.setValue(liveVC.currentChannel, key: "currenChannelTextField")
            
            // Source
            if let sourceToken = sourceTokenTextField.text,
                sourceToken.count > 0 {
                liveVC.sourceToken = sourceTokenTextField.text
                DefaultValue.setValue(sourceToken, key: "sourceTokenTextField")
            }
            
            if let sourceUid = sourceUidTextField.text,
                sourceUid.count > 0,
                let uid = UInt(sourceUid) {
                liveVC.sourceUid = uid
                DefaultValue.setValue(sourceUid, key: "sourceUidTextField")
            }
            
            if let sourceChannel = sourceChannelTextField.text,
                sourceChannel.count > 0 {
                liveVC.sourceChannel = sourceChannel
                DefaultValue.setValue(sourceChannel, key: "sourceChannelTextField")
            }
            
            // Destinations
            liveVC.destinationList = destinationList
            
            var writeList = [[String: Any]]()
            
            for item in destinationList {
                let dic = item.dic()
                writeList.append(dic)
            }
            
            DefaultValue.setValue(writeList, key: "destinationList")
            
            liveVC.delegate = self
        case "mainToDestination":
            let destVC = segue.destination as! DestinationViewController
            destVC.collectionView?.dataSource = self
            destVC.updateLayout(updateCollectionView())
            self.collectionView = destVC.collectionView
        case "mainToVideoResolution":
            let resolutionVC = segue.destination as! VideoResolutionViewController
            resolutionVC.delegate = self
        default:
            break
        }
    }
    
    override func viewWillDisappear(_ animated: Bool) {
        super.viewWillDisappear(animated)
        view.endEditing(true)
    }
    
    override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
        view.endEditing(true)
    }
    
    override func viewDidLoad() {
        readFromDefaultValue()
    }
    
    @IBAction func doJoinPressed(_ sender: UIButton) {
        if let channel = currenChannelTextField.text, channel.count <= 0 {
            alert("channel nil")
            return
        }
        
        if checkDestinationListIsLegal() {
            showRoleSelection()
        } else {
            alert("destination channel nil")
        }
    }
}

private extension MainViewController {
    func updateCollectionView() -> UICollectionViewFlowLayout {
        let layout = UICollectionViewFlowLayout()
        let screenWidth = UIScreen.main.bounds.width
        let width = (screenWidth - 48 - 32) * 0.5
        layout.itemSize = CGSize(width: width, height: 180)
        return layout
    }
    
    func readFromDefaultValue() {
        currentUidTextField.text = DefaultValue.getStringValueFor(key: "currentUidTextField")
        currenChannelTextField.text = DefaultValue.getStringValueFor(key: "currenChannelTextField")
        
        sourceUidTextField.text = DefaultValue.getStringValueFor(key: "sourceUidTextField")
        sourceTokenTextField.text = DefaultValue.getStringValueFor(key: "sourceTokenTextField")
        sourceChannelTextField.text = DefaultValue.getStringValueFor(key: "sourceChannelTextField")
        
        // Destinations
        guard let list = DefaultValue.getDicListValueFor(key: "destinationList") else {
            return
        }
        
        for i in 0..<maxDestination {
            let dic = list[i]
            let item = destinationList[i]
            item.update(dic)
        }
    }
    
    func checkDestinationListIsLegal() -> Bool {
        var legal = true
        
        for item in destinationList {
            guard item.isLegal() == false else {
                continue
            }
            legal = false
            break
        }
        return legal
    }
    
    func showRoleSelection() {
        let sheet = UIAlertController(title: nil, message: nil, preferredStyle: .alert)
        let broadcaster = UIAlertAction(title: "Broadcaster", style: .default) { [weak self] _ in
            self?.join(withRole: .broadcaster)
        }
        let audience = UIAlertAction(title: "Audience", style: .default) { [weak self] _ in
            self?.join(withRole: .audience)
        }
        let cancel = UIAlertAction(title: "Cancel", style: .cancel, handler: nil)
        sheet.addAction(broadcaster)
        sheet.addAction(audience)
        sheet.addAction(cancel)
        present(sheet, animated: true, completion: nil)
    }
    
    func alert(_ text: String) {
        let alert = UIAlertController(title: nil, message: text, preferredStyle: .alert)
        let action = UIAlertAction(title: "OK", style: .default)
        alert.addAction(action)
        present(alert, animated: true, completion: nil)
    }
}

private extension MainViewController {
    func join(withRole role: AgoraClientRole) {
        performSegue(withIdentifier: "mainToLive", sender: NSNumber(value: role.rawValue as Int))
    }
}

extension MainViewController: LiveRoomVCDelegate {
    func liveVCNeedClose(_ liveVC: LiveRoomViewController) {
        let _ = navigationController?.popViewController(animated: true)
    }
}

extension MainViewController: UICollectionViewDataSource {
    func collectionView(_ collectionView: UICollectionView, numberOfItemsInSection section: Int) -> Int {
        return maxDestination
    }
    
    func collectionView(_ collectionView: UICollectionView, cellForItemAt indexPath: IndexPath) -> UICollectionViewCell {
        let cell = collectionView.dequeueReusableCell(withReuseIdentifier: "DestinationView", for: indexPath) as! DestinationView
        cell.delegate = self
        let info = destinationList[indexPath.row]
        cell.updateFrom(info)
        return cell
    }
}

extension MainViewController: VideoResolutionVCDelegate {
    func videoResolutionSelected(_ config: VideoConfig) {
        self.videoConfig = config
    }
    
    func vdeoResolutionCancelSelected() {
        self.videoConfig = nil
    }
}

extension MainViewController: DestinationViewDelegate {
    func destinationView(_ cell: DestinationView, willUpdate info: Destination) {
        if let index = collectionView?.indexPath(for: cell) {
            destinationList[index.item] = info
        }
    }
}

extension MainViewController: UITextFieldDelegate {
    func textFieldShouldReturn(_ textField: UITextField) -> Bool {
        self.view.endEditing(true)
        return true
    }
}
