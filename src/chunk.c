#include <stdlib.h>

#include "chunk.h"
#include "memory.h"


/* LINE COUNT */

static uint16_t encodeLine(int line, int count) {
   uint16_t lineNum = (uint16_t) ((unsigned int) line);
   uint16_t c = (uint16_t) ((unsigned int) count);
   return ((c << 11) + (lineNum & 2047));
}

static void decodeLine(uint16_t line, int* lineNum, int* count) {
   *count = (int)(line >> 11);
   *lineNum = (int)(line & 2047);
}

void initLineCount(LineCount* lc) {
  lc->count = 0;
  lc->capacity = 0;
  lc->lines = NULL;
}

void freeLineCount(LineCount* lc) {
  FREE_ARRAY(uint16_t, lc->lines, lc->capacity);
  initLineCount(lc);
}

void writeLineCount(LineCount* lc, int line) {
  // Check if Op array needs to be grown.
  int prevCount, prevLineNum;
if (lc->capacity == 0) {
    lc->capacity = GROW_CAPACITY(0);
    lc->lines = GROW_ARRAY(uint16_t, lc->lines, 0, lc->capacity);
    lc->lines[lc->count] = encodeLine(line, 1);
    lc->count++;
    return;
  }

  decodeLine(lc->lines[lc->count - 1], &prevLineNum, &prevCount);

  // Make sure the counter still fits in 4 bits.
  if (prevLineNum == line && prevCount < 15) {
    prevCount++;
    lc->lines[lc->count - 1] = encodeLine(prevLineNum, prevCount);
    return;
  }

  // Check if Line array needs to be grown.
  if (lc->capacity < lc->count + 1) {
    int oldCapacity = lc->capacity;
    lc->capacity = GROW_CAPACITY(oldCapacity);
    lc->lines = GROW_ARRAY(uint16_t, lc->lines, oldCapacity, lc->capacity);
  }

  lc->lines[lc->count] = encodeLine(line, 1);
  lc->count++;

}


/* CHUNK */
void initChunk(Chunk* chunk) {
  chunk->count = 0;
  chunk->capacity = 0;
  chunk->code = NULL;
  initLineCount(&chunk->lines);
  initValueArray(&chunk->constants);
}

void freeChunk(Chunk* chunk) {
  FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);    
  freeLineCount(&chunk->lines);
  freeValueArray(&chunk->constants);
  initChunk(chunk);
}

void writeChunk(Chunk* chunk, uint8_t byte, int line) {
  // Check if Op array needs to be grown.
  if (chunk->capacity < chunk->count + 1) {
    int oldCapacity = chunk->capacity;
    chunk->capacity = GROW_CAPACITY(oldCapacity);
    chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
  }

  chunk->code[chunk->count] = byte;
  writeLineCount(&chunk->lines, line);
  chunk->count++;
}

uint32_t addConstant(Chunk* chunk, Value value) {
  writeValueArray(&chunk->constants, value);
  // TODO: count should be uint32_t here
  return chunk->constants.count - 1;
}

void writeConstant(Chunk* chunk, uint8_t constant, int line) {
  writeChunk(chunk, OP_CONSTANT, line);
  writeChunk(chunk, constant, line);
}

void writeConstantLong(Chunk* chunk, uint32_t constant, int line) {
  writeChunk(chunk, OP_CONSTANT_LONG, line);
  // BE layout
  writeChunk(chunk, (uint8_t)((constant >> 16) & 0xFF), line); // [23:16]
  writeChunk(chunk, (uint8_t)((constant >> 8) & 0xFF), line); // [15:8]
  writeChunk(chunk, (uint8_t)(constant & 0xFF), line); // [7:0]
}


int getLine(Chunk* chunk, int instIdx) {
  int count = 0;
   
  for (int i = 0; i <= chunk->lines.count; i++) {
    int lineNum, lineCount;
    decodeLine(chunk->lines.lines[i], &lineNum, &lineCount);
    if (instIdx >= count && instIdx <= (count+lineCount - 1)) {
      return lineNum;
    }
    count += lineCount;
  }
  // Should not get here
  return -1;
}
