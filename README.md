
[Official GBAtemp thread](http://gbatemp.net/threads/wip-safesysupdater.409392/)

This is a fork of [SysUpdater](https://github.com/profi200/sysUpdater) who permit 3DS user on firmware between 9.3 and 10.3 to downgrade.

This app is an improvement of TuxSH memchunkhax2 implementation (who was been,infortunately,deleted of github) and can:

- automatically load update information (needed files, md5) based on your 3ds model and region
- md5 check all the cia update files before downgrading
- can check your update files without actually downgrading

To use it you have to :

- Put SafeSysUpdater.3dsx somewhere you can launch it...
- Put your downgrade pack as usual ("/updates/*.cia")
- Launch it multiple times until it pass the black screen (hax)...
- Pray


    PLEASE NOTE THAT THIS HAVE THE SAME DOWNGRADE CODE THAN ORIGINAL SYSUPDATER
    DOWNGRADE PROCESS IS NEVER 100% SAFE
    BE SURE TO BE ON 10.3 BEFORE DOWNGRADING, SEEMS TO PREVENT SOME SOFT BRICK
    SOME TIPS TO PREVENT A BRICK : [HERE](https://gbatemp.net/threads/wip-list-of-known-bricks-due-to-an-attempt-to-downgrade-to-9-2.407920/)


Notes:
- If it freeze just after you press (Y) to downgrade or "init ->" more than 1 second, restart..
- The 9.2 JPN package is in fact a 9.1 package, no problem with this..

Changes :

- v09: add downgrade logs ("/SafeSys.log")
- v08: fix JPN files path..
- v07: finally fixed the input freeze... by removing this second confirmation screen.
- v06: try to fix input freeze... again
- v05: minor input fix
- v04: added japan support (with bad title skipping)
- v03: big improvement in hax success rate
- v02: Add simulation mode to check your update files without actually downgrading
- v01: Removed the config files, just put the ".3dsx" file somewhere and downgrade... and pray.
- v01: Added a CIA version to downgrade from emunand so you don't need to try hard to get the hax to succeed (so it's just for testing/reporting bugs). Testing this will ensure the MD5 for each regions/models (only o3DS/n3DS USA/EUR for now) and downgrade process is correct/working. Thanks for reports !

Credits:
- derrek for the initial memchunkhax2 flaw discovery
- [Steveice10](https://github.com/Steveice10) and all people who worked on [memchunkhax2](https://github.com/Steveice10/memchunkhax2)
- [TuxSH](http://github.com/TuxSH) for the downgrade code (memchunkhax2 implementation)
- This app was totally rewritted,but thank profi200 for the original app

Screenshot:

![SafeSysUpdater Top](http://files.mydedibox.fr/files/Dev/3ds/ssu1.png "SafeSysUpdater Top")

![SafeSysUpdater Down](http://files.mydedibox.fr/files/Dev/3ds/ssu2.png "SafeSysUpdater Down")

