//
//  DefaultValue.swift
//  OpenLive
//
//  Created by CavanSu on 2019/3/12.
//  Copyright © 2019 Agora. All rights reserved.
//

import UIKit

class DefaultValue: NSObject {
    static func setValue(_ value: Any, key: String) {
        UserDefaults.standard.set(value, forKey: key)
    }
    
    static func getStringValueFor(key: String) -> String? {
        return UserDefaults.standard.object(forKey: key) as? String
    }
    
    static func getDicValueFor(key: String) -> [String: Any]? {
        return UserDefaults.standard.object(forKey: key) as? [String: Any]
    }
    
    static func getDicListValueFor(key: String) -> [[String: Any]]? {
        return UserDefaults.standard.object(forKey: key) as? [[String: Any]]
    }
    
    static func getValueFor(key: String) -> Any? {
        return UserDefaults.standard.object(forKey: key)
    }
}
