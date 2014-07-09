#ifndef DEBUG_CFG_H_INCLUDED
#define DEBUG_CFG_H_INCLUDED

#define DEBUG
#define NO_CFG

//#define OUT printf
#define DOUT(FORMAT,...)  printf(FORMAT,##__VA_ARGS__)

#define DEBUG_ASSERT
#define DEBUG_HINT
#define DEBUG_CHECKPOINT

#endif  // DEBUG_CFG_H_INCLUDED
