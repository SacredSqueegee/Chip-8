# Chip-8 Interpreter

### Current Status:
> #### Working on:
> - SDL2 graphics for Chip-8 implementation
> - Chip-8 object definitions

Notes about the project so far:
- built on macOS w/ m1 chip
- relies on machines using little endianness
    - So far, only the color configuration is reliant on this.
- relies on sdl2 libraries being in /opt/homebrew/Cellar/sdl2/2.28.3
    - makefile is reliant on this

### TODO Items:
- add RAM viewer
- add register viewer
- add instruction stepping capability
    - need to figure out how to handle timers during this?
    - maybe calculate avg. instruction time and advance timer that much???
- add pause feature
- add keyboard re-mapping feature
- add ROM hot reloading feature
- add save states???
- add live display resizing/scaling

### Helper Function TODO Items:
- Better logging features
    - more colors
    - nesting support
    - etc
- Custom SDL2 wrapper specifically for emulators