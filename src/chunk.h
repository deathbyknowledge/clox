#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include "value.h"

typedef enum {
  OP_RETURN,
  OP_CONSTANT,
} OpCode;

typedef struct {
  int count;
  int capacity;
  // might want to swap this to unsigned too
  uint16_t* lines;  //(TODO: revisit encoding) [15:12]: instruciton index - [11:0]: line number 
} LineCount;

typedef struct {
  int count;
  int capacity;
  uint8_t* code;

  int linesCount;
  int linesCapacity;
  LineCount lines;
  ValueArray constants;
} Chunk;


void initChunk(Chunk* chunk);
void freeChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte, int line);
int addConstant(Chunk* chunk, Value value);
int getLine(Chunk* chunk, int inst_idx);

void initLineCount(LineCount* lc);
void freeLineCount(LineCount* lc);
void writeLineCount(LineCount* lc, int line);


#endif
