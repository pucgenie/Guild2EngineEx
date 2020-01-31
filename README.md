# Guild2EngineEx
Guild II has LUA modding capabilities. Unfortunately, it is very limited. Therefore, I decided to expand those by some GuildII.exe patches and this extension DLL.

Currently this extension adds the following LUA functions:

|Name|Description|
|---|---|
|Number GeeType(Number or Table or String or Boolean variable)|Returns the type of the variable as number(1=bool, 3=number, 4=string, 5=table)|
|Number GeeVersion()|Returns the current engine extension version (=2)|
|Number GeeLength(String or Table)|Returns the length of the parameter|


# How to Install
- Build solution of this project with Visual Studio 2017 Community or download the latest binary [release](https://github.com/zokradonh/Guild2EngineEx/releases) (`Guild2EngineEx.dll`).
- Copy resulting `Guild2EngineEx.dll` to your Guild II program folder.
- Patch your GuildII.exe binary according to `gee.1337` in this repository with any hex editor. You can also use [x64dbg](https://x64dbg.com/) to import the .1337 file and patch your executable. (You don't need to copy `gee.1337` to game folder)


# How to Update
- Build or download newest `Guild2EngineEx.dll`
- Replace old file.
- If file `gee.1337` has been modified in the new version then you need to patch the `GuildII.exe` again, but I try to avoid updates there.