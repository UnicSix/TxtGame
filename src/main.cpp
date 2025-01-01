/*
 *  Friendly reminding for myself (Unic Six):
 *  #1. Make it work, not make it perfect
 *  #2. Focus on how software should work, not hardware
 */
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <ostream>
#include <bitset>
#include <queue>
#include <utility>
#include "GameTypes.h"

const int mapWidth = 30;
const int mapHeight = 30;
const int mapArea = 900;

static int score;
static int treasureCnt=0; // manage amount of treasure in map

typedef struct graphNode{
  int curPos = -1;
  int prevPos = -1;
  int dStart  = UINT32_MAX;
  int dDest   = UINT32_MAX;
  int dSum    = UINT32_MAX;
  bool     isVisited = false;
  // Value of sum must be updated AFTER dStart and dDest
  // There is no possibility of obtaining a negative edge in this case
  bool operator>(const graphNode &other) const{
    return dSum > other.dSum;
  }
  friend std::ostream& operator<<(std::ostream& os, const graphNode&node){
    os << "Current Node: "  << node.curPos  << "\n"
       << "Previous Node: " << node.prevPos << "\n"
       << "Sum Dist: "      << node.dSum
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
  return !(worldMap[pA]&ROCK && worldMap[pB]&ROCK)
  ? std::abs(pA/30-pB/30) + std::abs(pA%30-pB%30)
  : INT32_MAX;
}

int main(){

  std::cout << "Game Start\n";
  std::srand(std::time(nullptr));
  uint32_t worldMap[900]={0};
  graphNode mapGraph[900];

  int pos = std::rand()%900;
  int treasurePos;
  int tarrainSpawnCnt = std::rand()%8+3;

  for (int i=0; i<tarrainSpawnCnt; i++) {
    genTerrain(worldMap);
  }
  treasurePos = genTreasure(worldMap);

  // WARN: Player sometimes spawns in terrain
  printf("Player Init Location: (%2d, %2d)\n", (pos)%30+1, (pos)/30+1);
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
    pathfindingAStar(worldMap, mapGraph, std::pair<int, int>(pos, treasurePos));
  }

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
*   Shortest dSum and unvisited
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
        break;
      }
      std::cout << tileChar.find(tile)->second << ' ' << RESET;
    }
    if(i%30==29) std::cout << std::endl;
  }
}


void pathfindingAStar(uint32_t *worldMap, graphNode *mapGraph, const std::pair<uint32_t, uint32_t> coord){
  uint32_t start = coord.first;
  uint32_t dest = coord.second;
  uint32_t cur = start;
  uint32_t curLeft, curRight, curUp, curDown;
  std::priority_queue
    <graphNode, std::vector<graphNode>, std::greater<graphNode>> nodeQueue;

  // Mark player position as start node
  mapGraph[start].curPos = start;
  mapGraph[start].prevPos = start;
  mapGraph[start].dStart = 0;
  mapGraph[start].dDest = manhattanDist(worldMap, start, dest);
  mapGraph[start].dSum = mapGraph[start].dStart + mapGraph[start].dDest;
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
    if(cur>29 && cur<900){ // Not at the top of the map
      // 1 is the edge weight of this grid-based mapGraph
      // dDest is NOT actual distance but theoritically shortest distance
      curUp = cur-30;
      mapGraph[curUp].curPos = curUp;
      mapGraph[curUp].prevPos = cur;
      mapGraph[curUp].dStart = mapGraph[cur].dStart+1; // this is the actual distance
      mapGraph[curUp].dDest = manhattanDist(worldMap, curUp, dest);
      mapGraph[curUp].dSum = mapGraph[curUp].dDest == INT32_MAX
        ? INT32_MAX
        : mapGraph[curUp].dStart + mapGraph[curUp].dDest;
      if (mapGraph[curUp].dSum != INT32_MAX) {
        nodeQueue.push(mapGraph[curUp]);
      }
    }
    if(cur<900 && cur>870){ // Not bottom
      curDown = cur+30;
      mapGraph[curDown].curPos = curDown;
      mapGraph[curDown].prevPos = cur;
      mapGraph[curDown].dStart = mapGraph[cur].dStart+1;
      mapGraph[curDown].dDest = manhattanDist(worldMap, curDown, dest);
      mapGraph[curDown].dSum = mapGraph[curDown].dDest == INT32_MAX
        ? INT32_MAX
        : mapGraph[curDown].dStart + mapGraph[curDown].dDest;
      if (mapGraph[curDown].dSum != INT32_MAX) {
        nodeQueue.push(mapGraph[curDown]);
      }
    }
    if(cur%30!=0){ // Not at left side of the map
      curLeft = cur-1;
      mapGraph[curLeft].curPos = curLeft;
      mapGraph[curLeft].prevPos = cur;
      mapGraph[curLeft].dStart = mapGraph[cur].dStart+1;
      mapGraph[curLeft].dDest = manhattanDist(worldMap, curLeft, dest);
      mapGraph[curLeft].dSum = mapGraph[curLeft].dDest == INT32_MAX
        ? INT32_MAX
        : mapGraph[curLeft].dStart + mapGraph[curLeft].dDest;
      if (mapGraph[curLeft].dSum != INT32_MAX) {
        nodeQueue.push(mapGraph[curLeft]);
      }
    }
    if(cur%30!=29){ // Not at Right side of the map
      curRight = cur+1;
      mapGraph[curRight].curPos = curRight;
      mapGraph[curRight].prevPos = cur;
      mapGraph[curRight].dStart = mapGraph[cur].dStart+1;
      mapGraph[curRight].dDest = manhattanDist(worldMap, curRight, dest);
      mapGraph[curRight].dSum = mapGraph[curRight].dDest == INT32_MAX
        ? INT32_MAX
        : mapGraph[curRight].dStart + mapGraph[curRight].dDest;
      if (mapGraph[curRight].dSum != INT32_MAX) {
        nodeQueue.push(mapGraph[curRight]);
      }
    }
    cur = nodeQueue.top().curPos;
    worldMap[mapGraph[cur].curPos] |= PATH;
    nodeQueue.pop();
  }
  int nodeIdx = dest;
  // while (nodeIdx!=start) {
  //   worldMap[mapGraph[nodeIdx].prevPos] |= PATH;
  //   nodeIdx = mapGraph[nodeIdx].prevPos;
  //   std::cout << "Current node index: " << nodeIdx << std::endl;
  // }
}


