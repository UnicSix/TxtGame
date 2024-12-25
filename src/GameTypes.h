#ifndef GAME_TYPES_H
#define GAME_TYPES_H
#include <map>
#include <vector>
#endif

#include <cstdint>

enum tileType{
  PLAYER    = uint32_t(1),
  ROCK      = uint32_t(1)<<1,
  TREASURE  = uint32_t(1)<<2,
  MONSTER  =  uint32_t(1)<<3,
  GROUND    = UINT32_MAX     // ground has lowest print priority
};

inline const std::map<uint32_t, char> tileChar = {
  {PLAYER,   '^'},
  {ROCK,     '0'},
  {TREASURE, '$'},
  {MONSTER,  'M'},
  {GROUND,   '.'}
};

inline const std::map<char, int> moveDirec = {
  {'w', 0}, {'W', 0},
  {'s', 1}, {'S', 1},
  {'a', 2}, {'A', 2},
  {'d', 3}, {'D', 3}
};

// pre-defined terrains
inline const std::vector<std::vector<int>>tarrains = {
  { // generate a 3x3 block of rock on map
    -31, -30, -29,
     -1,   0,   1,
     29,  30,  31
  },
  {
         -30, -29,
     -1,   0,
     29,
     59
  },
  {
    -31, -30, -29,
     -1,   0,
          30,
          60,  61,  62,
          90,  91,  92,
         120, 121,
              151
  },
};
