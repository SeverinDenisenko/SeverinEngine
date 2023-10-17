#pragma once

#include <SDL3/SDL.h>

#include <memory>
#include <unordered_map>
#include <vector>

namespace se {

using Event = SDL_Event;

enum class Events {
    KeyDown = SDL_EVENT_KEY_DOWN,
    KeyUp = SDL_EVENT_KEY_UP,
    Quit = SDL_EVENT_QUIT
};

class EventListner {
public:
    virtual void listen(Event& event) = 0;
};

class EventSystem {
public:
    void processEvents()
    {
        Event event;

        while (SDL_PollEvent(&event) != 0) {
            auto type = Events(event.type);

            for (auto listner : listners_[type])
                listner->listen(event);
        }
    }

    void addListner(std::shared_ptr<EventListner> listner, Events event_type)
    {
        listners_[event_type].push_back(listner);
    }

private:
    std::unordered_map<Events, std::vector<std::shared_ptr<EventListner>>> listners_;
};
} // namespace se
