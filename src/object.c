#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "value.h"
#include "table.h"
#include "vm.h"

#define ALLOCATE_OBJ(type, objectType) \
  (type*)allocateObject(sizeof(type), objectType)

static Obj* allocateObject(size_t size, ObjType type) {
  Obj* obj = (Obj*)reallocate(NULL, 0, size);
  obj->type = type;
  obj->next = vm.objects;
  vm.objects = obj;
  return obj;
}

static ObjString* allocateString(const char* chars, int length, uint32_t hash) {
  ObjString* string = (ObjString*)allocateObject(sizeof(ObjString) + length * sizeof(char), OBJ_STRING);
  string->hash = hash;
  string->length = length;
  memcpy(string->chars, chars, string->length);
  tableSet(&vm.strings, string, NIL_VAL);
  return string;
}

static uint32_t hashString(char* key, int length) {
  uint32_t hash = 2166136261u;
  for (int i = 0; i < length; i++) {
    hash ^= (uint8_t)key[i];
    hash *= 16777619;
  }
  return hash;
}

ObjString* takeString(char* chars, int length) {
  uint32_t hash = hashString(chars, length);
  ObjString* interned = tableFindString(&vm.strings, chars, length, hash);

  if (interned != NULL) return interned;
  
  ObjString* str = allocateString((const char*)chars, length, hash);
  // AllocateString copies the chars so we free them after.
  // Maybe this should be left to the caller?
  FREE_ARRAY(char, chars, length + 1);
  return str;
}

ObjString* copyString(const char* chars, int length) {
  uint32_t hash = hashString(chars, length);
  ObjString* interned = tableFindString(&vm.strings, chars, length, hash);

  if (interned != NULL) return interned;

  return allocateString(chars, length, hash);
}

void printObject(Value value) {
  switch(OBJ_TYPE(value)) {
    case OBJ_STRING:
      printf("%s", AS_CSTRING(value));
      break;
  }
}
