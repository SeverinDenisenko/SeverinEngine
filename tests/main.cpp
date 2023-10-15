#include "generics.h"

#include "clock.hpp"
#include "event_system.hpp"
#include "game_renderer.hpp"
#include "game_window.hpp"

#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <Metal/shared_ptr.hpp>
#include <QuartzCore/QuartzCore.hpp>

#include <SDL3/SDL.h>

namespace {
#include "triangle_metallib.h"
}

#define DEFINE_LIBRARY(name, renderer)                           \
    dispatch_data_s* name##_library_data = dispatch_data_create( \
        &name##_metallib[0], name##_metallib_len,                \
        dispatch_get_main_queue(),                               \
        ^ {});                                                   \
    se::GameLibraryId name = renderer.addLibrary(name##_library_data)

class ExitLister : public se::EventListner {
public:
    virtual void listen(SDL_Event& event) override
    {
        if (event.type == SDL_EVENT_QUIT) {
            exited_ = true;
        }

        if (event.type == SDL_EVENT_KEY_DOWN && event.key.keysym.sym == SDLK_q) {
            exited_ = true;
        }
    }

    bool exited()
    {
        return exited_;
    }

private:
    bool exited_ = false;
};

struct Square {
    AAPLVertex vertices[6] = {
        // 2D positions,    RGBA colors
        { { 10, 10 }, { 1, 1, 1, 1 } },
        { { 10, -10 }, { 1, 1, 1, 1 } },
        { { -10, -10 }, { 1, 1, 1, 1 } },
        { { -10, 10 }, { 1, 1, 1, 1 } },
        { { 10, 10 }, { 1, 1, 1, 1 } },
        { { -10, -10 }, { 1, 1, 1, 1 } }
    };

    void move(float x, float y)
    {
        for (size_t i = 0; i < 6; i++) {
            vertices[i].position[0] += x;
            vertices[i].position[1] += y;
        }
    }
};

class MoveListner : public se::EventListner {
public:
    MoveListner(Square& square, se::Clock& clock)
        : square(square)
        , clock(clock)
    {
    }

    virtual void listen(SDL_Event& event) override
    {
        float delta = clock.delta();
        float distance = 10.0 * delta;
        if (event.type == SDL_EVENT_KEY_DOWN && event.key.keysym.sym == SDLK_w) {
            square.move(0, distance);
        }
        if (event.type == SDL_EVENT_KEY_DOWN && event.key.keysym.sym == SDLK_s) {
            square.move(0, -distance);
        }
        if (event.type == SDL_EVENT_KEY_DOWN && event.key.keysym.sym == SDLK_a) {
            square.move(-distance, 0);
        }
        if (event.type == SDL_EVENT_KEY_DOWN && event.key.keysym.sym == SDLK_d) {
            square.move(distance, 0);
        }
    }

private:
    Square& square;
    se::Clock& clock;
};

int main(int argc, char** argv)
{
    se::GameWindow gameWindow(true);
    se::GameRenderer gameRenderer(gameWindow);
    se::Clock clock;
    se::EventSystem eventSystem;

    DEFINE_LIBRARY(triangle, gameRenderer);

    se::ShaderId vertex = gameRenderer.loadShaderFromLibrary(triangle, "vertexShader");
    se::ShaderId fragment = gameRenderer.loadShaderFromLibrary(triangle, "fragmentShader");

    se::PipelineId pipeline = gameRenderer.createPipeline(vertex, fragment);

    Square player;

    auto exit_listner = std::make_shared<ExitLister>();
    auto move_listner = std::make_shared<MoveListner>(player, clock);
    eventSystem.addListner(exit_listner, SDL_EVENT_QUIT);
    eventSystem.addListner(exit_listner, SDL_EVENT_KEY_DOWN);
    eventSystem.addListner(move_listner, SDL_EVENT_KEY_DOWN);

    float w = gameWindow.getViewport()[0];
    float h = gameWindow.getViewport()[1];

    while (!exit_listner->exited()) {
        clock.update();
        eventSystem.processEvents();

        gameRenderer.beginFrame();
        gameRenderer.drawVertices(player.vertices, 6, pipeline);
        gameRenderer.endFrame();
    }

    return 0;
}