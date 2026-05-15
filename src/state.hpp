// This module hosts the leaderboard and contains binary IO functionality. 
//
// It is called "state" because I have initially wanted to include more things 
// in here (like the last score and custom character avatar), but because of 
// time constraints I ended up not doing that.
//
// Currently scores are loaded from "save.bin" on launch and saved on exit 
// (not after each run). "save.bin" contains all scores from the last session 
// as unsigned 32-bit little-endian integers, sorted from highest to lowest, 
// encoded as raw bytes.
//
// submitScore is called every time the game scene gets destroyed (see 
// entities.cpp scoreCounter::destroy). 

#pragma once

#include <cstddef>
#include <cstdint>
#include <array>

namespace state {
    constexpr size_t highscoresSize = 10;
    extern const uint32_t *highscoresPtr;

    void submitScore(uint32_t newScore);
    void saveScores();
    void loadScores();
}
