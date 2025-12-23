# Call Of Duty 1: United Offensive BSP recompiler

This vibe coded toolkit can be used to manually edit maps (BSP files),
for ex. add vehicles to already existing maps.

- **dumpbsp**: dumps given .bsp file to editable text
- **mkbsp**: constructs a .bsp file based on the text output of dumpbsp
- **getbrushes**: dumps brushsides for given brush(es)

Note: if no cmdline arg is given to mkbsp then it reads from stdin.

## Example #1: Adding a BMW motorcycle

1. Download the
[zzz_vehicle_bmw.pk3](https://www.moddb.com/games/call-of-duty-united-offensive/downloads/coduo-mp-vehicles-test-map)
and place it in the `uo` directory of the game.

2. Run **dumpbsp**, then open the text for editing.

3. Add this to end of the `Materials lump (0)`:

```
[164] Name: textures/common/clip_nosight_metal, Flags: 0xD040A0 0x28031640
```

If you have 12 materials then change index between the [] brackets to 13.
If you already have a clip_nosight_metal entry then you don't need to add
another.

4. Go to the `Entities lump (29)` and add this to the end:

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

5. Add this to the end of the `Models lump (27)`:

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

6. Add this to the end of the `Brushes lump (4)`:

```
[3369] Sides: 8, MatID: 164
[3370] Sides: 6, MatID: 164
```

Make sure that correct new indexes are used between the [] brackets and also
update the Brushes index at the model list (it should point to the index of
the first line).

Update the material ID (MatID) to the material index of clip_nosight_metal.

7. Add these new entries to the `Brushsides lump (3)`:

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

8. Add the following line to the .gsc file of your map as described
[here](https://www.moddb.com/games/call-of-duty-united-offensive/tutorials/vehicles-in-coduo-mp):

```
level thread maps\mp\_vehdrive_dmod::main();
```

9. Add **soundloadspecs/mp/yourmapname.csv** to enable vehicle sounds:

```
#DEFAULTS
dialog_mp.csv
gmi_dialog_mp.csv
generic_mp_sound.csv
gmi_mp_weap_all_allied.csv
gmi_mp_artillery.csv

#VEHICLE
gmi_mp_vehicles_ru.csv
gmi_mp_vehicles_ge.csv
gmi_mp_vehicles_us.csv
gfm_artillery_pak43.csv
dmod_vehicles.csv
```

## Example #2: Adding a jeep

1. Run **dumpbsp**, then open the text for editing.

2. Add this to end of the `Materials lump (0)`:

```
[164] Name: textures/common/clip_nosight_metal, Flags: 0xD040A0 0x28031640
[165] Name: textures/common/clip_metal, Flags: 0xD000A0 0x280306C0
```

Note: you need to change the index in the brackets.

4. Go to the `Entities lump (29)` and add this to the end:

```
{
"classname" "script_vehicle"
"model" "xmodel/mp_v_us_lnd_willysjeep"
"vehicletype" "willyjeep_mp"
"angles" "0 270 0"
"origin" "-136 -1364 -4"
}
{
"targetname" "xmodel/mp_v_us_lnd_willysjeep"
"classname" "script_vehicle_collmap"
"model" "*5"
}
```

Note: you need to change the model index "*5".

5. Add this to the end of the `Models lump (27)`:

```
[5] Mins: (C2960000 C2280000 00000000), Maxs: (42B20000 42200000 42740000)
    TriSoups: off=0 size=0
    ColAABBs: off=0 size=0
    Brushes:  off=3369 size=9
```

Note: you need to change the **Brushes off**.
The size=9 tells us that there are 9 brushes for this model listed at the
offset.

6. Add this to the end of the `Brushes lump (4)`:

```
[3369] Sides: 9, MatID: 165
[3370] Sides: 8, MatID: 164
[3371] Sides: 8, MatID: 164
[3372] Sides: 8, MatID: 164
[3373] Sides: 8, MatID: 164
[3374] Sides: 8, MatID: 165
[3375] Sides: 6, MatID: 165
[3376] Sides: 6, MatID: 164
[3377] Sides: 9, MatID: 165
```

Note: you need to change the index in the brackets and update the Brushes
index at the model list (it should point to the index of the first line).

Update the material ID (MatID) to the material index of clip_nosight_metal
and clip_metal.

These brushes have their corresponding brushsides. To dump all brushsides
run `./getbrushes mp_rhinevalley.txt 17489-17497 28777`
This example dumps brushes from 17489 to 17497 from mp_rhinevalley.txt and
it will start the output entry index from 28777.

7. Add these new entries to the `Brushsides lump (3)`:

These entries were dumped using **getbrushes** in the previous step.

```
[28777] Union: 42A00000, MatID: 165
[28778] Union: 42B20000, MatID: 165
[28779] Union: C2280000, MatID: 165
[28780] Union: 42200000, MatID: 165
[28781] Union: 41880000, MatID: 165
[28782] Union: 41B80000, MatID: 165
[28783] Union: 00009CA5, MatID: 165
[28784] Union: 00009CA4, MatID: 165
[28785] Union: 00009CA6, MatID: 165
[28786] Union: 42300000, MatID: 164
[28787] Union: 429E0000, MatID: 164
[28788] Union: 41E00000, MatID: 164
[28789] Union: 42100000, MatID: 164
[28790] Union: 80000000, MatID: 164
[28791] Union: 41880000, MatID: 164
[28792] Union: 00009CA2, MatID: 164
[28793] Union: 00009CA3, MatID: 164
[28794] Union: 42300000, MatID: 164
[28795] Union: 429E0000, MatID: 164
[28796] Union: C2140000, MatID: 164
[28797] Union: C1E80000, MatID: 164
[28798] Union: 80000000, MatID: 164
[28799] Union: 41880000, MatID: 164
[28800] Union: 00009CA2, MatID: 164
[28801] Union: 00009CA3, MatID: 164
[28802] Union: C25C0000, MatID: 164
[28803] Union: C1A00000, MatID: 164
[28804] Union: 41E00000, MatID: 164
[28805] Union: 42100000, MatID: 164
[28806] Union: 80000000, MatID: 164
[28807] Union: 41880000, MatID: 164
[28808] Union: 00009CA0, MatID: 164
[28809] Union: 00009CA1, MatID: 164
[28810] Union: C25C0000, MatID: 164
[28811] Union: C1A00000, MatID: 164
[28812] Union: C2140000, MatID: 164
[28813] Union: C1E80000, MatID: 164
[28814] Union: 80000000, MatID: 164
[28815] Union: 41880000, MatID: 164
[28816] Union: 00009CA0, MatID: 164
[28817] Union: 00009CA1, MatID: 164
[28818] Union: C2960000, MatID: 165
[28819] Union: C2800000, MatID: 165
[28820] Union: C1F00000, MatID: 165
[28821] Union: 40000000, MatID: 165
[28822] Union: 42380000, MatID: 165
[28823] Union: 42740000, MatID: 165
[28824] Union: 00009C9E, MatID: 165
[28825] Union: 00009C9F, MatID: 165
[28826] Union: C2960000, MatID: 165
[28827] Union: C2800000, MatID: 165
[28828] Union: 40000000, MatID: 165
[28829] Union: 41D80000, MatID: 165
[28830] Union: 42380000, MatID: 165
[28831] Union: 425C0000, MatID: 165
[28832] Union: C2960000, MatID: 164
[28833] Union: 42A00000, MatID: 164
[28834] Union: C2280000, MatID: 164
[28835] Union: 42200000, MatID: 164
[28836] Union: 41880000, MatID: 164
[28837] Union: 42380000, MatID: 164
[28838] Union: 41C80000, MatID: 165
[28839] Union: 42A00000, MatID: 165
[28840] Union: C2280000, MatID: 165
[28841] Union: 42200000, MatID: 165
[28842] Union: 42380000, MatID: 165
[28843] Union: 425C0000, MatID: 165
[28844] Union: 00009C9C, MatID: 165
[28845] Union: 00009C9B, MatID: 165
[28846] Union: 00009C9D, MatID: 165
```

Update the material ID (MatID) to the material index of clip_nosight_metal
and clip_metal.

8. Add the vehicle enable function call to the .gsc file and add the .csv file
to enable vehicle sounds (see previous example).
