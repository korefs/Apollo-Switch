# Moonlight-Switch — Current Architecture Analysis
## Phase 0 — Repository Analysis (Apollo Switch Migration)

> **Status:** Read-only analysis. No code has been modified.

---

## 1. Repository Layout

```
Moonlight-Switch/
├── app/
│   ├── include/          # Public headers (StreamProfileResolver.hpp)
│   ├── src/              # All application source
│   │   ├── libgamestream/ # GameStream HTTP/XML protocol layer (GPL)
│   │   ├── streaming/     # Session, decoder, renderer, input, discovery
│   │   │   ├── audio/     # IAudioRenderer + backends
│   │   │   ├── ffmpeg/    # FFmpeg decoder + platform helpers
│   │   │   ├── switch/    # Switch-specific decoder/render provider
│   │   │   └── video/     # IVideoRenderer + platform renderers
│   │   ├── crypto/        # MbedTLS / OpenSSL crypto manager
│   │   ├── gestures/      # Touch gesture recognizers
│   │   ├── keyboards/     # On-screen keyboard layouts
│   │   ├── utils/         # Settings singleton, BoxArt, Singleton base
│   │   └── views/         # BooleanSliderCell
│   └── platforms/        # Desktop/iOS/Android/PSV platform glue
├── extern/
│   ├── moonlight-common-c/  # moonlight-common-c (Limelight) — core protocol
│   ├── borealis/            # UI framework
│   ├── mdns/                # mDNS discovery library
│   ├── CImg/                # Image processing (boxart)
│   └── cmake/               # Build helpers
├── lib/switch/           # Prebuilt Switch libraries (deko3d, etc.)
├── resources/            # XML layouts, i18n, fonts, shaders, images
└── docs/                 # Existing docs
```

---

## 2. Application Entry Points

| File | Role |
|------|------|
| `app/src/main.cpp` | Main entry point: init Borealis, set Switch core affinity, register XML views, push MainActivity |
| `app/src/main_args.cpp` | Deep-link / command-line argument handling (startFromArgs) |
| `app/src/main_activity.cpp` | MainActivity — Borealis root activity shell |
| `app/src/main_tabs_view.cpp` | MainTabs — tab bar hosting Host/AddHost/Settings/About/Favorites |

---

## 3. Module Inventory & Classification

Classification codes:
- **A** — Reusable streaming core (keep as-is)
- **B** — Switch platform integration (keep, may rename)
- **C** — Moonlight-specific branding / behavior (rename / evolve)
- **D** — UI / screens (evolve)
- **E** — Configuration / persistence (evolve)
- **F** — Apollo integration candidate (introduce)

### 3.1 Streaming Core

| Module | Files | Class |
|--------|-------|-------|
| Session manager | `streaming/MoonlightSession.*` | **C/A** — Wraps LiStartConnection. Name is Moonlight-branded, logic is generic. |
| GameStream client | `streaming/GameStreamClient.*` | **C/A** — connect, pair, applist, start, quit via libgamestream. |
| Host discovery | `streaming/DiscoverManager.*` | **A** — mDNS + address-list polling. |
| Wake-on-LAN | `streaming/WakeOnLanManager.*` | **A** — Sends WoL magic packets. |
| Frame queue | `streaming/AVFrameHolder.*` | **A** — Decouples decoder/renderer; frame pacing; statistics. |
| Profile resolver | `streaming/StreamProfileResolver.*` + `include/StreamProfileResolver.hpp` | **E/F** — Per-host/network profile resolution. Already detects WiFi/Ethernet via nifm. |

### 3.2 Protocol Layer

