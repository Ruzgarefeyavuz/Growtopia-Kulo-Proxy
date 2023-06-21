#include "events.h"
#include "dialog.h"
#include "json.hpp"
#include "gt.hpp"
#include "proton/hash.hpp"
#include "proton/rtparam.hpp"
#include "proton/variant.hpp"
#include "server.h"
#include <vector> 
#include "utils.h"
#include <thread>
#include <limits.h>
#include "HTTPRequest.hpp"
#include "items_dat_decode.h"
#include "namespace_class.h"
#include "cpphook.h"
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "player.h"
#include "world.h"
#include "Pathfinder/PathFinder.h"

int type_delay = 250;
int roulettex = 0;
int roulettey = 0;
int typenumber = 36;
int color_type = 4;
bool type = false;
int yuzde2 = 10;
int notaxx = 0;
int notaxy = 0;
int lampx = 0;
int lampy = 0;
int lampx2 = 0;
int lampy2 = 0;
int punchx = 0;
int punchy = 0;
bool csn_punch = false;
int selectednumber = 6;
bool reme = false, legit = false, qq = false;
int yposautohoster = 0, xposautohoster = 0;
std::vector<int> intToDigits(int num_)
{
    std::vector<int> ret;
    string iStr = to_string(num_);

    for (int i = iStr.size() - 1; i >= 0; --i)
    {
        int units = pow(10, i);
        int digit = num_ / units % 10;
        ret.push_back(digit);
    }

    return ret;
}

void send_vset() {
    if (iswear) {
        int skincolor = 1685231359;
        variantlist_t liste{ "OnSetClothing" };
        liste[1] = vector3_t{ hair,  shirt,  pants };
        liste[2] = vector3_t{ shoe,  face,  hand };
        liste[3] = vector3_t{ back,  mask,  neck };
        liste[4] = skincolor;
        liste[5] = vector3_t{ ances , 1.f, 0.f };

        g_server->send(true, liste, g_server->m_world.local.netid, -1);
        g_server->sendState(g_server->m_world.local.netid);
    }
}
bool events::out::variantlist(gameupdatepacket_t* packet) {
    variantlist_t varlist{};
    varlist.serialize_from_mem(utils::get_extended(packet));
    /*if (iswear) {
        int skincolor = 1685231359;
        variantlist_t liste{ "OnSetClothing" };
        liste[1] = vector3_t{ hair,  shirt,  pants };
        liste[2] = vector3_t{ shoe,  face,  hand };
        liste[3] = vector3_t{ back,  mask,  neck };
        liste[4] = skincolor;
        liste[5] = vector3_t{ ances , 1.f, 0.f };

        g_server->send(true, liste, g_server->m_world.local.netid, -1);
        g_server->sendState(g_server->m_world.local.netid);
    }*/
    if (debug)
    PRINTS("varlist: %s\n", varlist.print().c_str());
    return false;
}

bool events::out::pingreply(gameupdatepacket_t* packet) {
    //since this is a pointer we do not need to copy memory manually again
    packet->m_vec2_x = 1000.f;  //gravity
    packet->m_vec2_y = 250.f;   //move speed
    packet->m_vec_x = 64.f;     //punch range
    packet->m_vec_y = 64.f;     //build range
    packet->m_jump_amount = 0;  //for example unlim jumps set it to high which causes ban
    packet->m_player_flags = 0; //effect flags. good to have as 0 if using mod noclip, or etc.
    return false;
}
bool fastdrop = false;
bool hostermode = false;
void force() {

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    fastdrop = false;
}

void forceacc()
{

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    bruhmoment = false;
}
bool isInside(int circle_x, int circle_y, int rad, int x, int y) {
    // Compare radius of circle with distance
    // of its center from given point
    if ((x - circle_x) * (x - circle_x) + (y - circle_y) * (y - circle_y) <= rad * rad)
        return true;
    else
        return false;
}

void SendPacket(int a1, string a2, ENetPeer* enetPeer)
{
    if (enetPeer)
    {
        ENetPacket* v3 = enet_packet_create(0, a2.length() + 5, 1);
        memcpy(v3->data, &a1, 4);
        //*(v3->data) = (DWORD)a1;
        memcpy((v3->data) + 4, a2.c_str(), a2.length());

        //cout << std::hex << (int)(char)v3->data[3] << endl;
        enet_peer_send(enetPeer, 0, v3);
    }
}

void save_details() {
    ifstream ifs("save/config.json");   
    if (ifs.is_open()) {
        json j;
        ifs >> j;
        j["wm"] = wrench;
        j["ptp"] = pathfinding;
        j["modas"] = modas;
        j["modas3"] = ban_all;
        j["modas2"] = exit_world;
        ofstream f_("save/config.json");
        f_ << j;
        f_.close();
    }

}
int balance() {
    int wl = 0;
    wl += g_server->local_player.inventory.getItemCount(242);
    wl += g_server->local_player.inventory.getItemCount(1796) * 100;
    wl += g_server->local_player.inventory.getItemCount(7188) * 10000;
    return wl;
}

int item_count(int itemid) {
    int wl = g_server->local_player.inventory.getItemCount(itemid);
    return wl;
}

bool daw()
{
    if (item_count(242) > 0) {
        dropwl = true;
        g_server->send(false, "action|drop\n|itemID|242");
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|242|\ncount|" + to_string(item_count(242))); //242
    }
    if (item_count(1796) > 0) {
        dropdl = true;
        g_server->send(false, "action|drop\n|itemID|1796");
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|1796|\ncount|" + to_string(item_count(1796))); //242
    }
    if (item_count(7188) > 0) {
        dropwl = true;
        g_server->send(false, "action|drop\nitemID|7188");
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|7188|\ncount|" + to_string(item_count(7188))); //242
    }
    return true;
}

void autoc() {
    gt::send_log("`9Collecting items in 10far");
    for (auto it = g_server->m_world.objects.begin(); it != g_server->m_world.objects.end(); ++it)
    {
        if (isInside(it->second.pos.m_x, it->second.pos.m_y, 10 * 32, g_server->m_world.local.pos.m_x, g_server->m_world.local.pos.m_y))
        {
            GameUpdatePacket packet{ 0 };
            packet.pos_x = it->second.pos.m_x;
            packet.pos_y = it->second.pos.m_y;
            packet.type = 11;
            packet.netid = -1;
            packet.object_id = it->second.uid;
            g_server->send(false, 4, (uint8_t*)&packet, sizeof(GameUpdatePacket));
        }
    }
}

void placeaga(int id, int posx, int posy) {
    if (posx == 0 || posy == 0)
    {
        gt::send_log("position cannot be null!");
        return;
    }
    auto pos = g_server->m_world.local.pos;
    GameUpdatePacket legitpacket{ 0 };
    legitpacket.type = PACKET_STATE;
    legitpacket.item_id = id;
    legitpacket.int_x = posx;
    legitpacket.int_y = posy;
    legitpacket.vec_x = pos.m_x;
    legitpacket.vec_y = pos.m_y;
    legitpacket.flags = 2592;
    // her þey doðruda aaqq

        g_server->send(false, NET_MESSAGE_GAME_PACKET, (uint8_t*)&legitpacket, sizeof(GameUpdatePacket));
    
}
void place_tile(int id, int posx, int posy) {
    if (posx == 0 || posy == 0)
    {
        gt::send_log("position cannot be null!");
        return;
    }
    auto pos = g_server->m_world.local.pos;

    g_server->enterDoor(false, posx, posy);


    GameUpdatePacket legitpacket{ 0 };
    legitpacket.type = PACKET_STATE;
    legitpacket.item_id = id;
    legitpacket.int_x = posx;
    legitpacket.int_y = posy;
    legitpacket.vec_x = pos.m_x;
    legitpacket.vec_y = pos.m_y;
    legitpacket.flags = 2592;
    // her þey doðruda aaqq
    if (legit)
    {
        g_server->send(false, NET_MESSAGE_GAME_PACKET, (uint8_t*)&legitpacket, sizeof(GameUpdatePacket));
    }
    GameUpdatePacket packet{ 0 };
    packet.type = PACKET_TILE_CHANGE_REQUEST;
    packet.item_id = id;
    packet.int_x = posx;
    packet.int_y = posy;
    packet.vec_x = pos.m_x;
    packet.vec_y = pos.m_y;
    g_server->send(false, NET_MESSAGE_GAME_PACKET, (uint8_t*)&packet, sizeof(GameUpdatePacket));
}

void break_tile(int id, int posx, int posy) {
    //g_server->breakBlock(false, posx, posy);
    place_tile(18, posx, posy);
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    place_tile(18, posx, posy);
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    place_tile(18, posx, posy);
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    place_tile(18, posx, posy);
}


/*void farm_auto() {

    while (farmauto) {
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        if (done) {
            int posx = g_server->m_world.local.pos.m_x / 32;
            int posy = g_server->m_world.local.pos.m_y / 32;
            place_tile(farmid, posx + 1, posy);
            done = false;
        }
        else {
            int posx = g_server->m_world.local.pos.m_x / 32;
            int posy = g_server->m_world.local.pos.m_y / 32;
            break_tile(farmid, posx + 1, posy);
            done = true;
        }
    }
}*/

void TpRetard(float x, float y)
{
    vector2_t pos;
    pos.m_x = x;
    pos.m_y = y;
    variantlist_t varlist{ "OnSetPos" };
    varlist[1] = pos;
    g_server->m_world.local.pos = pos;
    g_server->send(true, varlist, g_server->m_world.local.netid, -1);
}


int events::out::get_punch_id(const int id_) {
    switch (id_) {
    case 138: return 1;
    case 366:
    case 1464:
        return 2;
    case 472: return 3;
    case 594:
    case 10130:
    case 5424:
    case 5456:
    case 4136:
    case 10052:
        return 4;
    case 768: return 5;
    case 900:
    case 7760:
    case 9272:
    case 5002:
    case 7758:
        return 6;
    case 910:
    case 4332:
        return 7;
    case 930:
    case 1010:
    case 6382:
        return 8;
    case 1016:
    case 6058:
        return 9;
    case 1204:
    case 9534:
        return 10;
    case 1378: return 11;
    case 1440: return 12;
    case 1484:
    case 5160:
    case 9802:
        return 13;
    case 1512:
    case 1648:
        return 14;
    case 1542: return 15;
    case 1576: return 16;
    case 1676:
    case 7504:
        return 17;
    case 1748:
    case 8006:
    case 8008:
    case 8010:
    case 8012:
        return 19;
    case 1710:
    case 4644:
    case 1714:
    case 1712:
    case 6044:
    case 1570:
        return 18;
    case 1780: return 20;
    case 1782:
    case 5156:
    case 9776:
    case 9782:
    case 9810:
        return 21;
    case 1804:
    case 5194:
    case 9784:
        return 22;
    case 1868:
    case 1998:
        return 23;
    case 1874: return 24;
    case 1946:
    case 2800:
        return 25;
    case 1952:
    case 2854:
        return 26;
    case 1956: return 27;
    case 1960: return 28;
    case 2908:
    case 6312:
    case 9496:
    case 8554:
    case 3162:
    case 9536:
    case 4956:
    case 3466:
    case 4166:
    case 4506:
    case 2952:
    case 9520:
    case 9522:
    case 8440:
    case 3932:
    case 3934:
    case 8732:
    case 3108:
    case 9766:
    case 12368:
        return 29;
    case 1980: return 30;
    case 2066:
    case 4150:
    case 11082:
    case 11080:
    case 11078:
        return 31;
    case 2212:
    case 5174:
    case 5004:
    case 5006:
    case 5008:
        return 32;
    case 2218: return 33;
    case 2220: return 34;
    case 2266: return 35;
    case 2386: return 36;
    case 2388: return 37;
    case 2450:
        return 38;
    case 2476:
    case 4208:
    case 12308:
    case 10336:
    case 9804:
        return 39;
    case 4748:
    case 4294:
        return 40;
    case 2512:
    case 9732:
    case 6338:
        return 41;
    case 2572: return 42;
    case 2592:
    case 9396:
    case 2596:
    case 9548:
    case 9812:
        return 43;
    case 2720: return 44;
    case 2752: return 45;
    case 2754: return 46;
    case 2756: return 47;
    case 2802: return 49;
    case 2866: return 50;
    case 2876: return 51;
    case 2878:
    case 2880:
        return 52;
    case 2906:
    case 4170:
    case 4278:
        return 53;
    case 2886: return 54;
    case 2890: return 55;
    case 2910: return 56;
    case 3066: return 57;
    case 3124: return 58;
    case 3168: return 59;
    case 3214:
    case 9194:
        return 60;
    case 7408:
    case 3238:
        return 61;
    case 3274: return 62;
    case 3300: return 64;
    case 3418: return 65;
    case 3476: return 66;
    case 3596: return 67;
    case 3686: return 68;
    case 3716: return 69;
    case 4290: return 71;
    case 4474: return 72;
    case 4464:
    case 9500:
        return 73;
    case 4746: return 75;
    case 4778:
    case 6026: case 7784:
        return 76;
    case 4996:
    case 3680:
    case 5176:
        return 77;
    case 4840: return 78;
    case 5206: return 79;
    case 5480:
    case 9770:
    case 9772:
        return 80;
    case 6110: return 81;
    case 6308: return 82;
    case 6310: return 83;
    case 6298: return 84;
    case 6756: return 85;
    case 7044: return 86;
    case 6892: return 87;
    case 6966: return 88;
    case 7088:
    case 11020:
        return 89;
    case 7098:
    case 9032:
        return 90;
    case 7192: return 91;
    case 7136:
    case 9738:
        return 92;
    case 3166: return 93;
    case 7216: return 94;
    case 7196:
    case 9340:
        return 95;
    case 7392:
    case 9604:
        return 96;
    case 7384: return 98;
    case 7414: return 99;
    case 7402: return 100;
    case 7424: return 101;
    case 7470: return 102;
    case 7488: return 103;
    case 7586:
    case 7646:
    case 9778:
        return 104;
    case 7650: return 105;
    case 6804:
    case 6358:
        return 106;
    case 7568:
    case 7570:
    case 7572:
    case 7574:
        return 107;
    case 7668: return 108;
    case 7660:
    case 9060:
        return 109;
    case 7584:
        return 110;
    case 7736:
    case 9116:
    case 9118:
    case 7826:
    case 7828:
    case 11440:
    case 11442:
    case 11312:
    case 7830:
    case 7832:
    case 10670:
    case 9120:
    case 9122:
    case 10680:
    case 10626:
    case 10578:
    case 10334:
    case 11380:
    case 11326:
    case 7912:
    case 11298:
    case 10498:
    case 12342:
        return 111;
    case 7836:
    case 7838:
    case 7840:
    case 7842:
        return 112;
    case 7950: return 113;
    case 8002: return 114;
    case 8022: return 116;
    case 8036: return 118;
    case 9348:
    case 8372:
        return 119;
    case 8038: return 120;
    case 8816:
    case 8818:
    case 8820:
    case 8822:
        return 128;
    case 8910: return 129;
    case 8942: return 130;
    case 8944:
    case 5276:
        return 131;
    case 8432:
    case 8434:
    case 8436:
    case 8950:
        return 132;
    case 8946: case 9576: return 133;
    case 8960: return 134;
    case 9006: return 135;
    case 9058: return 136;
    case 9082:
    case 9304:
        return 137;
    case 9066:
        return 138;
    case 9136: return 139;
    case 9138:
        return 140;
    case 9172: return 141;
    case 9254: return 143;
    case 9256: return 144;
    case 9236: return 145;
    case 9342: return 146;
    case 9542: return 147;
    case 9378: return 148;
    case 9376: return 149;
    case 9410: return 150;
    case 9462: return 151;
    case 9606:
        return 152;
    case 9716:
    case 5192:
        return 153;
    case 10048: return 167;
    case 10064: return 168;
    case 10046: return 169;
    case 10050: return 170;
    case 10128: return 171;
    case 10210:
    case 9544:
        return 172;
    case 10330: return 178;
    case 10398: return 179;
    case 10388:
    case 9524:
    case 9598:
        return 180;
    case 10442: return 184;
    case 10506: return 185;
    case 10652: return 188;
    case 10676: return 191;
    case 10694: return 193;
    case 10714: return 194;
    case 10724: return 195;
    case 10722: return 196;
    case 10754: return 197;
    case 10800: return 198;
    case 10888: return 199;
    case 10886:
    case 11308:
        return 200;
    case 10890: return 202;
    case 10922: case 9550: return 203;
    case 10990: return 205;
    case 10998: return 206;
    case 10952: return 207;
    case 11000: return 208;
    case 11006: return 209;
    case 11046: return 210;
    case 11052: return 211;
    case 10960: return 212;
    case 10956:
    case 9774:
        return 213;
    case 10958: return 214;
    case 10954: return 215;
    case 11076: return 216;
    case 11084: return 217;
    case 11118:
    case 9546:
    case 9574:
        return 218;
    case 11120: return 219;
    case 11116: return 220;
    case 11158: return 221;
    case 11162: return 222;
    case 11142: return 223;
    case 11232: return 224;
    case 11140: return 225;
    case 11248:
    case 9596:
    case 9636:
        return 226;
    case 11240: return 227;
    case 11250: return 228;
    case 11284: return 229;
    case 11292: return 231;
    case 11314: return 233;
    case 11316: return 234;
    case 11324: return 235;
    case 11354: return 236;
    case 11760:
    case 11464:
    case 11438:
    case 12230:
    case 11716:
    case 11718:
    case 11674:
    case 11630:
    case 11786:
    case 11872:
    case 11762:
    case 11994:
    case 12172:
    case 12184:
    case 11460:
    case 12014:
    case 12016:
    case 12018:
    case 12020:
    case 12022:
    case 12024:
    case 12246:
    case 12248:
    case 12176:
    case 12242:
    case 11622:
    case 12350:
    case 12300:
    case 12374:
    case 12356:
        return 237;
    case 11814:
    case 12232:
    case 12302:
        return 241;
    case 11548:
    case 11552:
        return 242;
    case 11704:
    case 11706:
        return 243;
    case 12180:
    case 12346:
    case 12344:
        return 244;
    case 11506:
    case 11508:
    case 11562:
    case 11768:
    case 11882:
    case 11720:
    case 11884:
        return 245;
    case 12432:
    case 12434:
        return 246;
    case 11818:
    case 11876:
    case 12000:
    case 12240:
    case 12642:
    case 12644:
        return 248;

    }
    return 0;
}

bool dropitem(int itemid, string count) {
    cdrop = true;
    g_server->send(false, "action|drop\nitemID|" + std::to_string(itemid));
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|242|\ncount|" + count); //242
    return true;
}


bool unaccessing = false;
void unaccess() {
    unaccessing = true;
        g_server->send(false, "action|input\n|text|/unaccess");
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    g_server->send(false, "action|dialog_return\ndialog_name|unaccess\nbuttonClicked|Yes"); //242
    gt::send_log("`2Done Unaccess.");
}
void tptopos(float x, float y)
{
    vector2_t pos;
    pos.m_x = x;
    pos.m_y = y;
    variantlist_t varlist{ "OnSetPos" };
    varlist[1] = pos;
    g_server->m_world.local.pos = pos;
    g_server->send(true, varlist, g_server->m_world.local.netid, -1);
}


