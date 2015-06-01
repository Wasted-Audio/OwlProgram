#include "SharedMemory.h"
#include "SampleBuffer.hpp"
#include "PatchProcessor.h"
#include "basicmaths.h"
#include "owlcontrol.h"
#include "HeavyPatch.hpp"

PatchProcessor processor;

PatchProcessor* getInitialisingPatchProcessor(){
  return &processor;
}

#define REGISTER_PATCH(T, STR, IN, OUT) registerPatch(STR, IN, OUT, new T)

void registerPatch(const char* name, uint8_t inputs, uint8_t outputs, Patch* patch){
  if(getSharedMemory()->registerPatch != NULL)
    getSharedMemory()->registerPatch(name, inputs, outputs);
  processor.setPatch(patch);
}

void setup(){
  REGISTER_PATCH(HeavyPatch, "Heavy", 2, 2);
}

SampleBuffer buffer;

void processBlock(){
  buffer.split(getSharedMemory()->audio_input, getSharedMemory()->audio_blocksize);
  processor.setParameterValues(getSharedMemory()->parameters);
  processor.patch->processAudio(buffer);
  buffer.comb(getSharedMemory()->audio_output);
}
