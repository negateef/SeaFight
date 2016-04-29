#include <string>

#include "GameManager.h"
#include "ServerAPI.h"



int main (int argc, const char *argv[]) {
    
    if (argc > 1) {
        ServerAPI::GetInstance().Init(std::string(argv[0]));
    } else {
        ServerAPI::GetInstance().Init();
    }
    GameManager::GetInstance().Init();
    GameManager::GetInstance().Play();
}
