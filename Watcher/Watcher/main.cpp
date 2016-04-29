//
//  main.cpp
//  Watcher
//
//  Created by Misha Babenko on 4/29/16.
//  Copyright © 2016 Misha Babenko. All rights reserved.
//

#include <iostream>
#include "Watcher.h"


int main(int argc, const char * argv[]) {
    Watcher::GetInstance().Init();
    Watcher::GetInstance().Play();
    return 0;
}
