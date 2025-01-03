/*
 *  Friendly reminding for myself (Unic Six):
 *  #1. Make it work, not make it perfect
 *  #2. Focus on how software should work, not hardware
 */
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <bitset>
#include <queue>
#include <stdexcept>
#include <utility>
#include <chrono>
#include "GameTypes.h"

const int mapWidth = 30;
const int mapHeight = 30;
const int mapArea = 900;

static int score;
static int treasureCnt=0; // manage amount of treasure in map

typedef struct graphNode{
  int curPos  = -1;
  int prevPos = -1;
  int distToStart  = INT32_MAX;
  int distToDest   = INT32_MAX;
  int distSum    = INT32_MAX;
  bool     isVisited = true;
  // Value of sum must be updated AFTER distToStart and distToDest
  // There is no possibility of obtaining a negative edge in this case
  bool operator>(const graphNode &other) const{
    return distSum > other.distSum;
  }
  friend std::ostream& operator<<(std::ostream& os, const graphNode&node){
    os << "Current Node: "  << node.curPos%30 << " ," << node.curPos/30  << "\n"
       << "Previous Node: " << node.prevPos%30 << " ," << node.prevPos/30 << "\n"
       << "Sum Dist: "      << node.distSum
       << std::endl;
    return os;
  }
} graphNode;

uint32_t playerMove(const uint32_t pos, const char direc, uint32_t *worldMap);
void printMap(const uint32_t *worldMap);
uint32_t genTreasure(uint32_t *worldMap);
void genTerrain(uint32_t *worldMap);
inline uint32_t currentTopTile(const uint32_t posInfo){
  // the return value can be used to search correspond tile char defined in GameType.h
  return posInfo&(~posInfo+1);
}

// Notice: the pair parameter here only contain 2 numbers 
// but represents 2 nodes' coordinates
void pathfindingAStar(uint32_t *worldMap, graphNode *mapGraph, const std::pair<uint32_t, uint32_t> coord);
inline int manhattanDist(uint32_t *worldMap, int pA, int pB){
  return std::abs(pA/30-pB/30) + std::abs(pA%30-pB%30);
}

int main(){

  std::cout << "Game Start\n";
  std::srand(std::time(nullptr));
  uint32_t worldMap[900]={0};

  int pos;
  int treasurePos;
  int tarrainSpawnCnt = std::rand()%15+5;

  for (int i=0; i<20; i++) {
    std::cout << "Gen Terrain " << i << ' ';
    genTerrain(worldMap);
  }
  std::cout << std::endl;
  treasurePos = genTreasure(worldMap);
  do {
    pos = rand()%900;
  }while (worldMap[pos]&ROCK);

  // edge test case
  // treasurePos = 899;
  // pos = 0;
  // for (int i=0; i<30; i++) {
  //   worldMap[0] |= PLAYER;
  //   worldMap[899] |= TREASURE;
  //   if(i%4==1){
  //     for (int j=0; j<29; j++) {
  //       if ((i*30+j)%2==0) {
  //         worldMap[i*30+j] |= ROCK;
  //       }
  //     }
  //   }
  //   else if(i%4==3){
  //     for (int j=1; j<30; j++) {
  //       worldMap[i*30+j] |= ROCK;
  //     }
  //   }
  // }

  printf("Player Init Location: (%2d, %2d)\n", (pos)%30, (pos)/30);
  worldMap[pos] |= PLAYER;
  printf("pos: %d, pos val: %d \n", pos, worldMap[pos]);
  std::cout << "'wasd' to move , 'i' for inspect" << std::endl;
  printMap(worldMap);

  char input;
  uint32_t newPos;
  // game loop
  // while (std::cin >> input && input != 'q') {
  //   system("cls");
  //   newPos = playerMove(pos, input, worldMap);
  //   if (pos != newPos) {
  //     worldMap[pos] &= ~PLAYER;
  //     worldMap[newPos] |= PLAYER;
  //     pos = newPos;
  //   }
  //   worldMap[pos] |= PLAYER;
  //   if(worldMap[pos] & (~worldMap[pos]|uint32_t(1)<<2)){
  //     score += 5;
  //     worldMap[pos] ^= TREASURE;
  //     genTreasure(worldMap);
  //   }
  //   
  //   printMap(worldMap);
  // }
  { // block for testing graph functionalities
    graphNode mapGraph[900];
    auto start = std::chrono::high_resolution_clock::now();
    pathfindingAStar(worldMap, mapGraph, std::pair<int, int>(pos, treasurePos));
    auto end = std::chrono::high_resolution_clock::now();
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    auto us = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << std::fixed << std::setprecision(9)
              << "Execution time: " << ns.count() << " ns"
              << " (" << ns.count() / 1e9 << " seconds)" << std::endl;
  }
  printMap(worldMap);

  std::cout << "Quit Game\n";
  return 0;
}

