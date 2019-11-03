cmake_minimum_required(VERSION 3.10)
project(pdcurses C)

set(CMAKE_C_STANDARD 11)

include_directories(../libsdl2/include)
link_directories(../libsdl2/cmake-build-release/) #todo: do correctly
link_libraries(SDL2-static)

include_directories(.)
include_directories(sdl2)

add_library(pdcurses
        sdl2/deffont.h
        sdl2/deficon.h
        sdl2/pdcclip.c
        sdl2/pdcdisp.c
        sdl2/pdcgetsc.c
        sdl2/pdckbd.c
        sdl2/pdcscrn.c
        sdl2/pdcsdl.h
        sdl2/pdcsetsc.c
        sdl2/pdcutil.c
        sdl2/sdltest.c
        acs_defs.h
        addch.c
        addchstr.c
        addstr.c
        attr.c
        beep.c
        bkgd.c
        border.c
        clear.c
        color.c
        curses.h
        curspriv.h
        debug.c
        delch.c
        deleteln.c
        deprec.c
        getch.c
        getstr.c
        getyx.c
        inch.c
        inchstr.c
        initscr.c
        inopts.c
        insch.c
        insstr.c
        instr.c
        kernel.c
        keyname.c
        mouse.c
        move.c
        outopts.c
        overlay.c
        pad.c
        panel.c
        panel.h
        printw.c
        refresh.c
        scanw.c
        scr_dump.c
        scroll.c
        slk.c
        term.h
        termattr.c
        terminfo.c
        touch.c
        util.c
        window.c)


target_include_directories(pdcurses
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        )