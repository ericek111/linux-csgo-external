linux-csgo-external
===================

Linux CS:GO cheat with X11 overlay

Features:
* *Glow ESP*
* *WallHack*
* *BunnyHop*
* *No Post-Processing* *(numpad 1)*
* *Recoil cross*
* *Netvar manager and dumper (-d CLI argument)*

## BunnyHop
Bind +alt1 to whatever key you want in CS:GO.

## Toggles
| Hack          | Key           |
| ------------- |:-------------:|
| Glow          | Alt_L         |
| Glow (guns)   | Control_R     |
| BunnyHop      | +alt1         |
| NoPP          | KP_1 (numpad) |

### Installation
```
sudo apt-get update && sudo apt-get upgrade
sudo apt-get install make cmake gcc git libcurl-dev libx11-dev libxtst-dev
git clone https://raw.githubusercontent.com/ericek111/linux-csgo-external
cd linux-csgo-external && cmake . && make
sudo ./build/csgo_external
```

## Tested Distributions
- Ubuntu
- Xubuntu
- Arch Linux
- Debian (Jessie + Sid)
- Gentoo

## Credits:
- Original author: s0beit
- Original author credits: BoToX & buFFy
- Maintainer/Updater: laazyboy13, owerosu, McSwaggens
- Overlay, current maintainer: ericek111

## TODO:
- allow properties of nested netvar tables to be retrieved - some kind of tree structure?
- use OpenGL for overlay (?)
- clickable GUI (something like ImGUI, maybe custom?)
- **make the code better, it is horrible**
- add aimbot/RCS
- eventually give up and focus on Teklad's [csgo-horker](https://github.com/Teklad/csgo-horker) instead
