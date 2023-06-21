#include "server.h"
#include <iostream>
#include "events.h"
#include "gt.hpp"
#include "proton/hash.hpp"
#include "proton/rtparam.hpp"
#include "utils.h"
#include <chrono>
#include "items_dat_decode.h"
#include "print.h"

using namespace std;
int getState() {
    int val = 0;
    val |= gt::noclip ? 1 : 2;
    return val;
}

int getState2() {
    int val = 0;
    val |= gt::noclip2 ? 1 : 2;
    val |= gt::doublejump ? 1 : 1;
    return val;
}

struct PlayerMovings {
    int packetType;
    int netID;
    float x;
    float y;
    int characterState;
    int plantingTree;
    float XSpeed;
    float YSpeed;
    int punchX;
    int punchY;
    int SecondaryNetID;
}; 
bool collecting = true;

BYTE* packPlayerMovings(PlayerMovings* dataStruct) {
    BYTE* data = new BYTE[56];
    for (int i = 0; i < 56; i++) {
        data[i] = 0;
    }
    memcpy(data, &dataStruct->packetType, 4);
    memcpy(data + 4, &dataStruct->netID, 4);
    memcpy(data + 8, &dataStruct->SecondaryNetID, 4);
    memcpy(data + 12, &dataStruct->characterState, 4);
    memcpy(data + 20, &dataStruct->plantingTree, 4);
    memcpy(data + 24, &dataStruct->x, 4);
    memcpy(data + 28, &dataStruct->y, 4);
    memcpy(data + 32, &dataStruct->XSpeed, 4);
    memcpy(data + 36, &dataStruct->YSpeed, 4);
    memcpy(data + 44, &dataStruct->punchX, 4);
    memcpy(data + 48, &dataStruct->punchY, 4);
    return data;
}

PlayerMovings* unpackRaw(BYTE* data) {
    PlayerMovings* p = new PlayerMovings;
    memcpy(&p->packetType, data, 4);
    memcpy(&p->netID, data + 4, 4);
    memcpy(&p->characterState, data + 12, 4);
    memcpy(&p->plantingTree, data + 20, 4);
    memcpy(&p->x, data + 24, 4);
    memcpy(&p->y, data + 28, 4);
    memcpy(&p->XSpeed, data + 32, 4);
    memcpy(&p->YSpeed, data + 36, 4);
    memcpy(&p->punchX, data + 44, 4);
    memcpy(&p->punchY, data + 48, 4);
    return p;
}
void server::NoclipState(int netid) {
    PlayerMovings data;
    data.packetType = 0x14;
    data.characterState = 0; // animation
    data.x = 1000;
    data.y = 100;
    data.punchX = 500;
    data.punchY = 0;
    data.XSpeed = 2000;
    data.YSpeed = 500;
    data.netID = netid;
    data.plantingTree = getState();
    BYTE* raw = packPlayerMovings(&data);
    int var = 0x808000;
    float water = 125.0f;
    memcpy(raw + 1, &var, 3);
    memcpy(raw + 16, &water, 4);
    g_server->SendPacketRaw(true, 4, raw, 56, 0, ENET_PACKET_FLAG_RELIABLE);
}


void server::NoclipState2(int netid) {
    PlayerMovings data;
    data.packetType = 0x14;
    data.characterState = 0; // animation
    data.x = 1000;
    data.y = 100;
    data.punchX = 500;
    data.punchY = 0;
    data.XSpeed = 2000;
    data.YSpeed = 500;
    data.netID = netid;
    data.plantingTree = getState2();
    BYTE* raw = packPlayerMovings(&data);
    int var = 0x808000;
    float water = 125.0f;
    memcpy(raw + 1, &var, 3);
    memcpy(raw + 16, &water, 4);
    g_server->SendPacketRaw(true, 4, raw, 56, 0, ENET_PACKET_FLAG_RELIABLE);
}

