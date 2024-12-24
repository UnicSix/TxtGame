#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include "GameTypes.h"

const int mapWidth = 30;
const int mapHeight = 30;
const int mapArea = 900;

static int score;
static int treasureCnt=0; // manage amount of treasure in map

uint32_t currentTopTile(uint32_t pos);
uint32_t playerMove(const uint32_t pos, char direc);
void printMap(const uint32_t *worldMap);
void genTreasure(uint32_t *worldMap);
void genTarrain(uint32_t *worldMap);

int main(){

  std::cout << "Game Start\n";
  std::srand(std::time(nullptr));
  uint32_t worldMap[900]={0};
  int pos = std::rand()%900;
  genTreasure(worldMap);
  genTarrain(worldMap);
  genTarrain(worldMap);
  genTarrain(worldMap);

  printf("Player Init Location: (%2d, %2d)\n", (pos)%30+1, (pos)/30+1);
  worldMap[pos] |= PLAYER;
  printf("pos: %d, pos val: %d \n", pos, worldMap[pos]);
  std::cout << "'wasd' to move , 'i' for inspect" << std::endl;
  printMap(worldMap);

  char input;
  uint32_t newPos;
  // game loop
  while (std::cin >> input && input != 'q') {
    // TODO: blocking player with rock
    newPos = playerMove(pos, input);
    if (pos != newPos) {
      worldMap[pos] &= ~PLAYER;
      worldMap[newPos] |= PLAYER;
      pos = newPos;
    }
    worldMap[pos] |= PLAYER;
    system("cls");
    if(worldMap[pos] & (~worldMap[pos]|uint32_t(1)<<2)){
      score += 5;
      worldMap[pos] ^= TREASURE;
    }
    
    printMap(worldMap);
  }

  std::cout << "Quit Game\n";
  return 0;
}

void genTarrain(uint32_t *worldMap){
  int startPos = std::rand()%900;
  int terrainType = std::rand()%3;
  // generate default terrain (3x3 block)
  for(auto offset : tarrains.at(terrainType)){
    if(startPos + offset > 0){ // exclude negative indices
      // write rock to position
      worldMap[startPos+offset] |= ROCK;
    }
  }
}

void genTreasure(uint32_t *worldMap){
  int treasurePos = std::rand()%900;
  worldMap[treasurePos] |= TREASURE;
  printf("pos: %d, pos val: %d \n", treasurePos, worldMap[treasurePos]);
}

uint32_t playerMove(uint32_t pos, char direc){
  switch (moveDirec.find(direc)->second) {
    case 0: // Up
      if (pos<30) {
        break;
      }
      else{return pos-30;}
    case 1: // Down
      if (pos>=30*29) {
        break;
      }
      else{return pos+30;}
    case 2: // Left
      if (pos%30==0) {
        break;
      }
      else{return pos-1;}
    case 3: // Right
      if (pos%30==29) {
        break;
      }
      else{return pos+1;}
    default:
      return pos;
  }
  return pos;
}

void printMap(const uint32_t *worldMap){
  printf("Score: %d,\n", score);
  for(int i=0; i<mapArea; i++){
    // print first non-zero tile obj in map
    if(worldMap[i] == 0){
      std::cout << tileChar.find(GROUND)->second << ' ';
    }
    else{
      std::cout << tileChar.find(currentTopTile(worldMap[i]))->second << ' ';
    }
    if(i%30==29) std::cout << std::endl;
  }
}

uint32_t currentTopTile(uint32_t posInfo){
  return posInfo&(~posInfo+1);
}
