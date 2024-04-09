#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "value.h"
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

static ObjString* allocateString(const char* chars, int length) {
  ObjString* string = (ObjString*)allocateObject(sizeof(ObjString) + length * sizeof(char), OBJ_STRING);
  string->length = length;
  memcpy(string->chars, chars, string->length);
  return string;
}

ObjString* takeString(char* chars, int length) {
  ObjString* str = allocateString((const char*)chars, length);
  // AllocateString copies the chars so we free them after.
  FREE_ARRAY(char, chars, length);
  return str;
}

ObjString* copyString(const char* chars, int length) {
  return allocateString(chars, length);
}

void printObject(Value value) {
  switch(OBJ_TYPE(value)) {
    case OBJ_STRING:
      printf("%s", AS_CSTRING(value));
      break;
  }
}