void server::sendState(int netid2) {
    int32_t netid = netid2;
    int type, charstate, plantingtree, punchx, punchy;
    float x, y, xspeed, yspeed;
    type = PACKET_SET_CHARACTER_STATE;
    charstate = 0;
    int state = 0;
    state |= gt::ghost << 0; //ghost
    state |= doublejump << 1; //double jump
    state |= false << 1; //spirit form
    state |= false << 1; //hoveration
    state |= false << 1; //aurora
    state |= false << 2; //one ringe
    state |= false << 4; //mark of growganoth
    state |= false << 7; //halo
    state |= false << 13; //duct tape
    state |= false << 15; //lucky
    state |= false << 19; //geiger effect
    state |= false << 20; //spotlight
    state |= ssup << 24; //super suporter
    plantingtree = state;
    punchx = 0;
    punchy = 0;
    x = 1000.0f;
    y = 400.0f;
    xspeed = speed_x;

    yspeed = speed_y; //gravity

    BYTE* data = new BYTE[56];
    memset(data, 0, 56);
    memcpy(data + 0, &type, 4);
    memcpy(data + 4, &netid, 4);
    memcpy(data + 12, &charstate, 4);
    memcpy(data + 20, &plantingtree, 4);
    memcpy(data + 24, &x, 4);
    memcpy(data + 28, &y, 4);
    memcpy(data + 32, &xspeed, 4);
    memcpy(data + 36, &yspeed, 4);
    memcpy(data + 44, &punchx, 4);
    memcpy(data + 48, &punchy, 4);

    
    memcpy(data + 1, &punch_effect, 3);
    uint8_t build_range = (false ? -1 : 128);
    uint8_t punch_range = 128;
    memcpy(data + 2, &build_range, 1);
    memcpy(data + 3, &punch_range, 1);
    float waterspeed = 200.0f;
    memcpy(data + 16, &waterspeed, 4);
    g_server->SendPacketRaw(true, 4, data, 56, 0, ENET_PACKET_FLAG_RELIABLE);
}
void server::speedy(int netid, int XSpeed) {
    PlayerMovings data;
    data.packetType = 0x14;
    data.characterState = 0; // animation
    data.x = 1000;
    data.y = 100;
    data.punchX = 500;
    data.punchY = 0;
    data.XSpeed = 300;
    data.YSpeed = XSpeed;
    data.netID = netid;
    data.plantingTree = getState();
    BYTE* raw = packPlayerMovings(&data);
    int var = 0x808000;
    float water = 125.0f;
    memcpy(raw + 1, &var, 3);
    memcpy(raw + 16, &water, 4);
    g_server->SendPacketRaw(true, 4, raw, 56, 0, ENET_PACKET_FLAG_RELIABLE);
}


void send_state(int netid)
{
    
    int type, charstate, plantingtree, punchx, punchy;
    float x, y, xspeed, yspeed;
    type = PACKET_SET_CHARACTER_STATE;
    charstate = 0;
    int state = 0;
    state |= gt::ghost << 0; //ghost
    state |= true << 1; //double jump
    state |= false << 1; //spirit form
    state |= false << 1; //hoveration
    state |= false << 1; //aurora
    state |= false << 2; //one ring
    state |= false << 4; //mark of growganoth
    state |= false << 7; //halo
    state |= false << 13; //duct tape
    state |= false << 15; //lucky
    state |= false << 19; //geiger effect
    state |= false << 20; //spotlight
    state |= false << 24; //super suporter
    plantingtree = state;
    punchx = 0;
    punchy = 0;
    x = 1000.0f;
    y = 400.0f;
    xspeed = 250.0f;

    yspeed = 1000.0f; //gravity

    BYTE* data = new BYTE[56];
    memset(data, 0, 56);
    memcpy(data + 0, &type, 4);
    memcpy(data + 4, &netid, 4);
    memcpy(data + 12, &charstate, 4);
    memcpy(data + 20, &plantingtree, 4);
    memcpy(data + 24, &x, 4);
    memcpy(data + 28, &y, 4);
    memcpy(data + 32, &xspeed, 4);
    memcpy(data + 36, &yspeed, 4);
    memcpy(data + 44, &punchx, 4);
    memcpy(data + 48, &punchy, 4);

    memcpy(data + 1, &punch_effect, 3);
    uint8_t build_range = (pathfinding ? -1 : 128);
    uint8_t punch_range = 128;
    memcpy(data + 2, &build_range, 1);
    memcpy(data + 3, &punch_range, 1);
    float waterspeed = 200.0f;
    memcpy(data + 16, &waterspeed, 4);
    g_server->SendPacketRaw(true, 4, data, 56, 0, ENET_PACKET_FLAG_RELIABLE);
    delete[] data;
}
void server::stateReset(int netid) {
    PlayerMovings data;
    data.packetType = 0x14;
    data.characterState = 0; // animation
    data.x = 1000;
    data.y = 100;
    data.punchX = 500;
    data.punchY = 0;
    data.XSpeed = 300;
    data.YSpeed = 600;
    data.netID = netid;
    data.plantingTree = getState();
    BYTE* raw = packPlayerMovings(&data);
    int var = 0x808000;
    float water = 125.0f;
    memcpy(raw + 1, &var, 3);
    memcpy(raw + 16, &water, 4);
    g_server->SendPacketRaw(true, 4, raw, 56, 0, ENET_PACKET_FLAG_RELIABLE);
}


