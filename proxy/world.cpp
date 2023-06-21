//
//  world.cpp
//  GoGABOT
//
//  Created by Carmudi on 21/08/21.
//  Copyright © 2021 GoGABOT. All rights reserved.
//

#include <iostream>
#include <sstream>
#include "world.h"
#include "gt.hpp"

long world::posPtr = 0;
vector<world::ItemDefinitionStruct> world::itemDefs;
int16_t* world::foreground;
int16_t* world::background;

world::ItemDefinitionStruct world::GetItemDef(int itemID)
{
    if (itemID < 0 || itemID > this->itemDefs.size()) return this->itemDefs[0];
    ItemDefinitionStruct def = this->itemDefs[itemID];
    if (def.id != itemID)
    {
        // For some reason, something is off.
        for (auto& element : this->itemDefs) {
            if (element.id == itemID) return element;
        }
    }
    return def;
}

bool world::requiresTileExtra(int id)
{
    ItemDefinitionStruct def = this->GetItemDef(id);
    //    PRINTD("posPtr -> %ld, id -> %d, name -> %s, action_id -> %d\n", this->posPtr, def.id, def.itemName.c_str(), def.actionType);
    return
        def.actionType == 2 || // Door
        def.actionType == 3 || // Lock
        def.actionType == 10 || // Sign
        def.actionType == 13 || // Main Door
        def.actionType == 19 || // Seed
        def.actionType == 26 || // Portal
        def.actionType == 33 || // Mailbox
        def.actionType == 34 || // Bulletin Board
        def.actionType == 36 || // Dice Block
        def.actionType == 36 || // Roshambo Block
        def.actionType == 38 || // Chemical Source
        def.actionType == 40 || // Achievement Block
        def.actionType == 43 || // Sungate
        def.actionType == 46 ||
        def.actionType == 47 ||
        def.actionType == 49 ||
        def.actionType == 50 ||
        def.actionType == 51 || // Bunny Egg
        def.actionType == 52 ||
        def.actionType == 53 || // Game Generator
        def.actionType == 54 || // Xenonite
        def.actionType == 55 || // Phone Booth
        def.actionType == 56 || // Crystal
        def.id == 2246 || // Crystal
        def.actionType == 57 || // Crime In Progress
        def.actionType == 59 || // Spotlight
        def.actionType == 61 ||
        def.actionType == 62 ||
        def.actionType == 63 || // Fish Wall Port
        def.id == 3760 || // Data Bedrock
        def.actionType == 65 || // Solar Collector
        def.actionType == 66 || // Forge
        def.actionType == 67 || // Giving Tree
        def.actionType == 73 || // Sewing Machine
        def.actionType == 74 ||
        def.actionType == 75 || // Lobster Trap
        def.actionType == 76 || // Painting Easel
        def.actionType == 78 || // Pet Trainer (WHY?!)
        def.actionType == 80 || // Lock-Bot (Why?!)
        def.actionType == 81 ||
        def.actionType == 83 || // Display Shelf
        def.actionType == 84 ||
        def.actionType == 85 || // Challenge Timer
        def.actionType == 86 || // Challenge Start/End Flags
        def.actionType == 87 || // Fish Wall Mount
        def.actionType == 88 || // Portrait
        def.actionType == 89 ||
        def.actionType == 91 || // Fossil Prep Station
        def.actionType == 93 || // Howler
        def.actionType == 96 || // Chemsynth Tank
        def.actionType == 97 || // Storage Box Xtreme / Untrade-a-box
        def.actionType == 98 || // Home oven
        def.actionType == 99 || // Audio Gear / Audio Rack
        def.actionType == 100 || // Geiger Charger
        def.actionType == 101 || // The Adventure Begins
        def.actionType == 102 || // Tomb Robber
        def.actionType == 109 || // Training Port
        def.actionType == 111 || // Magplant
        def.actionType == 113 || // CyBot
        def.actionType == 115 || // Lucky Token
        def.actionType == 116 || // GrowScan 9000 ???
        def.actionType == 126 || // Storm Cloud
        def.actionType == 127 || // Cracked Stone Slab
        def.actionType == 130 ||
        (def.id % 2 == 0 && def.id >= 5818 && def.id <= 5932) ||
        // ...
        false;
}
void world::tileSerialize(uint8_t* extended, int location) {
    this->foreground[location] = *(int16_t*)(extended + this->posPtr);
    this->posPtr += 2;
    this->background[location] = *(int16_t*)(extended + this->posPtr);
    ItemDefinitionStruct def = GetItemDef(this->foreground[location]);
    this->posPtr += 2;
    short extraData = *(short*)(extended + this->posPtr);
    this->posPtr += 4;
    if (extraData > 0) {
        this->posPtr += 2;
    }

    if (this->requiresTileExtra(this->foreground[location]) && this->foreground[location] != 0) {
        this->tileExtraSerialize(extended, location);

    }

  
}

