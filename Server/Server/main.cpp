//
//  main.cpp
//  Server
//
//  Created by Misha Babenko on 4/27/16.
//  Copyright © 2016 Misha Babenko. All rights reserved.
//

#include <iostream>
#include "Server.h"

int main(int argc, const char * argv[]) {
    Server::GetInstance().Init();
    return 0;
}