void server::addBlock(bool client, int tile, int x, int y) {
    PlayerMovings data;
    data.packetType = 0x3;
    data.characterState = 0x0; // animation
    data.x = x;
    data.y = y;
    data.punchX = x;
    data.punchY = y;
    data.XSpeed = 0;
    data.YSpeed = 0;
    data.netID = g_server->m_world.local.netid;
    data.plantingTree = tile;
    g_server->SendPacketRaw(true, 4, packPlayerMovings(&data), 56, 0, ENET_PACKET_FLAG_RELIABLE); // If client = false then auto ban
}

void server::placeBlock(bool client, int tile, int x, int y) {
    PlayerMovings data;
    data.packetType = PACKET_TILE_CHANGE_REQUEST;
    data.characterState = 0x0; // animation
    data.x = g_server->m_world.local.pos.m_x;
    data.y = g_server->m_world.local.pos.m_y;
    data.punchX = x;
    data.punchY = y;
    data.XSpeed = 0;
    data.XSpeed = 0;
    data.YSpeed = 0; gameupdatepacket_t* packet;
    data.netID = g_server->m_world.local.netid;
    data.plantingTree = tile;
    g_server->SendPacketRaw(client, 4, packPlayerMovings(&data), 56, 0, ENET_PACKET_FLAG_RELIABLE);
}






void server::wearItem(int itemid) {
    variantlist_t varlist{ "OnEquipNewItem" };
    varlist[1] = itemid;
    g_server->send(false, varlist, g_server->m_world.local.netid, -1);
    PlayerMovings data;
    g_server->SendPacketRaw(false, 4, packPlayerMovings(&data), 56, 0, ENET_PACKET_FLAG_RELIABLE);
}

void server::breakBlock(bool client, int x, int y) {

}

BYTE* GetStructPointerFromTankPacket(ENetPacket* packet)
{
    const unsigned int packetLenght = packet->dataLength;
    BYTE* result = nullptr;
    if (packetLenght >= 0x3C)
    {
        BYTE* packetData = packet->data;
        result = packetData + 4;
        if (*static_cast<BYTE*>(packetData + 16) & 8)
        {
            if (packetLenght < *reinterpret_cast<int*>(packetData + 56) + 60)
            {
                result = nullptr;
            }
        }
        else
        {
            int zero = 0;
            memcpy(packetData + 56, &zero, 4);
        }
    }
    return result;
}


void server::enterDoor(bool client, int x, int y) {
   
    
    PlayerMovings data2{};
    data2.packetType = 8;
    data2.plantingTree = 6;
    data2.characterState = 0; // animation
    data2.x = g_server->m_world.local.pos.m_x;
    data2.y = g_server->m_world.local.pos.m_y;
    data2.punchX = x;
    data2.punchY = y;
    data2.XSpeed = 0;
    data2.YSpeed = 0;
    data2.netID = g_server->m_world.local.netid;
    g_server->SendPacketRaw(client, 4, packPlayerMovings(&data2), 56, 0, ENET_PACKET_FLAG_RELIABLE);
}

void agabe(ENetPacket* packet) {
    auto lalal = GetStructPointerFromTankPacket(packet);
    PlayerMovings* data = unpackRaw(GetStructPointerFromTankPacket(packet));
    cout << "agabe: " << int(data->plantingTree) << std::endl;
    cout << "agabe 2: " << int(data->XSpeed) << std::endl;
    cout << "agabe 3: " << int(data->YSpeed) << std::endl;
    cout << "agabe 4: " << int(data->punchX) << std::endl;
    cout << "agabe 5: " << int(data->punchY) << std::endl;
    cout << "agabe 6: " << int(data->characterState) << std::endl;
    cout << "agabe 7: " << int(data->netID) << std::endl;
    cout << "agabe 8: " << int(data->SecondaryNetID) << std::endl;
    cout << "agabe 9: " << int(data->x) << std::endl;
    cout << "agabe 10: " << int(data->y) << std::endl;
    cout << "agabe 11: " << int(data->packetType) << std::endl;

}

