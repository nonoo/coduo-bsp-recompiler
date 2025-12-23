Call Of Duty 1: United Offensive BSP recompiler
===============================================

This vibe coded toolkit can be used to manually edit maps (BSP files),
for ex. add vehicles to already existing maps.

- **dumpbsp**: dumps given .bsp file to editable text
- **mkbsp**: constructs a .bsp file based on the text output of dumpbsp

Note: if no cmdline arg is given to mkbsp then it reads from stdin.

Examples
--------

Adding a BMW motorcycle
***********************

1. Download the
[zzz_vehicle_bmw.pk3](https://www.moddb.com/games/call-of-duty-united-offensive/downloads/coduo-mp-vehicles-test-map)
and place it in the **uo** directory of the game.

2. Run **dumpbsp**, then open the text for editing.

3. Add this to end of the **Materials lump (0)**:

```
[164] Name: textures/common/clip_nosight_metal, Flags: 0xD040A0 0x28031640
```

If you have 12 materials then change index between the [] brackets to 13.
If you already have a clip_nosight_metal entry then you don't need to add
another.

4. Go to the **Entities lump (29)** and add this to the end:

```
{
"classname" "script_vehicle"
"model" "xmodel/mp_vehicle_bmw_mitsteib"
"vehicletype" "bmw_mp"
"angles" "0 270 0"
"origin" "-136 -1264 -4"
}
{
"model" "*5"
"classname" "script_vehicle_collmap"
"targetname" "xmodel/mp_vehicle_bmw_mitsteib"
}
```

5. Add this to the end of the **Models lump (27)**:

```
[5] Mins: (C2500000 C2180000 00000000), Maxs: (42480000 41C00000 42200000)
    TriSoups: off=0 size=0
    ColAABBs: off=0 size=0
    Brushes:  off=3369 size=2
```

Make sure the correct new index is used between the [] brackets.
Change the model index in the Entities lump if needed from *5 to the new
index.

You'll need to change the Brushes offset too, but first add the new brushes:

6. Add this to the end of the **Brushes lump (4)**:

```
[3369] Sides: 8, MatID: 164
[3370] Sides: 6, MatID: 164
```

Make sure that correct new indexes are used between the [] brackets and also
update the Brushes index at the model list (it should point to the index of
the first line).

Update the material ID (MatID) to the material index of clip_nosight_metal.

7. Add these new entries to the **Brushsides lump (3)**:

```
[28777] Union: C2500000, MatID: 164
[28778] Union: 41B00000, MatID: 164
[28779] Union: C2180000, MatID: 164
[28780] Union: 40000000, MatID: 164
[28781] Union: 80000000, MatID: 164
[28782] Union: 42200000, MatID: 164
[28783] Union: 0000000D, MatID: 164
[28784] Union: 0000000C, MatID: 164
[28785] Union: C2500000, MatID: 164
[28786] Union: 42480000, MatID: 164
[28787] Union: 41200000, MatID: 164
[28788] Union: 41C00000, MatID: 164
[28789] Union: 80000000, MatID: 164
[28790] Union: 41D00000, MatID: 164
```

Update the material ID (MatID) to the material index of clip_nosight_metal.
