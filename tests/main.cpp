#include "generics.h"

#include "clock.hpp"
#include "event_system.hpp"
#include "game_renderer.hpp"
#include "game_window.hpp"
#include "keyboard.hpp"

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

class Square {
public:
    Square(se::Keyboard& keyboard, se::Clock& clock)
        : keyboard(keyboard)
        , clock(clock)
    {
    }

    void update()
    {
        float delta = clock.delta();
        float distance = 1.0 * delta;
        if (keyboard.hold(se::Key('w')))
            move(0, distance);
        if (keyboard.hold(se::Key('s')))
            move(0, -distance);
        if (keyboard.hold(se::Key('a')))
            move(-distance, 0);
        if (keyboard.hold(se::Key('d')))
            move(distance, 0);
    }

    AAPLVertex vertices[6]
        = {
              // 2D positions,    RGBA colors
              { { 10, 10 }, { 1, 1, 1, 1 } },
              { { 10, -10 }, { 1, 1, 1, 1 } },
              { { -10, -10 }, { 1, 1, 1, 1 } },
              { { -10, 10 }, { 1, 1, 1, 1 } },
              { { 10, 10 }, { 1, 1, 1, 1 } },
              { { -10, -10 }, { 1, 1, 1, 1 } }
          };

private:
    void move(float x, float y)
    {
        for (size_t i = 0; i < 6; i++) {
            vertices[i].position[0] += x;
            vertices[i].position[1] += y;
        }
    }

    se::Keyboard& keyboard;
    se::Clock& clock;
};

int main(int argc, char** argv)
{
    se::GameWindow gameWindow(true);
    se::GameRenderer gameRenderer(gameWindow);
    se::EventSystem eventSystem;
    se::Clock clock;
    se::Keyboard keyboard(eventSystem);

    DEFINE_LIBRARY(triangle, gameRenderer);

    se::ShaderId vertex = gameRenderer.loadShaderFromLibrary(triangle, "vertexShader");
    se::ShaderId fragment = gameRenderer.loadShaderFromLibrary(triangle, "fragmentShader");

    se::PipelineId pipeline = gameRenderer.createPipeline(vertex, fragment);

    Square player(keyboard, clock);

    auto exit_listner = std::make_shared<ExitLister>();
    eventSystem.addListner(exit_listner, se::Events::Quit);
    eventSystem.addListner(exit_listner, se::Events::KeyDown);

    while (!exit_listner->exited()) {
        eventSystem.processEvents();
        clock.update();

        player.update();

        gameRenderer.beginFrame();
        gameRenderer.drawVertices(player.vertices, 6, pipeline);
        gameRenderer.endFrame();
    }

    return 0;
}