void server::handle_outgoing() {
    ENetEvent evt;
    while (enet_host_service(m_proxy_server, &evt, 0) > 0) {
        m_gt_peer = evt.peer;

        switch (evt.type) {
        case ENET_EVENT_TYPE_CONNECT: {
            if (!this->connect())
                return;
        } break;
        case ENET_EVENT_TYPE_RECEIVE: {
            int packet_type = get_packet_type(evt.packet);
         
            switch (packet_type) {
            case NET_MESSAGE_GENERIC_TEXT:
                if (events::out::generictext(utils::get_text(evt.packet))) {
                    enet_packet_destroy(evt.packet);
                    return;
                }
                break;
            case NET_MESSAGE_GAME_MESSAGE:
                if (events::out::gamemessage(utils::get_text(evt.packet))) {
                    enet_packet_destroy(evt.packet);
                    return;
                }
                break;

            case NET_MESSAGE_GAME_PACKET: {
                auto packet = utils::get_struct(evt.packet);
                auto packetGUP = utils::get_structGUP(evt.packet);
                //agabe(evt.packet);
                //agabe(evt.packet);
                if (!packet) 
                    break;
                  
                if (!packetGUP)
                    break;
                    
                switch (packet->m_type) {

                case PACKET_STATE:
                    if (events::out::state(packetGUP, packet)) {
                        enet_packet_destroy(evt.packet);
                        return;
                    }
                    break;

                case PACKET_ITEM_ACTIVATE_REQUEST: {
                    bool varmi = true;
                    if (packet->m_int_data != 1796 && packet->m_int_data != 242 && packet->m_int_data != 7188) varmi = false;
                    if (iswear && varmi == false) {

                        auto& world = g_server->m_world;
                        types types;
                        auto type = items[packet->m_int_data];
                        if (type.name.find("Ancestral") != -1 || type.name.find("Samille") != -1 || type.name.find("Chakram") != -1) {
                            ances = type.itemID == ances ? 0000.0 : type.itemID;
                        }
                        else
                            switch (type.clothingType) {
                            case 0:
                                hair = type.itemID == hair ? 0000.0 : type.itemID;
                                break;
                            case 1:
                                shirt = type.itemID == shirt ? 0000.0 : type.itemID;
                                break;
                            case 2:
                                pants = type.itemID == pants ? 0000.0 : type.itemID;
                                break;
                            case 3:
                                shoe = type.itemID == shoe ? 0000.0 : type.itemID;
                                break;
                            case 4:
                                face = type.itemID == face ? 0000.0 : type.itemID;
                                break;
                            case 5:
                                hand = type.itemID == hand ? 0000.0 : type.itemID;
                                break;
                            case 6:
                                back = type.itemID == back ? 0000.0 : type.itemID;

                                break;
                            case 7:
                                hair = type.itemID == hair ? 0000.0 : type.itemID;
                                break;
                            case 8:
                                neck = type.itemID == neck ? 0000.0 : type.itemID;
                                break;
                            default:
                                hair = type.itemID == hair ? 0000.0 : type.itemID;
                                break;
                            }
                        int skincolor = 1685231359;

                        if (events::out::get_punch_id(packet->m_int_data) != 0) {
                            punch_effect = events::out::get_punch_id(packet->m_int_data);
                        }
                        if (back != 0000.0) {
                            doublejump = true;
                        }
                        sendState(world.local.netid);
                        variantlist_t liste{ "OnSetClothing" };
                        liste[1] = vector3_t{ hair,  shirt,  pants };
                        liste[2] = vector3_t{ shoe,  face,  hand };
                        liste[3] = vector3_t{ back,  mask,  neck };
                        liste[4] = skincolor;
                        liste[5] = vector3_t{ ances , 1.f, 0.f };
                        g_server->send(true, liste, world.local.netid, -1);

                        gameupdatepacket_t t{ 0 };
                        return;
                    }

                   
                        int allah = g_server->local_player.inventory.getItemCount(packet->m_int_data);
                        if (allah == 0) return;
                    
                  
                }
                case PACKET_CALL_FUNCTION:
                    if (events::out::variantlist(packet)) {
                        enet_packet_destroy(evt.packet);
                        return;
                    }
                    break;

                case PACKET_PING_REPLY:
                    if (events::out::pingreply(packet)) {
                        enet_packet_destroy(evt.packet);
                        return;
                    }
                    break;
                case PACKET_DISCONNECT:
                case PACKET_APP_INTEGRITY_FAIL:
                    if (gt::in_game)
                        return;
                    break;

                default: break;
                }
            } break;
            case NET_MESSAGE_TRACK: //track one should never be used, but its not bad to have it in case.
            case NET_MESSAGE_CLIENT_LOG_RESPONSE: return;

            default: PRINTS("Got unknown packet of type %d.\n", packet_type); break;
            }

            if (!m_server_peer || !m_real_server)
                return;
            enet_peer_send(m_server_peer, 0, evt.packet);
            enet_host_flush(m_real_server);
        } break;
        case ENET_EVENT_TYPE_DISCONNECT: {
            if (gt::in_game)
                return;
            if (gt::connecting) {
                this->disconnectsr(false);
                gt::connecting = false;
                return;
            }

        } break;
        default: PRINTS("UNHANDLED\n"); break;
        }
    }
}
std::vector<std::string> ItemDataContainer::arr, ItemDataContainer::itd, ItemDataContainer::itr, ItemDataContainer::itt, ItemDataContainer::itx, ItemDataContainer::ity, ItemDataContainer::ita, ItemDataContainer::ith;