bool custom_drop(int sayi, vector2_t pos, float m_x, float m_y) {

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    string cdropcount = to_string(sayi);
    if (balance() < sayi) {
        gt::send_log("`9Dont have `#balance`9. balance: " + to_string(balance()) + ".");
        return true;
    }
    if (sayi < 100) {

        if (item_count(242) < sayi) {
            gameupdatepacket_t drop{ 0 };
            drop.m_type = PACKET_ITEM_ACTIVATE_REQUEST;
            drop.m_int_data = 1796;
            g_server->send(false, NET_MESSAGE_GAME_PACKET, (uint8_t*)&drop, sizeof(gameupdatepacket_t));
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        dropwl = true;
        g_server->send(false, "action|drop\n|itemID|242");
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|242|\ncount|" + cdropcount); //242
        gt::send_log("`9Dropping `c" + cdropcount + "`9 wls...");
    }

    else if (sayi > 10000) {


        int sayi1 = (sayi / 10000);

        int kalan = ((sayi / 100) - (sayi1 * 100));
        int kalan2 = sayi - ((kalan * 100) + (sayi1 * 10000));
        if (kalan > item_count(1796)) {
            gameupdatepacket_t drop{ 0 };
            drop.m_type = PACKET_ITEM_ACTIVATE_REQUEST;
            drop.m_int_data = 7188;
            g_server->send(false, NET_MESSAGE_GAME_PACKET, (uint8_t*)&drop, sizeof(gameupdatepacket_t));
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        else if (item_count(242) < kalan2) {
            gameupdatepacket_t drop{ 0 };
            drop.m_type = PACKET_ITEM_ACTIVATE_REQUEST;
            drop.m_int_data = 1796;
            g_server->send(false, NET_MESSAGE_GAME_PACKET, (uint8_t*)&drop, sizeof(gameupdatepacket_t));
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        dropbgl = true;
        g_server->send(false, "action|drop\n|itemID|7188");
        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|7188|\ncount|" + std::to_string(sayi1)); //242

        dropdl = true;
        g_server->send(false, "action|drop\n|itemID|1796");
        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|1796|\ncount|" + std::to_string(kalan)); //242

        dropwl = true;
        g_server->send(false, "action|drop\n|itemID|242");
        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|242|\ncount|" + std::to_string(kalan2)); //242

        gt::send_log("`9Dropping `c" + cdropcount + "`9 wls...");
    }
    else {
        int sayi1 = (sayi / 100);
        int kalan = (sayi % 100);

        if (item_count(242) < kalan) {
            gameupdatepacket_t drop{ 0 };
            drop.m_type = PACKET_ITEM_ACTIVATE_REQUEST;
            drop.m_int_data = 1796;
            g_server->send(false, NET_MESSAGE_GAME_PACKET, (uint8_t*)&drop, sizeof(gameupdatepacket_t));
            std::this_thread::sleep_for(std::chrono::milliseconds(10));

        }
        else if (item_count(1796) < sayi1) {
            gameupdatepacket_t drop{ 0 };
            drop.m_type = PACKET_ITEM_ACTIVATE_REQUEST;
            drop.m_int_data = 242;
            g_server->send(false, NET_MESSAGE_GAME_PACKET, (uint8_t*)&drop, sizeof(gameupdatepacket_t));
            std::this_thread::sleep_for(std::chrono::milliseconds(10));

        }
        dropdl = true;
        g_server->send(false, "action|drop\n|itemID|1796");
        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|1796|\ncount|" + std::to_string(sayi1)); //242

        dropwl = true;
        g_server->send(false, "action|drop\n|itemID|242");
        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|242|\ncount|" + std::to_string(kalan)); //242

        gt::send_log("`9Dropping `c" + cdropcount + "`9 wls...");
    }
    total_bet = 0;
}
void checkdrop(int pos1, int pos2) {
    int p5 = 0;
    int p6 = 0;
    for (auto it = g_server->m_world.objects.begin(); it != g_server->m_world.objects.end(); ++it)
    {
        p5 = it->second.pos.m_x / 32;
        p6 = it->second.pos.m_y / 32;
        if (p5 == pos1 && p6 == pos2) {
             
            gt::send_log("match");
        }
        else {
            gt::send_log("nah");
        }
    }
    gt::send_log("`4check" + to_string(p5) + "," + to_string(p6));
}

void takestuff(float box1x, float box1y, float box2x, float box2y) {
    for (auto it = g_server->m_world.objects.begin(); it != g_server->m_world.objects.end(); ++it)
    {
        if (isInside(it->second.pos.m_x, it->second.pos.m_y, 10 * 32, g_server->m_world.local.pos.m_x, g_server->m_world.local.pos.m_y))
        {
            GameUpdatePacket packet{ 0 };
            packet.pos_x = box1x, box2x;
            packet.pos_y = box1y, box2y;
            packet.type = 11;
            packet.netid = -1;
            packet.object_id = it->second.uid;
            g_server->send(false, 4, (uint8_t*)&packet, sizeof(GameUpdatePacket));
        }
    }
}
bool bandymas() {
    float posx = std::atoi(host::pos1x.c_str());
    float posy = std::atoi(host::pos1y.c_str());
    g_server->m_world.local.pos.m_x = posx;
    g_server->m_world.local.pos.m_y = posy;
    variantlist_t xdd{ "OnSetPos" };
    xdd[1] = g_server->m_world.local.pos;
    g_server->send(true, xdd, g_server->m_world.local.netid, -1);
    return true;
}
bool find_command(std::string chat, std::string name) {
    bool found = chat.find("/" + name) == 0;
    if (found)
        gt::send_log("`6" + chat);
    return found;
}
void ban_everyone() {
        gt::send_log("`9You have `4(Ban All)`9 When Mod joins option `2Enabled");
        gt::send_log("`4Banning `9Everyone in the world...");
        std::string username = "all";
        for (auto& player : g_server->m_world.players) {
            auto nik = player.name.substr(2); //remove color
            string banint = player.name.substr(2).substr(0, player.name.length() - 4);
            if (nik.find(username)) {
                g_server->send(false, "action|input\n|text|/ban " + banint);
            }
        }
    }

void exit_pasaulis() {
    gt::send_log("`9You have `4(Exit World)`9 When Mod joins option `2Enabled");
    gt::send_log("`bLeaving The World Now");
    g_server->send(false, "action|quit_to_exit", 3);
}
bool check_number(string num) {
    for (int i = 0; i < num.length(); i++)
        if (isdigit(num[i]) == false)
            return false;
    return true;
}
void tool_thread(int id, std::string name) {
    //std::this_thread::sleep_for(std::chrono::milliseconds(100));
    g_server->send(false, "action|dialog_return\ndialog_name|surgery\nbuttonClicked|tool" + std::to_string(id));
}
//Antiseptic *
void send_tool(int id, std::string content) {
    std::string name = "";

    switch (id) {
    case 1258:
        name = "`9Sponge``";
        break;
    case 1260:
        name = "`7Scalpel``";
        break;
    case 1262:
        name = "`3Anesthetic``";
        break;
    case 1264:
        name = "`7Antiseptic``";
        break;
    case 1266:
        name = "`2Antibiotic``";
        break;
    case 1268:
        name = "`1Splint``";
        break;
    case 1270:
        name = "`9Stitches``";
        break;
    case 4308:
        name = "`4Pins``";
        break;
    case 4310:
        name = "`8Transfusion``";
        break;
    case 4312:
        name = "`6Defibillator``";
        break;
    case 4314:
        name = "`5Clamp``";
        break;
    case 4316:
        name = "`4Ultrasound``";
        break;
    case 4318:
        name = "`7Lab Kit``";
        break;
    case 1296:
        name = "`1Gloves``";
        break;
    default:
        name = "`bid=" + std::to_string(id) + "``";
        break;
    }

    if (content.find("|noflags|" + std::to_string(id) + "|") == std::string::npos) {
        gt::send_log("You ran out of " + name + " tool!");
        id = 4320;
    }
    sel.push_back(std::thread(tool_thread, id, name));
    return;
}
void anti_gravity() {
    GameUpdatePacket packet{ 0 };
    packet.type = PACKET_TILE_CHANGE_REQUEST;
    packet.item_id = 4992;
    packet.int_x = 99;
    packet.int_y = 59;
    g_server->send(true, NET_MESSAGE_GAME_PACKET, (uint8_t*)&packet, sizeof(GameUpdatePacket));
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    auto pos = g_server->m_world.local.pos;

    g_server->enterDoor(true, 99, 59);


    GameUpdatePacket legitpacket{ 0 };
    legitpacket.type = PACKET_STATE;
    legitpacket.item_id = 18;
    legitpacket.int_x = 99;
    legitpacket.int_y = 59;
    legitpacket.vec_x = pos.m_x;
    legitpacket.vec_y = pos.m_y;
    legitpacket.flags = 2592;
    g_server->send(true, NET_MESSAGE_GAME_PACKET, (uint8_t*)&legitpacket, sizeof(GameUpdatePacket));
}
bool itsmod(int netid, std::string growid = "")
{
    if (modas) {
        if (netid == g_server->m_world.local.netid)
            return false;
        else if (growid.find(g_server->m_world.local.name) != std::string::npos)
            return false;
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        for (auto g : g_server->m_world.players)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(5));
            std::transform(g.name.begin(), g.name.end(), g.name.begin(), [](unsigned char c) { return std::tolower(c); });
            if (netid == g.netid)
                return false;
            else if (growid.find(g.name) != std::string::npos)
                return false;
        }
        gt::send_log("`2A `#@Moderator `2Joined the World");
        variantlist_t varlist{ "OnAddNotification" };
        varlist[1] = "interface/atomic_button.rttex";
        varlist[2] = "`2A `#@Moderator `2Joined the World";
        varlist[3] = "audio/hub_open.wav";
        varlist[4] = 0;
        g_server->send(true, varlist);
        if (ban_all && exit_world) {
            ban_everyone();
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            unaccess();
            exit_pasaulis();

        }

        else if (ban_all)
        {
            ban_everyone();
            unaccess();
        }
        else if (exit_world) {
            exit_pasaulis();
            unaccess();
           
        }
        return true;
    }
}
bool events::out::worldoptions(std::string option)
{
    std::string username = "all";
    for (auto& player : g_server->m_world.players) {
        auto name_2 = player.name.substr(2); //remove color
        std::transform(name_2.begin(), name_2.end(), name_2.begin(), ::tolower);
        if (name_2.find(username)) {
            auto& bruh = g_server->m_world.local;
            if (option == "pull")
            {
                string plyr = player.name.substr(2).substr(0, player.name.length() - 4);
                if (plyr != bruh.name.substr(2).substr(0, player.name.length() - 4))
                {
                    g_server->send(false, "action|input\n|text|/pull " + plyr);
                }
            }
            if (option == "kick")
            {
                string plyr = player.name.substr(2).substr(0, player.name.length() - 4);
                if (plyr != bruh.name.substr(2).substr(0, player.name.length() - 4))
                {
                    g_server->send(false, "action|input\n|text|/kick " + plyr);

                }
            }
            if (option == "ban")
            {
                string plyr = player.name.substr(2).substr(0, player.name.length() - 4);
                if (plyr != bruh.name.substr(2).substr(0, player.name.length() - 4))
                {
                    g_server->send(false, "action|input\n|text|/ban " + plyr);

                }
            }
        }
    }

    return true;
}
void fast_shop(int aga) {
    for (int i = 0; i < aga; i++) {
        g_server->send(false, "action|buy\nitem|" + lastshop, 2);
    }
    shopmode = false;
    gt::send_log(" bought " + to_string(aga) +  " " + lastshop);
}

bool events::out::generictext(std::string packet) {
   
    if (iswear) send_vset();
   // PRINTS("Generic text: %s\n", packet.c_str());
    auto& world = g_server->m_world;
    rtvar var = rtvar::parse(packet);
    if (!var.valid())
        return false;
    /*if (iswear) {
        int skincolor = 1685231359;
        variantlist_t liste{ "OnSetClothing" };
        liste[1] = vector3_t{ hair,  shirt,  pants };
        liste[2] = vector3_t{ shoe,  face,  hand };
        liste[3] = vector3_t{ back,  mask,  neck };
        liste[4] = skincolor;
        liste[5] = vector3_t{ ances , 1.f, 0.f };

        g_server->send(true, liste, g_server->m_world.local.netid, -1);
        g_server->sendState(g_server->m_world.local.netid);
    }*/
    if (packet.find("buttonClicked|iso11pul1") != -1) {
        mode = "`5Pull"; //bannj
        return true;
    }
    if (packet.find("buttonClicked|iso11pul2") != -1) {
        hostermode = !hostermode; //bannj
        return true;
    }
    if (packet.find("growscan_dialog") != -1) {

        return true;
    }
    if (packet.find("buttonClicked|bannj") != -1) {
        mode = "`4Ban";
        return true;
    }
    if (packet.find("buttonClicked|kicjlabas") != -1) {
        mode = "`4Kick";
        return true;
    }
    if (packet.find("ac_page") != -1) {
        if (packet.find("ac_enable|") != -1) {
            try {
                std::string aaa = packet.substr(packet.find("ble|") + 4, packet.size());
                std::string number = aaa.c_str();
                while (!number.empty() && isspace(number[number.size() - 1]))
                    number.erase(number.end() - (76 - 0x4B));
                gt::autocollect = stoi(number);
            }
            catch (exception a)
            {
                gt::send_log("`4Critical Error: `2override detected");

            }
        }
        if (packet.find("ac_range") != -1) {
            try
            {
                std::string aaa = packet.substr(packet.find("nge|") + 4, packet.size());
                std::string number = aaa.c_str();
                while (!number.empty() && isspace(number[number.size() - 1]))
                    number.erase(number.end() - (76 - 0x4B));
                gt::ac_range = std::stoi(number);
            }
            catch (exception a)
            {
                gt::send_log("`4Critical Error: `2override detected");

            }
        }
        return true;
    }
    if (packet.find("mod_settings_spare") != -1) {
        if (packet.find("leaveworld|") != -1) {
            try {
                std::string aaa = packet.substr(packet.find("rld|") + 4, packet.size());
                std::string number = aaa.c_str();
                while (!number.empty() && isspace(number[number.size() - 1]))
                    number.erase(number.end() - (76 - 0x4B));
                exit_world = stoi(number);
            }
            catch (exception a)
            {
                gt::send_log("`4Critical Error: `2override detected");

            }
        }
        if (packet.find("isjunkvisusnx|") != -1) {
            try {
                std::string aaa = packet.substr(packet.find("snx|") + 4, packet.size());
                std::string number = aaa.c_str();
                while (!number.empty() && isspace(number[number.size() - 1]))
                    number.erase(number.end() - (76 - 0x4B));
                ban_all = stoi(number);
            }
            catch (exception a)
            {
                std::cout << "ERROR: " << a.what() << '\n';
                gt::send_log("`4Critical Error: `2override detected");

            }
        }
        return true;
    }
    if (packet.find("buttonClicked|kadaryt") != -1) {
        try {
            Dialog a;
            a.addLabelWithIcon("Auto Mod Detect Settings", 278, LABEL_BIG);
            a.addTextBox("`9When `#@Moderator`9 Joins The World You will:");
            a.addCheckbox("leaveworld", "`9Exit World", exit_world);
            a.addCheckbox("isjunkvisusnx", "`9Ban Everyone in World", ban_all);
            a.endDialog("mod_settings_spare", "Okey", "Cancel");
            variantlist_t liste{ "OnDialogRequest" };
            liste[1] = a.finishDialog();
            g_server->send(true, liste);
        }
        catch (exception a) {
            gt::send_log("`4Critical Error: `2override detected");
        }
        return true;
    }
    if (packet.find("speed_page") != -1) {
        try {
            if (packet.find("ed_x|") != -1) {
                std::string aaa = packet.substr(packet.find("d_x|") + 4, packet.size());
                std::string number = aaa.c_str();
               // cout << number << endl;
                while (!number.empty() && isspace(number[number.size() - 1]))
                    number.erase(number.end() - (76 - 0x4B));
                speed_x = stof(number);
                g_server->sendState(world.local.netid);
            }
            if (packet.find("ed_y|") != -1) {
                std::string aaa = packet.substr(packet.find("d_y|") + 4, packet.size());
                std::string number = aaa.c_str();
                //cout << number << endl;
                while (!number.empty() && isspace(number[number.size() - 1]))
                    number.erase(number.end() - (76 - 0x4B));
                speed_y = stof(number);
                g_server->sendState(world.local.netid);
            }
            
        }
        catch (exception a) {
            std::cout << "error?";
            std::cout << a.what();
        }
        return true;
    }
    if (packet.find("spam_text") != -1) {
        try {
            if (packet.find("c_text|") != -1) {
                std::string aaa = packet.substr(packet.find("ext|") + 4, packet.size());
                std::string number = aaa.c_str();
                while (!number.empty() && isspace(number[number.size() - 1]))
                    number.erase(number.end() - (76 - 0x4B));
                enabled_color = stoi(number);
            }
            if (packet.find("auto_enable|") != -1) {
                std::string aaa = packet.substr(packet.find("ble|") + 4, packet.size());
                std::string number = aaa.c_str();
                while (!number.empty() && isspace(number[number.size() - 1]))
                    number.erase(number.end() - (76 - 0x4B));
                son = stoi(number);
            }
            if (packet.find("spam_msg|") != -1) {
                std::string msg = packet.substr(packet.find("spam_msg|") + 9, packet.length() - packet.find("spam_msg") - 1);
                aspam = msg;
            }
            if (packet.find("delay_msg|") != -1) {
                std::string msg = packet.substr(packet.find("delay_msg|") + 10, packet.length() - packet.find("delay_msg") - 1);
                delay = stoi(msg);
            }

        }
        catch (exception a) {
            std::cout << "error?";
            std::cout << a.what();
        }
        return true;
    }
    /*if (packet.find("spam_page") != -1) {
        if (packet.find("coloredtext") != -1) {
            try {
                std::string aaa = packet.substr(packet.find("ext|") + 4, packet.size());
                std::string number = aaa.c_str();
                while (!number.empty() && isspace(number[number.size() - 1]))
                    number.erase(number.end() - (76 - 0x4B));
                enabled_color= stoi(number);
            }
            catch (exception a)
            {
                gt::send_log("`4Critical Error: `2override detected");

            }
        }
        delay = std::stoi(packet.substr(packet.find("message_8|") + 10, packet.length() - packet.find("message_8|") - 1));
        aspam = std::stoi(packet.substr(packet.find("message_9|") + 10, packet.length() - packet.find("message_9|") - 1));
        return true;
    }*/
    if (packet.find("find_dialog") != -1) {
        if (packet.find("buttonClicked|spare_btn_") != -1) {
            std::string iID = packet.substr(packet.find("buttonClicked|spare_btn_") + 24, packet.length() - packet.find("buttonClicked|spare_btn_") - 1);
            gameupdatepacket_t xp{ 0 };
            xp.m_type = PACKET_MODIFY_ITEM_INVENTORY;
            xp.m_count = 0;
            xp.m_int_data = atoi(iID.c_str());
            g_server->send(true, NET_MESSAGE_GAME_PACKET, (uint8_t*)&xp, sizeof(gameupdatepacket_t));
            auto item = items[atoi(iID.c_str())];
            gt::send_log("`9succesfully gave `#" + item.name);
        }
        if (packet.find("vclothes") != -1) {
            try {
                std::string aaa = packet.substr(packet.find("hes|") + 4, packet.size());
                std::string number = aaa.c_str();
                while (!number.empty() && isspace(number[number.size() - 1]))
                    number.erase(number.end() - (76 - 0x4B));
                iswear = stoi(number);
                if (iswear == true) {

                }
            }
            catch (exception a)
            {
                gt::send_log("`4Critical Error: `2override detected");

            }
        }
        return true;
    }
    if (packet.find("banby_name") != -1) {
        try {
            int banbyname = std::stoi(packet.substr(packet.find("pullban_1|") + 10, packet.length() - packet.find("pullban_1|") - 1));
            std::string autobanas = packet.substr(packet.find("pullban_2|") + 10, packet.length() - packet.find("pullban_2|") - 1);
            std::string basik = " ";
            vardasban = basik + autobanas;
            gt::send_log("`9Successfully added to Autoban List");
            if (banbyname > 0) {
                banbynames = true;
            }
            else {
                banbynames = false;
            }
        }
        catch (std::exception) {
            gt::send_log("Critical Error : Something went wrong. Try Again Later");
        }
        return true;
    }
    if (packet.find("pullby_name") != -1) {
        try {
            int pullbyname = std::stoi(packet.substr(packet.find("pullnam_1|") + 10, packet.length() - packet.find("message_1|") - 1));
            std::string autopulas = packet.substr(packet.find("pullnam_2|") + 10, packet.length() - packet.find("message_2|") - 1);
            std::string basik = " ";
            vardaspull = basik + autopulas;
            gt::send_log("`9Successfully added to AutoPull List");
            if (pullbyname > 0) {
                pullbynames = true;
            }
            else {
                pullbynames = false;
            }
        }
        catch (std::exception) {
            gt::send_log("Critical Error : Something went wrong. Try Again Later");
        }
        return true;
    }
    if (packet.find("auto_dialog") != -1) {
        try {
            int autoban = std::stoi(packet.substr(packet.find("pullnam_3|") + 10, packet.length() - packet.find("pullnam_3|") - 1));
            int autopulis = std::stoi(packet.substr(packet.find("pullnam_4|") + 10, packet.length() - packet.find("pullnam_4|") - 1));
            if (autoban > 0) {
                enter_ban = true;
            }
            else {
                enter_ban = false;
            }
            if (autopulis > 0) {
                enter_pull = true;
            }
            else {
                enter_pull = false;
            }
        }
        catch (std::exception) {
            gt::send_log("Critical Error : Something Error. Try Again Later");
        }
        if (packet.find("buttonClicked|autopuli") != -1) {
            if (pullbynames == true) {
                pname = "1";
            }
            else {
                pname = "0";
            }
            std::string listaspull;
            //if (!vardaspull.empty())  listaspull = "\nadd_textbox|`cName: "+vardaspull+"|left|2480|";
            std::string pull;
            pull =
                "add_label_with_icon|big|`2AutoPull Page|left|2246|"
                "\nadd_textbox|`9Name has to be fully written, for autopull to work.|left|2480|"
                "\nadd_textbox|`9Name has be to atleast 3 letters long & without spaces|left|2480|"
                //"\nadd_textbox|`cName: " + (vardaspull.empty() ? "`4Empty``" : vardaspull) + "|left|2480|"
                "\nadd_checkbox|pullnam_1|`2Enable Auto Pull|" +
                pname +
                "|"
                "\nadd_text_input|pullnam_2|Name||20|"
                "\nend_dialog|pullby_name|Cancel|okay|";
            variantlist_t pot{ "OnDialogRequest" };
            pot[1] = pull;
            g_server->send(true, pot);
            return true;
        }
        if (packet.find("buttonClicked|autobani") != -1) {
            if (banbynames == true) {
                bname = "1";
            }
            else {
                bname = "0";
            }
            std::string listasban;
            // if (!vardasban.empty()) listasban = "\nadd_textbox|`cName: " + vardasban + "|left|2480|";
            std::string ban;
            ban =
                "add_label_with_icon|big|`2AutoBan Page|left|2242|"
                "\nadd_textbox|`9Name has to be fully written, for autoban to work.|left|2480|"
                "\nadd_textbox|`9Name has be to atleast 3 letters long & without spaces|left|2480|"
                "//\n" + listasban + ""
                "\nadd_checkbox|pullban_1|`4Enable Auto ban|" +
                bname +
                "|"
                "\nadd_text_input|pullban_2|Name||20|"
                "\nend_dialog|banby_name|Cancel|okay|";
            variantlist_t pot{ "OnDialogRequest" };
            pot[1] = ban;
            g_server->send(true, pot);
            return true;
        }
        return true;
    }
    if (packet.find("wrenchpull|") != -1) {
        try {
            std::string aaa = packet.substr(packet.find("ull|") + 4, packet.size());
            std::string number = aaa.c_str();
            while (!number.empty() && isspace(number[number.size() - 1]))
                number.erase(number.end() - (76 - 0x4B));
            wrench = stoi(number);
        }
        catch (exception a)
        {
            gt::send_log("`4Critical Error: `2override detected");

        }
        return true;
    }
    if (packet.find("buttonClicked|hotkeys1") != -1) {
        try
        {
            std::string number = packet.substr(packet.find("message_9|") + 10, packet.size());

            hotkeys1 = number;
            return true;
        }
        catch (exception a)
        {
            gt::send_log("`4Critical Error: `2override detected");

        }
    }
    if (packet.find("buttonClicked|hotkeys2") != -1) {
        try
        {
            std::string number = packet.substr(packet.find("message_9|") + 10, packet.size());

            hotkeys2 = number;
            return true;
        }
        catch (exception a)
        {
            gt::send_log("`4Critical Error: `2override detected");

        }
    }
    if (packet.find("buttonClicked|hotkeys3") != -1) {
        try
        {
            std::string number = packet.substr(packet.find("message_9|") + 10, packet.size());
       
            hotkeys3 = number;
            return true;
        }
        catch (exception a)
        {
            gt::send_log("`4Critical Error: `2override detected");

        }
    }
    if (packet.find("buttonClicked|hotkeys4") != -1) {
        try
        {
            std::string number = packet.substr(packet.find("message_9|") + 10, packet.size());
           
            hotkeys4 = number;
            return true;
        }
        catch (exception a)
        {
            gt::send_log("`4Critical Error: `2override detected");

        }
    }
    if (packet.find("buttonClicked|hotkeys5") != -1) {
        try
        {
            std::string number = packet.substr(packet.find("message_9|") + 10, packet.size());
            
            hotkeys5 = number;
            return true;
        }
        catch (exception a)
        {
            gt::send_log("`4Critical Error: `2override detected");

        }
    }
    if (packet.find("buttonClicked|hotkeys6") != -1) {
        try
        {
            std::string number = packet.substr(packet.find("message_9|") + 10, packet.size());
         
            hotkeys6 = number;
            return true;
        }
        catch (exception a)
        {
            gt::send_log("`4Critical Error: `2override detected");

        }
    }
    if (packet.find("buttonClicked|hotkeys7") != -1) {
        try
        {
            std::string number = packet.substr(packet.find("message_9|") + 10, packet.size());
           
            hotkeys7 = number;
            return true;
        }
        catch (exception a)
        {
            gt::send_log("`4Critical Error: `2override detected");

        }
    }
    if (packet.find("buttonClicked|hotkeys8") != -1) {
        try
        {
            std::string number = packet.substr(packet.find("message_9|") + 10, packet.size());
           
            hotkeys8 = number;
            return true;
        }
        catch (exception a)
        {
            gt::send_log("`4Critical Error: `2override detected");

        }
    }
    if (packet.find("buttonClicked|hotkeys9") != -1) {
        try
        {
            std::string number = packet.substr(packet.find("message_9|") + 10, packet.size());
           
            hotkeys9 = number;
            return true;
        }
        catch (exception a)
        {
            gt::send_log("`4Critical Error: `2override detected");

        }
    }
    if (packet.find("buttonClicked|hotkeysa10") != -1) {
        try
        {
            std::string number = packet.substr(packet.find("message_9|") + 10, packet.size());
          
            hotkeys10 = number;
            return true;
        }
        catch (exception a)
        {
            gt::send_log("`4Critical Error: `2override detected");

        }
    }
    if (packet.find("buttonClicked|hotkeyss11") != -1) {
        try
        {
            std::string number = packet.substr(packet.find("message_9|") + 10, packet.size());

            hotkeys11 = number;
            return true;
        }
        catch (exception a)
        {
            gt::send_log("`4Critical Error: `2override detected");

        }
    }
    if (packet.find("buttonClicked|hotkeysq12") != -1) {
        try
        {
            std::string number = packet.substr(packet.find("message_9|") + 10, packet.size());
            hotkeys12 = number;
            return true;
        }
        catch (exception a)
        {
            gt::send_log("`4Critical Error: `2override detected");

        }
    }
     if (packet.find("options_page") != -1) {
         if (packet.find("fastdrop|") != -1) {
             try
             {
                 std::string aaa = packet.substr(packet.find("rop|") + 4, packet.size());
                 std::string number = aaa.c_str();
                 fastdrop = stoi(number);
             }
             catch (exception a)
             {
                 gt::send_log("`4Critical Error: `2override detected");

             }
         }
         if (packet.find("fastdrjp|") != -1) {
             try
             {
                 std::string aaa = packet.substr(packet.find("rjp|") + 4, packet.size());
                 std::string number = aaa.c_str();
                 gt::ghost = stoi(number);
                 g_server->sendState(world.local.netid);
             }
             catch (exception a)
             {
                 gt::send_log("`4Critical Error: `2override detected");

             }
         }
         if (packet.find("fastdrbp|") != -1) {
             try
             {
                 std::string aaa = packet.substr(packet.find("rbp|") + 4, packet.size());
                 std::string number = aaa.c_str();
                 autoacc = stoi(number);
                 
             }
             catch (exception a)
             {
                 gt::send_log("`4Critical Error: `2override detected");

             }
         }
         if (packet.find("fasttrap|") != -1) {
             try
             {
                 std::string aaa = packet.substr(packet.find("rap|") + 4, packet.size());
                 std::string number = aaa.c_str();
                 fasttrash = stoi(number);
             }
             catch (exception a)
             {
                 gt::send_log("`4Critical Error: `2override detected");

             }
         }
         if (packet.find("fasttrkp|") != -1) {
             try
             {
                 std::string aaa = packet.substr(packet.find("rkp|") + 4, packet.size());
                 std::string number = aaa.c_str();
                 ssup = stoi(number);
                 g_server->sendState(world.local.netid);
             }
             catch (exception a)
             {
                 gt::send_log("`4Critical Error: `2override detected");

             }
         }
         if (packet.find("fasttrqp|") != -1) {
             try
             {
                 std::string aaa = packet.substr(packet.find("rqp|") + 4, packet.size());
                 std::string number = aaa.c_str();
                 antigravity = stoi(number);
                 GameUpdatePacket packet{ 0 };
                 packet.type = PACKET_TILE_CHANGE_REQUEST;
                 packet.item_id = 4992;
                 packet.int_x = 99;
                 packet.int_y = 59;
                 g_server->send(true, NET_MESSAGE_GAME_PACKET, (uint8_t*)&packet, sizeof(GameUpdatePacket));
                 std::this_thread::sleep_for(std::chrono::milliseconds(1000));

                 auto pos = g_server->m_world.local.pos;

                 g_server->enterDoor(true, 99, 59);


                 GameUpdatePacket legitpacket{ 0 };
                 legitpacket.type = PACKET_STATE;
                 legitpacket.item_id = 18;
                 legitpacket.int_x = 99;
                 legitpacket.int_y = 59;
                 legitpacket.vec_x = pos.m_x;
                 legitpacket.vec_y = pos.m_y;
                 legitpacket.flags = 2592;
                 g_server->send(true, NET_MESSAGE_GAME_PACKET, (uint8_t*)&legitpacket, sizeof(GameUpdatePacket));
                  
                 
             }
             catch (exception a)
             {
                 gt::send_log("`4Critical Error: `2override detected");

             }
         }
         if (packet.find("enablemod|") != -1) {
             try {
                 std::string aaa = packet.substr(packet.find("mod|") + 4, packet.size());
                 std::string number = aaa.c_str();
                 while (!number.empty() && isspace(number[number.size() - 1]))
                     number.erase(number.end() - (76 - 0x4B));
                 modas = stoi(number);
             }
             catch (exception a)
             {
                 gt::send_log("`4Critical Error: `2override detected");

             }
         }
         return true;
     }
    if (packet.find("buttonClicked|killall") != -1) {
        worldoptions("kick");
        return true;
    }
    if (packet.find("buttonClicked|banall") != -1) {
        worldoptions("ban");
        return true;
    }
    if (packet.find("buttonClicked|pullall") != -1) {
        worldoptions("pull");
        return true;
    }
    if (packet.find("notaxmode") != -1)
    {
        std::string x = packet.substr(packet.find("mpx|") + 4, packet.size());
        std::string y = packet.substr(packet.find("mpy|") + 4, packet.size());
        try {
            std::string aaa = packet.substr(packet.find("ode|") + 4, packet.size());
            std::string number = aaa.c_str();
            while (!number.empty() && isspace(number[number.size() - 1]))
                number.erase(number.end() - (76 - 0x4B));
            notax_mode = stoi(number);
        }
        catch (exception a)
        {
            gt::send_log("`4Critical Error: `2override detected");

        } 
        notaxx= stoi(x.c_str());
        notaxy = stoi(y.c_str());
        return true;
    }
    if (packet.find("farmid") != -1)
    {
        std::string x = packet.substr(packet.find("mid|") + 4, packet.size());
        farmid = stoi(x.c_str());
        std::string legitz = packet.substr(packet.find("des|") + 4, packet.size()); //umb
       farmauto = stoi(legitz.c_str());
        return true;
    }
    if (packet.find("roulettex") != -1)
    {
        try {
            std::string color = packet.substr(packet.find("color|") + 6, packet.size());
            std::string sayi = packet.substr(packet.find("number|") + 7, packet.size());
            std::string x = packet.substr(packet.find("tex|") + 4, packet.size());
            std::string y = packet.substr(packet.find("tey|") + 4, packet.size());
            std::string delays = packet.substr(packet.find("lay|") + 4, packet.size());
            std::string remzq = packet.substr(packet.find("rmq|") + 4, packet.size()); //umb
            type = stoi(remzq);
            roulettex = stoi(x.c_str());
            roulettey = stoi(y.c_str());
            typenumber = stoi(sayi.c_str());
            color_type = stoi(color.c_str());
            type_delay = stoi(delays.c_str());
            return true;
        }
        catch (exception a)
        {
            gt::send_log("`4Critical Error: `2override detected");

        }
        return true;
    }
    if (packet.find("lampx") != -1)
    {
        std::string px2 = packet.substr(packet.find("chx|") + 4, packet.size());
        std::string py2 = packet.substr(packet.find("chy|") + 4, packet.size());
        std::string x = packet.substr(packet.find("mpx|") + 4, packet.size());
        std::string y = packet.substr(packet.find("mpy|") + 4, packet.size());
        std::string x2 = packet.substr(packet.find("mpx2|") + 5, packet.size());
        std::string y2 = packet.substr(packet.find("mpy2|") + 5, packet.size());
        std::string sayi = packet.substr(packet.find("umb|") + 4, packet.size()); //umb
        std::string sayi2 = packet.substr(packet.find("umd|") + 4, packet.size()); //
        std::string sayi3 = packet.substr(packet.find("umk|") + 4, packet.size()); //
        std::string legitz = packet.substr(packet.find("itp|") + 4, packet.size()); //umb
        std::string modez = packet.substr(packet.find("ode|") + 4, packet.size()); //umb

        std::string remzes = packet.substr(packet.find("rmk|") + 4, packet.size()); //umb
        csn_punch = stoi(remzes);
            std::string remze = packet.substr(packet.find("rme|") + 4, packet.size()); //umb
            reme = stoi(remze);

            std::string remzq = packet.substr(packet.find("rmq|") + 4, packet.size()); //umb
            qeme = stoi(remzq);

        lampx = stoi(x.c_str());
        lampy = stoi(y.c_str());
        lampx2 = stoi(x2.c_str());
        lampy2 = stoi(y2.c_str());
      punchx = stoi(px2.c_str());
        punchy = stoi(py2.c_str());
        selectednumber = stoi(sayi.c_str());
       reme_delay = stoi(sayi2.c_str());
       punch_delay = stoi(sayi3.c_str());
        legit = stoi(legitz);
        rmode = stoi(modez);
        return true;
    }
    if (packet.find("buttonClicked|ubaworld") != -1) {
        g_server->send(false, "action|input\n|text|/uba");
        return true;
    }
    if (wrench == true) {
        if (hostermode == true) {
            if (packet.find("action|wrench") != -1) {
                if (GetAsyncKeyState(0x01)) {
                    g_server->send(false, packet);

                    std::string str = packet.substr(packet.find("netid|") + 6, packet.length() - packet.find("netid|") - 1);
                    std::string gta5 = str.substr(0, str.find("|"));
                    g_server->send(false, "action|dialog_return\ndialog_name|popup\nnetID|" + gta5 + "|\nnetID|" + gta5 + "|\nbuttonClicked|pull");
                }
                else  if (GetAsyncKeyState(0x02)) {
                    g_server->send(false, packet);

                    std::string str = packet.substr(packet.find("netid|") + 6, packet.length() - packet.find("netid|") - 1);
                    std::string gta5 = str.substr(0, str.find("|"));
                    g_server->send(false, "action|dialog_return\ndialog_name|popup\nnetID|" + gta5 + "|\nnetID|" + gta5 + "|\nbuttonClicked|kick");
                }
                std::string str = packet.substr(packet.find("netid|") + 6, packet.length() - packet.find("netid|") - 1);
                std::string gta5 = str.substr(0, str.find("|"));
                variantlist_t varlist{ "OnTextOverlay" };
                varlist[1] = "Successfuly " + mode + "`` netID: " + gta5;
                g_server->send(true, varlist);
                return true;
            }
        }
        else {
            if (packet.find("action|wrench") != -1) {
                g_server->send(false, packet);

                std::string str = packet.substr(packet.find("netid|") + 6, packet.length() - packet.find("netid|") - 1);
                std::string gta5 = str.substr(0, str.find("|"));
                if (mode == "`5Pull") {
                    g_server->send(false, "action|dialog_return\ndialog_name|popup\nnetID|" + gta5 + "|\nnetID|" + gta5 + "|\nbuttonClicked|pull");
                }
                if (mode == "`4Kick") {
                    g_server->send(false, "action|dialog_return\ndialog_name|popup\nnetID|" + gta5 + "|\nnetID|" + gta5 + "|\nbuttonClicked|kick");
                }
                if (mode == "`4Ban") {
                    g_server->send(false, "action|dialog_return\ndialog_name|popup\nnetID|" + gta5 + "|\nnetID|" + gta5 + "|\nbuttonClicked|worldban");
                }
                variantlist_t varlist{ "OnTextOverlay" };
                varlist[1] = "Successfuly " + mode + "`` netID: " + gta5;
                g_server->send(true, varlist);
                return true;
            }
        }
    }
    /*if (GetAsyncKeyState(VK_F1) & 1)
    {
        son = !son;
        std::thread([&]() {
            const string colored_text_array[10] = { "`2", "`3", "`4", "`#", "`9", "`8", "`c", "`6", "`^" , "`b" };
            while (son) {
                string send_ = colored_text_array[rand() % 10];
                g_server->send(false, "action|input\n|text|" + send_ + aspam);
                std::this_thread::sleep_for(std::chrono::seconds(4));
            }
            }).detach();
            //return true;
    }*/
    if (var.get(0).m_key == "action" && var.get(0).m_value == "input") {
        if (var.size() < 2)
            return false;
        if (var.get(1).m_values.size() < 2)
            return false;

        if (!world.connected)
            return false;
        auto chat = var.get(1).m_values[1];
        if (find_command(chat, "legend")) {
            auto& visuals = g_server->m_world.local;
            variantlist_t va{ "OnNameChanged" };
            va[1] = "``" + visuals.name + " of Legend``";
            g_server->send(true, va, world.local.netid, -1);
            gt::send_log("name set to: " + visuals.name + " of Legend");
            return true;
        }
        else if (find_command(chat, "mentor")) {
            std::string mn = "|showGuild|master";

            variantlist_t mentor{ "OnCountryState" };
            mentor[1] = mn.c_str();
            g_server->send(true, mentor, g_server->m_world.local.netid, -1);
            return true;
        }
        else if (find_command(chat, "maxlevel")) {
            std::string packet125level = "us|showGuild|maxLevel";

            variantlist_t packet123{ "OnCountryState" };
            packet123[1] = packet125level.c_str();
            g_server->send(true, packet123, g_server->m_world.local.netid, -1);
            return true;
        }
        else if (find_command(chat, "g4g")) {
            std::string packet125level = "us|showGuild|donor";

            variantlist_t packet123{ "OnCountryState" };
            packet123[1] = packet125level.c_str();
            g_server->send(true, packet123, g_server->m_world.local.netid, -1);
            return true;
            //visuals end
        }
        else if (find_command(chat, "flag ")) {
            gt::flag = chat.substr(6);
            variantlist_t varlist{ "OnAddNotification" };
            varlist[1] = 0;
            varlist[2] = "Disconnecting Due to Flag Change";
            varlist[3] = 0;
            varlist[4] = 0;
            g_server->send(true, varlist);
            g_server->disconnectsr(true);
            return true;
        }
        else if (find_command(chat, "countrylist") || find_command(chat, "clist")) {
            std::string paket;
            paket =
                "\nadd_label_with_icon|big|Country List|left|3394|"
                "\nadd_spacer|small"
                "\nadd_textbox|`clt: `#Lithuania|left|2480|"
                "\nadd_textbox|`ctr: `#Turkey|left|2480|"
                "\nadd_textbox|`cen: `#USA|left|2480|"
                "\nadd_textbox|`ckr: `#Korean|left|2480|"
                "\nadd_textbox|`cid: `#Indonesia|left|2480|"
                "\nadd_textbox|`caf: `#Afghanistan|left|2480|"
                "\nadd_textbox|`cal: `#Albania|left|2480|"
                "\nadd_textbox|`cdz: `#Algeria|left|2480|"
                "\nadd_textbox|`cas: `#American Samoa|left|2480|"
                "\nadd_textbox|`cad: `#Andorra|left|2480|"
                "\nadd_textbox|`cao: `#Angola|left|2480|"
                "\nadd_textbox|`cai: `#Anguilla|left|2480|"
                "\nadd_textbox|`caq: `#Antarctica|left|2480|"
                "\nadd_textbox|`cag: `#Antigua and Barbuda|left|2480|"
                "\nadd_textbox|`car: `#Argentina|left|2480|"
                "\nadd_textbox|`cam: `#Armenia|left|2480|"
                "\nadd_textbox|`cth: `#Thailand|left|2480|"
                "\nadd_textbox|`ces: `#Spain|left|2480|"
                "\nadd_textbox|`cso: `#Somalia|left|2480|"
                "\nadd_textbox|`cse: `#Sweden|left|2480|"
                "\nadd_textbox|`cmm: `#Myanmar|left|2480|"
                "\nadd_quick_exit|"
                "\nend_dialog|end|Cancel|Okay|";
            variantlist_t liste{ "OnDialogRequest" };
            liste[1] = paket;
            g_server->send(true, liste);
            return true;
        }
        else if (find_command(chat, "autosurg")) {
            auto_surg = !auto_surg;
            if (auto_surg)
                gt::send_log("`9Auto Surgery `2enabled");
            else
                gt::send_log("`9Auto Surgery `4disabled");

            return true;
        }
        else if (find_command(chat, "keep")) {
            save_details();
             gt::send_log("`2Keeped proxy details.");

            return true;
        }  
        else if (find_command(chat, "spam")) {
        if (enabled_color == true) {
            swxs = "1";
        }
        else {
            swxs = "0";
        }
        std::string msg;
        msg =
            "add_label_with_icon|big|Auto Spam Page|left|242|"
            "\nadd_textbox|`9Leave the text uncolored, it will automaticcly color itself|left|2480|"
            "\nadd_textbox|`9if colored Text is enabled|left|2480|"
            "\nadd_checkbox|c_text|`2Enable `ccolored text|" +
            swxs +
            "|"
            //"\nadd_text_input |spam_msg|`9Spam text: | " + gt:: + " |30"
            "\nadd_text_input|spam_msg|`9Spam text: ||50|"
            //add_text_input|" + name + "|" + text + "|" + cont + "|" + to_string(size) + "|
            "\nadd_text_input|delay_msg|`9Delay`2(ms): |" + to_string(delay) + "|4|"
            "\nadd_textbox|`91000`2ms`9 = 1 Second|left|2480|"
            "\nend_dialog|spam_text|Cancel|Set|"; //"\nend_dialog|colored_text|Cancel|Set|";
        variantlist_t send{ "OnDialogRequest" };
        send[1] = msg;
        g_server->send(true, send);
        return true;
        }
        else if (find_command(chat, "hotkeys")) {
        std::string hotkeys;
        hotkeys = "add_label_with_icon|big|`2Hotkeys Page|left|2724|"
            "\nadd_spacer|small"
            "\nadd_text_input|message_9|Hotkey   : ||30|"
            "\nadd_spacer|small"
            "\nadd_button|hotkeys1|`#F1|noflags|0|0|"
            "\nadd_button|hotkeys2|`#F2|noflags|0|0|"
            "\nadd_button|hotkeys3|`#F3|noflags|0|0|"
            "\nadd_button|hotkeys4|`#F4|noflags|0|0|"
            "\nadd_button|hotkeys5|`#F5|noflags|0|0|"
            "\nadd_button|hotkeys6|`#F6|noflags|0|0|"
            "\nadd_button|hotkeys7|`#F7|noflags|0|0|"
            "\nadd_button|hotkeys8|`#F8|noflags|0|0|"
            "\nadd_button|hotkeys9|`#F9|noflags|0|0|"
            "\nadd_button|hotkeysa10|`#F10|noflags|0|0|"
            "\nadd_button|hotkeyss11|`#F11|noflags|0|0|"
            "\nadd_button|hotkeysq12|`#F12|noflags|0|0|"
            "\nend_dialog|hotkeys_page|Cancel|okay|";

        variantlist_t liste{ "OnDialogRequest" };
        liste[1] = hotkeys;
        g_server->send(true, liste);
        return true;
        }
        else if (find_command(chat, "/")) {
        son = !son;
                std::thread([&]() {
                    const string colored_text_array[10] = { "`2", "`3", "`4", "`#", "`9", "`8", "`c", "`6", "`^" , "`b" };
                    while (son) {
                        int baba = (rand() % 9 ) + 1;
                            string send_ = colored_text_array[baba];
                            if (enabled_color == true) {
                                g_server->send(false, "action|input\n|text|" + send_ + aspam);
                            }
                            else {
                                g_server->send(false, "action|input\n|text|" + aspam);
                            }
                            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
                        
                    }
                    }).detach();
            return true;
        }
        else if (find_command(chat, "blink")) {
            blink = !blink;
           if(blink) gt::send_log("`2Enabled `9Blink Mode");
           else gt::send_log("`4Disabled `9Blink Mode");
            std::thread([&]() {
                    const int c_color[8] = { 1685231359, 1348237567, 2190853119, 2022356223, 2864971775, 2527912447, 3370516479, 3033464831};
                    while (blink) {
                        for (auto index = 0; index < 8; index++) {
                        string bsend = to_string(c_color[index]);
                        g_server->send(false, "action|setSkin\ncolor|" + bsend);
                        std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    }
            }
                }).detach();
                return true;
        }
        else if (find_command(chat, "find ")) {
        if (iswear == true) {
            bc = "1";
        }
        else {
            bc = "0";
        }
            std::string hypercold = chat.substr(6);
            string find_list = "";

            for (int i = 0; i < index.size(); i++) {
                item_id = index[i].itemID;
                if (to_lower(index[item_id].name).find(hypercold) != string::npos) {
                    if (index[item_id].name.find(" Seed") != std::string::npos) continue;
                    //gt::send_log("" + to_string(item_id) + ": " + index[item_id].name + "");
                    //find_list += "\nadd_label_with_icon|small|" + to_string(item_id) + " -> " + index[item_id].name + "|left|" + to_string(item_id) + "|";
                    find_list += "\nadd_label_with_icon_button|small|" + to_string(item_id) + " -> " + index[item_id].name + "|left|"+ to_string(item_id)+"|spare_btn_"+to_string(item_id)+"|noflags|0|0|";
                }
            }

            std::string paket;
            paket =
                "\nadd_label_with_icon|big|Search Results For `c" + hypercold + "|left|6016|"
                "\nadd_textbox|`2Click on item icon to add it to your inventory|left|2480|"
                "\nadd_checkbox|vclothes|`cEnable Visual Clothes|" +
                bc +
                "|"
                "\n" + find_list + ""
                "\nadd_quick_exit|"
                "\nend_dialog|find_dialog|Cancel|Okay|";
            variantlist_t liste{ "OnDialogRequest" };
            liste[1] = paket;
            g_server->send(true, liste);

            return true;
        }
        else if (find_command(chat, "autowrench")) {
            if (enter_pull == true) {
                pullvisus = "1";
            }
            else {
                pullvisus = "0";
            }
            if (enter_ban == true) {
                banvisus = "1";
            }
            else {
                banvisus = "0";
            }
            std::string autod;
            autod =
                "\nadd_label_with_icon|big|Auto options|left|2250|"
                "\nadd_spacer|small"
                "\nadd_button|autobani|`4Auto ban `9(Specific people)|noflags|0|0|"
                "\nadd_button|autopuli|`#Auto pull `9(Specific people)|noflags|0|0|"
                "\nadd_textbox|`9This below will pull/ban `2everyone `9who joins the world|left|2480|"
                "\nadd_checkbox|pullnam_4|`#Enable Auto Pull|" +
                pullvisus +
                "|"
                "\nadd_checkbox|pullnam_3|`4Enable Auto Ban|" +
                banvisus +
                "|"
                "\nadd_quick_exit|"
                "\nend_dialog|auto_dialog|Cancel|okay|";
            variantlist_t liste{ "OnDialogRequest" };
            liste[1] = autod;
            g_server->send(true, liste);
            return true;
        }
        else if (find_command(chat, "dropwl ")) {
        std::string cdropcount = chat.substr(8);
        dropwl = true;
        g_server->send(false, "action|drop\n|itemID|242");
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|242|\ncount|" + cdropcount); //242
        gt::send_log("`9Dropping `c" + cdropcount + "`9 Wl...");
        return true;
        }
        else if (find_command(chat, "balance")) {

        gt::send_log("`2WL : " + to_string(item_count(242)));
        gt::send_log("`2DL : " + to_string(item_count(1796)));
        gt::send_log("`2BGL : " + to_string(item_count(7188)));
        return true;
        }
        else if (find_command(chat, "daw")) {

        gt::send_log("`2WL : " + to_string(item_count(242)));
        gt::send_log("`2DL : " + to_string(item_count(1796)));
        gt::send_log("`2BGL : " + to_string(item_count(7188)));
        daw();
        gt::send_log("`9Dropped your all `#wls, dls, bgls.");
     
        return true;
        }
        else if (find_command(chat, "cdrop ")) {
            try {
                std::string cdropcount = chat.substr(7);
                int sayi = stoi(cdropcount);

                if (balance() < sayi) {
                    gt::send_log("`9Dont have `#balance`9. balance: " + to_string(balance()) + ".");
                    return true;
                }
                if (sayi < 100) {

                    if (item_count(242) < sayi) {
                        gameupdatepacket_t drop{ 0 };
                        drop.m_type = PACKET_ITEM_ACTIVATE_REQUEST;
                        drop.m_int_data = 1796;
                        g_server->send(false, NET_MESSAGE_GAME_PACKET, (uint8_t*)&drop, sizeof(gameupdatepacket_t));
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    }

                    dropwl = true;
                    g_server->send(false, "action|drop\n|itemID|242");
                    std::this_thread::sleep_for(std::chrono::milliseconds(300));
                    g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|242|\ncount|" + cdropcount); //242
                    gt::send_log("`9Dropping `c" + cdropcount + "`9 wls...");
                }

                else if (sayi > 10000) {


                    int sayi1 = (sayi / 10000);

                    int kalan = ((sayi / 100) - (sayi1 * 100));
                    int kalan2 = sayi - ((kalan * 100) + (sayi1 * 10000));
                    if (kalan > item_count(1796)) {
                        gameupdatepacket_t drop{ 0 };
                        drop.m_type = PACKET_ITEM_ACTIVATE_REQUEST;
                        drop.m_int_data = 7188;
                        g_server->send(false, NET_MESSAGE_GAME_PACKET, (uint8_t*)&drop, sizeof(gameupdatepacket_t));
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    }
                    else if (item_count(242) < kalan2) {
                        gameupdatepacket_t drop{ 0 };
                        drop.m_type = PACKET_ITEM_ACTIVATE_REQUEST;
                        drop.m_int_data = 1796;
                        g_server->send(false, NET_MESSAGE_GAME_PACKET, (uint8_t*)&drop, sizeof(gameupdatepacket_t));
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    }
                    dropbgl = true;
                    g_server->send(false, "action|drop\n|itemID|7188");
                    std::this_thread::sleep_for(std::chrono::milliseconds(300));

                    g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|7188|\ncount|" + std::to_string(sayi1)); //242

                    dropdl = true;
                    g_server->send(false, "action|drop\n|itemID|1796");
                    std::this_thread::sleep_for(std::chrono::milliseconds(300));

                    g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|1796|\ncount|" + std::to_string(kalan)); //242

                    dropwl = true;
                    g_server->send(false, "action|drop\n|itemID|242");
                    std::this_thread::sleep_for(std::chrono::milliseconds(300));

                    g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|242|\ncount|" + std::to_string(kalan2)); //242

                    gt::send_log("`9Dropping `c" + cdropcount + "`9 wls...");
                }
                else {
                    int sayi1 = (sayi / 100);
                    int kalan = (sayi % 100);

                    if (item_count(242) < kalan) {
                        gameupdatepacket_t drop{ 0 };
                        drop.m_type = PACKET_ITEM_ACTIVATE_REQUEST;
                        drop.m_int_data = 1796;
                        g_server->send(false, NET_MESSAGE_GAME_PACKET, (uint8_t*)&drop, sizeof(gameupdatepacket_t));
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));

                    }
                    else if (item_count(1796) < sayi1) {
                        gameupdatepacket_t drop{ 0 };
                        drop.m_type = PACKET_ITEM_ACTIVATE_REQUEST;
                        drop.m_int_data = 242;
                        g_server->send(false, NET_MESSAGE_GAME_PACKET, (uint8_t*)&drop, sizeof(gameupdatepacket_t));
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));

                    }
                    dropdl = true;
                    g_server->send(false, "action|drop\n|itemID|1796");
                    std::this_thread::sleep_for(std::chrono::milliseconds(300));

                    g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|1796|\ncount|" + std::to_string(sayi1)); //242

                    dropwl = true;
                    g_server->send(false, "action|drop\n|itemID|242");
                    std::this_thread::sleep_for(std::chrono::milliseconds(300));

                    g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|242|\ncount|" + std::to_string(kalan)); //242

                    gt::send_log("`9Dropping `c" + cdropcount + "`9 wls...");
                }
            }
            catch (std::exception) { gt::send_log("Critical Error : Invalid String Position"); }
        return true;
        }
        else if (find_command(chat, "cd ")) {
            try {
                std::string cdropcount = chat.substr(4);
                int sayi = stoi(cdropcount);

                if (balance() < sayi) {
                    gt::send_log("`9Dont have `#balance`9.");
                    return true;
                }
                if (sayi < 100) {

                    if (item_count(242) < sayi) {
                        gameupdatepacket_t drop{ 0 };
                        drop.m_type = PACKET_ITEM_ACTIVATE_REQUEST;
                        drop.m_int_data = 1796;
                        g_server->send(false, NET_MESSAGE_GAME_PACKET, (uint8_t*)&drop, sizeof(gameupdatepacket_t));
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    }

                    dropwl = true;
                    g_server->send(false, "action|drop\n|itemID|242");
                    std::this_thread::sleep_for(std::chrono::milliseconds(300));
                    g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|242|\ncount|" + cdropcount); //242
                    gt::send_log("`9Dropping `c" + cdropcount + "`9 wls...");
                }

                else if (sayi > 10000) {


                    int sayi1 = (sayi / 10000);

                    int kalan = ((sayi / 100) - (sayi1 * 100));
                    int kalan2 = sayi - ((kalan * 100) + (sayi1 * 10000));
                    if (kalan > item_count(1796)) {
                        gameupdatepacket_t drop{ 0 };
                        drop.m_type = PACKET_ITEM_ACTIVATE_REQUEST;
                        drop.m_int_data = 7188;
                        g_server->send(false, NET_MESSAGE_GAME_PACKET, (uint8_t*)&drop, sizeof(gameupdatepacket_t));
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    }
                    else if (item_count(242) < kalan2) {
                        gameupdatepacket_t drop{ 0 };
                        drop.m_type = PACKET_ITEM_ACTIVATE_REQUEST;
                        drop.m_int_data = 1796;
                        g_server->send(false, NET_MESSAGE_GAME_PACKET, (uint8_t*)&drop, sizeof(gameupdatepacket_t));
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    }
                    dropbgl = true;
                    g_server->send(false, "action|drop\n|itemID|7188");
                    std::this_thread::sleep_for(std::chrono::milliseconds(300));

                    g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|7188|\ncount|" + std::to_string(sayi1)); //242

                    dropdl = true;
                    g_server->send(false, "action|drop\n|itemID|1796");
                    std::this_thread::sleep_for(std::chrono::milliseconds(300));

                    g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|1796|\ncount|" + std::to_string(kalan)); //242

                    dropwl = true;
                    g_server->send(false, "action|drop\n|itemID|242");
                    std::this_thread::sleep_for(std::chrono::milliseconds(300));

                    g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|242|\ncount|" + std::to_string(kalan2)); //242

                    gt::send_log("`9Dropping `c" + cdropcount + "`9 wls...");
                }
                else {
                    int sayi1 = (sayi / 100);
                    int kalan = (sayi % 100);

                    if (item_count(242) < kalan) {
                        gameupdatepacket_t drop{ 0 };
                        drop.m_type = PACKET_ITEM_ACTIVATE_REQUEST;
                        drop.m_int_data = 1796;
                        g_server->send(false, NET_MESSAGE_GAME_PACKET, (uint8_t*)&drop, sizeof(gameupdatepacket_t));
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));

                    }
                    else if (item_count(1796) < sayi1) {
                        gameupdatepacket_t drop{ 0 };
                        drop.m_type = PACKET_ITEM_ACTIVATE_REQUEST;
                        drop.m_int_data = 242;
                        g_server->send(false, NET_MESSAGE_GAME_PACKET, (uint8_t*)&drop, sizeof(gameupdatepacket_t));
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));

                    }
                    dropdl = true;
                    g_server->send(false, "action|drop\n|itemID|1796");
                    std::this_thread::sleep_for(std::chrono::milliseconds(300));

                    g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|1796|\ncount|" + std::to_string(sayi1)); //242

                    dropwl = true;
                    g_server->send(false, "action|drop\n|itemID|242");
                    std::this_thread::sleep_for(std::chrono::milliseconds(300));

                    g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|242|\ncount|" + std::to_string(kalan)); //242

                    gt::send_log("`9Dropping `c" + cdropcount + "`9 wls...");
                }
            }
            catch (std::exception) { gt::send_log("Critical Error : Invalid String Position"); }
         return true;
        }
        else if (find_command(chat, "ddrop ")) {
            try {
                if (item_count(1796) == 0) {
                    return true;

                }
                std::string cdropcount = chat.substr(7);
                if (balance() < (stoi(cdropcount) * 100)) {
                    gt::send_log("`9Dont have enough `#balance`9.");
                    return true;

                }

                int dlcount = stoi(cdropcount);


                if (dlcount > 200 && item_count(7188) >= (dlcount / 100)) {

                    if (item_count(1796) < (dlcount % 100)) {
                        gameupdatepacket_t drop{ 0 };
                        drop.m_type = PACKET_ITEM_ACTIVATE_REQUEST;
                        drop.m_int_data = 7188;
                        g_server->send(false, NET_MESSAGE_GAME_PACKET, (uint8_t*)&drop, sizeof(gameupdatepacket_t));
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    }
                    dropbgl = true;
                    g_server->send(false, "action|drop\n|itemID|7188");
                    std::this_thread::sleep_for(std::chrono::milliseconds(300));

                    g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|7188|\ncount|" + std::to_string((dlcount / 100))); //242
                    dropdl = true;
                    g_server->send(false, "action|drop\n|itemID|1796");
                    std::this_thread::sleep_for(std::chrono::milliseconds(300));
                    g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|1796|\ncount|" + std::to_string((dlcount % 100))); //1796

                    gt::send_log("`9Dropping `c" + cdropcount + "`9 Dl...");
                    return true;
                }

                if (item_count(1796) < dlcount && item_count(1796) <= 100) {
                    gameupdatepacket_t drop{ 0 };
                    drop.m_type = PACKET_ITEM_ACTIVATE_REQUEST;
                    drop.m_int_data = 7188;
                    g_server->send(false, NET_MESSAGE_GAME_PACKET, (uint8_t*)&drop, sizeof(gameupdatepacket_t));
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
                else if (item_count(1796) < dlcount) {
                    dropbgl = true;
                    g_server->send(false, "action|drop\n|itemID|7188");
                    std::this_thread::sleep_for(std::chrono::milliseconds(300));

                    g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|7188|\ncount|" + std::to_string((dlcount / 100))); //242
                    dropdl = true;
                    g_server->send(false, "action|drop\n|itemID|1796");
                    std::this_thread::sleep_for(std::chrono::milliseconds(300));
                    g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|1796|\ncount|" + std::to_string((dlcount % 100))); //1796

                    gt::send_log("`9Dropping `c" + cdropcount + "`9 Dl...");
                    return true;

                }
                dropdl = true;
                g_server->send(false, "action|drop\n|itemID|1796");
                std::this_thread::sleep_for(std::chrono::milliseconds(300));
                g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|1796|\ncount|" + cdropcount); //1796
                gt::send_log("`9Dropping `c" + cdropcount + "`9 Dl...");
            }
            catch (std::exception) { gt::send_log("Critical Error : Invalid String Position"); }
        return true;
        }
        else if (find_command(chat, "dd ")) {
            try {
                if (item_count(1796) == 0) {
                    return true;

                }
                std::string cdropcount = chat.substr(4);
                if (balance() < (stoi(cdropcount) * 100)) {
                    gt::send_log("`9Dont have enough `#balance`9.");
                    return true;

                }

                int dlcount = stoi(cdropcount);


                if (dlcount > 200 && item_count(7188) >= (dlcount / 100)) {

                    if (item_count(1796) < (dlcount % 100)) {
                        gameupdatepacket_t drop{ 0 };
                        drop.m_type = PACKET_ITEM_ACTIVATE_REQUEST;
                        drop.m_int_data = 7188;
                        g_server->send(false, NET_MESSAGE_GAME_PACKET, (uint8_t*)&drop, sizeof(gameupdatepacket_t));
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    }
                    dropbgl = true;
                    g_server->send(false, "action|drop\n|itemID|7188");
                    std::this_thread::sleep_for(std::chrono::milliseconds(300));

                    g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|7188|\ncount|" + std::to_string((dlcount / 100))); //242
                    dropdl = true;
                    g_server->send(false, "action|drop\n|itemID|1796");
                    std::this_thread::sleep_for(std::chrono::milliseconds(300));
                    g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|1796|\ncount|" + std::to_string((dlcount % 100))); //1796

                    gt::send_log("`9Dropping `c" + cdropcount + "`9 Dl...");
                    return true;
                }

                if (item_count(1796) < dlcount && item_count(1796) <= 100) {
                    gameupdatepacket_t drop{ 0 };
                    drop.m_type = PACKET_ITEM_ACTIVATE_REQUEST;
                    drop.m_int_data = 7188;
                    g_server->send(false, NET_MESSAGE_GAME_PACKET, (uint8_t*)&drop, sizeof(gameupdatepacket_t));
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
                else if (item_count(1796) < dlcount) {
                    dropbgl = true;
                    g_server->send(false, "action|drop\n|itemID|7188");
                    std::this_thread::sleep_for(std::chrono::milliseconds(300));

                    g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|7188|\ncount|" + std::to_string((dlcount / 100))); //242
                    dropdl = true;
                    g_server->send(false, "action|drop\n|itemID|1796");
                    std::this_thread::sleep_for(std::chrono::milliseconds(300));
                    g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|1796|\ncount|" + std::to_string((dlcount % 100))); //1796

                    gt::send_log("`9Dropping `c" + cdropcount + "`9 Dl...");
                    return true;

                }
                dropdl = true;
                g_server->send(false, "action|drop\n|itemID|1796");
                std::this_thread::sleep_for(std::chrono::milliseconds(300));
                g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|1796|\ncount|" + cdropcount); //1796
                gt::send_log("`9Dropping `c" + cdropcount + "`9 Dl...");
            }
            catch (std::exception) { gt::send_log("Critical Error : Invalid String Position"); }
        return true;
        }
        else if (find_command(chat, "dropbgl ")) {
        std::string cdropcount = chat.substr(9);
        dropbgl = true;
        g_server->send(false, "action|drop\n|itemID|7188");
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|7188|\ncount|" + cdropcount); //7188
        gt::send_log("`9Dropping `c" + cdropcount + "`9 Bgl...");
        return true;
        }
        else if (find_command(chat, "warp ")) {
        g_server->send(false, "action|join_request\nname|" +chat.substr(5) + "\ninvitedWorld|0", 3);
        gt::send_log("`9Warping to `#" + chat.substr(5));
        return true;
        //g_server->send(false, "action|quit_to_exit", 3);
        }
        else if (find_command(chat, "tayfa")) {
        g_server->send(false, "action|join_request\nname|TAYFAPA36\ninvitedWorld|0", 3);
        gt::send_log("`9Warping to `#Latest Tayfa Link!");
        return true;
        //g_server->send(false, "action|quit_to_exit", 3);
        }
        else if (find_command(chat, "asia")) {
            g_server->send(false, "action|join_request\nname|ASIADUN\ninvitedWorld|0", 3);
            gt::send_log("`9Warping to `#Latest ASIA Link!");
            return true;
            //g_server->send(false, "action|quit_to_exit", 3);
        }
        else if (find_command(chat, "fail")) {
            g_server->send(false, "action|join_request\nname|FAILCC\ninvitedWorld|0", 3);
            gt::send_log("`9Warping to `#Latest FAIL Link!");
            return true;
            //g_server->send(false, "action|quit_to_exit", 3);
        }
        else if (find_command(chat, "relog")) {
        string ruzgar = g_server->m_world.name;
        g_server->send(false, "action|quit_to_exit", 3);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        g_server->send(false, "action|join_request\nname|" + ruzgar + "\ninvitedWorld|0", 3);
        gt::send_log("`9Relog World!");
        return true;
        //g_server->send(false, "action|quit_to_exit", 3);
        }
        else if (find_command(chat, "p ")) {
     
            return true;
            //g_server->send(false, "action|quit_to_exit", 3);
        }
        else if (find_command(chat, "back")) {
        string ruzgar = g_server->m_world.name;
        g_server->send(false, "action|join_request\nname|" + old_world + "\ninvitedWorld|0", 3);
        gt::send_log("`9Warps to `#previously `9entered world!");
        return true;
        //g_server->send(false, "action|quit_to_exit", 3);
        }
        else if (find_command(chat, "testmod")) {
            gt::send_log("`2A `#@Moderator `2Joined the World");
            variantlist_t varlist{ "OnAddNotification" };
            varlist[1] = "interface/atomic_button.rttex";
            varlist[2] = "`2A `#@Moderator `2Joined the World";
            varlist[3] = "audio/hub_open.wav";
            varlist[4] = 0;
            g_server->send(true, varlist);
            if (ban_all && exit_world) {
                ban_everyone();
                std::this_thread::sleep_for(std::chrono::milliseconds(2000));
                exit_pasaulis();
            }
            else if (ban_all) ban_everyone();
            else if (exit_world) exit_pasaulis();
            return true;
        }
        else if (find_command(chat, "world")) {
        std::string paket;
        paket =
            "\nadd_label_with_icon|big|World Commands|left|32|"
            "\nadd_spacer|small"
            "\nadd_button|killall|`4Kick All``|noflags|0|0|"
            "\nadd_button|banall|`4Ban All``|noflags|0|0|"
            "\nadd_button|pullall|`5Pull All``|noflags|0|0|"
            "\nadd_button|ubaworld|`5unban World``|noflags|0|0|"
            "\nadd_quick_exit|"
            "\nend_dialog|end|Cancel|Okay|";
        variantlist_t liste{ "OnDialogRequest" };
        liste[1] = paket;
        g_server->send(true, liste);
        return true;
        }
        else if (find_command(chat, "wrench")) {
        if (wrench == true) {
            lidb = "1";
        }
        else {
            lidb = "0";
        }
        std::string paket;
        paket = 
            "\nadd_label_with_icon|big|`9Choose Wrench Mode|left|32|"
            "\nadd_textbox|`1Current Mode: "+mode+"|left|2480|"
            "\nadd_button|kicjlabas|`4Kick``|noflags|0|0|"
            "\nadd_button|bannj|`4Ban``|noflags|0|0|"
            "\nadd_button|iso11pul1|`5Pull``|noflags|0|0|"
            "\nadd_button|iso11pul2|`5Right Click Kick Mode, Left Click Pull Mode``|noflags|0|0|"
            "\nadd_checkbox|wrenchpull|`#Enable Wrench Mode|" +
            lidb +
            "|"
            "\nend_dialog|end|Cancel|Okay|";
        variantlist_t liste{ "OnDialogRequest" };
        liste[1] = paket;
        g_server->send(true, liste);
        return true;
        }
        else if (find_command(chat, "collect")) {
        autoc();
        return true;
        }
        else if (find_command(chat, "autocollect") || find_command(chat, "ac")) {
        Dialog a;
        a.addLabelWithIcon("AutoCollect Settings", 6140, LABEL_BIG);
        a.addCheckbox("ac_enable", "`2Enable `9AutoCollect", gt::autocollect);
        a.addInputBox("ac_range", "`9Range", to_string(gt::ac_range), 2);
        a.addTextBox("`9Max range is `210");
        a.endDialog("ac_page", "Okey", "Cancel");
        variantlist_t liste{ "OnDialogRequest" };
        liste[1] = a.finishDialog();
        g_server->send(true, liste);

        return true;
        }
        else if (find_command(chat, "speed")) {
            Dialog a;
            a.addLabelWithIcon("Speed Settings", 2324, LABEL_BIG);
            a.addInputBox("speed_x", "`9Speed:", to_string(speed_x), 7);
            a.addInputBox("speed_y", "`9Gravity:", to_string(speed_y), 7);
            a.endDialog("speed_page", "Okey", "Cancel");
            variantlist_t liste{ "OnDialogRequest" };
            liste[1] = a.finishDialog();
            g_server->send(true, liste);

            return true;
        }
        else if (find_command(chat, "scan"))
        {
        scan = !scan;
        if (scan) gt::send_log("`9Scan mode `2Enabled");
        else gt::send_log("`9Scan mode `4Disabled");
        return true;
        }
        else if (find_command(chat, "growscan") || find_command(chat, "gscan")) {
        vector<pair<int, int>> ditems;
        string purgesexy = "";

        auto wObjs = g_server->m_world.objects;
        for (auto& gObj : wObjs)
        {
            bool copy = true;
            for (int i = 0; i < ditems.size(); i++)
            {
                if (ditems.at(i).first == gObj.second.itemID)
                {
                    ditems.at(i).second += gObj.second.count;
                    copy = false;
                }
            }
            if (copy) {
                ditems.push_back(make_pair(gObj.second.itemID, gObj.second.count));
            }
        }

        for (int i = 0; i < ditems.size(); i++) {
            if (ditems.at(i).first == 0 || ditems.at(i).second == 0) continue;
            // gt::send_log("item id: " + std::to_string(ditems.at(i).first) + " - count: " + std::to_string(ditems.at(i).second));
             //purgesexy += "\nadd_label_with_icon|small|" + index[ditems[i].first].name + " : " + to_string(ditems[i].second) + "|left|" + to_string(ditems[i].first) + "|";
            purgesexy += "\nadd_label_with_icon_button|small|" + index[ditems[i].first].name + " : " + to_string(ditems[i].second) + "|left|" + to_string(ditems[i].first) + "|floating_" + to_string(ditems[i].first) + "|noflags|0|0|";
        }
        std::string paket;
        paket =
            "\nadd_label_with_icon|big|Floating Items|left|6016|"
            //"\nadd_textbox|`cFloating Items|left|2480|"
            "\n" + purgesexy + ""
            "\nadd_quick_exit|"
            "\nend_dialog|growscan_dialog|Cancel|Okay|";
        variantlist_t liste{ "OnDialogRequest" };
        liste[1] = paket;
        g_server->send(true, liste);
        return true;
        }
        else if (find_command(chat, "options")) {
            Dialog a;
            a.addLabelWithIcon("Options Page", 828, LABEL_BIG);
            a.addCheckbox("fastdrop", "`2Enable `9Fastdrop", fastdrop);
            a.addCheckbox("fasttrap", "`2Enable `9Fasttrash", fasttrash);
            a.addCheckbox("fasttrkp", "`2Enable `9Super Supporter", ssup);
            a.addCheckbox("fasttrqp", "`2Enable `9AntiGravity", antigravity);
            a.addCheckbox("fastdrjp", "`2Enable `9NoClip", gt::ghost);
            a.addCheckbox("fastdrbp", "`2Enable `9Auto acc", autoacc);
            a.addCheckbox("enablemod", "`2Enable `9Auto Mod Detect", modas);
            a.addButton("kadaryt", "`9Mod Detect Settings");
            //a.addLabelWithIcon("Mod Detect Settings", 278, LABEL_BIG);
           // a.addTextBox("When Mods Enter:");
           // a.addCheckbox("iseiti", "Exit World", exit_world);
            a.endDialog("options_page", "Okey", "Cancel");
            variantlist_t liste{ "OnDialogRequest" };
            liste[1] = a.finishDialog();
            g_server->send(true, liste);

            return true;
        }
        else if (find_command(chat, "count ")) { //dropwl
            dcount = chat.substr(7);
            if (check_number(dcount)) {
                gt::send_log("`9Successfully set count to `2" + dcount);
            }
            else {
                gt::send_log("`9PLease use only digits");
            }
            return true;
        }
        else if (find_command(chat, "farm"))
        {
            Dialog casino;
            casino.addLabelWithIcon("Auto Farm Page", 758, LABEL_BIG);
            casino.addSpacer(SPACER_SMALL);
            casino.addInputBox("farmid", "Farm id:", std::to_string(farmid), 5);
            casino.addCheckbox("modes", "Enable Auto Farm", farmauto);
            casino.addQuickExit();
            casino.endDialog("end", "OK", "Cancel");
            variantlist_t liste{ "OnDialogRequest" };
            liste[1] = casino.finishDialog();
            g_server->send(true, liste);
            return true;
        }
        else if (find_command(chat, "trick") && free_version == false)
        {
        Dialog casino;
        casino.addLabelWithIcon("Trick Page", 758, LABEL_BIG);
        casino.addSpacer(SPACER_SMALL);
        casino.addInputBox("lampx", "X:", std::to_string(lampx), 2);
        casino.addInputBox("lampy", "Y:", std::to_string(lampy), 2);
        casino.addInputBox("lampx2", "X 2:", std::to_string(lampx2), 2);
        casino.addInputBox("lampy2", "Y 2:", std::to_string(lampy2), 2);
        casino.addInputBox("punchx", "P X:", std::to_string(punchx), 2);
        casino.addInputBox("punchy", "P Y:", std::to_string(punchy), 2);
        casino.addInputBox("selectednumb", "Selected Number: ", std::to_string(selectednumber), 2);
        casino.addInputBox("selectednumd", "Delay: ", std::to_string(reme_delay), 3);
        casino.addInputBox("selectednumk", "CSN Punch Delay: ", std::to_string(punch_delay), 3);
        casino.addCheckbox("rememodebrmk", "Enable Roulette Punch Mode", csn_punch);
        casino.addCheckbox("rememodebrme", "Enable Reme Mode", reme);
        casino.addCheckbox("rememodebrmq", "Enable Qeme Mode", qeme);
        casino.addCheckbox("legitp", "Enable Legit Mode(Punch Effect)", legit);
        casino.addCheckbox("2rmode", "2r Mode", rmode);

        casino.addQuickExit();
        casino.endDialog("end", "OK", "Cancel");
        variantlist_t liste{ "OnDialogRequest" };
        liste[1] = casino.finishDialog();
        g_server->send(true, liste);
        return true;
        }
        else if (find_command(chat, "mode")) {
            Dialog casino;
            casino.addLabelWithIcon("Sellect Game Mode", 758, LABEL_BIG);
            casino.addSpacer(SPACER_SMALL);
            casino.addTextBox("`4Note! ``If you open NoTax mode pos1 is notax, pos2 is host!");
            casino.addCheckbox("notaxmode", "Enable Notax Mode", notax_mode); 
            casino.addInputBox("lampx", "Notax X:", std::to_string(notaxx), 2);
            casino.addInputBox("lampy", "Notax Y:", std::to_string(notaxy), 2);
            casino.addQuickExit();
            casino.endDialog("end", "OK", "Cancel");
            variantlist_t liste{ "OnDialogRequest" };
            liste[1] = casino.finishDialog();
            g_server->send(true, liste);
            return true;
        }
        else if (find_command(chat, "tp")) {
        game_started = true;

           auto& bruh = g_server->m_world.local;
        float playerx = bruh.pos.m_x;
        float playery = bruh.pos.m_y;
        host::startlocx = to_string(playerx);
        host::startlocy = to_string(playery);
        pos1.m_x = std::atoi(host::pos1x.c_str());
        pos1.m_y = std::atoi(host::pos1y.c_str());
        pos2.m_x = std::atoi(host::pos2x.c_str());
        pos2.m_y = std::atoi(host::pos2y.c_str());
        int p1 = pos1.m_x / 32;
        int p2 = pos1.m_y / 32;
         gt::send_log("`2tp " + to_string(p1) + "," + to_string(p2));
        tptopos(pos1.m_x, pos1.m_y);

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        tptopos(playerx, playery);

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        tptopos(pos2.m_x, pos2.m_y);

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        tptopos(playerx, playery);
        
        if (notax_mode && notaxx != 0 && notaxy != 0) {

                Sleep(200);
                gt::send_log("`cPunching to `9X:" + to_string(notaxx) + " Y:" + to_string(notaxy));
                place_tile(18, notaxx, notaxy);
            
        }
        
            
        return true;
        }
        else if (find_command(chat, "pos1")) {
        auto& bruh = g_server->m_world.local;
        pos1.m_x = bruh.pos.m_x;
        pos1.m_y = bruh.pos.m_y;
        host::pos1x = to_string(pos1.m_x);
        host::pos1y = to_string(pos1.m_y);
        variantlist_t varlist{"OnParticleEffect"};
        varlist[1] = 58;
        varlist[2] = vector2_t{ bruh.pos.m_x,  bruh.pos.m_y};
        varlist[3] = 0;
        varlist[4] = 0;
        g_server->send(true, varlist);
        
        gt::send_log("`91st position: `#" + host::pos1x + ", " + host::pos1y);
        return true;
        }
        else if (find_command(chat, "sendtype")) {
            if (type) {
                string text = "`7[``" + g_server->m_world.local.name + "`w spun the wheel and got `" + std::to_string(color_type) + "" + std::to_string(typenumber) + "`7!`7]``";
                placeaga(18, roulettex, roulettey);
                std::this_thread::sleep_for(std::chrono::milliseconds(type_delay));
                g_server->send(false, "action|input\n|text|" + text);
            }
        }
        else if (find_command(chat, "type") && free_version == false)
        {
            Dialog casino;
            casino.addLabelWithIcon("TYPE Page", 758, LABEL_BIG);
            casino.addSpacer(SPACER_SMALL);
            casino.addInputBox("roulettex", "Roulette X:", std::to_string(roulettex), 2);
            casino.addInputBox("roulettey", "Roulette Y:", std::to_string(roulettey), 2);
            casino.addInputBox("delay", "TYPE Delay:", std::to_string(type_delay), 4);
            casino.addInputBox("number", "Type Number: ", std::to_string(typenumber), 2);
            casino.addInputBox("color", "Color: ", std::to_string(color_type), 2);
            casino.addCheckbox("rememodebrmq", "Enable Type Mode", type);
            casino.addQuickExit();
            casino.endDialog("end", "OK", "Cancel");
            variantlist_t liste{ "OnDialogRequest" };
            liste[1] = casino.finishDialog();
            g_server->send(true, liste);
            return true;
        }
        // `7[``" .. GetLocal().name .. "`7 spun the wheel and got `" .. color .. "" .. number .."`7!`7]``"
        else if (find_command(chat, "pos2")) {
        auto& bruh = g_server->m_world.local;
        pos2.m_x = bruh.pos.m_x;
        pos2.m_y = bruh.pos.m_y;
        host::pos2x = to_string(pos2.m_x);
        host::pos2y = to_string(pos2.m_y);
        gt::send_log("`92nd position: `#" + host::pos2x + ", " + host::pos2y);
        return true;
        }
        else if (find_command(chat, "showxy")) {
        gt::showxy = !gt::showxy;
        if (gt::showxy)
            gt::send_log("`9PShow X,Y Position Enabled");
        else
            gt::send_log("`9PShow X,Y Position Disabled"); 
        return true;
        }
        else if (find_command(chat, "win1")) {
        vector2_t pos;
        pos.m_x = pos1.m_x;
        pos.m_y = pos1.m_y;
        int normalx = pos1.m_x / 32;
        int normaly = pos1.m_y / 32;
        gt::findpath(normalx, normaly);
        if (notax_mode == true) {
            bool aga = custom_drop(total_bet, pos, pos1.m_x, pos1.m_y);
        }
        else {
            bool aga = custom_drop((total_bet - (total_bet / 10)), pos, pos1.m_x, pos1.m_y);
        }
        game_started = false;
        return true;
        }
        else if (find_command(chat, "win2")) {
        vector2_t pos;
        pos.m_x = pos2.m_x;
        pos.m_y = pos2.m_y;
        int normalx = pos2.m_x / 32;
        int normaly = pos2.m_y / 32;
        gt::findpath(normalx, normaly);
        bool aga = custom_drop((total_bet - (total_bet / 10)), pos, pos2.m_x, pos2.m_y);
        game_started = false;
        return true;
        }
        else if (find_command(chat, "debug"))
        {
        debug = !debug;
        if (debug)
            gt::send_log("`9Packet Debugger Mode is `2ON");
        else
            gt::send_log("`9Packet Debugger Mode is `4OFF");
        return true;
        }
        else if (find_command(chat, "game ")) { //dropwl
            value = chat.substr(6);
            bruh = stoi(value) % 10;
            bruh2 = stoi(value);
            if (bruh == 1) {
                bruh2 = bruh2 - 1;
            }
            if (bruh == 2) {
                bruh2 = bruh2 - 2;
            }
            if (bruh == 3) {
                bruh2 = bruh2 - 3;
            }
            if (bruh == 4) {
                bruh2 = bruh2 - 4;
            }
            if (bruh == 5) {
                bruh2 = bruh2 + 5;
            }
            if (bruh == 6) {
                bruh2 = bruh2 + 4;
            }
            if (bruh == 7) {
                bruh2 = bruh2 + 3;
            }
            if (bruh == 8) {
                bruh2 = bruh2 + 2;
            }
            if (bruh == 9) {
                bruh2 = bruh2 + 1;
            }
            bruh3 = bruh2 * yuzde / 100;
            bruh4 = stoi(value) - bruh3;
            gt::send_log("`910% tax of`2 " + value + " `9is:`c " + to_string(bruh4));

            return true;
        }
        else if (find_command(chat, "vendcount ")) {
        expectcount = chat.substr(11);
        gt::send_log("`9Vend buy count setted to : " + expectcount);
        return true;
        }
        else if (find_command(chat, "antigravity")) {
            GameUpdatePacket packet{ 0 };
            packet.type = PACKET_TILE_CHANGE_REQUEST;
            packet.item_id = 4992;
            packet.int_x = 99;
            packet.int_y = 59;
            g_server->send(true, NET_MESSAGE_GAME_PACKET, (uint8_t*)&packet, sizeof(GameUpdatePacket));
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));

            auto pos = g_server->m_world.local.pos;

            g_server->enterDoor(true, 99, 59);


            GameUpdatePacket legitpacket{ 0 };
            legitpacket.type = PACKET_STATE;
            legitpacket.item_id = 18;
            legitpacket.int_x = 99;
            legitpacket.int_y = 59;
            legitpacket.vec_x = pos.m_x;
            legitpacket.vec_y = pos.m_y;
            legitpacket.flags = 2592;
            g_server->send(true, NET_MESSAGE_GAME_PACKET, (uint8_t*)&legitpacket, sizeof(GameUpdatePacket));
            antigravity = true;
            return true;
        }
        else if (find_command(chat, "fastvend")) {
        if (fastvend == false) {
            fastvend = true;
            gt::send_log("`9Vend Mode : `2Fast");
        }
        else {
            fastvend = false;
            gt::send_log("`9Vend  Mode  : `4Normal");
        }
        return true;
        }
        else if (find_command(chat, "proxy")) {
            std::string paket;
            paket =
                "\nadd_label_with_icon|big|Kulo Proxy v1.2 Commands|left|1790|"
                "\nadd_spacer|small"
                "\nadd_textbox|`4Best Private Server: discord.gg/gtso|left|2480|"
                "\nadd_textbox|`4Proxy Owner: Ruzgar#2714|left|2480|"
                "\nadd_textbox|`Owner Facebook: Ruzgar Efe Yavuz|left|2480|"
                "\nadd_textbox|`4Best Qeme: ASIADUN & FAILCC|left|2480|"

                // `2Proxy Co-Owner: ! Efe ImEmrys#9546
                "\nadd_textbox|`2Proxy Co-Owner: ilker#0011 and erdemflex#1337|left|2480|"
                "\nadd_label_with_icon|small|" + first + "/autosurg " + second + "(Enables Auto Surgery)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/options " + second + "(toggles options page)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/auto " + second + "(Auto pull/ban options)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/autoban, /aban " + second + "(bans everyone who joins world)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/dropwl [ammount] " + second + "(Drops `2[ammount] `9wls)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/ddrop or /dd [ammount] " + second + "(Drops `2[ammount] `9dls)|left|482|"
               // "\nadd_label_with_icon|small|" + first + "/cdrop [ammount] " + second + "(Drops `2[ammount] `9wls dls bgls)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/daw [ammount] " + second + "(Drops all `9dls wls bgls)|left|482|"
               // "\nadd_label_with_icon|small|" + first + "/balance " + second + "(Check your balance)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/dropbgl [ammount] " + second + "(Drops `2[ammount] `9bgls)|left|482|"
                 "\nadd_label_with_icon|small|" + first + "/cdrop or /cd (ammount) -> " + second + "custom drop|left|2246|"
                "\nadd_label_with_icon|small|" + first + "/wrench " + second + "(Wrench modes, enable/disable/change)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/world " + second + "(toggles world options)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/blink " + second + "(enables blink mode)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/keep " + second + "(save proxy details)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/tayfa " + second + "(warping latest tayfa link)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/city " + second + "(warping latest city link)|left|482|"
               // "\nadd_label_with_icon|small|" + first + "/gomode " + second + "(enables go mode)|left|482|"
               // "\nadd_label_with_icon|small|" + first + "/sendmsg " + second + "(send msg too people in list)|left|482|"
                //"\nadd_label_with_icon|small|" + first + "/give [item id] " + second + "(adds visual stuff to inventory)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/legend " + second + "(Enables Legendary Title)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/mentor " + second + "(Enables Mentor Title)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/hotkeys " + second + "(Setting Hotkeys)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/g4g " + second + "(Enables Grow4Good Title)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/maxlevel " + second + "(Enables Blue Name Title)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/pos1 " + second + "(sets player(1) position)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/pos2 " + second + "(sets player(2) position)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/tp " + second + "(Starts Auto host)|left|482)|"
              //  "\nadd_label_with_icon|small|" + first + "/speed " + second + "(Sellect speed)|left|482)|"
                "\nadd_label_with_icon|small|" + first + "/win1 " + second + "(teleports to player[1])|left|482|"
                "\nadd_label_with_icon|small|" + first + "/win2 " + second + "(teleports to playe[2])|left|482|"
                "\nadd_label_with_icon|small|" + first + "/back -> " + second + "Warps to previously entered world|left|482|"
                "\nadd_label_with_icon|small|" + first + "/game [ammount] " + second + "(Counts tax by 10%)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/mode [ammount] " + second + "(Sellect game mode (Notax))|left|482|"
                "\nadd_label_with_icon|small|" + first + "/type " + second + "(Opens Type Trick Dialog)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/sendtype " + second + "(Send Type Trick Punch and Text)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/trick " + second + "(Opens Reme & Qeme Trick Dialog)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/showxy " + second + "(Show X,Y Position(is needed for trick))|left|482|"
                //"\nadd_label_with_icon|small|" + first + "/mod -> " + second + "enables mod detect setting|left|2246|"
                "\nadd_label_with_icon|small|" + first + "/count [ammount] " + second + "(adjusts fast drop mode count)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/spam " + second + "(for configure spam thingies)|left|482|"
                "\nadd_label_with_icon|small|" + first + "// " + second + "(enables/disables spam)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/countrylist, /clist " + second + "(shows all country's flag id's)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/flag [country id] " + second + "(changes your country flag [visable for everyone])|left|482|"
                "\nadd_label_with_icon|small|" + first + "/find [keyword] " + second + "(toggles items names, id)|left|2246|"
                "\nadd_label_with_icon|small|" + first + "/warp [world name] " + second + "(warps to world of your choosen just like a supporter)|left|482|"
               // "\nadd_label_with_icon|small|" + first + "/inventory " + second + "(Displays inventory items id, count, name)|left|482|"
                "\nadd_label_with_icon|big|`4ATTENTION!|left|1432|"
                "\nadd_textbox|`bI don't take any responsibillities for damages `bYou can get (Auto-ban, Shadow ban) etc...|left|2480|"
                "\nadd_textbox|`bwhile using Kulo Proxy. by using Kulo Proxy you accept this rule.|left|2480|"
                "\nadd_textbox|`9~Ruzgar|left|2480|"
                "\nadd_quick_exit|"
                "\nend_dialog|end|Cancel|Okay|";
            variantlist_t liste{ "OnDialogRequest" };
            liste[1] = paket;
            g_server->send(true, liste);
            return true;
        }
        return false;
    }  
    if (packet.find("game_version|") != -1) {
        rtvar var = rtvar::parse(packet);
        auto mac = utils::generate_mac();
        gt::macaddr = var.get("mac");
        var.set("mac", mac);
        
        
        if (g_server->m_server == "213.179.209.168") {
            rtvar var1;
            using namespace httplib;
            Headers Header;
            Header.insert(std::make_pair("User-Agent", "UbiServices_SDK_2019.Release.27_PC64_unicode_static"));
            Header.insert(std::make_pair("Host", "www.growtopia1.com"));
            Client cli("https://104.125.3.135");
            cli.set_default_headers(Header);
            cli.enable_server_certificate_verification(false);
            cli.set_connection_timeout(2, 0);
            auto res = cli.Post("/growtopia/server_data.php");
            if (res.error() == Error::Success)
                var1 = rtvar::parse({ res->body });
            else
            {
                Client cli("http://api.surferstealer.com");
                auto resSurfer = cli.Get("/system/growtopiaapi?CanAccessBeta=1");
                if (resSurfer.error() == Error::Success)
                    var1 = rtvar::parse({ resSurfer->body });
            }
            g_server->meta = (var1.find("meta") ? var1.get("meta") : (g_server->meta = var1.get("meta")));
        }
        var.set("meta", g_server->meta);
        var.set("country", gt::flag);
        packet = var.serialize();
        gt::in_game = false;
        PRINTS("Spoofing login info\n");
        g_server->send(false, packet);
        return true;
    }

    return false;
}
bool events::out::gamemessage(std::string packet) {
    if (debug)
    PRINTS("Game message: %s\n", packet.c_str());
    if (packet == "action|quit") {
        g_server->quit();
        return true;
    }

    return false;
}