void genTerrain(uint32_t *worldMap){
  int startPos = std::rand()%900;
  int terrainType = std::rand()%3;
  for(auto offset : terrains.at(terrainType)){
    if(startPos + offset > 0){ // exclude negative indices
      // write rock to position
      worldMap[startPos+offset] |= ROCK;
    }
  }
}

// Notes on A star pathfinding //
/*
* We have a start node Ns, and a dest node Nd.
* Calculate the distance between Ns and Nd.
* For any given node Na(ax, ay) and Nb(bx, by),
* the distance between them can be presented as:
* dist = | ax-bx | + | ay-by | (also known as Manhattan Distance)
* In this case, we can define the distance function by the simple operation shown above.
* For A* pathfinding, each node in graph needs to store 3 infos:
* G cost: D(Ns, Nn) (Nn: any node in graph)
* H cost: D(Nd, Nn)
* F cost: G cost + H cost
*
* ASSUMPTION: The Manhattan Distance func should only be applied to adjacent nodes (as a fundamental measure method)
* The distance fot those farthur nodes (relative to Ns) are measured by data stored in nodes nearby them (adjacent nodes).
* 
* We not only need to know the cost to neighboor nodes, but also need to know how far
* our neighboors is from the destination.
*
* The init state: dist(Ns, anynode) = inf
* Check adjacent nodes of Ns, Update the dist(Ns, N_neightbors)
* Save the node dist to a priority queue, sorted by dist ofc.
*
* Which Node to Search?
*   Shortest distSum and unvisited
* So we might need a priority queue to make nodes wait in line for being searched 
*/

uint32_t genTreasure(uint32_t *worldMap){
  uint32_t treasurePos = std::rand()%900;
  while((worldMap[treasurePos]&ROCK)){
    treasurePos = std::rand()%900;
  }
  worldMap[treasurePos] |= TREASURE;
  return treasurePos;
}

uint32_t playerMove(const uint32_t pos, const char direc, uint32_t *worldMap){
  uint32_t originPos = pos;
  // Main map bound check
  switch (moveDirec.find(direc)->second) {
    case 0: // Up
      if (pos<30) {break;}
      else if(worldMap[pos-30]&ROCK) {break;}
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
      std::cout << BLACK << tileChar.find(GROUND)->second << ' ' << RESET;
      // std::cout << tileChar.find(GROUND)->second << ' ';
    }
    else{
      uint32_t tile = currentTopTile(worldMap[i]);
      switch (tile) {
        case(PLAYER):
          std::cout << BLUE;
        break;
        case(ROCK):
          std::cout << RED;
        break;
        case(TREASURE):
          std::cout << YELLOW;
        break;
        default:
          std::cout << RESET;
        break;
      }
      std::cout << tileChar.find(tile)->second << ' ' << RESET;
      // std::cout << tileChar.find(tile)->second << ' ';
    }
    if(i%30==29) std::cout << std::endl;
  }
}


