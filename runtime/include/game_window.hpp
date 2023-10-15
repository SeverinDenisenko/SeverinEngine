#pragma once

#include "logging.hpp"

#include "generics.h"

#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <Metal/shared_ptr.hpp>
#include <QuartzCore/QuartzCore.hpp>

#include <SDL3/SDL.h>
#include <fmt/format.h>

namespace se {

class GameRenderer;

class GameWindow {
public:
    GameWindow(bool fullscreen = true)
    {
        SDL_SetHint(SDL_HINT_RENDER_DRIVER, "metal");
        SDL_InitSubSystem(SDL_INIT_VIDEO);

        if (fullscreen)
            window = SDL_CreateWindow("Game", w, h,
                SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_METAL);
        else
            window = SDL_CreateWindow("Game", w, h,
                SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_METAL);

        if (!window) {
            FATAL("Failed to create window");
        }

        if (fullscreen) {
            SDL_SetWindowFullscreen(window, SDL_TRUE);
            SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

            const SDL_DisplayMode* DM = SDL_GetCurrentDisplayMode(SDL_GetDisplayForWindow(window));
            if (!DM) {
                FATAL("DisplayMode is unavalable");
            }
            w = DM->w;
            h = DM->h;

            SDL_SetWindowSize(window, w, h);
            SDL_RestoreWindow(window);
        }
    }

    ~GameWindow()
    {
        SDL_DestroyWindow(window);
        window = nullptr;
        SDL_Quit();
    }

    vector_uint2 getViewport()
    {
        return { w, h };
    }

    friend class GameRenderer;

private:
    SDL_Window* window { nullptr };
    uint32_t w;
    uint32_t h;
};

} // namespace se
