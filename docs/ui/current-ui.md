# Current UI inventory and feature-preservation checklist

Status: inspection complete on 2026-09-11. This document is the baseline for
the visual refresh; it describes current behavior, not desired behavior.

## UI technology and current composition

- The UI is a native C++20 [Borealis](https://github.com/natinusala/borealis)
  application. Layout is XML in `resources/xml/`; view behavior is in
  `app/src/` and public view declarations in `app/include/`.
- `MainActivity` loads `xml/activity/main.xml`, an `AppletFrame` containing a
  `MainTabs` `TabFrame`. The initial shell is a permanent sidebar: optional
  Favorites, one tab per saved host, Add host, Settings, and About.
- Most controls are Borealis `DetailCell`, `SelectorCell`, `BooleanCell`,
  `Slider`, `Dialog`, and `Header` primitives. There is no application design
  token set beyond two caption colors and three About-page metrics in
  `app/src/main.cpp`.
- App artwork is requested from the host on demand, written to the box-art
  cache, then compressed to a roughly 300 × 400 source image by
  `BoxArtManager`. App cards load that cached file.
- Streaming video is rendered directly by `StreamingView` / the existing
  session provider. The user interface overlays it using Borealis activities
  and NanoVG text. It must remain separate from the stream protocol,
  decoder, renderer, audio, and input implementation.
- Settings persist through the existing `Settings` model and serialization.
  Per-host favorites, profiles, quality aggregates, and detected Apollo
  capabilities also persist there. A visual refresh must not alter those
  schemas or value semantics.

## Screen and interaction inventory

| Current surface | Current behavior and controller actions | Redesign destination |
| --- | --- | --- |
| Main shell / sidebar | `MainTabs` dynamically presents Favorites only when one exists, every saved host, Add host, Settings, and About. Sidebar selection changes the content tab. | **Hosts** becomes the default dashboard. Saved hosts become large host cards; global destinations live in a compact utility menu. Favorites remains discoverable from Hosts and each Host Home. |
| Host tab | On entry, asynchronously connects and shows Fetching / Ready / Unable plus address. A opens app list if available; unavailable hosts can Wake on LAN; Start renames; Remove opens confirmation; Switch also exposes per-host streaming profiles. Apollo capabilities are detected after successful connect. | **Host Home**. Retain refresh/status, launch, Wake on LAN, rename, remove, capability detection, and profiles. Put rename/remove/profiles in Host Quick Actions and make A open the host. |
| Add host | Manual host address input, Connect, LAN discovery list, X refresh discovery, progress spinner, and discovery failure status. A discovered/manual host connection may prompt pairing with a generated PIN; already-paired hosts are saved after alert. | **Add Host** flow from Hosts. Keep manual address, discover, X refresh, pairing PIN, already-paired handling, cancellation, and all discovery errors. |
| Pairing/loading/error dialogs | Reusable spinner dialog is used for host connect, PIN pairing, and Wake on LAN. Generic error dialog displays raw result text. Alerts use a single Close button. | Shared **Connection state**, **Dialog**, and **Error sheet** components. Preserve all result strings and actions; present a friendly summary with expandable diagnostics where an error has technical detail. |
| Favorites tab | Groups saved favorite apps by host in 5-column grids. A launches; Y unfavorites; Switch/iOS X creates a forwarder. It refreshes after favorite changes. | **Favorites** section in Hosts / Host Home. Preserve host grouping, launch, Y removal, focus restoration, and forwarder creation. |
| App list | Reconnects and requests applications on entry. Displays seven-column art cards, sorts running app first then favorites, and marks running/favorite/unavailable cards. A launches active cards; Y toggles favorite; X reloads. If an app is running, Back asks to terminate it. | **Host Home** Applications and Games. Preserve server ordering rules, app availability, running state, art loading, A launch, Y favorite toggle, X reload, and Back termination prompt. Do not invent “Continue Playing” history: show only the current running app when the host reports one. |
| App card | Shows portrait box art, title, running-play icon, favorite-star icon, and grey inactive overlay when another app is running. | Reusable **GameCard**. Keep all four states: normal, focused/pressed, favorite, running, and unavailable. Artwork remains cached, not duplicated or transformed repeatedly. |
| Global settings | One long scrolling tab containing video, bitrate, image adjustment, stream/audio, controller, keyboard/mouse, Apollo virtual-display, and debug controls. Settings save when the view is destroyed. | **Settings hub** with category screens. Every listed row below remains available with identical storage and semantics. |
| Per-host stream profiles (Switch) | Three contexts: Local Wi-Fi, Local Ethernet, Remote. Each has custom-profile enablement and inherited/effective summary. A context exposes resolution, native scale, FPS, codec, HDR, frame pacing, bitrate, upscaling, dithering + strength, RCAS + strength. | **Profiles** category and a Host Quick Action. Retain all contexts, inherited values, effective summaries, custom enablement, capability/compile-time visibility, save timing, and precedence. The UI may also show device state (handheld/docked) as resolved information; it must not add new profile semantics. |
| Controller mapping selection | Selects an existing game mapping layout or creates one. Y on an editable layout opens the editor. | **Controllers → Game mapping**. Preserve creation, selection, edit eligibility, and existing default layouts. |
| Mapping layout editor | Four columns of 16 controller buttons. A chooses a replacement; Y resets an overridden button. RB or header click renames; Back removes layout after confirmation. | Full-screen **Mapping Editor**. Preserve the exact edit/reset/rename/remove behaviors and controller actions, while making destructive actions explicit. |
| Button-combination capture | Non-cancelable dialog captures buttons. One-key capture immediately saves; multi-key capture confirms after four seconds; some actions allow Off. Conflicting combinations show an error. | Shared **Shortcut capture dialog**. Preserve timer, Off eligibility, conflict protection, and all target actions. |
| Stream start | `StreamingView` immediately renders a generic full-screen spinner, connects host, resolves profile, applies existing Apollo virtual-display overrides where relevant, and starts the existing session. Failure terminates the view after showing raw error. | **Connecting** screen. It may label only observed states: host connection complete, stream start requested, and failure. No fabricated session stages or changed sequencing. |
| Streaming view | Direct session video. It prevents dimming, forwards controller/touch/mouse input, invokes configured shortcuts, and closes on terminated session. A small “Bad connection...” NanoVG message appears when the session reports poor connection. | Preserve rendering and input unchanged. Replace only presentation of the poor-connection notice with a lightweight status indicator. |
| Streaming statistics | In-game Options toggle enables a detailed green NanoVG diagnostic text dump (FPS, dropped frames, timing, GPU/post-processing when available, and frame-queue metrics). | **Streaming overlay → Stats** with Off / Compact / Detailed presentation. Compact must contain only values already available from the session/settings; Detailed retains every existing diagnostic metric. |
| In-game overlay | Configured controller combo or configured Home/Screenshot override opens a two-tab overlay: Options and Logout. A touch edge swipe can open it when no controllers are connected. | **Stream overlay** with the same activation paths and choices. Retain B dismissal, pointer/input lock handling, and all current options/logout actions. |
| In-game Options | Mouse-input overlay launch; keyboard type/finger count; touchscreen mouse; Guide shortcut and system override; host close/switch-window shortcuts; volume; rumble; stick-as-D-pad; mouse acceleration; image adjustment settings; debug statistics and show-logs. | **Stream overlay → Quick settings / Input / Diagnostics**. Keep all rows and their immediate effects. |
| In-game Logout | Disconnect stops the client stream; Terminate stops the host application as well. | **Stream overlay → End session** confirmation. Preserve the distinct disconnect versus terminate-host behavior. |
| Mouse-input overlay | Translucent full-screen mode: sticks move/scroll mouse; triggers click; B hides hints or exits; X toggles keyboard; keyboard mode uses LB/RB arrows, Y space, X delete. | **Mouse & Keyboard overlay** with the same controls and input behavior. |
| On-screen keyboard | Compact/full keyboard with touch and controller focus, language selection, shift/toggle behavior, haptic feedback, and key forwarding. Multi-finger touch in streaming can open it. | Preserve as an input surface; only reskin safe visual elements after controller/focus regression testing. |
| About | Apollo logo, version, provenance/thanks text, and GitHub link. Patreon/GBAtemp UI exists in XML but is intentionally inactive in code. | **About** in utility menu. Preserve active GitHub link and unchanged non-active links. |
| Forwarder/deep launch | Favorite cards can create Switch forwarders. Command-line/deep-link launch bypasses the dashboard and opens a stream for a matched saved host/app. Applet-mode unsupported is shown as a dialog. | Not visually central, but all launch paths remain untouched. Any refreshed connection screen must work for normal and direct launches. |

## Settings preservation checklist

The following is the current exposed settings set. A checked item is a design
requirement for implementation—not evidence that a future implementation has
been verified.

### Streaming and video

- [ ] Resolution (Native, 360p, 480p, 540p, 720p, 1080p, and 1440p where supported).
- [ ] Native-resolution scale (0.5×, 0.75×, 1×, and 2× where supported).
- [ ] Frame rate (platform-specific available rates).
- [ ] Video codec (H.264, HEVC, and AV1 only when available).
- [ ] Request HDR only when compiled/supported.
- [ ] Decoder thread count.
- [ ] Switch frame-pacing mode.
- [ ] Hardware decoding control where the platform permits it.
- [ ] Video bitrate and its existing platform limits.
- [ ] Streaming Optimal Playable Settings.
- [ ] Play Audio on PC.
- [ ] Audio driver selector where available.
- [ ] Volume amplification and stream-overlay volume control.
- [ ] Upscaling mode/toggle only where compiled/supported.
- [ ] Dithering toggle and strength only where compiled/supported.
- [ ] RCAS toggle and strength only where compiled/supported.

### Profiles

- [ ] Local Wi-Fi profile.
- [ ] Local Ethernet profile.
- [ ] Remote profile.
- [ ] Enable/disable custom profile and global-default inheritance.
- [ ] Effective profile summary (resolution, FPS, codec, bitrate, HDR, pacing and image adjustments as currently resolved).
- [ ] Existing profile-precedence behavior and persistence.

### Controllers, shortcuts, keyboard, and mouse

- [ ] UI A/B and X/Y swap.
- [ ] Game mapping layout selection and creation.
- [ ] Game mapping layout editing, individual reset, rename, and remove.
- [ ] Left and right stick dead zones.
- [ ] Rumble strength.
- [ ] Single Joy-Con stick-as-D-pad.
- [ ] Guide button combination and Switch system-button override.
- [ ] In-game overlay hold duration, button combination, and system-button override.
- [ ] Host close-app (Alt+F4) combination.
- [ ] Host switch-window (Alt+Tab) combination.
- [ ] Mouse-input hold duration and combination.
- [ ] Keyboard type and multi-finger open threshold/Off.
- [ ] Touchscreen mouse mode.
- [ ] Swap mouse buttons.
- [ ] Invert vertical mouse scrolling.
- [ ] Mouse acceleration.
- [ ] On-screen keyboard language and all existing key forwarding behavior.

### Apollo and diagnostics

- [ ] Virtual Display mode: Off, Automatic, Always.
- [ ] Virtual Display resolution: Auto, 720p, 1080p, 1440p.
- [ ] Virtual Display refresh rate: Auto/60 Hz, 30 Hz, 60 Hz, 120 Hz.
- [ ] Detected Apollo/Sunshine capability data must remain available and gate host-specific presentation.
- [ ] Debugging view / log visibility.
- [ ] Stream statistics toggle and its complete existing metric set.

## Feature preservation checklist

- [ ] Saved host list, dynamic tab/menu refresh, host rename, and host removal.
- [ ] Manual host address handling, including remote-address classification.
- [ ] LAN host discovery, refresh, cancellation/pause lifecycle, and discovery failure feedback.
- [ ] Connection state refresh, active-address choice, offline state, and Wake on LAN.
- [ ] Pairing PIN creation, pairing request, already-paired handling, success persistence, and failure recovery.
- [ ] Host app-list request, current-app detection, sorting, reload, and error dismissal path.
- [ ] Box-art download, cache, compression, and cached-image reuse.
- [ ] Launching an app from host list, favorites, forwarder, command-line, and deep link.
- [ ] Current-running-app restriction and host application termination confirmation.
- [ ] Favorite add/remove and focus recovery after removal.
- [ ] Switch forwarder generation and installation feedback.
- [ ] Existing stream profile resolution, device/network context detection, and global-default fallback.
- [ ] Existing Apollo virtual-display overrides applied only to compatible server sessions.
- [ ] Existing protocol handshake, pairing, stream negotiation, decoder, rendering, audio, controller forwarding, touch/mouse forwarding, and session teardown.
- [ ] In-game overlay activation through configured combination, configured system button, keyboard Escape long-press, and controller-less touch gesture.
- [ ] In-game disconnect vs host-app terminate distinction.
- [ ] Mouse-input mode and on-screen keyboard entry/exit controls.
- [ ] All dialogs: remove host/layout, terminate app, key capture, errors, alerts, loading/cancellation, and applet-mode unsupported.
- [ ] Back navigation always returns to the prior surface without losing a reachable focus target.
- [ ] Localization: new strings added in English and tracked for every existing locale; no current string key removed until all callers are migrated.

## Current controller conventions

| Context | Current convention |
| --- | --- |
| Standard Borealis lists/cards | A activates focused control; B is framework back/dismiss unless overridden. |
| Host | Start renames host. |
| Add Host | X refreshes discovery. |
| App list/card | X reloads app list; Y favorites/unfavorites; Back terminates current host app when present. |
| Favorites card | Y removes favorite; X creates forwarder on Switch/iOS. |
| Mapping editor | A selects replacement; Y resets override; RB renames; Back removes layout. |
| Streaming overlay | User-configurable button combination or configured Home/Screenshot override opens it. |
| Mouse-input overlay | B hide hints/exit; X keyboard; keyboard has LB/RB arrows, Y space, X delete. |

## Known current UI constraints and preservation uncertainties

1. `ApolloCapabilities::detect()` currently identifies any Sunshine-compatible
   server and reports a fixed capability set. The global Apollo virtual-display
   controls are always shown today, even before a compatible host has been
   contacted. The refreshed UI can correctly gate *host-specific* options by
   cached capabilities, but must not silently make an existing global setting
   unreachable. This needs an explicit implementation decision.
2. The current profile model is network-context based (Wi-Fi/Ethernet/Remote),
   while device mode is part of resolution at stream launch rather than a
   separate editable profile. The requested Device section can be a resolved,
   informational view only unless new semantics are approved.
3. The app list does not provide historical play data. “Continue Playing” can
   safely represent the current host-reported running app only; an empty state
   is required otherwise.
4. Current stream statistics do not expose a single canonical packet-loss or
   RTT field in the inspected overlay path. Compact overlay metrics must be
   selected from existing session data and must not fabricate missing values.