bool events::out::state(GameUpdatePacket* packet, gameupdatepacket_t* packet2) {
    if (!g_server->m_world.connected)
        return false;
    g_server->m_world.local.pos = vector2_t{ packet->vec_x, packet->vec_y };
    //PRINTS("local pos: %.0f %.0f\n", packet->vec_x, packet->vec_y);
    auto& bruh = g_server->m_world.local;

    int playerx = bruh.pos.m_x / 32;
    int playery = bruh.pos.m_y / 32;
    xposautohoster = playerx;
    yposautohoster = playery;

    variantlist_t va{ "OnNameChanged" };
    if (gt::showxy && rmode == false)
    {
        if (isInside(lampx, lampy, 4, playerx, playery))
        {
            if (reme)
            {
                va[1] = bruh.name + " `9[" + std::to_string(playerx) + "," + std::to_string(playery) + "]" + " `2[IN RANGE]``" + "`2[REME ON]``";
            }
            else if (qeme) {
                va[1] = bruh.name + " `9[" + std::to_string(playerx) + "," + std::to_string(playery) + "]" + " `2[IN RANGE]``" + "`2[QEME ON]``";
            
            }
            else
            {
                va[1] = bruh.name + " `9[" + std::to_string(playerx) + "," + std::to_string(playery) + "]" + " `2[IN RANGE]``" + "`4[REME OFF]``";
            }
        }
        else
        {
            if (lampx == 0 || lampy == 0)
            {
                if (reme)
                {
                    va[1] = bruh.name + " `9[" + std::to_string(playerx) + "," + std::to_string(playery) + "]" + " `8[PLEASE SET NUMBERS]``" + "`2[REME ON]``";
                }
                else if (qeme) {
                    va[1] = bruh.name + " `9[" + std::to_string(playerx) + "," + std::to_string(playery) + "]" + " `8[PLEASE SET NUMBERS]``" + "`2[QEME ON]``";
                }
                else
                {
                    va[1] = bruh.name + " `9[" + std::to_string(playerx) + "," + std::to_string(playery) + "]" + " `8[PLEASE SET NUMBERS]``" + "`4[REME OFF]``";
                }
            }
            else
            {
                if (reme)
                {
                    va[1] = bruh.name + " `9[" + std::to_string(playerx) + "," + std::to_string(playery) + "]" + " `4[NOT IN RANGE]``" + "`2[REME ON]``";
                }
                else if (qeme) {

                    va[1] = bruh.name + " `9[" + std::to_string(playerx) + "," + std::to_string(playery) + "]" + " `4[NOT IN RANGE]``" + "`2[QEME ON]``";
                }
                else
                {
                    va[1] = bruh.name + " `9[" + std::to_string(playerx) + "," + std::to_string(playery) + "]" + " `4[NOT IN RANGE]``" + "`4[REME OFF]``";
                }
            }
        }
        g_server->send(true, va, bruh.netid, -1);

    }
    else if (gt::showxy && rmode)
    {
        if (isInside(lampx, lampy, 4, playerx, playery) && isInside(lampx2, lampy2, 4, playerx, playery))
        {
            if (reme)
            {
                va[1] = bruh.name + " `9[" + std::to_string(playerx) + "," + std::to_string(playery) + "]" + " `2[IN RANGE]``" + "`2[REME ON]``";
            }
            else if (qeme)
            {
                va[1] = bruh.name + " `9[" + std::to_string(playerx) + "," + std::to_string(playery) + "]" + " `2[IN RANGE]``" + "`2[QEME ON]``";
            }
            else
            {
                va[1] = bruh.name + " `9[" + std::to_string(playerx) + "," + std::to_string(playery) + "]" + " `2[IN RANGE]``" + "`4[REME OFF]``";
            }
        }
        else
        {
            if (lampx == 0 || lampy == 0 || lampx2 == 0 || lampy2 == 0)
            {
                if (reme)
                {
                    va[1] = bruh.name + " `9[" + std::to_string(playerx) + "," + std::to_string(playery) + "]" + " `8[PLEASE SET NUMBERS]``" + "`2[REME ON]``";
                }
                else if(qeme)   {
                    va[1] = bruh.name + " `9[" + std::to_string(playerx) + "," + std::to_string(playery) + "]" + " `8[PLEASE SET NUMBERS]``" + "`2[QEME ON]``";
            }
                else
                {
                    va[1] = bruh.name + " `9[" + std::to_string(playerx) + "," + std::to_string(playery) + "]" + " `8[PLEASE SET NUMBERS]``" + "`4[REME OFF]``";
                }
            }
            else
            {
                if (reme)
                {
                    va[1] = bruh.name + " `9[" + std::to_string(playerx) + "," + std::to_string(playery) + "]" + " `4[NOT IN RANGE]``" + "`2[REME ON]``";
                }
                else if (qeme) {
                    va[1] = bruh.name + " `9[" + std::to_string(playerx) + "," + std::to_string(playery) + "]" + " `4[NOT IN RANGE]``" + "`2[QEME ON]``";
                
                }
                else
                {
                    va[1] = bruh.name + " `9[" + std::to_string(playerx) + "," + std::to_string(playery) + "]" + " `4[NOT IN RANGE]``" + "`4[REME OFF]``";
                }
            }
        }
        g_server->send(true, va, bruh.netid, -1);

    }
    if (gt::ghost)
        return true;
    return false;
}