void server::handle_incoming() {
    //TO-DO Fix Here
    ENetEvent event;

    while (enet_host_service(m_real_server, &event, 0) > 0) {
        switch (event.type) {
        case ENET_EVENT_TYPE_CONNECT: PRINTC("connection event\n"); break;
        case ENET_EVENT_TYPE_DISCONNECT: this->disconnectsr(true); return;
        case ENET_EVENT_TYPE_RECEIVE: {
            if (event.packet->data) {
                int packet_type = get_packet_type(event.packet);
                switch (packet_type) {
                case NET_MESSAGE_GENERIC_TEXT:
                    if (events::in::generictext(utils::get_text(event.packet))) {
                        enet_packet_destroy(event.packet);
                        return;
                    }
                    break;
                case NET_MESSAGE_GAME_MESSAGE:
                    if (events::in::gamemessage(utils::get_text(event.packet))) {
                        enet_packet_destroy(event.packet);
                        return;
                    }
                    break;

                case NET_MESSAGE_GAME_PACKET: {
                    auto packet = utils::get_struct(event.packet);
                    if (!packet)
                        break;

                    switch (packet->m_type) {
                    case PACKET_PING_REQUEST: {
                        gameupdatepacket_t pkt = {};
                        pkt.m_type = PACKET_PING_REPLY;
                        pkt.m_int_data = packet->m_int_data;
                        pkt.m_vec_x = 64.f;
                        pkt.m_vec_y = 64.f;
                        pkt.m_vec2_x = 1000.f;
                        pkt.m_vec2_y = 250.f;
                        g_server->send(true, NET_MESSAGE_GAME_PACKET, (uint8_t*)&pkt, sizeof(gameupdatepacket_t));
                    } break;
                    case PACKET_SEND_INVENTORY_STATE: {
                        local_player.inventory.slotCount = 0;
                        local_player.inventory.itemCount = 0;
                        local_player.inventory.items.clear();
                        std::vector<Item> invBuf;
                        LPBYTE extended_ptr = utils::get_extended(packet);
                        memcpy(&local_player.inventory.slotCount, extended_ptr + 5, 4);
                        memcpy(&local_player.inventory.itemCount, extended_ptr + 9, 2);
                        invBuf.resize(local_player.inventory.itemCount);
                        memcpy(invBuf.data(), extended_ptr + 11, invBuf.capacity() * sizeof(Item));
                        for (Item& item : invBuf)
                            local_player.inventory.items.operator[](item.id) = item;
                       
                    }break;                    
                    case PACKET_MODIFY_ITEM_INVENTORY: {
                        auto packetGUP = utils::get_structGUP(event.packet);
                        auto s_ptr = &local_player.inventory.items;
                        if (packetGUP->gained_item_count != 0) s_ptr->operator[](packet->m_int_data).count += (packetGUP->gained_item_count);
                        else if (s_ptr->operator[](packet->m_int_data).count > packet->m_jump_amount) s_ptr->operator[](packet->m_int_data).count -= (packet->m_jump_amount);
                        else  s_ptr->erase(packet->m_int_data);
    
                    }break;

                    case PACKET_TILE_CHANGE_REQUEST: {
                        if (packet->m_int_data == 18) {

                        }
                        else {

                            if (packet->m_player_flags == m_world.local.netid) {
                                auto s_items_ptr = &local_player.inventory.items;
                                auto it = s_items_ptr->find(packet->m_int_data);
                                if (it != s_items_ptr->end()) {
                                    if (it->second.count > 1)
                                        it->second.count -= 1;
                                    else
                                        s_items_ptr->erase(packet->m_int_data);
                                }
                            }
                        }

                    }break;

                    case PACKET_ITEM_CHANGE_OBJECT: {
                        if (packet->m_vec_x == 0 && packet->m_vec_y == 0) {
                            if (packet->m_player_flags == m_world.local.netid) {
                                auto object = m_world.objects.find(packet->m_int_data);
                                if (object->second.itemID != 112) {
                                    auto s_items_ptr = &local_player.inventory.items;
                                    int buffer = local_player.inventory.getObjectAmountToPickUpUnsafe(object->second);
                                    if (!local_player.inventory.doesItemExistUnsafe(object->second.itemID)) { //haven't tested yet.
                                        Item item;
                                        item.id = object->second.itemID;
                                        item.count = buffer;
                                        item.type = 0;
                                        s_items_ptr->operator[](item.id) = item;
                                    }
                                   
                                    else s_items_ptr->operator[](object->second.itemID).count += buffer;

                                    if (game_started) {
                                        if (object->second.itemID == 242) {
                                            total_bet += buffer;
                                        }
                                        if (object->second.itemID == 1796) {
                                            total_bet += (buffer * 100);
                                        }
                                    }
                                    //cout << "degisen: " << buffer << endl;
                                    if (s_items_ptr->operator[](242).count >= 100) {
                                        gameupdatepacket_t drop{ 0 };
                                        drop.m_type = PACKET_ITEM_ACTIVATE_REQUEST;
                                        drop.m_int_data = 242;
                                        g_server->send(false, NET_MESSAGE_GAME_PACKET, (uint8_t*)&drop, sizeof(gameupdatepacket_t));
                                     
                                    }
                                }
                                else {
                                    local_player.gems_balance += object->second.count;
                                }

                            }
                            m_world.objects.erase(packet->m_int_data);
                        }
                        else {
                            if (packet->m_player_flags == -1) {
                                DroppedItem item;
                                item.itemID = packet->m_int_data;
                                item.pos = vector2_t(packet->m_vec_x, packet->m_vec_y);
                                item.count = uint32_t(packet->m_struct_flags);
                                item.flags = 0; //set this to what??
                                item.uid = ++m_world.lastDroppedUid;
                                m_world.objects[item.uid] = item;
                            }
                            else if (packet->m_player_flags == -3) {
                                auto obj = m_world.objects.find(packet->m_item);
                                if (obj != m_world.objects.end()) {
                                    obj->second.itemID = packet->m_int_data;
                                    obj->second.pos = vector2_t(packet->m_vec_x, packet->m_vec_y);
                                    obj->second.count = uint32_t(packet->m_struct_flags);
                                }
                            }
                        }

                    }break;

                        
                    case PACKET_CALL_FUNCTION:
                        //
                        if (events::in::variantlist(packet)) {
                            enet_packet_destroy(event.packet);
                            return;
                        }
                        break;
                    case PACKET_SEND_MAP_DATA:
                        if (events::in::sendmapdata(packet, event.packet->dataLength)) {
                            enet_packet_destroy(event.packet);
                            return;
                        }
                        break;
                    case PACKET_STATE:
                        if (events::in::state(packet)) {
                            enet_packet_destroy(event.packet);
                            return;
                        }
                        break;
                    
                        //no need to print this for handled packet types such as func call, because we know its 1
                    default: break;
                    }
                } break;

                    //ignore tracking packet, and request of client crash log
                case NET_MESSAGE_TRACK:
                    if (events::in::tracking(utils::get_text(event.packet))) {
                        enet_packet_destroy(event.packet);
                        return;
                    }
                    break;
                case NET_MESSAGE_CLIENT_LOG_REQUEST: return;

                default: PRINTS("Got unknown packet of type %d.\n", packet_type); break;
                }
            }

            if (!m_gt_peer || !m_proxy_server)
                return;
            enet_peer_send(m_gt_peer, 0, event.packet);
            enet_host_flush(m_proxy_server);
            
        } break;

        default: break;
        }
    }
}

