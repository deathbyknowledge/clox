#include "common.h"
#include "chunk.h"
#include "debug.h"

int main(int argc, const char* argv[]) {
  Chunk chunk;
  initChunk(&chunk);

  int constant = addConstant(&chunk, 1.2);
  writeChunk(&chunk, OP_CONSTANT, 123);
  writeChunk(&chunk, constant, 123);
  writeChunk(&chunk, OP_RETURN, 123);
  writeChunk(&chunk, OP_RETURN, 27);

  constant = addConstant(&chunk, 37.0);
  writeChunk(&chunk, OP_CONSTANT, 27);
  writeChunk(&chunk, constant, 27);
  writeChunk(&chunk, OP_RETURN, 43);
  disassembleChunk(&chunk, "test chunk");

  freeChunk(&chunk);
  return 0;
}
