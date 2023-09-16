# Chip-8 Interpreter

### Current Status:
> #### Working on SDL2 graphics for Chip-8 implementation

Notes about the project so far:
- built on macOS w/ m1 chip
- relies on machines using little endianness
    - So far, only the color configuration is reliant on this.
- relies on sdl2 libraries being in /opt/homebrew/Cellar/sdl2/2.28.3
    - makefile is reliant on this
