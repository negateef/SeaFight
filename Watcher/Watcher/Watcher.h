//
//  Watcher.h
//  Watcher
//
//  Created by Misha Babenko on 4/29/16.
//  Copyright © 2016 Misha Babenko. All rights reserved.
//

#ifndef Watcher_h
#define Watcher_h

#include "Visualizer.h"
#include "ServerAPI.h"
#include "Field.h"

#include <thread>
#include <mutex>
#include <chrono>
#include <atomic>
#include <string>

#include <iostream>

class Watcher {
private:
    Visualizer visualizer_;
    
    Field first_field_;
    Field second_field_;    
    
    Watcher () {}
    
public:
    Watcher &operator =(const Watcher &) = delete;
    Watcher(const Watcher &) = delete;
    
    static Watcher &GetInstance() {
        static Watcher watcher;
        return watcher;
    }
    
    void Init() {
        visualizer_.Init();
    }
    
    
    
    void Play() {    
        ServerAPI::GetInstance().Init();
        
        std::mutex mutex;
        
        while (true) {
            
            std::unique_lock<std::mutex> lock(mutex);
            
            visualizer_.DrawFields(first_field_, second_field_);
            lock.unlock();
            
            std::thread update_fields_thread([this, &mutex] () {
                ServerAPI::GetInstance().GetFields([this, &mutex] (Field field1, Field field2) {
                    std::lock_guard<std::mutex> lock(mutex);
                    first_field_ = field1;
                    second_field_ = field2;
                });
            });
            
            update_fields_thread.detach();
                
        }
    }
};


#endif /* Watcher_h */
