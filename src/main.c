#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"

int main(int argc, const char* argv[]) {
  initVM();

  Chunk chunk;
  initChunk(&chunk);

  writeConstant(&chunk, 69, 33);
  writeChunk(&chunk, OP_RETURN, 123);
  //disassembleChunk(&chunk, "test chunk");
  interpret(&chunk);

  freeVM();
  freeChunk(&chunk);
  return 0;
}
