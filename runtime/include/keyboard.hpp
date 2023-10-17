#pragma once

#include <unordered_set>

#include "event_system.hpp"

#include <SDL3/SDL.h>

namespace se {

using Key = SDL_KeyCode;

class Keyboard {
public:
    explicit Keyboard(EventSystem& eventSystem)
    {
        auto listner = std::make_shared<KeyboardListner>(*this);
        eventSystem.addListner(listner, Events::KeyDown);
        eventSystem.addListner(listner, Events::KeyUp);
    }

    void reset()
    {
        pressed_keys_.clear();
        released_keys_.clear();
    }

    bool pressed(Key code)
    {
        return pressed_keys_.find(code) != pressed_keys_.end();
    }

    bool released(Key code)
    {
        return released_keys_.find(code) != released_keys_.end();
    }

    bool hold(Key code)
    {
        return hodled_keys_.find(code) != hodled_keys_.end();
    }

private:
    class KeyboardListner : public EventListner {
    public:
        KeyboardListner(Keyboard& keyboard)
            : keyboard(keyboard)
        {
        }

        virtual void listen(Event& event) override
        {
            auto type = Events(event.type);
            auto key = Key(event.key.keysym.sym);

            switch (type) {
            case Events::KeyDown:
                keyboard.hodled_keys_.insert(key);
                keyboard.pressed_keys_.insert(key);
                break;
            case Events::KeyUp:
                if (auto iter = keyboard.hodled_keys_.find(key); iter != keyboard.hodled_keys_.end())
                    keyboard.hodled_keys_.erase(iter);
                keyboard.released_keys_.insert(key);
                break;
            default:
                break;
            }
        }

    private:
        Keyboard& keyboard;
    };

    friend class KeyboardListner;

    std::unordered_set<Key> hodled_keys_;
    std::unordered_set<Key> pressed_keys_;
    std::unordered_set<Key> released_keys_;
};
} // namespace se
