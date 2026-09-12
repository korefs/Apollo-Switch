# Apollo Switch visual / UX refresh plan

Status: proposed only. No application UI code is authorized by this document.
Implementation begins only after approval of this plan and the preservation
baseline in `docs/ui/current-ui.md`.

## Product direction

Apollo Switch becomes a dark-first console dashboard built around the Switch
controller and a host-to-game flow:

```
Launch → Hosts → Host Home → Application → Connecting → Stream
                         └→ Settings / Profiles / Host actions
```

The visual language is original: deep charcoal surfaces, a restrained Apollo
cyan/indigo accent, high-contrast focus rings, and real game artwork. It will
not reproduce Nintendo, Steam, PlayStation, or Xbox chrome.

## Proposed information architecture

| Destination | Content | Existing behavior preserved |
| --- | --- | --- |
| Hosts | Large saved-host cards; Add Host; compact utility access to Settings, Favorites, and About. | Host list, discovery entry point, host management, favorites, settings, about. |
| Host Home | Host identity/status, current running app if supplied, Favorites, Applications/Games, profile summary, and Quick Actions. | App-list retrieval, running-app state, favorite state, launch availability, reload, profiles, Wake on LAN, rename/remove. |
| Host Quick Actions | Refresh, Wake on LAN when offline/supported, Profiles, Rename, Remove. | Existing contextual host actions. |
| Settings hub | Streaming, Profiles, Video, Audio, Controllers, Network, Apollo, Interface, Advanced. | Every existing global setting, reorganized only. Categories without existing exposed options remain omitted or contain only currently available related settings—no fake controls. |
| Profiles | Three current network contexts and current effective summary. | Profile persistence and precedence, no device-profile model added. |
| Stream overlay | Quick settings, Input, Stats, End session. | Existing overlay activation, options, input overlay, diagnostics, disconnect, and terminate-host-app behavior. |
| Dialogs / feedback | Consistent confirmations, errors, toasts where safe, and observable connection stages. | All dialogs, cancellation, raw technical diagnostics, and operation outcomes. |

### Settings allocation

| Category | Existing settings to place there |
| --- | --- |
| Streaming | Resolution, native scale, frame rate, bitrate, and Streaming Optimal Playable Settings. |
| Profiles | Local Wi-Fi, Local Ethernet, Remote custom profiles and their effective summaries. |
| Video | Codec, HDR, hardware decode, decoder threads, frame pacing, and image adjustments. Each setting has exactly one editable row. |
| Audio | Audio driver, Play Audio on PC, volume amplification. |
| Controllers | UI key swap, mapping layouts/editor, dead zones, rumble, single Joy-Con behavior, Guide/overlay/host shortcuts. |
| Network | No existing user-facing network settings. Show resolved connection context/profile as informational only if it can be derived; do not add network configuration. |
| Apollo | Existing virtual-display mode/resolution/refresh. When a compatible cached host is known, describe its supported capability; do not expose unimplemented Apollo controls. |
| Interface | Keyboard type, multi-finger keyboard gesture, touchscreen mouse mode, mouse button/scroll swap, mouse acceleration. |
| Advanced | Debugging/log view and implementation-facing decode/image controls that should not be prominent. Existing semantics and compile-time availability remain intact. |

## Small reusable visual system

The implementation should add only primitives used in at least two surfaces.
Use Borealis composition and XML/C++ view classes; do not introduce another UI
framework.

### Tokens

| Token family | Proposed use |
| --- | --- |
| Typography | `Display` (host title), `Title` (page), `SectionTitle`, `Body`, `Secondary`, `Caption`. Handheld minimum body size must remain readable at 720p; docked sizes may use constrained responsive steps rather than uniform scaling. |
| Spacing | `XS 8`, `S 12`, `M 20`, `L 32`, `XL 48` logical pixels, with per-layout safe-area insets. Final values must be validated in the actual Borealis scale. |
| Color | Near-black canvas; elevated charcoal cards; off-white primary text; muted blue-grey secondary text; cyan/indigo Apollo accent; semantic green/amber/red status colors. Color is never the only focus/status signal. |
| Geometry | 12–16 px card radius, 8–12 px control radius, consistent 1–2 px focused outline, modest elevation only on focused items. |
| Motion | 100–160 ms focus scale/opacity and 160–220 ms screen/dialog transitions; no blur, live shadow, or per-frame artwork effects. Respect available Borealis animation mechanisms and disable/non-animate on performance regressions. |