| Module | Files | Class |
|--------|-------|-------|
| GameStream HTTP | `libgamestream/client.*` | **C (GPL)** — gs_init, gs_pair, gs_applist, gs_start_app, gs_quit_app. GPL attribution must remain. |
| HTTP transport | `libgamestream/http.*` | **A** — libcurl wrapper. |
| XML parsing | `libgamestream/xml.*` | **A** — TinyXML2 wrapper for GFE XML responses. |
| Limelight (extern) | `extern/moonlight-common-c/` | **A** — Core RTSP/streaming protocol. Do not modify. |
| mDNS (extern) | `extern/mdns/` | **A** — Do not modify. |

### 3.3 Video Pipeline

| Module | Files | Class |
|--------|-------|-------|
| Decoder interface | `streaming/ffmpeg/IFFmpegVideoDecoder.hpp` | **A** |
| FFmpeg decoder | `streaming/ffmpeg/FFmpegVideoDecoder.*` | **A** — H264/H265/AV1 + platform helpers. |
| Renderer interface | `streaming/video/IVideoRenderer.hpp` | **A** |
| Deko3D renderer | `streaming/video/deko3d/DKVideoRenderer.*` | **B** — Switch-native GPU renderer + FSR1 upscaling. Critical. |
| OpenGL renderer | `streaming/video/OpenGL/` | **A** |
| Metal renderer | `streaming/video/Metal/` | **A** |
| D3D11 renderer | `streaming/video/D3D11/` | **A** |
| Android renderer | `streaming/video/Android/` | **A** |
| Codec support | `streaming/ffmpeg/VideoCodecSupport.*` | **A** |

### 3.4 Audio Pipeline

| Module | Files | Class |
|--------|-------|-------|
| Audio interface | `streaming/audio/IAudioRenderer.hpp` | **A** |
| Audren backend | `streaming/audio/AudrenAudioRenderer.*` | **B** — Switch-native audren + Opus decoding. |
| SDL backend | `streaming/audio/SDLAudioRenderer.*` | **A** |
| Debug recorder | `streaming/audio/DebugFileRecorderAudioRenderer.*` | **A** |

### 3.5 Input Pipeline

| Module | Files | Class |
|--------|-------|-------|
| Input manager | `streaming/InputManager.*` | **C/B** — MoonlightInputManager; controller mapping, rumble, host shortcuts, touch/mouse. |
| Keyboard view | `keyboard_view.*`, `keyboards/` | **D** — On-screen keyboard with locale layouts. |
| Button mapping | `mapping_layout_editor.*` | **D** |
| Gesture recognizers | `gestures/` | **A** |

### 3.6 Switch Platform Integration

| Module | Files | Class |
|--------|-------|-------|
| Switch provider factory | `streaming/switch/SwitchMoonlightSessionDecoderAndRenderProvider.*` | **B** — Selects FFmpegVideoDecoder + DKVideoRenderer + AudrenAudioRenderer. |
| Provider interface | `streaming/MoonlightSessionDecoderAndRenderProvider.hpp` | **A** — Pure interface for decoder/renderer factory. |
| Switch libnx glue | `app/src/switch/wrapper.c` | **B** |
| Network detection | `streaming/StreamProfileResolver.cpp` L51-67 | **B** — nifmGetInternetConnectionStatus. Already in profile resolver. |
| Operation mode | `main.cpp` L77-88, `streaming_view.cpp` | **B** — Core affinity, wireless priority. No dedicated handheld/docked class yet. |

### 3.7 Configuration & Persistence

| Module | Files | Class |
|--------|-------|-------|
| Settings singleton | `utils/Settings.*` | **E** — Monolithic: hosts, stream, audio, input, UI. ~500-line header. |
| Host model | `utils/Settings.hpp` L101-127 | **E** — address, remoteAddress, hostname, mac, favorites, per-context streamProfiles. |
| Stream profile | `utils/Settings.hpp` L32-46 | **E** — Per-context optional overrides. |
| Profile context | `utils/Settings.hpp` L26-30 | **E** — LocalWifi / LocalEthernet / Remote. Maps to NetworkProfile concept. |
| Quality aggregate | `utils/Settings.hpp` L50-63 | **E/F** — Per-session quality telemetry stored per host/context. |
| BoxArt manager | `utils/BoxArtManager.*` | **D** |
| Crypto manager | `crypto/` | **A** — TLS key/cert management for pairing. |

