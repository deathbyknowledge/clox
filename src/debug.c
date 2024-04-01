#include <stdio.h>
#include <string.h>

#include "debug.h"
#include "value.h"

static int simpleInstruction(const char* name, int offset) {
  printf("%s\n", name);
  return offset + 1;
}

static int constantInstruction(const char* name, Chunk* chunk, int offset) {
  int constant, inc;
  if (strcmp(name,"OP_CONSTANT_LONG") == 0) {
    int8_t hbyte = chunk->code[offset + 1]; // [23:16]
    int8_t mbyte = chunk->code[offset + 2]; // [15:8]
    int8_t lbyte = chunk->code[offset + 3]; // [7:0]
    constant = (int)(hbyte << 16) + (int)(mbyte << 8) + (int)lbyte;
    inc = 4;
  } else {
    constant = (int) chunk->code[offset + 1];
    inc = 2;
  }
  printf("%-16s %4d '", name, constant);
  printValue(chunk->constants.values[constant]);
  printf("'\n");
  return offset + inc;
}

int disassembleInstruction(Chunk* chunk, int offset) {
  printf("%04d ", offset);
  
  int currentLine = getLine(chunk, offset);
  if (offset > 0 && currentLine == getLine(chunk, offset - 1)) {
    printf("   | "); 
  } else {
    printf("%4d ", currentLine);
  }

  uint8_t instruction = chunk->code[offset];
  switch (instruction) {
    case OP_RETURN:
      return simpleInstruction("OP_RETURN", offset);
    case OP_CONSTANT:
      return constantInstruction("OP_CONSTANT", chunk, offset);
    case OP_CONSTANT_LONG:
      return constantInstruction("OP_CONSTANT_LONG", chunk, offset);
    default:
      printf("Unknown opcode %d\n", instruction);
      return offset + 1;
  }
}

void disassembleChunk(Chunk* chunk, const char* name) {
  printf("== %s ==\n", name);
  
  for (int offset = 0; offset < chunk->count;) {
    offset = disassembleInstruction(chunk, offset);
  }
}

