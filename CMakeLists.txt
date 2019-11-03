cmake_minimum_required(VERSION 3.10)
project(crimesquad)

set(CMAKE_CXX_STANDARD 14)

add_definitions(-DDONT_INCLUDE_SDL -D_CRT_SECURE_NO_WARNINGS) #todo: disable "no warnings"

set( SDL_STATIC ON CACHE BOOL "" FORCE )
set( SDL_SHARED OFF CACHE BOOL "" FORCE )
add_subdirectory(lib/libsdl2)

if(WIN32 OR MSVC)
    add_subdirectory(lib/pdcurses)
    link_libraries(pdcurses)
else()
    set(CURSES_NEED_NCURSES TRUE)
    find_package(Curses REQUIRED)
    include_directories(${CURSES_INCLUDE_DIRS})
    link_libraries(${CURSES_LIBRARIES})
endif()

link_libraries(SDL2-static)

include_directories(src)
include_directories(src/common)
include_directories(src/creature)
include_directories(src/daily)
include_directories(src/items)
include_directories(src/locations)
include_directories(src/log)
include_directories(src/monthly)
include_directories(src/news)
include_directories(src/politics)
include_directories(src/sitemode)
include_directories(src/vehicle)
include_directories(tests/include)
include_directories(tests/include/catch)

add_executable(crimesquad
    src/basemode/activate.cpp
    src/basemode/activate_sleepers.cpp
    src/basemode/baseactions.cpp
    src/basemode/basemode.cpp
    src/basemode/liberalagenda.cpp
    src/basemode/reviewmode.cpp
    src/combat/chase.cpp
    src/combat/fight.cpp
    src/combat/haulkidnap.cpp
    src/common/commonactions.cpp
    src/common/commondisplay.cpp
    src/common/consolesupport.cpp
    src/common/equipment.cpp
    src/common/getnames.cpp
    src/common/help.cpp
    src/common/interval.cpp
    src/common/interval.h
    src/common/misc.cpp
    src/common/squad.cpp
    src/common/stringconversion.cpp
    src/common/translateid.cpp
    src/creature/attributes.cpp
    src/creature/attributes.h
    src/creature/augmentation.cpp
    src/creature/augmentation.h
    src/creature/augmenttype.cpp
    src/creature/augmenttype.h
    src/creature/creature.cpp
    src/creature/creature.h
    src/creature/creaturenames.cpp
    src/creature/creaturetype.cpp
    src/creature/creaturetype.h
    src/creature/creaturetypecache.cpp
    src/creature/creaturetypecache.h
    src/creature/creaturetypes.cpp
    src/creature/gender.h
    src/creature/skill.cpp
    src/creature/skill.h
    src/daily/activities.cpp
    src/daily/activities.h
    src/daily/daily.cpp
    src/daily/daily.h
    src/daily/date.cpp
    src/daily/date.h
    src/daily/interrogation.cpp
    src/daily/recruit.cpp
    src/daily/recruit.h
    src/daily/shopsnstuff.cpp
    src/daily/shopsnstuff.h
    src/daily/siege.cpp
    src/daily/siege.h
    src/items/armor.cpp
    src/items/armor.h
    src/items/armortype.cpp
    src/items/armortype.h
    src/items/clip.cpp
    src/items/clip.h
    src/items/cliptype.cpp
    src/items/cliptype.h
    src/items/item.cpp
    src/items/item.h
    src/items/itemtype.cpp
    src/items/itemtype.h
    src/items/loot.cpp
    src/items/loot.h
    src/items/loottype.cpp
    src/items/loottype.h
    src/items/money.cpp
    src/items/money.h
    src/items/weapon.cpp
    src/items/weapon.h
    src/items/weapontype.cpp
    src/items/weapontype.h
    src/locations/locations.cpp
    src/locations/locations.h
    src/locations/world.cpp
    src/locations/world.h
    src/log/log.cpp
    src/log/log.h
    src/monthly/justice.cpp
    src/monthly/justice.h
    src/monthly/lcsmonthly.cpp
    src/monthly/monthly.cpp
    src/monthly/monthly.h
    src/monthly/sleepers.cpp
    src/monthly/sleepers.h
    src/news/ads.cpp
    src/news/headline.cpp
    src/news/layout.cpp
    src/news/majorevent.cpp
    src/news/news.cpp
    src/news/news.h
    src/news/squadstory_text.cpp
    src/politics/alignment.cpp
    src/politics/alignment.h
    src/politics/amendments.cpp
    src/politics/amendments.h
    src/politics/law.cpp
    src/politics/law.h
    src/politics/politics.cpp
    src/politics/politics.h
    src/sitemode/advance.cpp
    src/sitemode/mapspecials.cpp
    src/sitemode/miscactions.cpp
    src/sitemode/newencounter.cpp
    src/sitemode/shop.cpp
    src/sitemode/shop.h
    src/sitemode/sitedisplay.cpp
    src/sitemode/sitemap.cpp
    src/sitemode/sitemap.h
    src/sitemode/sitemode.cpp
    src/sitemode/stealth.cpp
    src/sitemode/talk.cpp
    src/title/highscore.cpp
    src/title/initfile.cpp
    src/title/newgame.cpp
    src/title/saveload.cpp
    src/title/titlescreen.cpp
    src/vehicle/vehicle.cpp
    src/vehicle/vehicle.h
    src/vehicle/vehicletype.cpp
    src/vehicle/vehicletype.h
    src/common.h
    src/compat.cpp
    src/compat.h
    src/configfile.cpp
    src/configfile.h
    src/cursesgraphics.cpp
    src/cursesgraphics.h
    src/cursesmovie.cpp
    src/cursesmovie.h
    src/externs.h
    src/game.cpp
    src/globals.cpp
    src/includes.h
    src/lcsio.cpp
    src/lcsio.h
    src/tinydir.h
    src/tinyxml2.cpp
    src/tinyxml2.h
    )

#   tests/common/test-interval.cpp
#   tests/creature/test-attributes.cpp
#   tests/creature/test-creaturetype.cpp
#   tests/creature/test-creaturetypecache.cpp
#   tests/creature/test-skill.cpp
#   tests/creature/test-uniquecreatures.cpp
#   tests/include/catch/catch.hpp
#   tests/include/catch/catch_reporter_tap.hpp
#   tests/politics/test-alignment.cpp
#   tests/vehicle/test-vehicletype.cpp
#   tests/test-lcs.cpp