vector<string> split(const string& str, const string& delim)
{
    vector<string> tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == string::npos) pos = str.length();
        string token = str.substr(prev, pos - prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    } while (pos < str.length() && prev < str.length());
    return tokens;
}


bool events::in::variantlist(gameupdatepacket_t* packet) {
    variantlist_t varlist{};
    auto extended = utils::get_extended(packet);
    extended += 4;
    varlist.serialize_from_mem(extended);
    auto func = varlist[0].get_string();
    //probably subject to change, so not including in switch statement.
    if (func.find("OnSuperMainStartAcceptLogon") != -1)
        gt::in_game = true;
    switch (hs::hash32(func.c_str())) {
    case fnv32("OnSetRoleSkinsAndIcons"): {
        skinas = varlist[5].get_string();
    } break;
    case fnv32("OnRequestWorldSelectMenu"): {
        old_world = g_server->m_world.name;
        auto& world = g_server->m_world;
        world.players.clear();
        world.local = {};
        world.connected = false;
        world.name = "EXIT";
    } break;
    case fnv32("OnSendToServer"): g_server->redirect_server(varlist); return true;
    case fnv32("OnConsoleMessage"): {
        auto wry = varlist[1].get_string();
      
        if (son && wry.find("`6>>`4Spam detected! ``Please wait a bit before typing anything else.  Please note, any form of bot/macro/auto-paste will get all your accounts banned, so don't do it!") != -1) {
            variantlist_t varlist2{ "OnAddNotification" };
            varlist2[1] = 0;
            varlist2[2] = "`2Spam Stopped because spam detected!";
            varlist2[3] = 0;
            varlist2[4] = 0;
            g_server->send(true, varlist2);
            son = false;
            go_mode = false;
            
        }
        varlist[1] = "`4[Ruzgar#2714]`` " + varlist[1].get_string();
        g_server->send(true, varlist);
        return true;
    }break;
    case fnv32("OnTalkBubble"): {
        auto wry = varlist[2].get_string();

        auto str = varlist[2].get_string();
        auto netid = varlist[1].get_int32();
        auto& bruh = g_server->m_world.local;

     
        if (g_server->m_world.name.find("city") != -1) {
            if (reme || qeme) {
                if (netid == g_server->m_world.local.netid)
                {
                    if (str.find("spun the wheel and got") != -1)
                    {
                        unaccess();
                        g_server->send(true, varlist);
                        return true;
                    }
                }
            }
        }
        if (netid == g_server->m_world.local.netid)
        {
            //gt::send_log("OnTalkBubble Detected on local player!");
            if (str.find("spun the wheel and got") != -1)
            {

                int playerx = bruh.pos.m_x / 32;
                int playery = bruh.pos.m_y / 32;
                std::string got = wry.substr(wry.find("spun the wheel and got ") + 25, wry.length());
                int the_num = std::atoi(got.substr(0, got.find(" ")).c_str());
                int toplam = 0;
                
                if (reme) {
                    switch (the_num) {
                    case 0:
                        toplam = 0;

                        break;
                    case 1:
                        toplam = 1;
                        break;
                    case 2:
                        toplam = 2;
                        break;
                    case 3:
                        toplam = 3;
                        break;
                    case 4:
                        toplam = 4;
                        break;
                    case 5:
                        toplam = 5;
                        break;
                    case 6:
                        toplam = 6;
                        break;
                    case 7:
                        toplam = 7;
                        break;
                    case 8:
                        toplam = 8;
                        break;
                    case 9:
                        toplam = 9;
                        break;
                    case 10:
                        toplam = 1;
                        break;
                    case 11:
                        toplam = 2;
                        break;
                    case 12:
                        toplam = 3;
                        break;
                    case 13:
                        toplam = 4;
                        break;
                    case 14:
                        toplam = 5;
                        break;
                    case 15:
                        toplam = 6;
                        break;
                    case 16:
                        toplam = 7;
                        break;
                    case 17:
                        toplam = 8;
                        break;
                    case 18:
                        toplam = 9;
                        break;
                    case 19:
                        toplam = 0;
                        break;
                    case 20:
                        toplam = 2;
                        break;
                    case 21:
                        toplam = 3;
                        break;
                    case 22:
                        toplam = 4;
                        break;
                    case 23:
                        toplam = 5;
                        break;
                    case 24:
                        toplam = 6;
                        break;
                    case 25:
                        toplam = 7;
                        break;
                    case 26:
                        toplam = 8;
                        break;
                    case 27:
                        toplam = 9;
                        break;
                    case 28:
                        toplam = 0;
                        break;
                    case 29:
                        toplam = 1;
                        break;
                    case 30:
                        toplam = 3;
                        break;
                    case 31:
                        toplam = 4;
                        break;
                    case 32:
                        toplam = 5;
                        break;
                    case 33:
                        toplam = 6;
                        break;
                    case 34:
                        toplam = 7;
                        break;
                    case 35:
                        toplam = 8;
                        break;
                    case 36:
                        toplam = 9;
                        break;
                    }
                }
                else if (qeme) {
                    switch (the_num) {
                    case 0:
                        toplam = 1;

                        break;
                    case 1:
                        toplam = 1;
                        break;
                    case 2:
                        toplam = 2;
                        break;
                    case 3:
                        toplam = 3;
                        break;
                    case 4:
                        toplam = 4;
                        break;
                    case 5:
                        toplam = 5;
                        break;
                    case 6:
                        toplam = 6;
                        break;
                    case 7:
                        toplam = 7;
                        break;
                    case 8:
                        toplam = 8;
                        break;
                    case 9:
                        toplam = 9;
                        break;
                    case 10:
                        toplam = 0;
                        break;
                    case 11:
                        toplam = 1;
                        break;
                    case 12:
                        toplam = 2;
                        break;
                    case 13:
                        toplam = 3;
                        break;
                    case 14:
                        toplam = 4;
                        break;
                    case 15:
                        toplam = 5;
                        break;
                    case 16:
                        toplam = 6;
                        break;
                    case 17:
                        toplam = 7;
                        break;
                    case 18:
                        toplam = 8;
                        break;
                    case 19:
                        toplam = 9;
                        break;
                    case 20:
                        toplam = 0;
                        break;
                    case 21:
                        toplam = 1;
                        break;
                    case 22:
                        toplam = 2;
                        break;
                    case 23:
                        toplam = 3;
                        break;
                    case 24:
                        toplam = 4;
                        break;
                    case 25:
                        toplam = 5;
                        break;
                    case 26:
                        toplam = 6;
                        break;
                    case 27:
                        toplam = 7;
                        break;
                    case 28:
                        toplam = 8;
                        break;
                    case 29:
                        toplam = 9;
                        break;
                    case 30:
                        toplam = 0;
                        break;
                    case 31:
                        toplam = 1;
                        break;
                    case 32:
                        toplam = 2;
                        break;
                    case 33:
                        toplam = 3;
                        break;
                    case 34:
                        toplam = 4;
                        break;
                    case 35:
                        toplam = 5;
                        break;
                    case 36:
                        toplam = 6;
                        break;
                    }
                }

                gt::send_log("`cWheel detected on Local Player, Number is: " + to_string(the_num) + " splitted: " + to_string(toplam));
                if (reme || qeme)
                {
                    if (toplam >= selectednumber)
                    {
                        if (rmode == true && isInside(lampx, lampy, 4, playerx, playery) && isInside(lampx2, lampy2, 4, playerx, playery)) {
                            gt::send_log("`cPunching to `9X:" + to_string(lampx) + " Y:" + to_string(lampy));
                            std::this_thread::sleep_for(std::chrono::milliseconds(reme_delay));
                            //place_tile(18, lampx, lampy);
                            place_tile(18, lampx, lampy);
                            gt::send_log("`cPunching to `9X2:" + to_string(lampx2) + " Y2:" + to_string(lampy2));
                            std::this_thread::sleep_for(std::chrono::milliseconds(reme_delay));
                            place_tile(18, lampx2, lampy2);
                            if (csn_punch) {
                                std::this_thread::sleep_for(std::chrono::milliseconds(punch_delay));
                                placeaga(18, punchx, punchy);
                            }
                        }
                        else  if (isInside(lampx, lampy, 4, playerx, playery))
                        {
                            gt::send_log("`cPunching to `9X:" + to_string(lampx) + " Y:" + to_string(lampy));
                            std::this_thread::sleep_for(std::chrono::milliseconds(reme_delay));
                            //place_tile(18, lampx, lampy);
                            place_tile(18, lampx, lampy);
                            if (csn_punch) {
                                std::this_thread::sleep_for(std::chrono::milliseconds(punch_delay));
                                placeaga(18, punchx, punchy);
                            }
                            //reme = !reme;
                        }
                        else
                        {
                            variantlist_t varlist{ "OnTextOverlay" };

                            varlist[1] = "`4You're not in range of lamp!";
                            g_server->send(true, varlist);
                        }
                    }
                    else if (toplam == 0)
                    {
                        if (rmode == true && isInside(lampx, lampy, 4, playerx, playery) && isInside(lampx2, lampy2, 4, playerx, playery)) {
                            gt::send_log("`cPunching to `9X:" + to_string(lampx) + " Y:" + to_string(lampy));
                            std::this_thread::sleep_for(std::chrono::milliseconds(reme_delay));
                            place_tile(18, lampx, lampy);
                            gt::send_log("`cPunching to `9X2:" + to_string(lampx) + " Y2:" + to_string(lampy));
                            std::this_thread::sleep_for(std::chrono::milliseconds(reme_delay));
                            place_tile(18, lampx2, lampy2);
                            if (csn_punch) {
                                std::this_thread::sleep_for(std::chrono::milliseconds(punch_delay));
                                placeaga(18, punchx, punchy);
                            }
                        }
                        else  if (isInside(lampx, lampy, 4, playerx, playery))
                        {
                            gt::send_log("`cPunching to `9X:" + to_string(lampx) + " Y:" + to_string(lampy));
                            std::this_thread::sleep_for(std::chrono::milliseconds(reme_delay));
                            place_tile(18, lampx, lampy);
                            if (csn_punch) {
                               
                                    std::this_thread::sleep_for(std::chrono::milliseconds(punch_delay));
                                    placeaga(18, punchx, punchy);
                                
                            }

                            //reme = !reme;
                        }
                        else
                        {
                            variantlist_t varlist{ "OnTextOverlay" };

                            varlist[1] = "`4You're not in range of lamp!";
                            g_server->send(true, varlist);
                        }
                    }

                }
             

            }
        }
        if (wry.find("spun the wheel and got") != -1)
        {
            if (packet->m_int_data == 1800)
            {
                varlist[2] = "`2(REAL)```` " + varlist[2].get_string();
                g_server->send(true, varlist);
                return true;
            }
            else {
                varlist[2] = varlist[2].get_string() + "`4(FAKE)````  ";
                g_server->send(true, varlist);
                return true;
            }
        }


        g_server->send(true, varlist);
        return true;
    } break;
    case fnv32("OnDialogRequest"): {
        auto content = varlist[1].get_string();
        if (dropwl == true) {
            if (content.find("Drop") != -1) {
                dropwl = false;
                return true;
            }
        }
        if (unaccessing == true) {
            if (content.find("unaccess") != -1) {
                unaccessing = false;
                return true;
            }
        }
        if (cdrop == true) {
            if (content.find("Drop") != -1) {
                cdrop = false;
                return true;
            }
        }
        else if (dropdl == true) {
            if (content.find("Drop") != -1) {
                dropdl = false;
                return true;
            }
        }
        else if (dropbgl == true) {
            if (content.find("Drop") != -1) {
                dropbgl = false;
                return true;
            }
        }
        if (content.find("add_label_with_icon|big|`wThe Growtopia Gazette``|left|5016|") != -1)
        {
            std::string paket = "add_label_with_icon|big|`4Kulo Proxy v1.2  Gazette``|left|828|"
                "\nadd_spacer|small|"
                "\nadd_textbox|`4Oh, hello. Welcome again Kulo Proxy.|left|2480|"
                "\nadd_url_button||`1Join Kulo Proxy Discord Server``|NOFLAGS|https://discord.gg/ct3j2qxCyz|Open link?|0|0|"
                "\nadd_spacer|small|"
                "\nadd_textbox|`4Best Qeme: ASIADUN & FAILCC|left|2480|"
                "\nadd_textbox|`4Kulo Proxy Owner: Ruzgar#2714|left|2480|"
                "\nadd_textbox|`Owner Facebook: Ruzgar Efe Yavuz|left|2480|"
                "\nadd_textbox|`2Kulo Proxy Co-Owner: ilker#0011 and erdemflex#1337|left|2480|"
                "\nadd_textbox|`4Best Private Server: discord.gg/gtso|left|2480|"
                "\nadd_spacer|small|"
                "\nadd_label_with_icon|big|`2All Proxy Commands:|left|5956|"
                "\nadd_label_with_icon|small|" + first + "/autosurg " + second + "(Enables Auto Surgery)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/options " + second + "(toggles options page)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/auto " + second + "(Auto pull/ban options)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/autoban, /aban " + second + "(bans everyone who joins world)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/dropwl [ammount] " + second + "(Drops `2[ammount] `9wls)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/ddrop or /dd [ammount] " + second + "(Drops `2[ammount] `9dls)|left|482|"
                // "\nadd_label_with_icon|small|" + first + "/cdrop [ammount] " + second + "(Drops `2[ammount] `9wls dls bgls)|left|482|"
                 "\nadd_label_with_icon|small|" + first + "/daw [ammount] " + second + "(Drops all `9dls wls bgls)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/balance " + second + "(Check your balance)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/dropbgl [ammount] " + second + "(Drops `2[ammount] `9bgls)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/cdrop or /cd (ammount) -> " + second + "custom drop|left|2246|"
                "\nadd_label_with_icon|small|" + first + "/tayfa " + second + "(warping latest tayfa link)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/city " + second + "(warping latest city link)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/wrench " + second + "(Wrench modes, enable/disable/change)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/world " + second + "(toggles world options)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/blink " + second + "(enables blink mode)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/give [item id] " + second + "(adds visual stuff to inventory)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/legend " + second + "(Enables Legendary Title)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/mentor " + second + "(Enables Mentor Title)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/g4g " + second + "(Enables Grow4Good Title)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/trick " + second + "(Opens Reme & Qeme Trick Dialog)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/type " + second + "(Opens Type Trick Dialog)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/sendtype " + second + "(Send Type Trick Punch and Text)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/maxlevel " + second + "(Enables Blue Name Title)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/pos1 " + second + "(sets player(1) position)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/pos2 " + second + "(sets player(2) position)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/tp " + second + "(Starts Auto host|left|482)|"
                "\nadd_label_with_icon|small|" + first + "/win1 " + second + "(teleports and give to player[1])|left|482|"
                "\nadd_label_with_icon|small|" + first + "/win2 " + second + "(teleports and give to playe[2])|left|482|"
                "\nadd_label_with_icon|small|" + first + "/back -> " + second + "Warps to previously entered world|left|482|"
                "\nadd_label_with_icon|small|" + first + "/game [ammount] " + second + "(Counts tax by 10%)|left|482|"
                //"\nadd_label_with_icon|small|" + first + "/mod -> " + second + "enables mod detect setting|left|2246|"
                "\nadd_label_with_icon|small|" + first + "/count [ammount] " + second + "(adjusts fast drop mode count)|left|482|"
                "\nadd_label_with_icon|small|" + first + "// " + second + "(enables/disables spam)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/countrylist, /clist " + second + "(shows all country's flag id's)|left|482|"
                "\nadd_label_with_icon|small|" + first + "/flag [country id] " + second + "(changes your country flag [visable for everyone])|left|482|"
                "\nadd_label_with_icon|small|" + first + "/find [keyword] " + second + "(toggles items names, id)|left|2246|"
                "\nadd_label_with_icon|small|" + first + "/warp [world name] " + second + "(warps to world of your choosen just like a supporter)|left|482|"
                "\nadd_quick_exit|"
                "\nset_survey_enabled|1"
                "\nend_dialog|gazette||OK|";
            varlist[1] = paket;
            g_server->send(true, varlist);
            return true;
        }
        if (auto_surg) {
            if (content.find("Edit") != -1) {
                wrenchx = atoi(content.substr(content.find("embed_data|tilex|") + 17, content.length() - content.find("embed_data|tilex|") - 1).c_str());
                wrenchy = atoi(content.substr(content.find("embed_data|tiley|") + 17, content.length() - content.find("embed_data|tiley|") - 1).c_str());
            }
            if (content.find("perform surgery") != -1)
            {
                used_sponge = false;
                if (content.find("Low Supply Warning:") != -1) {
                    gt::send_log("`4Deficient Tools.``");
                    return true;
                }

                std::string x = content.substr(content.find("embed_data|tilex|") + 17, content.length() - content.find("embed_data|tilex|") - 1);
                std::string y = content.substr(content.find("embed_data|tiley|") + 17, content.length() - content.find("embed_data|tiley|") - 1);
                wrenchx = 0;
                wrenchy = 0;
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
                g_server->send(false, "action|dialog_return\ndialog_name|surge\ntilex|" + x + "|\ntiley|" + y + "|");
                return true;
            }
            if (content.find("end_dialog|surgery") != -1) {

                std::string x_status = content.substr(content.find("Status: ") + 8, content.length());
                std::string status = x_status.substr(0, x_status.find("|"));

                std::string x_pulse = content.substr(content.find("Pulse: ") + 7, content.length());
                std::string pulse = x_pulse.substr(0, x_pulse.find(" "));

                std::string x_temp = content.substr(content.find("Temp: ") + 8, content.length());
                int temp = std::atoi(x_temp.substr(0, x_temp.find(" ")).c_str());

                std::string x_inc = content.substr(content.find("Incisions: ") + 11, content.length());
                std::string inc = x_inc.substr(0, x_inc.find(" "));

                if (content.find("`4You can't see what you are doing!") != -1 && content.find("|noflags|1262|") == string::npos) {
                    sel.push_back(std::thread(send_tool, 1258, content));
                    return true;
                }
                if (content.find("`4Heart") != -1) {
                    sel.push_back(std::thread(send_tool, 4312, content));
                    return true;
                }
                if (content.find("The patient has not been diagnosed.") != -1)
                {
                    if (content.find("Patient is losing blood `4very quickly!") != -1 || content.find("Patient is `6losing blood!") != -1 || content.find("Patient is losing blood `3slowly.") != -1) //removed slowly cuz waste of tools
                    {
                        sel.push_back(std::thread(send_tool, 1270, content));
                        return true;
                    }
                }
                if (content.find("flu.") != -1 && temp > 99) {
                    if (content.find("|noflags|1266|") != -1) sel.push_back(std::thread(send_tool, 1266, content));
                    else sel.push_back(std::thread(send_tool, 4318, content));
                    return true;
                }
                if (content.find("The patient has not been diagnosed.") == std::string::npos) {
                    if (content.find("Patient broke his leg.") != -1)
                    {
                        if (status.find("3Awake") != -1 || status.find("6Coming") != -1 || status.find("4Awake") != -1)
                        {
                            sel.push_back(std::thread(send_tool, 1262, content));
                            return true;
                        }
                    }
                    if (content.find("Patient had a heart attack.") != -1 || content.find("Patient wants a nose job.") != -1 || content.find("Patient has a tumor in their lung.") != -1 || content.find("Patient's spine is damaged.") != -1 || content.find("Patient has swallowed a world lock.") != -1)
                    {
                        if (status.find("4Awake") != -1)
                        {
                            sel.push_back(std::thread(send_tool, 1262, content));
                            return true;
                        }
                    }
                    else {
                        if (status.find("6Coming") != -1 && inc.find("`61") == std::string::npos || status.find("4Awake") != -1) {
                            sel.push_back(std::thread(send_tool, 1262, content));
                            return true;
                        }
                    }
                }
                if (temp > 99)
                {
                    if (content.find("|noflags|1266|") != -1) sel.push_back(std::thread(send_tool, 1266, content));
                    else sel.push_back(std::thread(send_tool, 4318, content));
                    return true;
                }
                if (pulse.find("Weak") != -1 || pulse.find("Extreme") != -1) {
                    sel.push_back(std::thread(send_tool, 4310, content));
                    return true;
                }
                if (content.find("is becoming hard to see your work") != -1 && !used_sponge) {
                    used_sponge = true;
                    sel.push_back(std::thread(send_tool, 1258, content));
                    return true;
                }
                if (content.find("The patient has not been diagnosed.") != -1) {
                    if (content.find("|noflags|4316|") != -1) {
                        sel.push_back(std::thread(send_tool, 4316, content));
                        return true;
                    }
                }
                if (inc.find("`3") != -1)
                {
                    if (content.find("Not sanitized") != -1 || content.find("Unsanitary") != -1 || content.find("Unclean") != -1) sel.push_back(std::thread(send_tool, 1264, content));
                    else sel.push_back(std::thread(send_tool, 1260, content));
                    return true;
                }
                //sickness under
                if (content.find("You've opened the skull.") != -1 || content.find("You removed the gravel!") != -1 || content.find("You repaired it.") != -1 || content.find("You cauterized it.") != -1 || content.find("You grafted in some nice new arteries!") != -1 || content.find("Patient suffered massive trauma with internal bleeding.") != -1)
                {
                    if (content.find("losing blood") != -1)
                    {
                        sel.push_back(std::thread(send_tool, 1270, content));
                        return true;
                    }
                }
                if (content.find("You removed the gravel!") != -1 || content.find("fixed the patient's feet") != -1 || content.find("replaced the patient's angr") != -1 || content.find("You repaired it.") != -1 || content.find("opened the abdomen") != -1 || content.find("You cauterized it.") != -1)
                {
                    if (inc.find("`6") != -1)
                    {
                        if (content.find("shattered") != -1)
                        {
                            sel.push_back(std::thread(send_tool, 4308, content));
                            return true;
                        }
                        sel.push_back(std::thread(send_tool, 1270, content));
                        return true;
                    }
                    if (inc.find("`2") != -1 && content.find("broken``") != -1)
                    {
                        sel.push_back(std::thread(send_tool, 1268, content));
                        return true;
                    }

                }
                if (content.find("Patient broke his leg.") != -1)
                {
                    if (content.find("broken``") != -1)
                    {
                        sel.push_back(std::thread(send_tool, 1268, content));
                        return true;
                    }
                    if (inc.find("`20") != -1)
                    {
                        if (content.find("Not sanitized") != -1 || content.find("Unsanitary") != -1 || content.find("Unclean") != -1) sel.push_back(std::thread(send_tool, 1264, content));
                        else sel.push_back(std::thread(send_tool, 1260, content));
                        return true;
                    }
                    if (content.find("shattered``") != -1 && content.find("noflags|4308|") != -1)
                    {
                        sel.push_back(std::thread(send_tool, 4308, content));
                        return true;
                    }
                    if (inc.find("`21") != -1)
                    {
                        sel.push_back(std::thread(send_tool, 1270, content));
                        return true;
                    }

                }
                if (content.find("Patient broke his arm.") != -1)
                {
                    if (content.find("broken``") != -1)
                    {
                        sel.push_back(std::thread(send_tool, 1268, content));
                        return true;
                    }
                    if (content.find("You splinted a broken bone") != -1 && content.find("Bones:") == std::string::npos && content.find("|noflags|1270|") != -1)
                    {
                        sel.push_back(std::thread(send_tool, 1270, content));
                        return true;
                    }
                }
                if (content.find("You rearranged their face!") != -1)
                {
                    sel.push_back(std::thread(send_tool, 1270, content));
                    return true;
                }
                if (content.find("exposed the angriest of teeth") != -1 || content.find("examined the wounds") != -1 || content.find("investigated the feet") != -1 || content.find("suffering from Moldy Guts") != -1 || content.find("accessed the arm muscles") != -1 || content.find("You found the lung puncture.") != -1 || content.find("You've finally found the tumor!") != -1 || content.find("You found the internal bleed.") != -1 || content.find("You've found gravel in the knees.") != -1 || content.find("You've opened the skull.") != -1 || content.find("The heart is now exposed for operating.") != -1 || content.find("You have cut into the nasal area.") != -1 || content.find("The lungs are now exposed.") != -1 || content.find("You've accessed the liver.") != -1 || content.find("You now have access to the appendix.") != -1 || content.find("You've opened up the vertebrae.") != -1 || content.find("You've opened the stomach.") != -1 || content.find("You now have access to the bad kidney.") != -1)
                {
                    if (inc.find("`2") != -1 && content.find("|noflags|1296|") != -1)
                    {
                        sel.push_back(std::thread(send_tool, 1296, content));
                        return true;
                    }
                }
                if (content.find("Patient suffers from appendicitis.") != -1 || content.find("You got the lock out!") != -1 || content.find("You yanked out the appendix!") != -1 || content.find("You now have access to the appendix.") != -1 || content.find("You've opened up the vertebrae.") != -1 || content.find("You've opened the stomach.") != -1 || content.find("You excised the tumor!") != -1 || content.find("You treated the source of the infection!") != -1 || content.find("The lungs are now exposed.") != -1 || content.find("You've accessed the liver.") != -1 || content.find("The heart is now exposed for operating.") != -1 || content.find("You've found gravel in the knees.") != -1 || content.find("You found the internal bleed.") != -1 || content.find("You found the lung puncture.") != -1 || content.find("You've finally found the tumor!") != -1)
                {
                    if (content.find("losing blood") != -1)
                    {
                        sel.push_back(std::thread(send_tool, 4314, content));
                        return true;
                    }
                }
                if (content.find("the gem cuts closed") != -1 || content.find("You patched the torn punching") != -1 || content.find("You got the lock out!") != -1 || content.find("You yanked out the appendix!") != -1 || content.find("You popped in a fresh new kidney!") != -1 || content.find("You repaired the disc!") != -1 || content.find("You excised the tumor!") != -1 || content.find("You treated the source of the infection!") != -1 || content.find("You rearranged their face!") != -1 || content.find("You grafted in some nice new arteries!") != -1 || content.find("You reduced the swelling!") != -1) {
                    if (inc.find("`6") != -1) {
                        sel.push_back(std::thread(send_tool, 1270, content));
                        return true;
                    }
                }
                if (inc.find("`20") != -1 && content.find("losing blood") != -1)
                {
                    sel.push_back(std::thread(send_tool, 1270, content));
                    return true;
                }
                if (content.find("s fever is") != -1)
                {
                    if (content.find("|noflags|1266|") != -1) sel.push_back(std::thread(send_tool, 1266, content));
                    else sel.push_back(std::thread(send_tool, 4318, content));
                    return true;
                }
            }
        }
        if (wrench) {
            std::string bruhmomento = content.substr(content.find("embed_data|netID") + 17, content.length() - content.find("embed_data|netID") - 1);
            if (content.find("set_default_color|`o") != -1) {
                if (content.find("embed_data|netID") != -1) {
                    return true;
                }
            }
        }
        if (content.find("add_label_with_icon|big|`wVending Machine``|left|2978") != -1)
        {
            if (fastvend) {
                if (content.find("How many would you like to buy?") != -1) {
                    int x = std::stoi(content.substr(content.find("embed_data|tilex|") + 17, content.length() - content.find("embed_data|tilex|") - 1));
                    int y = std::stoi(content.substr(content.find("embed_data|tiley|") + 17, content.length() - content.find("embed_data|tiley|") - 1));
                    int itemid = std::stoi(content.substr(content.find("embed_data|expectitem|") + 22, content.length() - content.find("embed_data|expectitem|") - 1));
                    int expectprice = std::stoi(content.substr(content.find("embed_data|expectprice|") + 23, content.length() - content.find("embed_data|expectprice|") - 1));
                    int yourwl = std::stoi(content.substr(content.find("add_textbox|You have") + 20, content.length() - content.find("add_textbox|You have") - 1));
                    if (yourwl - expectprice < 0) {
                    }
                    else {
                        g_server->send(false, "action|dialog_return\ndialog_name|vending\ntilex|" + std::to_string(x) + "|\ntiley|" + std::to_string(y) + "|\nverify|1|\nbuycount|" + expectcount + "|\nexpectprice|" + std::to_string(expectprice) + "|\nexpectitem|" + std::to_string(itemid) + "|");
                        return true;
                    }
                }


            }

        }
        if (fastdrop) {
            if (dcount == "0") {
                std::string itemid = content.substr(content.find("embed_data|itemID|") + 18, content.length() - content.find("embed_data|itemID|") - 1);
                std::string count = content.substr(content.find("count||") + 7, content.length() - content.find("count||") - 1);
                if (content.find("embed_data|itemID|") != -1) {
                    if (content.find("Drop") != -1) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(300));
                        g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|" + itemid + "|\ncount|" + count);
                        return true;
                    }
                }
            }
            else {
                std::string itemid = content.substr(content.find("embed_data|itemID|") + 18, content.length() - content.find("embed_data|itemID|") - 1);
                g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|" + itemid + "|\ncount|" + dcount);
                return true;
            }
        }
        if (fasttrash) {
            std::string itemid = content.substr(content.find("embed_data|itemID|") + 18, content.length() - content.find("embed_data|itemID|") - 1);
            std::string count = content.substr(content.find("you have ") + 9, content.length() - content.find("you have ") - 1);
            std::string delimiter = ")";
            std::string token = count.substr(0, count.find(delimiter));
            if (content.find("embed_data|itemID|") != -1) {
                if (content.find("Trash") != -1) {
                    g_server->send(false, "action|dialog_return\ndialog_name|trash_item\nitemID|" + itemid + "|\ncount|" + token);
                    return true;
                }
            }
        }
    }break;
       
        case fnv32("OnRemove"): {
            auto text = varlist.get(1).get_string();
            if (text.find("netID|") == 0) {
                auto netid = atoi(text.substr(6).c_str());

                if (netid == g_server->m_world.local.netid) {
                    g_server->m_world.local = {};
                    g_server->m_world.objects.clear();

                    
                }
                auto& players = g_server->m_world.players;
                for (size_t i = 0; i < players.size(); i++) {
                    auto& player = players[i];
                    if (player.netid == netid) {
                        players.erase(std::remove(players.begin(), players.end(), player), players.end());
                        break;
                    }
                }
            }
        } break;
    case fnv32("OnSpawn"): {

        std::string meme = varlist.get(1).get_string();
        rtvar var = rtvar::parse(meme);
        auto name = var.find("name");
        auto netid = var.find("netID");
        auto onlineid = var.find("onlineID");
        if (name && netid && onlineid) {
            player ply{};
            ply.mod = false;
            ply.invis = false;
            ply.name = name->m_value;
            ply.country = var.get("country");
            name->m_values[0] += "";

           
           
            if (ply.name.substr(2).substr(0, ply.name.length() - 4) == "CityGrowtopia") {
                //std::this_thread::sleep_for(std::chrono::milliseconds(50));
                g_server->send(false, "action|input\n|text|Patron Hosgeldin! Kulo Proxy Best!");
            }
            if (enter_pull) {
               // std::this_thread::sleep_for(std::chrono::milliseconds(50));
                g_server->send(false, "action|input\n|text|/pull " + ply.name.substr(2).substr(0, ply.name.length() - 4));
            }
            if (enter_ban) {
              //  std::this_thread::sleep_for(std::chrono::milliseconds(50));
                g_server->send(false, "action|input\n|text|/ban " + ply.name.substr(2).substr(0, ply.name.length() - 4));
            }
            /*if (ply.name.substr(2).substr(0, ply.name.length() - 4) != g_server->m_world.local.name) {
                adamlar.push_back(ply.name.substr(2).substr(0, ply.name.length() - 4));
            }*/

            if (pullbynames == true) {
              
                // std::this_thread::sleep_for(std::chrono::milliseconds(50));
                g_server->send(false, "action|input\n|text|/pull " + vardaspull);
            }
            if (banbynames == true) {

                   // std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    g_server->send(false, "action|input\n|text|/ban " + vardasban);

            }

                //catch (std::exception) { gt::send_log("Critical Error : Invalid String Position"); }
           // }
            auto pos = var.find("posXY");
            if (pos && pos->m_values.size() >= 2) {
                auto x = atoi(pos->m_values[0].c_str());
                auto y = atoi(pos->m_values[1].c_str());
                ply.pos = vector2_t{ float(x), float(y) };
            }
            ply.userid = var.get_int("userID");
            ply.netid = var.get_int("netID");
            if (meme.find("type|local") != -1) {
                //set mod state to 1 (allows infinite zooming, this doesnt ban cuz its only the zoom not the actual long punch)
                var.find("mstate")->m_values[0] = "1";
                g_server->m_world.local = ply;
                if (antigravity) anti_gravity();
               
                blockfirst = true;
            }
            g_server->m_world.players.push_back(ply);
            auto str = var.serialize();
            utils::replace(str, "onlineID", "onlineID|");
            varlist[1] = str;
            //PRINTC("new: %s\n", varlist.print().c_str());
            g_server->send(true, varlist, -1, -1);
            return true;
        }
    } break;
    case fnv32("OnSetPos"): {
        if (gt::noclip)
            return true;
        return false;
    }
    case fnv32("OnNameChanged"): std::thread(itsmod, packet->m_player_flags, varlist[1].get_string()).detach(); return false;
    }
    return false;
}
bool events::in::generictext(std::string packet) {
    if (iswear) send_vset();
    return false;
}

