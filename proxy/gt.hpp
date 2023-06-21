#pragma once
#include <string>
#include "packet.h"

namespace gt {
    extern std::string version;
    extern std::string flag;
    extern std::string macaddr;
    extern int block_id;
    extern int max_dropped_block;
  extern bool showxy;
    extern int ac_range;
    extern bool resolving_uid2;
    extern bool connecting;
    extern bool aapbypass;
    extern int path_amount;
    extern bool resolving_uid;
    extern bool resolving_uid2;
    extern bool in_game;
    extern bool ghost;
    extern bool noclip;
    extern bool noclip2;
    extern bool doublejump;
    extern bool autocollect;
    extern std::string hotkey1;
    extern std::string hotkey2;
    extern std::string hotkey3;
    extern std::string hotkey4;
    extern std::string hotkey5;
    extern std::string hotkey6;
    extern std::string hotkey7;
    extern std::string hotkey8;
    extern std::string hotkey9;
    extern std::string hotkey10;
    extern std::string hotkey11;
    extern std::string hotkey12;

    extern bool ruzgar;
    void send_log(std::string text);
    void solve_captcha(std::string text);
     void findpath(int x, int y);
}
