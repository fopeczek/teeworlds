/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#pragma once

enum class Class {
    None,
    Hunter, //note:Offensive
    Medic,//note:Support TODO implement:can place in custom places pickupables, 2 options: onk cheap place 1 use pickup, or twok expensive place respawnable (permanent) pickupable
    Scout, //note:Offensive
    Tank, //note:Offensive
    Spider, //note:Offensive/Defensive
    Engineer, //note:Defensive
    Armorer,//note:Support TODO make better name, implement: can (like medic place custom pickupables same 2 options, but for weapons), maybe can create aura of battle or slt
    Necromancer//note:Offensive/Defensive|HARD to implement! TODO can summon offensive zombies and defensive ones, he is weak, after killing anyone he automatically summons free zombie private guard ! HARD TO IMPLEMENT !
};

int GetClassID(Class aClass);
Class GetClass(int ClassID);

static constexpr int MAX_ACTIVE_SPIDER_WEBS=6*5;//shotgun shoots 5 walls every shot
static constexpr int MAX_ACTIVE_ENGINEER_WALLS= 2*2;
static constexpr int MIN_SPIDER_SENSE_DISTANCE= 400;