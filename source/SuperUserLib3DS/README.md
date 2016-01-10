# Lib SuperUser for Nintendo 3DS

## What this is

This lib is meant to gather various exploits of the system and use them to offer the user an elevation of his privileges over the console, from ARM11 and hopefully ARM9 side.

The term takes inspiration from the administrator privileged user nickname for UNIX systems.

It is currently a work in progress project, that will be implementing new exploits when they will be public available, trying to support
the latest system version possible.

## Stability

The library itself should not cause any brick, but i do not own any responsability of the use anyone makes of it.

The bootrate of the exploits is right now pretty reliable, but the app fails to return to the Homebrew Launcher.

## Exploits Used

Target | Exploit name | Support | Credits
------------ | ------------- | ------------- | -------------
ARM11 Kernel | Memchunkhax II | All 3DSes Ver. <= 10.3 | Reproduction of the exploit presented [here](https://media.ccc.de/v/32c3-7240-console_hacking). Credit goes so to derrek, smea and plutoo, while this implementation has been realized with the help of Steveice10, julian20, MassExplosion, TuxSH and motezazer.




