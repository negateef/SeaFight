//
//  main.cpp
//  Watcher
//
//  Created by Misha Babenko on 4/29/16.
//  Copyright © 2016 Misha Babenko. All rights reserved.
//

#include <iostream>
#include <string>

#include "Watcher.h"


int main(int argc, const char * argv[]) {
    if (argc > 1) {
        ServerAPI::GetInstance().Init(std::string(argv[1]));
    } else {
        ServerAPI::GetInstance().Init();
    }
    Watcher::GetInstance().Init();
    Watcher::GetInstance().Play();
    return 0;
}
