#include <SDL3/SDL.h>

#include <memory>
#include <unordered_map>
#include <vector>

namespace se {
class EventListner {
public:
    virtual void listen(SDL_Event& event) = 0;
};

class EventSystem {
public:
    void processEvents()
    {
        SDL_Event e;

        while (SDL_PollEvent(&e) != 0) {
            for (auto listner : listners_[e.type])
                listner->listen(e);
        }
    }

    void addListner(std::shared_ptr<EventListner> listner, Uint32 event_type)
    {
        listners_[event_type].push_back(listner);
    }

private:
    std::unordered_map<Uint32, std::vector<std::shared_ptr<EventListner>>> listners_;
};
} // namespace se
