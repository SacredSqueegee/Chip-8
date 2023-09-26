# Chip-8 Interpreter

### Current Status:
> #### Working on:
> - Chip-8 instruction emulation 0xIXXX
>   - I -> Instruction upper nibble
>   - XXX -> Rest of instruction
>       - ~~0x0~~
>           - 0x0nnn -> SYS addr -> Not Implementing
>           - ~~0x00E0 -> CLS~~
>           - ~~0x00EE -> RET~~
>       - ~~0x1~~
>           - ~~0x1nnn -> JP addr~~
>       - ~~0x2~~
>           - ~~0x2nnn -> CALL addr~~
>       - 0x3
>           - 0x3xkk -> SE Vx, byte
>       - 0x4
>           - 0x4xkk -> SE Vx, Vy
>       - 0x5
>           - 0x5xy0 -> SE Vx, Vy
>       - 0x6
>           - 0x6xkk -> LD Vx, byte
>       - 0x7
>           - 0x7xkk -> ADD Vx, byte
>       - 0x8
>           - 0x8xy0 -> LD Vx, Vy
>           - 0x8xy1 -> OR Vx, Vy
>           - 0x8xy2 -> AND Vx, Vy
>           - 0x8xy3 -> XOR Vx, Vy
>           - 0x8xy4 -> ADD Vx, Vy
>           - 0x8xy5 -> SUB Vx, Vy
>           - 0x8xy6 -> SHR Vx {, Vy}
>           - 0x8xy7 -> SUBN Vx, Vy
>           - 0x8xyE -> SHL Vx {, Vy}
>       - 0x9
>           - 0x9xy0 -> SNE Vx, Vy
>       - 0xa
>           - 0xAnnn -> LD I, addr
>       - 0xb
>           - 0xBnnn -> JP V0, addr
>       - 0xc
>           - 0xCxkk -> RND Vx, byte
>       - 0xd
>           - 0xDxyn -> DRW Vx, Vy, nibble
>       - 0xe
>           - 0xEx9E -> SKP Vx
>           - 0xExA1 -> SKNP Vx
>       - 0xf
>           - 0xFx07 -> LD Vx, DT
>           - 0xFx0A -> LD Vx, K
>           - 0xFx15 -> LD DT, Vx
>           - 0xFx18 -> LD ST, Vx
>           - 0xFx1E -> ADD I, Vx
>           - 0xFx29 -> LD F, Vx
>           - 0xFx33 -> LD B, Vx
>           - 0xFx55 -> LD [I], Vx
>           - 0xFx65 -> LD Vx, [I]
> - ~~SDL2 graphics for Chip-8 implementation~~
> - ~~Chip-8 object definitions~~

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