### Components and required states

| Component | Purpose and states |
| --- | --- |
| `HostCard` | Host name, Apollo label when known, status, connection class when safely known. Default, Focused, Pressed, Offline, Fetching. |
| `GameCard` | Artwork, title, running/favorite badges, unavailable treatment. Default, Focused, Pressed, Disabled, Running, Favorite. |
| `SettingsRow` / `SelectorRow` / `ToggleRow` | Large title/value row with optional one-line explanation. Default, Focused, Pressed, Disabled, Error. |
| `SectionHeader` | Page/section title and optional contextual action/count. |
| `StatusBadge` / `StatusIndicator` | Text plus icon/shape: Online, Offline, Fetching, Apollo, network class. |
| `Dialog` / `ErrorSheet` | Clear primary decision; destructive option is distinct; Details exposes unchanged diagnostic string when present. |
| `LoadingState` | Meaningful title and only actually observed steps; supports cancel where existing flow allows it. |
| `StreamStatsPanel` | Off / Compact / Detailed modes drawn only from current session statistics. |

## Screen-by-screen behavior plan

### Hosts (UI02)

- Replace the permanent host-as-tab list with a controller-first card grid or
  vertical stack optimized for the number of saved hosts.
- Card hierarchy: host name, `ONLINE`/`OFFLINE`/`CHECKING`, Apollo badge only
  when capabilities confirm it, then compact LAN/Wi-Fi/Remote context only
  when known. Raw addresses move to Quick Actions/details.
- A opens Host Home. X opens Quick Actions. Add Host is a first-class final
  card/button. Settings/Favorites/About remain reachable from a clear global
  action/menu.
- Focus order is deterministic: row-major cards, then Add Host, then utility
  actions. Empty state focuses Add Host.

### Add Host and pairing (UI02 / UI08)

- Retain manual entry and discovery in one purposeful flow. Discovery list
  remains selectable and X refreshes it.
- Pairing uses the existing generated PIN and request sequence. A connection
  state view distinguishes only: searching/connecting, paired, and failed
  where those callbacks exist.
- Error view wording: a human summary followed by an optional Details action
  containing the unchanged raw result. Do not map or discard protocol errors.

### Host Home and application library (UI03)

- Header: host name, status badge, Apollo/network compact line when known.
- `Continue Playing` appears only when `currentGame` resolves to an app; no
  invented recency list.
- `Favorites` is a filtered row only if the host has favorites. `Applications`
  is the complete host list. The existing current-app-first/favorite-second
  sort is retained unless a presentation grouping safely consumes the same
  ordered list.
- Reuse cached art and `GameCard`; never prefetch/download an unbounded
  library merely to populate a carousel.
- A launches eligible apps. Y toggles favorite. X reloads. When a host reports
  a different app is running, unavailable cards retain the existing disabled
  behavior. Back continues to prompt for host-app termination only in the
  current semantics.

### Settings shell and categories (UI04–UI07)

- Create a settings landing page of large category rows. Each category opens a
  short scrolling list with the active value readable from one TV viewing
  distance.
- Move the existing setting wiring into category views without changing its
  `Settings` calls, range limits, defaults, conditional compilation, or save
  lifecycle. Factor row styling, not setting behavior.
- Profiles retain their exact three connection contexts. Display `ACTIVE` only
  for the currently resolved context/device state and only when it is known.
- Apollo remains limited to Virtual Display controls already implemented. For
  a host-specific entry point, use cached `ApolloCapabilities`; retain a
  global route for the existing global values so no setting disappears.

### Connection, dialogs, and failures (UI08)

