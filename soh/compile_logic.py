#!/usr/bin/env python

from sys import argv
import os

if len(argv) < 3:
    print("compile_logic.py type input-directory output-directory")
    exit()

class Cpp:
    RR_PRELUDE = """
#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"
#include "soh/Enhancements/randomizer/dungeon.h"

using namespace Rando;
extern "C" SaveContext gSaveContext;

void RegionTable_Init_Generated() {
    // clang-format off
 """

    RR_EPILOGUE = """
    // clang-format on
}"""

    def generate_rr(self, dirname, RRs):
        checks = {} # relying on ordered inserts, which set lacks
        result = []
        output = result.append
        output(self.RR_PRELUDE)
        for rr in RRs:
            if isinstance(rr, str):
                output(rr)
                output("\n")
                continue
            output(f"areaTable[{rr.name}] = Region(\"{rr.ui_name}\", {rr.scene}, {'true' if rr.timepass else 'false'}, {{{','.join(rr.areas)}}}, {{")
            if rr.events:
                output("\n")
                for name, code in rr.events:
                    output(f"\tEventAccess(&logic->{name}, []{{return {self.compile_rr(rr, code)};}}),\n")
            output("}, {")

            if rr.checks:
                output("\n")
                for name, code in rr.checks:
                    checks[name] = None
                    output(f"\tLOCATION({name}, {self.compile_rr(rr, code)}),\n")
            output("}, {")

            if rr.exits:
                output("\n")
                for name, code in rr.exits:
                    deprioritize = isinstance(code, list) and isinstance(code[0], str) and code[0] == "@deprioritize"
                    if deprioritize:
                        code = code[1]
                    output(f"\tEntrance({name}, []{{return {self.compile_rr(rr, code)};}}")
                    if deprioritize:
                        output(", false")
                    output("),\n")
            output("});\n")

        output(self.RR_EPILOGUE)
        with open(dirname + "/regions.cpp", "w", encoding="ascii") as f:
            for s in result:
                f.write(s)
        with open(dirname + "/regions.h", "w", encoding="ascii") as f:
            f.write("#pragma once\n")
            f.write("typedef enum {\n")
            f.write(f"    RR_NONE,\n")
            for rr in RRs:
                f.write(f"    {rr.name},\n")
            f.write(f"    RR_MAX,\n")
            f.write("} RandomizerRegion;\n")
        with open(dirname + "/checks.h", "w", encoding="ascii") as f:
            f.write("#pragma once\n")
            f.write("typedef enum {\n")
            f.write(f"    RC_UNKNOWN_CHECK,\n")
            for check in checks.keys():
                f.write(f"    {check},\n")
            f.write(f"    RC_MAX,\n")
            f.write("} RandomizerCheck;\n")

    def compile_rr(self, rr, ast):
        result = []
        output = result.append
        if ast:
            if isinstance(ast, str):
                f = ast
                ast = [f]
            else:
                f = ast[0]
                assert isinstance(f, str)
            if f in LOGIC:
                output(f"logic->{f}")
            elif f in logicFUNC:
                output(f"logic->{f}(")
                output(", ".join(self.compile_rr(rr, node) for node in ast[1:]))
                output(")")
            elif f in ctxFUNC:
                output(f"ctx->{f}(")
                output(", ".join(self.compile_rr(rr, node) for node in ast[1:]))
                output(")")
            elif f in FUNC:
                output(f"{f}(")
                output(", ".join(self.compile_rr(rr, node) for node in ast[1:]))
                output(")")
            elif f in binOP:
                if f == "==" and isinstance(ast[1], str) and ast[1].startswith("RSK_"):
                    output(f"{self.compile_rr(rr, ast[1])}.Is({self.compile_rr(rr, ast[2])})")
                elif f == "!=" and isinstance(ast[1], str) and ast[1].startswith("RSK_"):
                    output(f"{self.compile_rr(rr, ast[1])}.IsNot({self.compile_rr(rr, ast[2])})")
                else:
                    output("(")
                    output(binOP[f].join(self.compile_rr(rr, node) for node in ast[1:]))
                    output(")")
            elif f == "not":
                output("!")
                output(self.compile_rr(rr, ast[1]))
            elif f == "if":
                output("(")
                output(self.compile_rr(rr, ast[1]))
                output(" ? ")
                output(self.compile_rr(rr, ast[2]))
                output(" : ")
                output(self.compile_rr(rr, ast[3]))
                output(")")
            elif f.startswith("RSK_"):
                output(f"ctx->GetOption({f})")
                if f in ("RSK_BIG_POE_COUNT", "RSK_TRIFORCE_HUNT_PIECES_REQUIRED"):
                    output(".Get()")
            elif f.startswith("RT_"):
                output(f"(bool)ctx->GetTrickOption({f})")
            elif f == "IsDungeonVanilla":
                output(f"ctx->GetDungeon({self.compile_rr(rr, ast[1])})->IsVanilla()")
            elif f == "IsDungeonMQ":
                output(f"ctx->GetDungeon({self.compile_rr(rr, ast[1])})->IsMQ()")
            elif f == "IsTrialSkipped":
                output(f"ctx->GetTrial({self.compile_rr(rr, ast[1])})->IsSkipped()")
            elif f == "TriforcePiecesCollected":
                output("gSaveContext.ship.quest.data.randomizer.triforcePiecesCollected")
            elif f.startswith("HasProjectileAge"):
                output(f)
            elif f in RANDO:
                output(f"Rando::{f}")
            elif f == "Here":
                output(f"Here({rr.name}, []{{return {self.compile_rr(rr, ast[1])};}})")
            elif f in ("MQSpiritSharedStatueRoom", "MQSpiritSharedBrokenWallRoom"):
                output(f"{f}({self.compile_rr(rr, ast[1])}, []{{return {self.compile_rr(rr, ast[2])};}})")
            elif f != "//":
                if len(ast) != 1:
                    print("expected atom, got tree", ast)
                    assert false
                elif f.isupper() or f.isdigit() or f in ("true", "false"):
                    output(f)
                else:
                    print("invalid atom", f)
                    assert false
        return "".join(result)

