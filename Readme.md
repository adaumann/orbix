# Orbix V1.0.1
==============

Game for C64/C128 machines

*Copyright 2024 - fleischgemuese*

V1.0.1: 
* C128 Turbo mode support - Nearly doubled the speed! - Please use x128.exe from Vice to run
* Better handling for deleting eliminated orbz


## Download
You can download binaries from itch.io: https://fleischgemuese.itch.io. Donation is welcome

## Description
Welcome to the Orbix solar system! Your mission is simple: eliminate all the orange Orbz by launching a ball from the 
top to clear each level. But it's not just about accuracy-plan your shots carefully, use puzzle-solving skills, or 
rely on a bit of luck to conquer the game.

Watch out for special elements like:

* Tray: Save balls by targeting the tray.
* Cannons: Catch the balls and fire them in your wanted direction.
* Portals: Teleport your ball to different parts of the screen.
* Lasers: Dangerous, do not touch them or deactivate them.
* Switches: Unlock hidden areas or activate special mechanics.
* Bombs: Cause explosive chain reactions to clear obstacles.
* Gravity swaps: Change the direction of gravity to add a twist to your shots.
* Extra balls: Need some more power, unlock other balls.
* Bumbers: Boost your ball.

Plan ahead, aim wisely, and overcome every challenge to rack up as many points as you can! But be careful-the number of balls is limited. Run out, and its game over.

## Installation
You need Oscar C64 Compiler V1.13.255 (https://github.com/drmortalwombat/oscar64/releases/download/v1.31.255/oscar64setup.msi) (newer versions need more memory, need to fix it later) : https://github.com/drmortalwombat/oscar64 to compile, in src/ Folder run build.bat or build_128.bat

This game is easyflash only. Therefore you need to build a cartridge to play on real hardware. Please refer easyflash documentation.

Alternativly you can use Ultimate II or Ultimate 64 extension module

The game is fully playable on Vice Emulator: VICE - the Versatile Commodore Emulator. Is is recommended to use the C128 (x128.exe) emulator instead of C64 (x64sc.exe) which has a smoother gameplay

Also the game runs on TheC64 machine... retro games

It was tested with PAL, NTSC

## Controls
--------
Play with Joystick port#2. Move left/Right to choose direction, fire to shoot. Press Fire+Down to self-destruct.

## Scoring system

The more Orbz eliminated the multiplier rises which multiplies the points.
* Orange orbz: 100 points.
* Blue orbz: 10 points.
* Bonus orbz: 500 points.
* All clear Bonus orbz: 500 points.
* Round score: If the ball is gone you get points for hit orbz.
* Level score: When the level is completed, you get points for remaining balls.

## Credits
Programming, graphics, and levels by Andy Daumann (fleischgemuese).
Music by picrard.
Special thanks to DrMortalWombat for providing Oscar 64 C Compiler and the great bitmap C library. Tools used: Oscar64 C Compiler, Sid SFX, Goattracker

## Licence
All rights reserverd