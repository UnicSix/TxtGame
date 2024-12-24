#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <ostream>
#include <bitset>
#include "GameTypes.h"

const int mapWidth = 30;
const int mapHeight = 30;
const int mapArea = 900;

static int score;
static int treasureCnt=0; // manage amount of treasure in map

uint32_t currentTopTile(uint32_t pos);
uint32_t playerMove(const uint32_t pos, char direc, uint32_t *worldMap);
void printMap(const uint32_t *worldMap);
void genTreasure(uint32_t *worldMap);
void genTarrain(uint32_t *worldMap);

int main(){

  std::cout << "Game Start\n";
  std::srand(std::time(nullptr));
  uint32_t worldMap[900]={0};
  int pos = std::rand()%900;
  int tarrainSpawnCnt = std::rand()%8+3;
  for (int i=0; i<tarrainSpawnCnt; i++) {
    std::cout << tarrainSpawnCnt << std::endl;
    genTarrain(worldMap);
  }
  genTreasure(worldMap);

  printf("Player Init Location: (%2d, %2d)\n", (pos)%30+1, (pos)/30+1);
  worldMap[pos] |= PLAYER;
  printf("pos: %d, pos val: %d \n", pos, worldMap[pos]);
  std::cout << "'wasd' to move , 'i' for inspect" << std::endl;
  printMap(worldMap);

  char input;
  uint32_t newPos;
  // game loop
  while (std::cin >> input && input != 'q') {
    system("cls");
    // TODO: if treasure destroyed, create a new one
    newPos = playerMove(pos, input, worldMap);
    if (pos != newPos) {
      worldMap[pos] &= ~PLAYER;
      worldMap[newPos] |= PLAYER;
      pos = newPos;
    }
    worldMap[pos] |= PLAYER;
    if(worldMap[pos] & (~worldMap[pos]|uint32_t(1)<<2)){
      score += 5;
      worldMap[pos] ^= TREASURE;
      genTreasure(worldMap);
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
  while(((worldMap[treasurePos]&ROCK)&(~worldMap[treasurePos]+ROCK))){
    treasurePos = std::rand()%900;
  }
  worldMap[treasurePos] |= TREASURE;
}

uint32_t playerMove(uint32_t pos, char direc, uint32_t *worldMap){
  uint32_t originPos = pos;
  // Main map bound check
  switch (moveDirec.find(direc)->second) {
    case 0: // Up
      if (pos<30) {break;}
      else if(worldMap[pos-30]&ROCK) {
        break;
      }
      else{return pos-30;}
    case 1: // Down
      if (pos>=30*29) {break;}
      else if(worldMap[pos+30]&ROCK){break;}
      else{return pos+30;}
    case 2: // Left
      if (pos%30==0) {break;}
      else if(worldMap[pos-1]&ROCK){break;}
      else{return pos-1;}
    case 3: // Right
      if (pos%30==29) {break;}
      else if(worldMap[pos+1]&ROCK){break;}
      else{return pos+1;}
    default:
      return pos;
  }
  printf("Location Blocked !!\n");
  return pos;
}

void printMap(const uint32_t *worldMap){
  printf("Score: %d\n", score);
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