void server::lockThread()
{
    if (threadID != std::hash<std::thread::id>{}(std::this_thread::get_id())) {
        if (mutexStatus.load()) {
            mutexStatus.store(false);
            this->cv.notify_all();
        }
    }
}
void server::unlockThread()
{
    if (threadID != std::hash<std::thread::id>{}(std::this_thread::get_id())) {
        if (!mutexStatus.load()) {
            mutexStatus.store(true);
            this->cv.notify_all();
        }
    }
}

void server::poll() {
    //outgoing packets going to real server that are intercepted by our proxy server
    this->handle_outgoing();

    if (!m_real_server)
        return;

    //ingoing packets coming to gt client intercepted by our proxy client
    this->handle_incoming();

}


bool server::start() {

    ENetAddress address;
    enet_address_set_host(&address, "0.0.0.0");
    address.port = m_proxyport;
    m_proxy_server = enet_host_create(&address, 1024, 10, 0, 0);
    m_proxy_server->usingNewPacket = false;
    this->threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
    if (!m_proxy_server) {
        PRINTS("failed to start the proxy server!\n");
        return false;
    }
    m_proxy_server->checksum = enet_crc32;
    auto code = enet_host_compress_with_range_coder(m_proxy_server);
    if (code != 0)
        PRINTS("enet host compressing failed\n");
    PRINTS("started the enet server.\n");
    return setup_client();
}


void server::quit() {
    gt::in_game = false;
    this->disconnectsr(true);
}