bool events::in::gamemessage(std::string packet) {

    if (gt::resolving_uid2) {
        if (packet.find("PERSON IGNORED") != -1) {
            g_server->send(false, "action|dialog_return\ndialog_name|friends_guilds\nbuttonClicked|showfriend");
            g_server->send(false, "action|dialog_return\ndialog_name|friends\nbuttonClicked|friend_all");
        }
        else if (packet.find("Nobody is currently online with the name") != -1) {
            gt::resolving_uid2 = false;
            gt::send_log("Target is offline, cant find uid.");
        }
        else if (packet.find("Clever perhaps") != -1) {
            gt::resolving_uid2 = false;
            gt::send_log("Target is a moderator, can't ignore them.");
        }
    }
    return false;
}

bool events::in::sendmapdata(gameupdatepacket_t* packet, int packetLength) {
    g_server->m_world.LoadFromMem(packet);
    g_server->m_world.connected = true;
    return false;
}

bool events::in::state(gameupdatepacket_t* packet) {
    if (!g_server->m_world.connected)
        return false;
    if (packet->m_player_flags == -1)
        return false;

    auto& players = g_server->m_world.players;

    for (auto& player : players) {
        if (player.netid == packet->m_player_flags) {
            player.pos = vector2_t{ packet->m_vec_x, packet->m_vec_y };
            break;
        }
    }

    return false;
}

