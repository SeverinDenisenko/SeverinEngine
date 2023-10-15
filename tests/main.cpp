#include "generics.h"

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

int main(int argc, char** argv)
{
    se::GameWindow gameWindow(true);
    se::GameRenderer gameRenderer(gameWindow);
    se::EventSystem eventSystem;

    DEFINE_LIBRARY(triangle, gameRenderer);

    se::ShaderId vertex = gameRenderer.loadShaderFromLibrary(triangle, "vertexShader");
    se::ShaderId fragment = gameRenderer.loadShaderFromLibrary(triangle, "fragmentShader");

    se::PipelineId pipeline = gameRenderer.createPipeline(vertex, fragment);

    auto exit_listner = std::make_shared<ExitLister>();
    eventSystem.addListner(exit_listner, SDL_EVENT_QUIT);
    eventSystem.addListner(exit_listner, SDL_EVENT_KEY_DOWN);

    AAPLVertex squareVertices[] = {
        // 2D positions,    RGBA colors
        { { 0, 0 }, { 1, 1, 1, 1 } },
        { { 0, 10 }, { 1, 1, 1, 1 } },
        { { 10, 10 }, { 1, 1, 1, 1 } },
        { { 10, 0 }, { 1, 1, 1, 1 } },
        { { 0, 0 }, { 1, 1, 1, 1 } },
        { { 10, 10 }, { 1, 1, 1, 1 } }
    };

    while (!exit_listner->exited()) {
        eventSystem.processEvents();

        gameRenderer.beginFrame();
        gameRenderer.drawVertices(squareVertices, 6, pipeline);
        gameRenderer.endFrame();
    }

    return 0;
}