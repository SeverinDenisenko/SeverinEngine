#pragma once

#include <SDL3/SDL.h>

namespace se {
class Clock {
public:
    void update()
    {
        last = now;
        now = SDL_GetPerformanceCounter();
        deltaTime = (float)((now - last) * 1000 / (float)SDL_GetPerformanceFrequency());
    }

    float delta() const
    {
        return deltaTime;
    }

private:
    Uint64 now = SDL_GetPerformanceCounter();
    Uint64 last = 0;
    float deltaTime;
};
}