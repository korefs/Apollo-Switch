import os

base_dir = r"c:\Users\kore\Documents\Projetos\2026\Switch\Moonlight-Switch\app\src\streaming"

# Rename 1: MoonlightSession -> StreamSession

# 1. HPP
with open(os.path.join(base_dir, "MoonlightSession.hpp"), "r", encoding="utf-8") as f:
    hpp_content = f.read()

hpp_content = hpp_content.replace("MoonlightSession", "StreamSession")
# Remove the old alias which is now `using StreamSession = StreamSession;`
hpp_content = hpp_content.replace("using StreamSession = StreamSession;\n", "")
# Add backward compat alias at the bottom, before or after the last newline
if hpp_content.endswith("\n"):
    hpp_content += "using MoonlightSession = StreamSession;\n"
else:
    hpp_content += "\nusing MoonlightSession = StreamSession;\n"

with open(os.path.join(base_dir, "StreamSession.hpp"), "w", encoding="utf-8") as f:
    f.write(hpp_content)

# 1. CPP
with open(os.path.join(base_dir, "MoonlightSession.cpp"), "r", encoding="utf-8") as f:
    cpp_content = f.read()

cpp_content = cpp_content.replace("MoonlightSession", "StreamSession")
cpp_content = cpp_content.replace('MoonlightSession.hpp', 'StreamSession.hpp')

with open(os.path.join(base_dir, "StreamSession.cpp"), "w", encoding="utf-8") as f:
    f.write(cpp_content)

# 1. Backward compat files
with open(os.path.join(base_dir, "MoonlightSession.hpp"), "w", encoding="utf-8") as f:
    f.write("""// Apollo Switch — backward compatibility header
// MoonlightSession has been renamed to StreamSession (Milestone 5)
#pragma once
#include "StreamSession.hpp"
using MoonlightSession = StreamSession;
""")

with open(os.path.join(base_dir, "MoonlightSession.cpp"), "w", encoding="utf-8") as f:
    f.write("""// MoonlightSession.cpp has been renamed to StreamSession.cpp
// This file is retained as an empty placeholder.
// Build system should reference StreamSession.cpp instead.
""")

# Update SwitchStreamProvider.cpp
switch_provider_path = os.path.join(base_dir, "switch", "SwitchStreamProvider.cpp")
with open(switch_provider_path, "r", encoding="utf-8") as f:
    switch_content = f.read()
switch_content = switch_content.replace("MoonlightSession.hpp", "StreamSession.hpp")
with open(switch_provider_path, "w", encoding="utf-8") as f:
    f.write(switch_content)

# Update FFmpegVideoDecoder.cpp
ffmpeg_decoder_path = os.path.join(base_dir, "ffmpeg", "FFmpegVideoDecoder.cpp")
with open(ffmpeg_decoder_path, "r", encoding="utf-8") as f:
    ffmpeg_content = f.read()
ffmpeg_content = ffmpeg_content.replace("MoonlightSession.hpp", "StreamSession.hpp")
with open(ffmpeg_decoder_path, "w", encoding="utf-8") as f:
    f.write(ffmpeg_content)

# Rename 2: MoonlightInputManager -> StreamInputManager

# 2. HPP
with open(os.path.join(base_dir, "InputManager.hpp"), "r", encoding="utf-8") as f:
    input_hpp_content = f.read()

input_hpp_content = input_hpp_content.replace("MoonlightInputManager", "StreamInputManager")
input_hpp_content = input_hpp_content.replace("using StreamInputManager = StreamInputManager;\n", "")
if input_hpp_content.endswith("\n"):
    input_hpp_content += "using MoonlightInputManager = StreamInputManager;\n"
else:
    input_hpp_content += "\nusing MoonlightInputManager = StreamInputManager;\n"

with open(os.path.join(base_dir, "StreamInputManager.hpp"), "w", encoding="utf-8") as f:
    f.write(input_hpp_content)

# 2. CPP
with open(os.path.join(base_dir, "InputManager.cpp"), "r", encoding="utf-8") as f:
    input_cpp_content = f.read()

input_cpp_content = input_cpp_content.replace("MoonlightInputManager", "StreamInputManager")
input_cpp_content = input_cpp_content.replace('InputManager.hpp', 'StreamInputManager.hpp')

with open(os.path.join(base_dir, "StreamInputManager.cpp"), "w", encoding="utf-8") as f:
    f.write(input_cpp_content)

# Update StreamSession.cpp to include StreamInputManager.hpp instead of InputManager.hpp
with open(os.path.join(base_dir, "StreamSession.cpp"), "r", encoding="utf-8") as f:
    stream_sess_cpp = f.read()
stream_sess_cpp = stream_sess_cpp.replace('InputManager.hpp', 'StreamInputManager.hpp')
with open(os.path.join(base_dir, "StreamSession.cpp"), "w", encoding="utf-8") as f:
    f.write(stream_sess_cpp)

# 2. Backward compat files
with open(os.path.join(base_dir, "InputManager.hpp"), "w", encoding="utf-8") as f:
    f.write("""// Apollo Switch — backward compatibility header  
// MoonlightInputManager has been renamed to StreamInputManager (Milestone 5)
#pragma once
#include "StreamInputManager.hpp"
using MoonlightInputManager = StreamInputManager;
""")

with open(os.path.join(base_dir, "InputManager.cpp"), "w", encoding="utf-8") as f:
    f.write("""// InputManager.cpp has been renamed to StreamInputManager.cpp
// This file is retained as an empty placeholder.
// Build system should reference StreamInputManager.cpp instead.
""")

print("Done")