class RR:
    __slots__ = "name", "scene", "timepass", "areas", "ui_name", "events", "checks", "exits"
    def __init__(self, name, scene, timepass, *areas):
        self.name = name
        self.scene = scene
        self.timepass = timepass == "true"
        self.areas = areas
        self.ui_name = ""
        self.events = []
        self.checks = []
        self.exits = []

def parse(code):
    if code[0] == "(" and code[-1] == ")":
        code = code[1:-1]
    stack = []
    ast = []
    balance = 0
    lastidx = 0
    for idx, ch in enumerate(code):
        if ch.isspace():
            if idx > lastidx:
                ast.append(code[lastidx:idx])
            lastidx = idx+1
        elif ch == "(":
            stack.append(ast)
            ast = []
            lastidx = idx+1
        elif ch == ")":
            if idx > lastidx:
                ast.append(code[lastidx:idx])
            subast = ast
            ast = stack.pop()
            ast.append(subast)
            lastidx = idx+1
    if len(code) > lastidx:
        ast.append(code[lastidx:])
    return ast

binOP = { "==": " == ", "and": " && ", "or": " || ", ">=": " >= ", "!=": " != ", ">": " > ", "<": " < ", "add": " + " }

LOGIC = {
    "IsChild",
    "IsAdult",
    "AtDay",
    "AtNight",
    "BigPoes",
}

RANDO = {
    "DEKU_TREE",
    "DODONGOS_CAVERN",
    "JABU_JABUS_BELLY",
    "FOREST_TEMPLE",
    "FIRE_TEMPLE",
    "WATER_TEMPLE",
    "SPIRIT_TEMPLE",
    "SHADOW_TEMPLE",
    "BOTTOM_OF_THE_WELL",
    "ICE_CAVERN",
    "GERUDO_TRAINING_GROUND",
    "GANONS_CASTLE",
}

