//
// Created by overlord on 6/30/25.
//
#pragma once

#include "core/logger.h"
#include "core/application.h"
#include "game_type.h"
#include "core/spa_assert.h"

// main entry point
extern Sparkle::Game *createGame();

int main() {
    Sparkle::Application::SetGameInst(createGame());

    Sparkle::Application::Init();
    Sparkle::Application::Run();
    Sparkle::Application::Shutdown();
    return 0;
}