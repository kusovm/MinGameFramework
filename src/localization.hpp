// This module contains functionality for localization.
//
// The game loads "loc.txt" on start and populates the displayedStrings table. 
// The text labels then call "getString" with IDs as arguments upon being 
// constructed, which returns the corresponding translated string that is 
// displayed to the user.
//
// Each entry in "loc.txt" must conform to roughly this regex:
//
//      /^\s*(?<key>.*):[ ]*(?<value>.*)$/
//
// Note that whitespace at the end of the value is not truncated.
// 
// The game has some hardcoded fallback entries, so if "loc.txt" is missing 
// the game will just use those.

#pragma once

#include <string>

namespace localization {
    void loadResourceFile();
    const char *getString(const std::string &key);
    void cleanup();
}