### 3.8 UI / Screens

| Module | Files | Class |
|--------|-------|-------|
| Streaming view | `streaming_view.*` | **D** — Active stream canvas; embeds MoonlightSession, overlay, keyboard. |
| In-game overlay | `ingame_overlay_view.*` | **D** — HUD: stats, shortcuts, disconnect. |
| Input overlay | `streaming_input_overlay.*` | **D** |
| Host tab | `host_tab.*` | **D** — Per-host: connect, app list, stream profiles, WoL, remove. |
| Add host tab | `add_host_tab.*` | **D** — mDNS discovery UI + manual IP entry. |
| App list view | `app_list_view.*` | **D** |
| App cell | `app_cell.*` | **D** |
| Stream profiles view | `stream_profiles_view.*` | **D/E** — Per-host, per-context profile editor. |
| Settings tab | `settings_tab.*` | **D/E** — Global settings (~830 lines). |
| Favorites tab | `favorite_tab.*` | **D** |
| About tab | `about_tab.*` | **D** |

---

## 4. Data Flow Summary

```
User selects Host + App
        |
        v
GameStreamClient::start()          <- libgamestream/client.cpp (gs_start_app)
        |                              <- HTTP to GFE/Sunshine/Apollo
        v
MoonlightSession::start()
        +-- StreamProfileResolver::resolve() -> ResolvedStreamSettings
        |     +-- nifmGetInternetConnectionStatus() (Switch only)
        +-- LiStartConnection()     <- moonlight-common-c
              +-- RTSP handshake
              +-- Video stream -> FFmpegVideoDecoder -> AVFrameHolder -> DKVideoRenderer
              +-- Audio stream -> AudrenAudioRenderer / SDLAudioRenderer
              +-- Input stream <- MoonlightInputManager -> LiSendControllerEvent()
```

---

## 5. Key Observations

### What Works Well — Do Not Touch
- moonlight-common-c integration is solid and battle-tested.
- FFmpegVideoDecoder with deko3d zero-copy path is the core of Switch performance.
- AVFrameQueue frame pacing system is mature.
- StreamProfileResolver already detects WiFi/Ethernet via libnx nifm.
- Provider abstraction cleanly separates platform from session.
- Host model already supports per-context StreamProfile overrides.

### Coupling Issues — Resolve Over Time
- Settings is a 500+ line monolith mixing stream config, UI config, host persistence, input config, and audio config.
- MoonlightSession and MoonlightInputManager names are Moonlight-branded (cosmetic, not structural).
- libgamestream/client.cpp is GPL code mixed with the Apollo-facing client layer — needs abstraction above it.
- StreamProfileContext only models LocalWifi/LocalEthernet/Remote — no Handheld/Docked device dimension.
- No dedicated class for Switch operation mode (handheld/docked) detection.
- No Apollo-specific host type or capabilities model.
- Settings tab is a single 830-line file.

### Switch-Specific Code Locations
- Core affinity: main.cpp L58-88
- Wireless priority: main.cpp L97
- Screenshot/home button override: streaming_view.cpp L43-69
- Audio backend selection: Settings.hpp L454-458 (#ifdef __SWITCH__)
- Frame pacing mode: Settings.hpp L218-223 (#ifdef __SWITCH__)
- Network detection: StreamProfileResolver.cpp L50-67 (#ifdef __SWITCH__)
- Deko3D renderer: streaming/video/deko3d/ (Switch-only)
- Audren audio: streaming/audio/AudrenAudioRenderer.* (#ifdef __SWITCH__)
- Provider factory: streaming/switch/SwitchMoonlightSessionDecoderAndRenderProvider.*
