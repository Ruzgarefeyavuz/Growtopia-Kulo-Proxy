#include "gt.hpp"
#include "packet.h"
#include "server.h"
#include "utils.h"
#include "Pathfinder/PathFinder.h"

std::string gt::version = "3.99";
std::string gt::flag = "mm";
bool gt::resolving_uid2 = false;
bool gt::connecting = false;
bool gt::in_game = false;
bool gt::ghost = false;
bool gt::resolving_uid = false;
bool gt::aapbypass = false;
bool gt::ruzgar = false;
bool gt::noclip = false;
bool gt::doublejump = false;
bool gt::autocollect = false;
int gt::ac_range = 10;
int gt::path_amount = 0;
string gt::hotkey1 = "";
string gt::hotkey2 = "";
string gt::hotkey3 = "";
string gt::hotkey4 = "";
string gt::hotkey5 = "";
string gt::hotkey6 = "";
string gt::hotkey7 = "";
string gt::hotkey8 = "";
string gt::hotkey9 = "";
string gt::hotkey10 = "";
string gt::hotkey11 = "";
string gt::hotkey12 = "";
bool gt::noclip2 = false;
int gt::block_id = 0;
int gt::max_dropped_block = 0;
extern int max_dropped_block;
bool gt::showxy = false;
std::string gt::macaddr = "16:69:f9:t6:ga:6b";

void gt::send_log(std::string text) {
    g_server->send(true, "action|log\nmsg|" + text, NET_MESSAGE_GAME_MESSAGE);
}

void gt::solve_captcha(std::string text) {
    //Get the sum :D
    utils::replace(text,
        "set_default_color|`o\nadd_label_with_icon|big|`wAre you Human?``|left|206|\nadd_spacer|small|\nadd_textbox|What will be the sum of the following "
        "numbers|left|\nadd_textbox|",
        "");
    utils::replace(text, "|left|\nadd_text_input|captcha_answer|Answer:||32|\nend_dialog|captcha_submit||Submit|", "");
    auto number1 = text.substr(0, text.find(" +"));
    auto number2 = text.substr(number1.length() + 3, text.length());
    int result = atoi(number1.c_str()) + atoi(number2.c_str());
    gt::send_log("Solved captcha as `2" + std::to_string(result) + "``");
    g_server->send(false, "action|dialog_return\ndialog_name|captcha_submit\ncaptcha_answer|" + std::to_string(result));
}
void TpRetard(float x, float y)
{
    vector2_t pos;
    pos.m_x = x * 32;
    pos.m_y = y * 32;
    variantlist_t varlist{ "OnSetPos" };
    varlist[1] = pos;
    g_server->m_world.local.pos = pos;
    g_server->send(true, varlist, g_server->m_world.local.netid, -1);
}

void gt::findpath(int x, int y)
{
    typedef pair<int, int> Pair;
    Pair src((int)g_server->m_world.local.pos.m_x / 32, (int)g_server->m_world.local.pos.m_y / 32);
    Pair dest((int)x, (int)y);
    Pathfinder* pathfinder = new Pathfinder(100, 60);
    for (int i = 0; i < pathfinder->width; i++) {
        for (int j = 0; j < pathfinder->height; j++) {
            pathfinder->nodes->insert(std::pair<Pathfinder::Point, int>(Pathfinder::Point(i, j), g_server->gtmap[j][i]));
        }
    }
    std::vector<Pathfinder::Point> path = pathfinder->findPath(Pathfinder::Point(src.first, src.second), Pathfinder::Point(dest.first, dest.second));
    if (path.size() < 35) {
        for (int i = 0; i < path.size(); i++) {
            GameUpdatePacket packet{ 0 };
            packet.type = PACKET_STATE;
            packet.int_data = 2250;
            packet.int_x = path[i].x;
            packet.int_y = path[i].y;
            packet.pos_x = path[i].x * 32;
            packet.pos_y = path[i].y * 32;
            packet.flags = 32 | (1 << 10) | (1 << 11);
            g_server->send(false, NET_MESSAGE_GAME_PACKET, (uint8_t*)&packet, sizeof(GameUpdatePacket));
           
        }
    }
    else {
        for (int i = 0; i < path.size(); i++) {
            GameUpdatePacket packet{ 0 };
            packet.type = PACKET_STATE;
            packet.int_data = 2250;
            packet.int_x = path[i].x;
            packet.int_y = path[i].y;
            packet.pos_x = path[i].x * 32;
            packet.pos_y = path[i].y * 32;
            packet.flags = 32 | (1 << 10) | (1 << 11);

            g_server->send(false, NET_MESSAGE_GAME_PACKET, (uint8_t*)&packet, sizeof(GameUpdatePacket));
        }
    }
    gt::path_amount = 0;
}