bool world::isSpecialTile(int id) {
    ItemDefinitionStruct def = this->GetItemDef(id);

    return
        def.actionType == 26 ||
        def.actionType == 27 ||
        def.actionType == 101;

}

void world::tileExtraSerialize(uint8_t* extended, int location) {
    short extraData = (short)extended[this->posPtr];
    short len = 0;

    switch (extraData)
    {
    case 0:
    {

    }
    break;
    case 1:
    {
        // this is door
        len = (int16_t)extended[this->posPtr + 1];
        this->posPtr += len + 4;
    }
    break;
    case 2:
    {
        // this is sign
        len = (int16_t)extended[this->posPtr + 1];
        this->posPtr += len + 7;
    }
    break;
    case 3:
    {
        // wl shit, owner userID etc...
        short adminCount = (short)extended[this->posPtr + 6];
        this->posPtr += ((adminCount * 4) + 18);
    }
    break;
    case 4:
    {
        // trees n timing shit, skipping...
        this->posPtr += 6;
    }
    break;
    case 0x8:
    {
        // Dice Block
        this->posPtr += 2;
    }
    break;
    case 0x9:
    {
        // provider
        this->posPtr += 5;
    }
    break;
    case 0xa:
    {
        // Achievement Block
        this->posPtr += 6;
    }
    break;
    case 0xb:
    {
        // hmon. fuaurk off ->_>
        len = (int16_t)extended[this->posPtr + 5];
        this->posPtr += len + 7;
    }
    break;
    case 0xe:
    {
        // mannequins
        len = (int16_t)extended[this->posPtr + 1];
        this->posPtr += len + 26;
    }
    break;
    case 0x0f:
    {
        // bunny egg
        this->posPtr += 5;
    }
    break;
    case 0x11:
    {
        // Game Generator
        this->posPtr += 1;
    }
    break;
    case 0x12:
    {
        // Xenonite
        this->posPtr += 6;
    }
    break;
    case 0x13:
    {
        // Phone Booth
        this->posPtr += 19;
    }
    break;
    case 0x14:
    {
        // crystal
        len = (int16_t)extended[this->posPtr + 1];
        this->posPtr += len + 3;
    }
    break;
    case 0x15:
    {
        // Crime in progress
        len = (short)extended[this->posPtr + 1];
        this->posPtr += len + 8;
    }
    break;
    case 0x16:
    {
        // Spotlight
        this->posPtr += 1;
    }
    break;
    case 0x17:
    {
        // display blocks
        this->posPtr += 5;
    }
    break;
    case 0x18:
    {
        // vends...
        this->posPtr += 9;
    }
    break;
    case 0x19:
    {
        // Fish Tank Port
        len = (int16_t)extended[this->posPtr + 2];
        this->posPtr += ((len * 4) + 6);
    }
    break;
    case 0x1A:
    {
        // skiping sollar collector
        this->posPtr += 6;
    }
    break;
    case 0x1B:
    {
        // Forge
        this->posPtr += 5;
    }
    break;
    case 0x1C: // deco
    {
        this->posPtr += 7;
    }
    break;
    case 0x20: // sewing machine
    {
        this->posPtr += 5;
    }
    break;
    case 0x21:
    {
        if (this->foreground[location] == 3394)
        {
            // Country flag
            len = (int16_t)extended[this->posPtr + 1];
            this->posPtr += len + 4;
        }
        else {
            this->posPtr += 1;
        }
    }
    break;
    case 0x22:
    {
        // Lobster Trap
        this->posPtr += 1;
    }
    break;
    case 0x23:
    {
        // Painting Easel
        len = (int16_t)extended[this->posPtr + 5];
        this->posPtr += len + 7;
    }
    break;
    case 0x25:
    {
        // Pet Trainer
        len = (int16_t)extended[this->posPtr + 1];
        this->posPtr += len + 35;
    }
    break;
    case 0x27:
    {
        // lock-bot
        this->posPtr += 5;
    }
    break;
    case 0x28:
    {
        // bg weather
        this->posPtr += 5;
    }
    break;
    case 0x2a:
    {
        // data bedrock (used for npc data storing, not properly handled tho, todo)
        this->posPtr += 4;
    }
    break;
    case 0x2b:
    {
        // Display self
        this->posPtr += 17;
    }
    break;
    case 0x2c:
    {
        // VIP Door
        len = (int16_t)extended[this->posPtr + 6];
        this->posPtr += ((len * 4) + 10);

    }
    break;
    case 0x2d:
    {
        // Challenge Timer
        this->posPtr += 1;

    }
    break;
    case 0x2f:
    {
        // Fish Wall Mount
        len = (int16_t)extended[this->posPtr + 1];
        this->posPtr += len + 8;
    }
    break;
    case 0x30:
    {
        // Portrain
        len = (int16_t)extended[this->posPtr + 1];
        this->posPtr += len + 29;
    }
    break;
    case 0x31:
    {
        // stuff weather
        this->posPtr += 10;
    }
    break;
    case 0x32:
    {
        // activity indicator in there, keep skipping as usual...
        this->posPtr += 5;
    }
    break;
    case 0x34:
    {
        // Howler => do not serialize or increase bytes read?
        this->posPtr += 1;
    }
    break;
    case 0x35:
    {
        // Chemsynth Tank
        this->posPtr += 9;
    }
    break;
    case 0x36:
    {
        // storage box xtreme
        len = (int16_t)extended[this->posPtr + 1];
        this->posPtr += len + 3;
    }
    break;
    case 0x37:
    {
        // Home oven shit and timer
        len = (short)extended[this->posPtr + 5];
        this->posPtr += ((len * 4) + 21);
    }
    break;
    case 0x38:
    {
        // Lucky token, Audio Gear, Audio Rack, and other
        len = (int16_t)extended[this->posPtr + 1];
        this->posPtr += len + 7;
    }
    break;
    case 0x39:
    {
        // geiger charger
        this->posPtr += 5;
    }
    break;
    case 0x3a:
    {
        // adventure begins
        this->posPtr += 1;
    }
    break;
    case 0x3b:
    {
        // tomb robber
        this->posPtr += 1;
    }
    break;
    case 0x3d:
    {
        // Training Port
        // TODO: - I'm not sure about it this will be recalculate in future
        this->posPtr += 36;
    }
    break;
    case 0x3e:
    {
        // MAGPLANT 5000
        this->posPtr += 15;
    }
    break;
    case 0x3f:
    {
        // cybots
        len = (int)extended[this->posPtr + 1];
        this->posPtr += ((len * 15) + 13);
    }
    break;
    case 0x41:
    {
        // guild item
        this->posPtr += 18;
    }
    break;
    case 0x42:
    {
        // growscan 9000
        this->posPtr += 2;
    }
    break;
    case 0x48:
    {
        // Storm Cloud
        this->posPtr += 13;
    }
    break;
    case 0x49:
    {
        // Cracked Stone Slab
        this->posPtr += 5;
    }
    break;
    case 0x4a:
    {
        // safe vault
        this->posPtr += 1;
    }
    break;
    case 0xF3:
    {
        len = (short)extended[this->posPtr + 4];
        this->posPtr += ((len * 4) + 16);
    }
    default:
        len = 0;
        break; // unknown tile visual type...
    }
}
