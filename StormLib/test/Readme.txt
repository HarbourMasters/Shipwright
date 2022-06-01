Testing MPQ set for StormLib test suite
=======================================

The path to the directory could be anywhere. Path to this directory
is set on a command line. If there is no command line argument passed,
one of the hard-coded paths is used.

The testing directory must contain the following files:

Path                                               File Size
-------------------------------------------------  -----------
patches\wow-update-13164.MPQ                       480 028 668  // Original patch files from WoW
patches\wow-update-13205.MPQ                             8 814 
patches\wow-update-13287.MPQ                       333 363 917 
patches\wow-update-13329.MPQ                       159 285 434 
patches\wow-update-13596.MPQ                       148 022 182 
patches\wow-update-13623.MPQ                            11 561 
patches\wow-update-base-13914.MPQ                  293 580 556 
patches\wow-update-base-14007.MPQ                      262 906 
patches\wow-update-base-14333.MPQ                  368 972 365 
patches\wow-update-base-14480.MPQ                       75 897 
patches\wow-update-base-14545.MPQ                    1 224 415 
patches\wow-update-base-14946.MPQ                  749 246 163 
patches\wow-update-base-15005.MPQ                    7 121 223 
patches\wow-update-base-15050.MPQ                        4 497 
patches\wow-update-enGB-16016.MPQ                   17 117 318 
patches\wow-update-enGB-16048.MPQ                       78 180 
patches\wow-update-enGB-16057.MPQ                        4 072 
patches\wow-update-enGB-16309.MPQ                   28 187 877 
patches\wow-update-enGB-16357.MPQ                        3 621 
patches\wow-update-enGB-16516.MPQ                   42 934 402 
patches\wow-update-enGB-16650.MPQ                      426 683 
patches\wow-update-enGB-16844.MPQ                   18 366 460 
patches\wow-update-enGB-16965.MPQ                      305 202 
patches\wow-update-oldworld-13154.MPQ               54 068 663 
patches\wow-update-oldworld-13286.MPQ                2 328 773 

AddFile.bin                                                  0  // Just an empty file   
AddFile.exe                                            432 640  // An arbitraty binary file
AddFile.wav                                            311 340  // An arbitrary WAVE file
ListFile_Blizzard.txt                                3 904 784  // Listfile for Blizzard games (pre-WoW)
ListFile_WarOfTheImmortals.txt                       3 904 858  // Listfile for War of the Immortals game
MPQ_1997_v1_Diablo1_DIABDAT.MPQ                    517 501 282  // Originally DIABDAT.MPQ from Diablo I
MPQ_1997_v1_Diablo1_DIABDAT.sha                            206
MPQ_1997_v1_Diablo1_single_0.sha                           201  // Single player savegame from Diablo I
MPQ_1997_v1_Diablo1_single_0.sv                         98 980
MPQ_1999_v1_WeakSignature.exe                        1 031 826  // War2Patch_202.exe from Warcraft II Bnet Edition
MPQ_1999_v1_WeakSignature.sha                              260
MPQ_2002_v1_BlockTableCut.MPQ                       27 498 436  // Cut file War3Patch.mpq from Warcraft III
MPQ_2002_v1_BlockTableCut.sha                              250
MPQ_2002_v1_ProtectedMap_HashTable_FakeValid.sha         1 067  
MPQ_2002_v1_ProtectedMap_HashTable_FakeValid.w3x     1 089 638  // Protected Warcraft III map
MPQ_2002_v1_ProtectedMap_InvalidMpqFormat.sha              507
MPQ_2002_v1_ProtectedMap_InvalidMpqFormat.w3x        2 429 195  // Protected Warcraft III map
MPQ_2002_v1_ProtectedMap_InvalidUserData.sha               293
MPQ_2002_v1_ProtectedMap_InvalidUserData.w3x           596 486  // Protected Warcraft III map
MPQ_2002_v1_StrongSignature.sha                            250
MPQ_2002_v1_StrongSignature.w3m                        306 818  // (10)DustwallowKeys.w3m from Warcraft III
MPQ_2009_v2_WoW_patch.MPQ.part                      31 396 380  // patch.MPQ.part from trial WoW build 10958
MPQ_2009_v2_WoW_patch.MPQ.sha                              226
MPQ_2010_v2_HashTableCompressed.MPQ.part            14 546 972  // interface.MPQ.part from WoW build 10958
MPQ_2010_v2_HashTableCompressed.MPQ.sha                    277
MPQ_2010_v2_HasUserData.s2ma                         1 972 177  // (4) - AI - Kulas Ravine (1x).s2ma from Starcraft II Beta
MPQ_2010_v2_HasUserData.sha                                261
MPQ_2010_v3_expansion-locale-frFR.MPQ                2 980 489  // expansion-locale-frFR.MPQ from WoW 12911
MPQ_2010_v3_expansion-locale-frFR.sha                      274
MPQ_2011_v2_EncryptedMpq.MPQE                       41 631 764  // Installer UI 2 esES.MPQE from Starcraft II Installer (build 15404)
MPQ_2011_v2_EncryptedMpq.sha                               255
MPQ_2011_v4_InvalidHetEntryCount.MPQ                   206 164  // An invalid MPQ created by a previous Stormlib version
MPQ_2011_v4_InvalidHetEntryCount.sha                       312
MPQ_2012_v2_EmptyMpq.MPQ                                    44  // Empty MPQ (Cache from WoW 11723)
MPQ_2012_v2_EmptyMpq.sha                                   212
MPQ_2012_v4_OldWorld.MPQ                         1 854 547 587  // OldWorld.MPQ from WoW 12911
MPQ_2012_v4_OldWorld.sha                                   270
MPQ_2013_v4_EmptyMpq.MPQ                                   208  // Empty MPQ created by StormLib
MPQ_2013_v4_EmptyMpq.sha                                   183
MPQ_2013_v4_expansion1.MPQ                       1 426 926 736  // expansion1.MPQ from WoW build 17538
MPQ_2013_v4_expansion1.sha                                 221
MPQ_2013_v4_locale-enGB.MPQ                        446 491 866  // locale-enGB.MPQ from WoW build 16965
MPQ_2013_v4_locale-enGB.sha                                273
MPQ_2013_v4_patch-base-16357.MPQ                           249  // patch-base-16357.MPQ from WoW build 17538
MPQ_2013_v4_patch-base-16357.sha                           229
MPQ_2013_v4_SC2_EmptyMap.SC2Map                         25 058  // Custom Starcraft II Map
MPQ_2013_v4_SC2_EmptyMap.sha                               243
MPQ_2013_v4_world.MPQ                            2 617 175 835  // world.MPQ from WoW build 12911
MPQ_2013_v4_world.sha                                      252
MPx_2013_v1_LongwuOnline.mpk                         2 146 549  // Scp.mpk from Longwu Online
MPx_2013_v1_LongwuOnline.sha                               277
MPx_2013_v1_WarOfTheImmortals.sha                          248
MPx_2013_v1_WarOfTheImmortals.sqp                   56 775 675  // Modified Other.sqp from War of the Immortals