- Replace bare spinners with the operation title and observable state:
  `Connecting to <host>` while host connect is pending; `Starting stream`
  after the existing stream start is invoked. Do not claim that an Apollo
  session is prepared unless an explicit observable event is added without
  touching protocol behavior.
- Standard confirmation placement: safe action first, destructive action
  second and visually distinct. Preserve existing callbacks and cancellation
  behavior exactly.
- Error sheet keeps `Try Again` only where the original operation can be
  retried safely; otherwise it exposes Close/Back and Details. Raw result
  remains accessible.

### Stream overlay and statistics (UI09)

- Keep the existing overlay shortcut and system-button paths unchanged.
- Refresh layout into Quick Settings, Input, Stats, and End Session. The
  existing settings remain, merely regrouped.
- Implement stats mode as UI-only state unless a persisted UI preference is
  specifically approved. `Detailed` preserves the existing NanoVG data set;
  `Compact` is limited to established resolution/FPS/codec/bitrate/session
  values. Do not show packet loss or latency if the live session does not
  supply them.
- The poor-connection indicator stays lightweight and does not allocate or
  blur per frame.

### Controller navigation and responsiveness (all phases)

- A confirms/selects; B returns/dismisses; X is refresh/quick actions where
  already conventional; Y is favorite/remove/reset where already
  conventional. L/R may change top-level sections only after focus tests.
- Every screen declares or verifies a default focus target. Empty/loading/error
  states must focus a recovery action. Dialogs must return focus to their
  invoking control where possible.
- Use safe margins around 1280 × 720 handheld and 1920 × 1080 docked output.
  Build breakpoints by available layout width and safe area—not a global scale
  multiplier.

## Incremental implementation plan and verification gate

| Phase | Scope | Required verification before next phase |
| --- | --- | --- |
| UI01 | Design tokens, shared row/card/status/dialog styling; no navigation change. | Desktop build; Switch build where toolchain is available; inspect focus/default/disabled states. |
| UI02 | Hosts, Add Host, pairing/loading/error surfaces. | Manual discovery/manual-host/pairing/Wake/rename/remove/controller routes. |
| UI03 | Host Home, app library, favorites, artwork cards. | App reload/launch/current app/favorite/forwarder/deep-link routes and focus recovery. |
| UI04 | Settings hub and category shell. | Verify every existing settings row appears once and writes the identical `Settings` value. |
| UI05 | Streaming/video/audio/interface categories. | Build plus targeted settings persistence and stream-start smoke test. |
| UI06 | Profiles. | Existing `profile_precedence_test`; manual Wi-Fi/Ethernet/Remote resolution checks. |
| UI07 | Apollo category. | Compatible/incompatible cached-host visibility and virtual-display behavior smoke test. |
| UI08 | Dialogs, errors, loading. | Pair/Wake/app-list/stream failure recovery and controller focus tests. |
| UI09 | Stream overlay/statistics/input presentation. | Stream, overlay shortcut, disconnect/terminate, mouse input, keyboard, and stats metrics check. |
| UI10 | Polish and performance pass. | Handheld and docked visual QA; memory/navigation responsiveness under streaming workload. |

## Expected files to change during implementation

No files below are changed in the planning pass.

