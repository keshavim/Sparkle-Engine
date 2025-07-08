#include <Sparkle.h>

using namespace Sparkle;

class TestGame : public Game {
public:
    TestGame() = default;
    ~TestGame() override = default;

    bool init() override {
        config.flags |= WindowFlags::Vulkan | WindowFlags::Resizable;
        SPA_LOG_INFO("TestGame init");
        return true;
    }

    bool render() override {
        return true;
    }

    bool update(float delta_time) override {
        if (Input::key_pressed(Key::SPACE)) {
            SPA_LOG_INFO("Jump!");
        }

        if (Input::key_down(Key::A)) {
            SPA_LOG_INFO("A");
        }
        return true;
    }

    void on_resize(int new_width, int new_height) override {
        //todo
    }

};

Game *createGame() {
    return new TestGame;
}