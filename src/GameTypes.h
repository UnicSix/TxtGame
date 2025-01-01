#ifndef GAME_TYPES_H
#define GAME_TYPES_H
#include <map>
#include <vector>
#endif

#define BLACK    "\033[30m"
#define RED      "\033[31m"
#define GREEN    "\033[32m"
#define YELLOW   "\033[33m"
#define BLUE     "\033[34m"
#define MAGENTA  "\033[35m"
#define CYAN     "\033[36m"
#define WHITE    "\033[37m"

// Background Colors
#define BG_BLACK    "\033[40m"
#define BG_RED      "\033[41m"
#define BG_GREEN    "\033[42m"
#define BG_YELLOW   "\033[43m"
#define BG_BLUE     "\033[44m"
#define BG_MAGENTA  "\033[45m"
#define BG_CYAN     "\033[46m"
#define BG_WHITE    "\033[47m"

// Reset
#define RESET   "\033[0m"

#include <cstdint>

enum tileType{
  PLAYER    = uint32_t(1),
  ROCK      = uint32_t(1)<<1,
  TREASURE  = uint32_t(1)<<2,
  MONSTER   =  uint32_t(1)<<3,
  PATH      =  uint32_t(1)<<4,
  GROUND    = UINT32_MAX     // ground has lowest print priority
};

inline const std::map<uint32_t, char> tileChar = {
  {PLAYER,   '1'},
  {ROCK,     '0'},
  {TREASURE, '$'},
  {MONSTER,  'M'},
  {PATH,     'p'},
  {GROUND,   '.'}
};

inline const std::map<char, int> moveDirec = {
  {'w', 0}, {'W', 0},
  {'s', 1}, {'S', 1},
  {'a', 2}, {'A', 2},
  {'d', 3}, {'D', 3}
};

// pre-defined terrains
inline const std::vector<std::vector<int>>terrains = {
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