void pathfindingAStar(uint32_t *worldMap, graphNode *mapGraph, const std::pair<uint32_t, uint32_t> coord){
  const uint32_t start = coord.first;
  const uint32_t dest = coord.second;
  uint32_t cur = start;
  uint32_t topPriorityNode = -1;
  std::priority_queue
    <graphNode, std::vector<graphNode>, std::greater<graphNode>> nodeQueue;

  // Mark player position as start node
  mapGraph[start].curPos = start;
  mapGraph[start].prevPos = start;
  mapGraph[start].distToStart = 0;
  mapGraph[start].distToDest = manhattanDist(worldMap, start, dest);
  mapGraph[start].distSum = mapGraph[start].distToStart + mapGraph[start].distToDest;
  mapGraph[start].isVisited = true;

  // Search for path until visit dest node
  while (true) {
    if(cur == dest){
      std::cout << "Start node: " << start << std::endl;
      std::cout << "Current node: " << cur << std::endl;
      std::cout << "Path is Found" << std::endl;
      break;
    }
  
    // This part needs to be revised if we want to change the ratio of the map
    for(int i=0; i<4; i++){
      int nxt, distSumNxt, distToDestNxt, distToStartNxt;
      if     (i==0) { nxt = cur>29  ? cur-30 : cur; }
      else if(i==1) { nxt = cur<870 ? cur+30 : cur; }
      else if(i==2) { nxt = cur%30!=0 ? cur-1 : cur; }
      else if(i==3) { nxt = cur%30!=29 ? cur+1 : cur; }
      else{ std::cout << "Position Condition Undefined!\n\n"; }
      if (worldMap[nxt]&ROCK || nxt == mapGraph[cur].prevPos) {
        // Dont walk back to prev node
        // skip every operation, check next node
        // printf("Skip Node: %2d, %2d\n\n", nxt%30, nxt/30);
      }
      else{
        // printf("Checking Node: %2d, %2d\n\n", nxt%30, nxt/30);
        distToStartNxt = mapGraph[cur].distToStart+1;
        distToDestNxt  = manhattanDist(worldMap, nxt, dest);
        distSumNxt     = distToStartNxt + distToDestNxt;
        // printf("New Dist Info: %d, %d, %d\n\n", distToStartNxt, distToDestNxt, distSumNxt);
        // printf("Cur Dist Info: %d, %d, %d\n\n", mapGraph[nxt].distToStart, mapGraph[nxt].distToDest, mapGraph[nxt].distSum);
        // if distSumNxt is less-equal than target node sum, update target node stats
        int shortestInQueue = nodeQueue.empty() ? INT32_MAX : nodeQueue.top().distSum;
        // printf("Shortest In Queue: %d\n", shortestInQueue);
        if(distSumNxt < mapGraph[nxt].distSum /*&& distSumNxt <= shortestInQueue*/){
          // printf("Will Push Node: %2d, %2d\n\n", nxt%30, nxt/30);
          mapGraph[nxt].distToStart = distToStartNxt;
          mapGraph[nxt].distToDest  = distToDestNxt;
          mapGraph[nxt].distSum     = distSumNxt;
          mapGraph[nxt].curPos      = nxt;
          mapGraph[nxt].prevPos     = cur;
          mapGraph[nxt].isVisited   = false;
          // isCheckingQueue = true;
          // if adjacent node available, traverse to it without push it to queue
          if (mapGraph[nxt].distSum == mapGraph[cur].distSum) {
            topPriorityNode = nxt; // next step WILL BE on this position
          }
        }
        // pushing node into the queue
        if (mapGraph[nxt].distSum != INT32_MAX && !mapGraph[nxt].isVisited && nxt!=topPriorityNode) {
          // std::cout << "Push node: " << nxt << "\n";
          // printf("Push Node:   %2d, %2d\n", nxt%30, nxt/30);
          // printf("Node Dist:   %d\n\n", mapGraph[nxt].distSum);
          mapGraph[nxt].isVisited = true;
          nodeQueue.push(mapGraph[nxt]);
        }
      }
    } // end of for-loop
    if (topPriorityNode!=-1) {
      cur = topPriorityNode;
      mapGraph[cur].isVisited = true;
      // printf("Fast Visit Node:  %2d, %2d\n", cur%30, cur/30);
      // printf("Node Dist :  %d\n\n", mapGraph[cur].distSum);
      topPriorityNode=-1;
    }
    else if(!nodeQueue.empty()){
      cur = nodeQueue.top().curPos;
      // std::cout << mapGraph[cur] << std::endl;
      nodeQueue.pop();
    }
    else if (nodeQueue.empty()) {
      std::cout << "Treasure is not reachable !\n";
      return;
    }
    worldMap[cur] |= PATH;
    // printMap(worldMap);
  }
  std::cout << "End pathfinding \n";
}