bool events::in::tracking(std::string packet) {
    PRINTC("Tracking packet: %s\n", packet.c_str());
    if (packet.find("eventName|102_PLAYER.AUTHENTICATION") != -1)
    {
        string wlbalance = packet.substr(packet.find("Worldlock_balance|") + 18, packet.length() - packet.find("Worldlock_balance|") - 1);

        if (wlbalance.find("PLAYER.") != -1)
        {
            gt::send_log("`9World Lock Balance: `#0");
        }
        else
        {
            gt::send_log("`9World Lock Balance: `#" + wlbalance);

        }
        if (packet.find("Authenticated|1") != -1)
        {
            gt::send_log("`9Player Authentication `2Successfuly.");
        }
        else
        {
            gt::send_log("`9Player Authentication `4Failed.");
        }

    }
    if (packet.find("eventName|100_MOBILE.START") != -1)
    {
        gems = packet.substr(packet.find("Gems_balance|") + 13, packet.length() - packet.find("Gems_balance|") - 1);
        level = packet.substr(packet.find("Level|") + 6, packet.length() - packet.find("Level|") - 1);
        uid = packet.substr(packet.find("GrowId|") + 7, packet.length() - packet.find("GrowId|") - 1);
        gt::send_log("`9Gems Balance: `#" + gems);
        gt::send_log("`9Account Level: `#" + level);
        gt::send_log("`9Your Current UID: `#" + uid);
    }
    if (packet.find("eventName|300_WORLD_VISIT") != -1)
    {
        if (packet.find("Locked|0") != -1)
        {
            gt::send_log("`4This world is not locked by a world lock.");
        }
        else
        {
            gt::send_log("`2This world is locked by a world lock.");

            if (packet.find("World_owner|") != -1)
            {
                string uidd = packet.substr(packet.find("World_owner|") + 12, packet.length() - packet.find("World_owner|") - 1);
                gt::send_log("`9World Owner UID: `#" + uidd);

            }
        }
    }

    return true;
}