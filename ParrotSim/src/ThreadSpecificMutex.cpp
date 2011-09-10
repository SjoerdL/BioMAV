#include "ThreadSpecificMutex.hpp"

void ThreadSpecificMutex :: lock() {
  bool doLock = true;
  SDL_LockMutex(checkMutex);
  if ((instanceCount > 0) && (threadID == SDL_ThreadID())) {
    doLock = false;
    instanceCount++;
  }
  SDL_UnlockMutex(checkMutex);

  if (doLock) {
    SDL_LockMutex(lockMutex);

    SDL_LockMutex(checkMutex);
    instanceCount = 1;
    threadID = SDL_ThreadID();
    SDL_UnlockMutex(checkMutex);
  }
}

void ThreadSpecificMutex :: unlock() {
  SDL_LockMutex(checkMutex);
  instanceCount--;
  if (instanceCount <= 0) {
    SDL_UnlockMutex(lockMutex);
  }
  SDL_UnlockMutex(checkMutex);
}

ThreadSpecificMutex :: ThreadSpecificMutex() {
  checkMutex = SDL_CreateMutex();
  lockMutex = SDL_CreateMutex();

  instanceCount = 0;
}

ThreadSpecificMutex :: ~ThreadSpecificMutex() {
  SDL_DestroyMutex(checkMutex);
  SDL_DestroyMutex(lockMutex);
}

