#include "Visualizer.h"
#include "Field.h"
#include "Ship.h"
#include "GameManager.h"


int main () {
    GameManager::GetInstance().Init();
    GameManager::GetInstance().Play();
}
