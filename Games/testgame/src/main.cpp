#include <Sparkle.h>


class TestGame : public Sparkle::Game {
public:
    TestGame() = default;
    ~TestGame() override = default;

    bool init() override {
        config.flags |= Sparkle::WindowFlags::Vulkan | Sparkle::WindowFlags::Resizable;
        SPA_LOG_INFO("TestGame init");
        return true;
    }

    bool render() override {
        return true;
    }

    bool update(float delta_time) override {
        return true;
    }

    void on_resize(int new_width, int new_height) override {
        //todo
    }

};

Sparkle::Game *createGame() {
    return new TestGame;
}