bool server::setup_client() {
    m_real_server = enet_host_create(0, 1, 2, 0, 0);
    m_real_server->usingNewPacket = true;
    if (!m_real_server) {
        PRINTC("failed to start the client\n");
        return false;
    }
    m_real_server->checksum = enet_crc32;
    auto code = enet_host_compress_with_range_coder(m_real_server);
    if (code != 0)
        PRINTC("enet host compressing failed\n");
    enet_host_flush(m_real_server);
    PRINTC("Started enet client\n");
    return true;
}


void server::redirect_server(variantlist_t& varlist) {
    m_port = varlist[1].get_uint32();
    m_token = varlist[2].get_uint32();
    m_user = varlist[3].get_uint32();
    auto str = varlist[4].get_string();

    auto doorid = str.substr(str.find("|"));
    m_server = str.erase(str.find("|")); //remove | and doorid from end
    PRINTC("port: %d token %d user %d server %s doorid %s\n", m_port, m_token, m_user, m_server.c_str(), doorid.c_str());
    varlist[1] = m_proxyport;
    varlist[4] = "127.0.0.1" + doorid;

    gt::connecting = true;
    send(true, varlist);
    if (m_real_server) {
        enet_host_destroy(m_real_server);
        m_real_server = nullptr;
    }
}

void server::disconnectsr(bool reset) {
    m_world.connected = false;
    m_world.local = {};
    m_world.players.clear();
    if (m_server_peer) {
        enet_peer_disconnect(m_server_peer, 0);
        m_server_peer = nullptr;
        enet_host_destroy(m_real_server);
        m_real_server = nullptr;
        cout << "Disconnected" << endl;
    }
    if (reset) {
        m_user = 0;
        m_token = 0;
        m_server = "213.179.209.168";
        m_port = 17198;
        cout << "Disconnected reset" << endl;
    }
    cout << "Disconnected not reset or server_peer" << endl;
}

bool server::connect() {
    PRINTS("Connecting to server.\n");
    ENetAddress address;
    enet_address_set_host(&address, m_server.c_str());
    address.port = m_port;
    PRINTS("port is %d and server is %s\n", m_port, m_server.c_str());
    if (!this->setup_client()) {
        PRINTS("Failed to setup client when trying to connect to server!\n");
        return false;
    }
    m_server_peer = enet_host_connect(m_real_server, &address, 2, 0);
    if (!m_server_peer) {
        PRINTS("Failed to connect to real server.\n");
        return false;
    }
    return true;
}

bool server::sendEnetPacket(ENetPacket* packet, bool client)
{
    auto peer = client ? m_gt_peer : m_server_peer;
    auto host = client ? m_proxy_server : m_real_server;


    if (!peer || !host)
    {
        printf("The packet could not be sent due to the peer or host its null. Type %s", (client == true) ? "Growtopia" : "Local");
        enet_packet_destroy(packet);
        goto failed;
    }
    else if (peer->state != ENET_PEER_STATE_CONNECTED) {
        /*auto packet2 = utils::get_struct(packet);
        cout << (int)packet2->m_type << endl;
        if (packet2->m_type == 1) {
            variantlist_t varlist{};
            auto extended = utils::get_extended(packet2);
            extended += 4; //since it casts to data size not data but too lazy to fix this
            varlist.serialize_from_mem(extended);
            auto func = varlist[0].get_string();
            cout << varlist[0].get_string() << endl;
        }*/
        enet_host_flush(host);
        std::cout << "ENET_PEER_STATE_CONNECTED is not connected!" << std::endl;
    }
    else  if (enet_list_size(&host->peers->sentReliableCommands) > 45)
    {
        printf("Packets have been cleared due to an excessive accumulation of packets.\n");
        enet_list_clear(&host->peers->sentReliableCommands);
        enet_packet_destroy(packet);
        goto failed;
    }
    else if (enet_peer_send(peer, 0, packet) != 0)
    {

            printf("The packet could not be sent due to the enet_peer_send function return false\n");
            enet_packet_destroy(packet);
            goto failed;
    }
    else
        enet_host_flush(host);
failed:
    return false;
}