| Area | Expected files | Scope limit |
| --- | --- | --- |
| UI bootstrap/tokens | `app/src/main.cpp`, new focused UI helper/header files under `app/include/views/` and `app/src/views/` | Register shared Borealis views, theme colors, and metrics only. |
| Main navigation and hosts | `app/include/main_tabs_view.hpp`, `app/src/main_tabs_view.cpp`, `app/include/host_tab.hpp`, `app/src/host_tab.cpp`, `app/include/add_host_tab.hpp`, `app/src/add_host_tab.cpp` | Replace presentation and navigation while retaining all host operations/callbacks. |
| Libraries/cards | `app/include/app_list_view.hpp`, `app/src/app_list_view.cpp`, `app/include/app_cell.hpp`, `app/src/app_cell.cpp`, `app/include/favorite_tab.hpp`, `app/src/favorite_tab.cpp`, optionally `grid_view.*` | Layout/focus/card treatment only; retain app-list, art-cache, favorite, launch, and forwarder behavior. |
| Settings/profiles | `app/include/settings_tab.hpp`, `app/src/settings_tab.cpp`, `app/include/stream_profiles_view.hpp`, `app/src/stream_profiles_view.cpp` plus new category views if warranted | Re-home controls without modifying setting semantics or serialization. |
| Streaming presentation | `app/include/loading_overlay.hpp`, `app/src/loading_overlay.cpp`, `app/include/streaming_view.hpp`, `app/src/streaming_view.cpp`, `app/include/ingame_overlay_view.hpp`, `app/src/ingame_overlay_view.cpp`, `app/include/streaming_input_overlay.hpp`, `app/src/streaming_input_overlay.cpp` | UI state and drawing only; no protocol/decoder/renderer/audio/input logic changes. |
| Reusable dialogs/input editor | `app/include/button_selecting_dialog.hpp`, `app/src/button_selecting_dialog.cpp`, `app/src/mapping_layout_editor.cpp`, `app/src/key_combo_settings.cpp`, `app/src/keyboard_view.cpp` | Visual and focus/accessibility improvements while preserving captured values and inputs. |
| Layout resources | `resources/xml/activity/main.xml`, `resources/xml/tabs/*.xml`, `resources/xml/views/*.xml`, `resources/xml/views/ingame_overlay/*.xml`, `resources/xml/cells/*.xml`, new narrowly scoped XML view files | Component composition/layout. |
| Text and assets | `resources/i18n/*/main.json`; new small icon assets only if existing font/icons cannot serve | Preserve current strings until migration; avoid large new texture packs. |
| Documentation/tests | `docs/ui/*.md`, existing `tests/profile_precedence_test.cpp` only if a UI-facing resolver assertion is necessary | Do not alter stream core tests/logic for visual work. |

### Explicitly out of scope

- `app/src/libgamestream/`, `app/src/streaming/` protocol/session/
  decoder/renderer/audio/input implementations, except minimal UI presentation
  calls in `StreamingView` that leave behavior and sequence intact.
- Apollo protocol implementation or capabilities beyond those currently
  exposed.
- Settings file format, migration, or underlying profile resolver semantics.
- Unrelated framework or project-wide architecture rewrites.

## Performance risks and mitigations

| Risk | Mitigation / acceptance criterion |
| --- | --- |
| More/larger game cards increase texture residency. | Continue using `BoxArtManager` cache; lazily load visible/near-visible art; keep one artwork representation; bound any new image cache. |
| Blur, translucency, shadows, and animations compete with stream rendering. | No backdrop blur; use flat alpha surfaces; short transform/opacity-only animations; test while streaming. |
| Card grids create excessive layout/reallocation on reload. | Reuse current incremental list behavior where possible, batch UI updates on main thread, and avoid rebuilding unchanged sections. |
| Stream stats text allocates/formats every frame. | Preserve current detailed mode behavior initially; compact mode should update at a bounded cadence and cache formatted strings where implementation permits. |
| Responsive layouts regress handheld readability or docked safe areas. | Verify at 1280×720 and 1920×1080 with controller-only traversal; establish minimum type sizes and edge insets before polish. |
| Refactoring the settings screen changes values accidentally. | Maintain an item-to-setter matrix from `current-ui.md`; test persistence and stream profile precedence after each category migration. |

## Approval-dependent decisions

1. Whether Favorites is a dedicated top-level dashboard section, a Hosts
   submenu, or both. The plan keeps it reachable in both places but avoids
   duplicate editing behavior.
2. Whether global Apollo virtual-display controls should be shown even before
   any compatible host capability has been cached. Hiding them entirely would
   remove an existing feature from some users; the safe default is a global
   Apollo category with clear compatibility copy.
3. Persistence for an Off/Compact/Detailed statistics display preference. The
   safe initial choice is session-only UI state, because current persistence
   has no equivalent setting.

## Stop condition

Planning is complete. Do not start UI01 or modify application UI files until
this plan is approved.