FUNC = {
    "CanPlantBean",
    "BothAges",
    "ChildCanAccess",
    "AdultCanAccess",
    "HasAccessTo",
    "CanBuyAnother",
}

logicFUNC = {
    "BlueFire",
    "CanBreakMudWalls",
    "HasItem",
    "HasBossSoul",
    "HasFireSource",
    "HasFireSourceWithTorch",
    "CanUse",
    "CanPassEnemy",
    "CanAvoidEnemy",
    "CanKillEnemy",
    "CanGetEnemyDrop",
    "CanGetDekuBabaSticks",
    "CanGetDekuBabaNuts",
    "CanBreakPots",
    "CanBorrowMasks",
    "CanShield",
    "CanStandingShield",
    "CanReflectNuts",
    "CanStunDeku",
    "CanSpawnSoilSkull",
    "CanGetNightTimeGS",
    "CanOpenUnderwaterChest",
    "CanOpenOverworldDoor",
    "CanHitSwitch",
    "CanHitEyeTargets",
    "CanDetonateUprightBombFlower",
    "CanDetonateBombFlowers",
    "CanUseProjectile",
    "CanBreakLowerBeehives",
    "CanBreakUpperBeehives",
    "CallGossipFairy",
    "CallGossipFairyExceptSuns",
    "HasExplosives",
    "HasProjectile",
    "CanCutShrubs",
    "CanBreakCrates",
    "CanBreakSmallCrates",
    "StoneCount",
    "CanBuildRainbowBridge",
    "TradeQuestStep",
    "GetGSCount",
    "BlastOrSmash",
    "CanOpenBombGrotto",
    "CanOpenStormsGrotto",
    "HookshotOrBoomerang",
    "TakeDamage",
    "CanAttack",
    "CanDamage",
    "CanUseSword",
    "CanJumpslash",
    "CanJumpslashExceptHammer",
    "HasBottle",
    "ScarecrowsSong",
    "SmallKeys",
    "IsFireLoopLocked",
    "OcarinaButtons",
    "Hearts",
    "EffectiveHealth",
    "FireTimer",
    "WaterTimer",
    "MQWaterLevel",
    "CanTriggerLACS",
    "BombchusEnabled",
}

ctxFUNC = {
    "GetTrickOption",
    "GetOption",
}

def main():
    RRs = []
    files = []
    for root, dirs, names in os.walk(argv[2]):
        for name in names:
            files.append(os.path.join(root, name))
    files.sort(key=lambda f: ((2 if "dungeons" in f else 1 if "overworld" in f else 0), f))
    for file in files:
        active_rr = None
        buf = ""
        pcount = 0
        for line in open(file, "r", encoding="ascii"):
            line = line.strip()
            if line.startswith("def "):
                if pcount != 0:
                    print("error parsing", line)
                    assert false
                defline = line.split()
                active_rr = RR(*defline[1:])
                RRs.append(active_rr)
                buf = ""
                pcount = 0
                continue
            if active_rr and not active_rr.ui_name:
                active_rr.ui_name = line
                continue
            if not active_rr or line.startswith("//"):
                continue
            pcount += line.count('(') - line.count(')')
            buf += " "
            buf += line
            if pcount != 0:
                continue
            if not buf or buf.isspace():
                continue
            try:
                thing, code = buf.split(None, 1)
            except:
                print("failed to parse line", repr(buf))
                assert false
                continue
            if thing.startswith("RR_"):
                active_rr.exits.append((thing, parse(code)))
            elif thing.startswith("RC_"):
                active_rr.checks.append((thing, parse(code)))
            else:
                LOGIC.add(thing)
                active_rr.events.append((thing, parse(code)))
            buf = ""

    generator = { "cpp": Cpp }[argv[1]]()
    generator.generate_rr(argv[3], RRs)

if __name__ == "__main__":
    main()