void server::SendPacketRaw2(bool client, int a1, void* packetData, size_t packetDataSize, void* a4, int packetFlag)
{
    ENetPacket* p;
    auto peer = client ? m_gt_peer : m_server_peer;
    if (peer) {
        if (a1 == 4 && *((BYTE*)packetData + 12) & 8)
        {
            p = enet_packet_create(0, packetDataSize + *((DWORD*)packetData + 13) + 5, packetFlag);
            int four = 4;
            memcpy(p->data, &four, 4);
            memcpy((char*)p->data + 4, packetData, packetDataSize);
            memcpy((char*)p->data + packetDataSize + 4, a4, *((DWORD*)packetData + 13));
            enet_peer_send(peer, 0, p);
        }
        else
        {
            p = enet_packet_create(0, packetDataSize + 5, packetFlag);
            memcpy(p->data, &a1, 4);
            memcpy((char*)p->data + 4, packetData, packetDataSize);
            enet_peer_send(peer, 0, p);
        }
    }
}
void server::SendPacketRaw(bool client, int a1, void* packetData, size_t packetDataSize, void* a4, int packetFlag) {
    ENetPacket* p;
    auto peer = client ? m_gt_peer : m_server_peer;
    if (peer) {
        if (a1 == 4 && *((BYTE*)packetData + 12) & 8) {
            p = enet_packet_create(0, packetDataSize + *((DWORD*)packetData + 13) + 5, packetFlag);
            int four = 4;
            memcpy(p->data, &four, 4);
            memcpy((char*)p->data + 4, packetData, packetDataSize);
            memcpy((char*)p->data + packetDataSize + 4, a4, *((DWORD*)packetData + 13));
            enet_peer_send(peer, 0, p);
        }
        else {
            p = enet_packet_create(0, packetDataSize + 5, packetFlag);
            memcpy(p->data, &a1, 4);
            memcpy((char*)p->data + 4, packetData, packetDataSize);
            enet_peer_send(peer, 0, p);
        }
    }
    delete (char*)packetData;
}

void server::send(bool client, int32_t type, uint8_t* data, int32_t len) {
    auto peer = client ? m_gt_peer : m_server_peer;
    auto host = client ? m_proxy_server : m_real_server;

    if (!peer || !host)
        return;
    auto packet = enet_packet_create(0, len + 5, ENET_PACKET_FLAG_RELIABLE);
    auto game_packet = (gametextpacket_t*)packet->data;
    game_packet->m_type = type;
    if (data)
        memcpy(&game_packet->m_data, data, len);

    memset(&game_packet->m_data + len, 0, 1);
    int code = enet_peer_send(peer, 0, packet);
    if (code != 0)
        PRINTS("Error Sending Packet! Code: %d\n", code);
    enet_host_flush(host);
}

//bool client: true - sends to growtopia client    false - sends to gt server
void server::send(bool client, variantlist_t& list, int32_t netid, int32_t delay) {
    auto peer = client ? m_gt_peer : m_server_peer;
    auto host = client ? m_proxy_server : m_real_server;

    if (!peer || !host)
        return;

    uint32_t data_size = 0;
    void* data = list.serialize_to_mem(&data_size, nullptr);

    //optionally we wouldnt allocate this much but i dont want to bother looking into it
    auto update_packet = MALLOC(gameupdatepacket_t, +data_size);
    auto game_packet = MALLOC(gametextpacket_t, +sizeof(gameupdatepacket_t) + data_size);

    if (!game_packet || !update_packet)
        return;

    memset(update_packet, 0, sizeof(gameupdatepacket_t) + data_size);
    memset(game_packet, 0, sizeof(gametextpacket_t) + sizeof(gameupdatepacket_t) + data_size);
    game_packet->m_type = NET_MESSAGE_GAME_PACKET;

    update_packet->m_type = PACKET_CALL_FUNCTION;
    update_packet->m_player_flags = netid;
    update_packet->m_packet_flags |= 8;
    update_packet->m_int_data = delay;
    memcpy(&update_packet->m_data, data, data_size);
    update_packet->m_data_size = data_size;
    memcpy(&game_packet->m_data, update_packet, sizeof(gameupdatepacket_t) + data_size);
    free(update_packet);

    auto packet = enet_packet_create(game_packet, data_size + sizeof(gameupdatepacket_t), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
    enet_host_flush(host);
    free(game_packet);
}


//bool client: true - sends to growtopia client    false - sends to gt server
void server::send(bool client, std::string text, int32_t type) {
    auto peer = client ? m_gt_peer : m_server_peer;
    auto host = client ? m_proxy_server : m_real_server;

    if (!peer || !host)
        return;
    auto packet = enet_packet_create(0, text.length() + 5, ENET_PACKET_FLAG_RELIABLE);
    auto game_packet = (gametextpacket_t*)packet->data;
    game_packet->m_type = type;
    memcpy(&game_packet->m_data, text.c_str(), text.length());

    memset(&game_packet->m_data + text.length(), 0, 1);
    int code = enet_peer_send(peer, 0, packet);
    if (code != 0)
        PRINTS("Error Sending Packet! Code: %d\n", code);
    enet_host_flush(host);
}