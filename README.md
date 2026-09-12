# mbl

Small graphics lib for my projects

## Modules

- **platform** — SDL2 window creation and per-frame input polling.
- **render** — shaders, textures, texture atlases, meshes, framebuffers, camera, bitmap fonts.
- **ui** — a static immediate-mode UI system (text, buttons, toggles, sliders, text input).
- **net** — minimal non-blocking TCP client/server and packet definitions.
- **utils** — AABBs, a stopwatch, facing/direction helpers, a thread pool.
- **math** — `vec<N, T>` / `mat<N, T>` templates and related free functions.
- **loader** — `.obj` mesh loading and `stb_image`-based texture loading.

Browse the generated API reference at https://scorpionnem.github.io/mbl/, or build it locally:

```sh
doxygen Doxyfile
open doxygen-build/html/index.html
```

## Building the library

```sh
make
```

Produces `libmbl.a`. Requires SDL2 (`sdl2-config` on `PATH`) and an OpenGL dev environment.
