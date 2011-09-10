#ifndef THREADSPECIFICMUTEX_HPP_
#define THREADSPECIFICMUTEX_HPP_

#include <SDL/SDL.h>

class ThreadSpecificMutex {
  protected:
    int instanceCount;
    Uint32 threadID;    

    SDL_mutex* checkMutex;
    SDL_mutex* lockMutex;
  public:
    virtual void lock();
    virtual void unlock();

    ThreadSpecificMutex();
    virtual ~ThreadSpecificMutex();
};

#endif // THREADSPECIFICMUTEX_HPP_
