// Apollo Switch
// StreamProfileResolver.hpp
//
// Backward-compatibility facade forwarding to ProfileResolver.

#pragma once

#include "ProfileResolver.hpp"

// Type alias preserving full source compatibility with existing codebase
using StreamProfileResolver = ProfileResolver;
