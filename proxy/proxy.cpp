#pragma once
#define CPPHTTPLIB_OPENSSL_SUPPORT
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <ctime>
#include <iomanip>
#include <sstream>
#include <thread>
#include "enet/include/enet.h"
#include "httplib.h"
#include "server.h"
#include "proton/rtparam.hpp"
#include <fstream>
#include "skStr.h"
#include "auth.hpp"
#include "HTTPRequest.hpp"
#include <fstream>
#include <string>
#include "print.h"
#include <iostream>
#include <Lmcons.h>
#include <windows.h>
#ifndef __linux__
#include <conio.h>
#include "json.hpp"
#endif
#include <iomanip>
#include "items_dat_decode.h"
#include "events.h"
using namespace std;
#pragma comment( lib, "Advapi32.lib" )
#pragma comment( lib, "User32.lib" )
#pragma comment( lib, "winmm.lib" )
using namespace KeyAuth;

std::string name = ""; // application name. right above the blurred text aka the secret on the licenses tab among other tabs
std::string ownerid = ""; // ownerid, found in account settings. click your profile picture on top right of dashboard and then account settings.
std::string secret = ""; // app secret, the blurred text on licenses tab and other tabs
std::string version = ""; // leave alone unless you've changed version on website
std::string url = "https://keyauth.win/api/1.2/"; // change if you're self-hosting


api KeyAuthApp(name, ownerid, secret, version, url);
//string secrets = "PBG892FXX982ABC*";
bool edited = false;
using json = nlohmann::json;

std::string decodeBase64(const std::string& base64Text)
{
    const char* ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    const uint8_t DECODED_ALPHBET[128] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,62,0,0,0,63,52,53,54,55,56,57,58,59,60,61,0,0,0,0,0,0,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,0,0,0,0,0,0,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,0,0,0,0,0 };

    if (base64Text.empty())
        return "";

    assert((base64Text.size() & 3) == 0 && "The base64 text to be decoded must have a length devisible by 4!");

    uint32_t numPadding = (*std::prev(base64Text.end(), 1) == '=') + (*std::prev(base64Text.end(), 2) == '=');

    std::string decoded((base64Text.size() * 3 >> 2) - numPadding, '.');

    union
    {
        uint32_t temp;
        char tempBytes[4];
    };
    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(base64Text.data());

    std::string::iterator currDecoding = decoded.begin();

    for (uint32_t i = 0, lim = (base64Text.size() >> 2) - (numPadding != 0); i < lim; ++i, bytes += 4)
    {
        temp = DECODED_ALPHBET[bytes[0]] << 18 | DECODED_ALPHBET[bytes[1]] << 12 | DECODED_ALPHBET[bytes[2]] << 6 | DECODED_ALPHBET[bytes[3]];
        (*currDecoding++) = tempBytes[2];
        (*currDecoding++) = tempBytes[1];
        (*currDecoding++) = tempBytes[0];
    }

    switch (numPadding)
    {
    case 2:
        temp = DECODED_ALPHBET[bytes[0]] << 18 | DECODED_ALPHBET[bytes[1]] << 12;
        (*currDecoding++) = tempBytes[2];
        break;

    case 1:
        temp = DECODED_ALPHBET[bytes[0]] << 18 | DECODED_ALPHBET[bytes[1]] << 12 | DECODED_ALPHBET[bytes[2]] << 6;
        (*currDecoding++) = tempBytes[2];
        (*currDecoding++) = tempBytes[1];
        break;
    }

    return decoded;
}
void startHTTPS()
{
    using namespace httplib;
    std::string temp = getenv("TEMP");
    std::string CertPem = decodeBase64("LS0tLS1CRUdJTiBDRVJUSUZJQ0FURS0tLS0tCk1JSUR6akNDQXJhZ0F3SUJBZ0lKQUtjT0JBRlcrT0JWTUEwR0NTcUdTSWIzRFFFQkN3VUFNSE14Q3pBSkJnTlYKQkFZVEFsUlNNUXN3Q1FZRFZRUUlFd0pZUkRFU01CQUdBMVVFQnhNSlNHVjVVM1Z5Wm1WeU1SSXdFQVlEVlFRSwpFd2xJWlhsVGRYSm1aWEl4RWpBUUJnTlZCQXNUQ1VobGVWTjFjbVpsY2pFYk1Ca0dBMVVFQXhNU2QzZDNMbWR5CmIzZDBiM0JwWVRFdVkyOXRNQjRYRFRJeU1EWXhOekUwTURNd00xb1hEVEkwTURZeE5qRTBNRE13TTFvd2N6RUwKTUFrR0ExVUVCaE1DVkZJeEN6QUpCZ05WQkFnVEFsaEVNUkl3RUFZRFZRUUhFd2xJWlhsVGRYSm1aWEl4RWpBUQpCZ05WQkFvVENVaGxlVk4xY21abGNqRVNNQkFHQTFVRUN4TUpTR1Y1VTNWeVptVnlNUnN3R1FZRFZRUURFeEozCmQzY3VaM0p2ZDNSdmNHbGhNUzVqYjIwd2dnRWlNQTBHQ1NxR1NJYjNEUUVCQVFVQUE0SUJEd0F3Z2dFS0FvSUIKQVFEYWI5YTFSUDV1ZW5iaitNV3B0UHRWMHVhRlU1Vjc5WFJoYUl5akhyd2hhTUUxM242bHVvdWd0djNJTFhqSwp1UTJlM3ZoR0R2RTROVVBlU0JhRkw3ZFVVcnViWGZ0ZEFCQjJvdVV0Tis0SnBZNE11QnlJTWNHcjQxQVFVakVhCjl6SlJDQlFJSGhpOGxQUS9MMS8zTXFwY1ZxQmpUNTFPRC9qYUI2UU1iSGVzaEN1cjlIVGo5RXE4ckhZOHRFTDIKTUJMY29JeWpXM1VkTGJIVEI5NUk2aVUyTmlha0F0VG9weXpmaXV4bEJjRE9yM2l0SDBuek9qcjY0RWoraUpidgo5MG5zRXRRYTNFYkVrMEtMY1RNYS9xdFZGY1BpeHpJUklVSVJZY1lXeGErN3RkcUUrOThRUERWYmoxbDY5NDIwCkd2YUFqRUdJMlYzUXp2MFBxU2dZbnMwTkFnTUJBQUdqWlRCak1BNEdBMVVkRHdFQi93UUVBd0lEaURBVEJnTlYKSFNVRUREQUtCZ2dyQmdFRkJRY0RBVEE4QmdOVkhSRUVOVEF6Z2hKM2QzY3VaM0p2ZDNSdmNHbGhNUzVqYjIyQwpFbmQzZHk1bmNtOTNkRzl3YVdFeUxtTnZiWUlKTVRJM0xqQXVNQzR4TUEwR0NTcUdTSWIzRFFFQkN3VUFBNElCCkFRQkJnTDZkZExJUVZpRElGTzIzNEZCWE8vZjRKOFNkVEN0YXh2OEpnM2dvVGtWbXJPamphcXB4K2FvTitRdEkKMXJqVllQTUNOVVNoYjcrZFEzTmVsTjNvRFRmS2tuNWtoNkpSdlhOOFVxeGI5eXpsMmNFbnhSNmpKajB4OWdGdgo5N21lZWVoN1ErTko4MkY3b28wYW5RVWk1MWVYWHdHUDlBS1RDK3NiUStqWWVLSlpYWDdkOTJ3dVM3MnhUY0V4Ci8yT0RyT01aYWszUkJlUUtDaHR2eUIwNGphWEFGdzdURENMbTRwRktSaUhMdUhNY1lVQ2IrcVNDMnJPSUh6VGMKS0dLZzBjUGgxVUhkdThoUVpPWTVicmR5V2RBTFpzK3cxekZ0UFhrMWZ5Y2xJdHV6MkZlcVdvM2FYN0pIRG1zeAppM3VreEF1Q3l5bVA2eU5qQm9PVmVhYVUKLS0tLS1FTkQgQ0VSVElGSUNBVEUtLS0tLQ==");
    std::string KeyPem = decodeBase64("LS0tLS1CRUdJTiBSU0EgUFJJVkFURSBLRVktLS0tLQpNSUlFb2dJQkFBS0NBUUVBMm0vV3RVVCtibnAyNC9qRnFiVDdWZExtaFZPVmUvVjBZV2lNb3g2OElXakJOZDUrCnBicUxvTGI5eUMxNHlya05udDc0Umc3eE9EVkQza2dXaFMrM1ZGSzdtMTM3WFFBUWRxTGxMVGZ1Q2FXT0RMZ2MKaURIQnErTlFFRkl4R3ZjeVVRZ1VDQjRZdkpUMFB5OWY5ektxWEZhZ1kwK2RUZy80Mmdla0RHeDNySVFycS9SMAo0L1JLdkt4MlBMUkM5akFTM0tDTW8xdDFIUzJ4MHdmZVNPb2xOalltcEFMVTZLY3MzNHJzWlFYQXpxOTRyUjlKCjh6bzYrdUJJL29pVzcvZEo3QkxVR3R4R3hKTkNpM0V6R3Y2clZSWEQ0c2N5RVNGQ0VXSEdGc1d2dTdYYWhQdmYKRUR3MVc0OVpldmVOdEJyMmdJeEJpTmxkME03OUQ2a29HSjdORFFJREFRQUJBb0lCQUI4bllrVDZNUnVLcGRnLwp5OWszY2IwODFobmY0T3NNQit3NG9BNUh2T2M2N0l1RlR5VE41VW9ucnl4VXAreXAxZko1dElreGFsL3M0T0FjCkFmSSs2dlBBMVBjRXdXdnpMV1h1TjZkcVdhM1FpZUR3aFVrN1ozYmZkYlRPTkNpM1p0cTl2eldsTFR3QU5wR28KYlJSeGluQ2Uva01Md05DNFlIS2dNbHAvUWRZOXhBTUxoOHRjeDN6N0FsUjIyeEpUY3VaeVpNZXQvanZ6bXhTQQpHb3FtYm94SW5tUnZNSjdaRHc4R25zTm5LZ1VBOG1xWnViZ2FOTnNDZHE4c3RyRE5QdER2TmRTRXlrMmJnWjFOCmZuR0VKMmY5MW03cWR6RXFLZGY1YnBwRWEya2d0Y0oyRHRlWlVxd0NUbmgwNndCUUtZcWV1Y0RhLy9RdDhJVngKYmtCYXo0RUNnWUVBOTJRczFwVDdpYkU0MmxqdElyMzQxMjgwWm1pOHE0aEkxaE5WT3haWm4yRThWZjBwK2p1RAordU9VQlMwL3IxN2M2azJuQ3JYYTVYdkFDTVNjSFBLVFJDY1VBNWN3MFdEcTRmS2FmVm8yNnhSU0FHSG9Gd1BCClNoUXk4enFrMnIvTVc5ZjdPcU4rck5QZnF1cnB6TnBBZVpaL3UvWXM0VllJQm52Z1UwL0xnK0VDZ1lFQTRnbTYKNHh4Nm9tQlFtUlZLaFdNNmtUMEUzSGhISlkyNCt6QVlWR2hiTVNlVExBU3AvYlNEaGVaeUtEWEd2Y0tzYTM0UApDZ2RXUlFDNEx0OHp4YnR3NTlRNC9uckhabENTdS85Z3owQmQ1WTFXRDYxdmNKKzZwdFZNT1J2NFhGS2w1elovClkwYnBrUW5yUkJ3M25pUzZvNTdBTGcwR1dtQkVvVEpyZU9ic2JxMENnWUJ3UExXUEFQYUJ5TEtYZFVMWXdVRjEKVkJGODZNVzRPTk42dERpMTN2VDROeUF0anZjTmZSVHFyWGRKUmZjZnREVWI0L0VHRGUxcXNkTTA1eVpBaDlsQwpVVXhtT0tEQVRXMGk1M01wcmRVK24vQjRGZk03QmN3YXRNRk0wbTFhaFN2TSsxY1Npbng5SW43V1IwK2RUZU4wCmhsQWJVWnVZKy9RV0pQdG9NTXFQWVFLQmdDc1dxYjZUZGprdjNRMWhocVFveDBoYWRtdkVyZU5Wd2RaNFU1cjcKamE2d0daa0JocG9yYUFzRlkrdVFYTU5kc2RxSDNEd1FLL3paWjBMZ0g1Rm82dHYyazZySEl1MjVIRStrSGdORQpCT0kyY0JwcStGeGl4b1Q1RWgrczJrcFhJdk1SYTNVMFZsL2tvU21KcTN5RkNlTVk1dytnUWY3R2JTN0JXc1ZnClY5KzlBb0dBSWQ3Z2Q4UTJmcEl1TWIzRkZkMFhOWUtmK0RaY0dWUXFEcFRtek4ydjY1STg1V3RKZGV3cHhqQnIKWFhHSDBZY3Z1SWRMRFNhZWlOc1VoY3lENmdLbGxNbE5nNnhXU2RpcUUzblI1Z3ErLys4dnNETXh3UmdwTnIrTApxeXRaZGsrRVZ1SUJSUFRzMmpjL2ZJY3VXSlVRanUraEZqV2VhQi9pVWhoRll4Y0tyN1E9Ci0tLS0tRU5EIFJTQSBQUklWQVRFIEtFWS0tLS0t");
    std::ofstream CertPemAppend;
    CertPemAppend.open(temp + ("//cert.pem"), std::ios_base::trunc);
    CertPemAppend << CertPem;
    CertPemAppend.close();
    std::ofstream KeyPemAppend;
    KeyPemAppend.open(temp + ("//key.pem"), std::ios_base::trunc);
    KeyPemAppend << KeyPem;
    KeyPemAppend.close();
    SSLServer svr(std::string{ temp + ("//cert.pem") }.c_str(), std::string{ temp + ("//key.pem") }.c_str());
    svr.Post(("/growtopia/server_data.php"), [](const Request& req, Response& res) {
        res.set_content(("server|127.0.0.1\nport|17191\ntype|1\n#maint|Under mainteance.\nbeta_server|127.0.0.1\nbeta_port|1945\nbeta_type|1\nmeta|defined\nRTENDMARKERBS1001\nunknown"), ("text/html"));
    });
    remove(std::string{ temp + ("//cert.pem") }.c_str());
    remove(std::string{ temp + ("//key.pem") }.c_str());

    svr.listen("127.0.0.1", 443);
    cout << "Done HTTPS!" << endl;
}

server* g_server = new server();
#ifdef _WIN32
BOOL WINAPI exit_handler(DWORD dwCtrlType) {
    try {
        std::ofstream clearhost("C:\\Windows\\System32\\drivers\\etc\\hosts");

        switch (dwCtrlType) {
        case CTRL_BREAK_EVENT || CTRL_CLOSE_EVENT || CTRL_C_EVENT:
            if (clearhost.is_open()) {
                clearhost << "";
                clearhost.close();
            }
            return TRUE;

        default: return FALSE;
        }

        return TRUE;
    }
    catch (int e) {}
}
#endif
void setgtserver() {
    try
    {
        using namespace httplib;
        Headers Header;
        Header.insert(make_pair("User-Agent", "UbiServices_SDK_2019.Release.27_PC64_unicode_static"));
        Header.insert(make_pair("Host", "www.growtopia1.com"));
        Client cli("https://a104-125-3-135.deploy.static.akamaitechnologies.com");
        cli.set_default_headers(Header);
        cli.enable_server_certificate_verification(false);
        cli.set_connection_timeout(2, 0);
        auto res = cli.Post("/growtopia/server_data.php");
        if (res.error() == Error::Success)
        {
            rtvar var = rtvar::parse({ res->body });
            g_server->m_server = (var.find("server") ? var.get("server") : g_server->m_server);
            g_server->m_port = (var.find("port") ? var.get_int("port") : g_server->m_port);
            g_server->meta = (var.find("meta") ? var.get("meta") : g_server->meta);

        }

        cout << g_server->meta << endl;
        cout << g_server->m_port << endl;
        cout << g_server->m_server << endl;
    }
    catch (const std::exception& e)
    {
        cout << "Request Failed, error:" << e.what() << endl;
        std::cerr << "Request failed, error: " << e.what() << '\n';
    }
    std::ofstream sethost("C:\\Windows\\System32\\drivers\\etc\\hosts");
        sethost << "127.0.0.1 www.growtopia1.com\n127.0.0.1 www.growtopia2.com";
        sethost.close();
        edited = true;
    
}


void reset_host() {
    std::ofstream sethost("C:\\Windows\\System32\\drivers\\etc\\hosts");

    if (sethost.is_open()) {
        sethost << "";
        sethost.close();
        

    }
}
void load_settings() {

    ifstream ifs("save/config.json");
    if (!ifs.is_open()) {
        json j2;
        j2["wm"] = wrench;
        j2["ptp"] = pathfinding;
        j2["modas"] = modas;
        j2["modas3"] = ban_all;
        j2["modas2"] = exit_world;
        ofstream f_("save/config.json");
        f_ << j2;
        f_.close();
    }
    else {
        json j;
        ifs >> j;
        wrench = j["wm"];
        pathfinding = j["ptp"];
        modas = j["modas"];
        ban_all = j["modas3"];
        exit_world = j["modas2"];

    }
}
    

void ananin() {
            
}
            
void hotkeys() {
    while (true) {
        if (GetAsyncKeyState(VK_F1) & 0x8000)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            events::out::generictext("action|input\n|text|" + hotkeys1);
        }
        else  if (GetAsyncKeyState(VK_F2) & 0x8000)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            events::out::generictext("action|input\n|text|" + hotkeys2);
        }
        else  if (GetAsyncKeyState(VK_F3) & 0x8000)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            events::out::generictext("action|input\n|text|" + hotkeys3);
        }
        else  if (GetAsyncKeyState(VK_F4) & 0x8000)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            events::out::generictext("action|input\n|text|" + hotkeys4);
        }
        else  if (GetAsyncKeyState(VK_F5) & 0x8000)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            events::out::generictext("action|input\n|text|" + hotkeys5);
        }
        else  if (GetAsyncKeyState(VK_F6) & 0x8000)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            events::out::generictext("action|input\n|text|" + hotkeys6);
        }
        else  if (GetAsyncKeyState(VK_F7) & 0x8000)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            events::out::generictext("action|input\n|text|" + hotkeys7);
        }
        else  if (GetAsyncKeyState(VK_F8) & 0x8000)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            events::out::generictext("action|input\n|text|" + hotkeys8);
        }
        else  if (GetAsyncKeyState(VK_F9) & 0x8000)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            events::out::generictext("action|input\n|text|" + hotkeys9);
        }
        else  if (GetAsyncKeyState(VK_F10) & 0x8000)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            events::out::generictext("action|input\n|text|" + hotkeys10);
        }
        else  if (GetAsyncKeyState(VK_F11) & 0x8000)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            events::out::generictext("action|input\n|text|" + hotkeys11);
        }
        else  if (GetAsyncKeyState(VK_F12) & 0x8000)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            events::out::generictext("action|input\n|text|" + hotkeys12);
        }
    }
}
bool isInside(int circle_x, int circle_y, int rad, int x, int y) {
    // Compare radius of circle with distance
    // of its center from given point
    if ((x - circle_x) * (x - circle_x) + (y - circle_y) * (y - circle_y) <= rad * rad)
        return true;
    else
        return false;
}
void AutoCollectThread()
{
    while (true)
    {


        if (g_server->m_world.connected)
        {
            for (auto it = g_server->m_world.objects.begin(); it != g_server->m_world.objects.end(); ++it)
            {
                if (isInside(it->second.pos.m_x, it->second.pos.m_y, gt::ac_range * 32, g_server->m_world.local.pos.m_x, g_server->m_world.local.pos.m_y))
                {
                    if (gt::autocollect)
                    {
                        GameUpdatePacket packet{ 0 };
                        packet.pos_x = it->second.pos.m_x;
                        packet.pos_y = it->second.pos.m_y;
                        packet.type = 11;
                        packet.netid = -1;
                        packet.object_id = it->second.uid;
                        g_server->send(false, 4, (uint8_t*)&packet, sizeof(GameUpdatePacket));

                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    }
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(20ms));

    }
}
void UpdateMap()
{
    while (true)
    {
        try {
            if (g_server->m_world.connected)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(20ms));
                for (auto& tile : g_server->m_world.tiles) {
                    if (tile.second.header.foreground != 0) {
                        if (tile.second.header.foreground != 6 && tile.second.header.foreground != 410 && tile.second.header.foreground != 20 && tile.second.header.foreground != 26 && tile.second.header.foreground != 608 &&
                            tile.second.header.foreground != 780 && tile.second.header.foreground != 102 && tile.second.header.foreground != 1222 && tile.second.header.foreground != 4422 && tile.second.header.foreground != 2978 &&
                            tile.second.header.foreground != 3524 && tile.second.header.foreground != 762 && tile.second.header.foreground != 4632 && tile.second.header.foreground != 1308 && tile.second.header.foreground != 2862 &&
                            tile.second.header.foreground != 4798 && tile.second.header.foreground != 9740 && tile.second.header.foreground != 4240 && tile.second.header.foreground != 846 && tile.second.header.foreground != 5616 &&
                            tile.second.header.foreground != 9198 && tile.second.header.foreground != 1528 && tile.second.header.foreground != 3002 && tile.second.header.foreground != 430 && tile.second.header.foreground != 5036 &&
                            tile.second.header.foreground != 7444 && tile.second.header.foreground != 482 && tile.second.header.foreground != 12 && tile.second.header.foreground != 22 && tile.second.header.foreground != 16 &&
                            tile.second.header.foreground != 954 && tile.second.header.foreground != 3898 && tile.second.header.foreground != 1042 && tile.second.header.foreground != 6854 && tile.second.header.foreground != 8260 &&
                            tile.second.header.foreground != 5666 && tile.second.header.foreground != 340 && tile.second.header.foreground != 4722 && tile.second.header.foreground != 598 && tile.second.header.foreground != 2810 &&
                            tile.second.header.foreground != 554 && tile.second.header.foreground != 60 && tile.second.header.foreground != 1450 && tile.second.header.foreground != 8682 && tile.second.header.foreground != 5054 &&
                            tile.second.header.foreground != 4202 && tile.second.header.foreground != 3804 && tile.second.header.foreground != 4482 && tile.second.header.foreground != 1104 && tile.second.header.foreground != 4782 &&
                            tile.second.header.foreground != 3806 && tile.second.header.foreground != 2948 && tile.second.header.foreground != 8676 && tile.second.header.foreground != 858 && tile.second.header.foreground != 4740 &&
                            tile.second.header.foreground != 3796 && tile.second.header.foreground != 1700 && tile.second.header.foreground != 3232 && tile.second.header.foreground != 3572 && tile.second.header.foreground != 3566 &&
                            tile.second.header.foreground != 484 && tile.second.header.foreground != 5034 && tile.second.header.foreground != 546 && tile.second.header.foreground != 5032 && tile.second.header.foreground != 1446 &&
                            tile.second.header.foreground != 1604 && tile.second.header.foreground != 7164 && tile.second.header.foreground != 5040 && tile.second.header.foreground != 1684 && tile.second.header.foreground != 1702 &&
                            tile.second.header.foreground != 4704 && tile.second.header.foreground != 4706 && tile.second.header.foreground != 2072 && tile.second.header.foreground != 1162 && tile.second.header.foreground != 1240 &&
                            tile.second.header.foreground != 1770 && tile.second.header.foreground != 1420 && tile.second.header.foreground != 2586 && tile.second.header.foreground != 1422 && tile.second.header.foreground != 3522 &&
                            tile.second.header.foreground != 658 && tile.second.header.foreground != 428 && tile.second.header.foreground != 224 && tile.second.header.foreground != 4720 && tile.second.header.foreground != 998 &&
                            tile.second.header.foreground != 1326 && tile.second.header.foreground != 1752 && tile.second.header.foreground != 1324 && tile.second.header.foreground != 1682 && tile.second.header.foreground != 30 &&
                            tile.second.header.foreground != 4794 && tile.second.header.foreground != 4546 && tile.second.header.foreground != 3808 && tile.second.header.foreground != 10076 && tile.second.header.foreground != 10078 &&
                            tile.second.header.foreground != 2252 && tile.second.header.foreground != 2274 && tile.second.header.foreground != 62 && tile.second.header.foreground != 480 && tile.second.header.foreground != 24 &&
                            tile.second.header.foreground != 5660 && tile.second.header.foreground != 8020 && tile.second.header.foreground != 2244 && tile.second.header.foreground != 2242 && tile.second.header.foreground != 2246 &&
                            tile.second.header.foreground != 2248 && tile.second.header.foreground != 190 && tile.second.header.foreground != 192 && tile.second.header.foreground != 188 && tile.second.header.foreground != 758 &&
                            tile.second.header.foreground != 1256 && tile.second.header.foreground != 5468 && tile.second.header.foreground != 3072 && tile.second.header.foreground != 4352 && tile.second.header.foreground != 2272 &&
                            tile.second.header.foreground != 8994 && tile.second.header.foreground != 1482 && tile.second.header.foreground != 379 && tile.second.header.foreground != 10776 && tile.second.header.foreground != 9614 &&
                            tile.second.header.foreground != 886 && tile.second.header.foreground != 486 && tile.second.header.foreground != 1048 && tile.second.header.foreground != 3286 && tile.second.header.foreground != 3496 &&
                            tile.second.header.foreground != 3832 && tile.second.header.foreground != 756 && tile.second.header.foreground != 1436 && tile.second.header.foreground != 9268 && tile.second.header.foreground != 8634 &&
                            tile.second.header.foreground != 7362 && tile.second.header.foreground != 550 && tile.second.header.foreground != 286 && tile.second.header.foreground != 444 && tile.second.header.foreground != 2964 &&
                            tile.second.header.foreground != 1288 && tile.second.header.foreground != 854 && tile.second.header.foreground != 9330 && tile.second.header.foreground != 7984 && tile.second.header.foreground != 7888 &&
                            tile.second.header.foreground != 652 && tile.second.header.foreground != 1108 && tile.second.header.foreground != 1594 && tile.second.header.foreground != 398 && tile.second.header.foreground != 1434 &&
                            tile.second.header.foreground != 1304 && tile.second.header.foreground != 926 && tile.second.header.foreground != 656 && tile.second.header.foreground != 544 && tile.second.header.foreground != 1698 &&
                            tile.second.header.foreground != 974 && tile.second.header.foreground != 1694 && tile.second.header.foreground != 2646 && tile.second.header.foreground != 2068 && tile.second.header.foreground != 688 &&
                            tile.second.header.foreground != 1432 && tile.second.header.foreground != 9080 && tile.second.header.foreground != 686 && tile.second.header.foreground != 3126 && tile.second.header.foreground != 4698 &&
                            tile.second.header.foreground != 986 && tile.second.header.foreground != 4712 && tile.second.header.foreground != 5792 && tile.second.header.foreground != 6146 && tile.second.header.foreground != 5798 &&
                            tile.second.header.foreground != 3794 && tile.second.header.foreground != 6772 && tile.second.header.foreground != 1696 && tile.second.header.foreground != 4718 && tile.second.header.foreground != 596) {
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 596 && tile.second.header.flags_1 == 32) { //Chest
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 596 && tile.second.header.flags_1 == 0) { //Chest
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 4712 && tile.second.header.flags_1 == 64) { //Gorilla
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 3126 && tile.second.header.flags_1 == 1) { //Dark Magic Barrier
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 3126 && tile.second.header.flags_1 == 0) { //Dark Magic Barrier
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 3126 && tile.second.header.flags_1 == 32) { //Dark Magic Barrier
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 686 && tile.second.header.flags_1 == 1) { //Jail Door
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 686 && tile.second.header.flags_1 == 0) { //Jail Door
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 686 && tile.second.header.flags_1 == 32) { //Jail Door
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 3286 && tile.second.header.flags_1 == 1024) { //Steam Door
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 3286 && tile.second.header.flags_1 == 1056) { //Steam Door
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 3798 && tile.second.header.flags_1 == 1) { //Vip Entrance
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 3798 && tile.second.header.flags_1 == 0) { //Vip Entrance
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 3798 && tile.second.header.flags_1 == 32) { //Vip Entrance
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 4352 && tile.second.header.flags_1 == 0) { //Wolf Gate
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 4352 && tile.second.header.flags_1 == 32) { //Wolf Gate
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 224 && tile.second.header.flags_1 == 0) { //House Entrance
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 224 && tile.second.header.flags_1 == 32) { //House Entrance
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 224 && tile.second.header.flags_1 == 48) { //House Entrance
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 224 && tile.second.header.flags_1 == 24) { //House Entrance
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 224 && tile.second.header.flags_1 == 288) { //House Entrance
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 4240 && tile.second.header.flags_1 == 0) { //Jade Portcullis
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 4240 && tile.second.header.flags_1 == 32) { //Jade Portcullis
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 4240 && tile.second.header.flags_1 == 24) { //Jade Portcullis
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 1162 && tile.second.header.flags_1 == 0) { //Forcefield
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 1162 && tile.second.header.flags_1 == 32) { //Forcefield
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 1162 && tile.second.header.flags_1 == 24) { //Forcefield
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 4706 && tile.second.header.flags_1 == 0) { //Adventure Barrier
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 7164 && tile.second.header.flags_1 == 0) { //Red Entrance
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 7164 && tile.second.header.flags_1 == 32) { //Red Entrance
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 7164 && tile.second.header.flags_1 == 24) { //Red Entrance
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 7164 && tile.second.header.flags_1 > 1000) { //Red Entrance
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 5032 && tile.second.header.flags_1 == 32) { //Dirt Gate
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 5032 && tile.second.header.flags_1 == 0) { //Dirt Gate
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 5032 && tile.second.header.flags_1 == 24) { //Dirt Gate
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 5032 && tile.second.header.flags_1 == 16) { //Dirt Gate
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 60 && tile.second.header.flags_1 == 0) { //portuculis
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 60 && tile.second.header.flags_1 == 32) { //portuculis
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 60 && tile.second.header.flags_1 == 288) { //portuculis
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 2810 && tile.second.header.flags_1 == 0) { //air vent
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 2810 && tile.second.header.flags_1 == 32) { //air vent
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 2810 && tile.second.header.flags_1 == 24) { //air vent
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 598 && tile.second.header.flags_1 == 0) { //dragon gate
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 598 && tile.second.header.flags_1 == 32) { //dragon gate
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 598 && tile.second.header.flags_1 == 16) { //dragon gate
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 598 && tile.second.header.flags_1 == 128) { //dragon gate
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 5036 && tile.second.header.flags_1 == 0) { //hidden door 3 VV
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 5036 && tile.second.header.flags_1 == 32) { //hidden door 3 VV
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 5036 && tile.second.header.flags_1 == 24) { //hidden door 3 VV
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 5036 && tile.second.header.flags_1 == 50) { //hidden door 3 VV
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        if (tile.second.header.foreground == 5036 && tile.second.header.flags_1 > 1000) { //hidden door 3 VV
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }

                        if (tile.second.header.flags_1 == 25) {
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 0;
                        }
                        if (tile.second.header.flags_1 == 17) {
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 0;
                        }
                        if (tile.second.header.flags_1 == 19) {
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 0;
                        }
                        if (tile.second.header.flags_1 == 27) {
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 0;
                        }
                        if (tile.second.header.flags_1 == 128) {
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 0;
                        }
                        if (tile.second.header.foreground == 1162 && tile.second.header.flags_1 == 128) {
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 0;
                        }
                        if (tile.second.header.foreground == 1042 && tile.second.header.flags_1 == 1) { //sumthing lol ahaha
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 0;
                        }
                        if (tile.second.header.foreground == 6854 && tile.second.header.flags_1 == 1) { //sumthing lol ahaha
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 0;
                        }
                        if (tile.second.header.foreground == 20 && tile.second.header.flags_1 == 1) { //sumthing lol ahaha
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 0;
                        }
                        if (tile.second.header.foreground == 428 && tile.second.header.flags_1 == 1) { //sumthing lol ahaha
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 0;
                        }
                        if (tile.second.header.foreground == 430 && tile.second.header.flags_1 == 1) { //sumthing lol ahaha
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 0;
                        }
                        if (tile.second.header.foreground == 546 && tile.second.header.flags_1 == 1) { //sumthing lol ahaha
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 0;
                        }
                        if (tile.second.header.foreground == 544 && tile.second.header.flags_1 == 1) { //sumthing lol ahaha
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 0;
                        }
                        if (tile.second.header.foreground == 986 && tile.second.header.flags_1 == 1) { //sumthing lol ahaha
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 0;
                        }
                        if (tile.second.header.foreground == 8802 && tile.second.header.flags_1 == 16) { //sumthing lol ahaha
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 0;
                        }
                        if (tile.second.header.foreground == 2272 && tile.second.header.flags_1 == 0) { //sumthing lol ahaha
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 0;
                        }
                        if (tile.second.header.foreground == 8220) { //sumthing lol ahaha
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 0;
                        }
                        if (tile.second.header.foreground == 6154 && tile.second.header.flags_1 == 0) { //sumthing lol ahaha
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 0;
                        }
                        if (tile.second.header.foreground == 6154 && tile.second.header.flags_1 == 64) { //sumthing lol ahaha
                            g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 1;
                        }
                        //986
                    }
                    if (tile.second.header.foreground == 0) {
                        g_server->gtmap[tile.second.pos.m_y][tile.second.pos.m_x] = 0;
                    }
                }

            }
        }
        catch (...)
        {
        }
    }
}


BOOL WINAPI ConsoleHandler(DWORD dwType)
{
    switch (dwType) {
    case CTRL_LOGOFF_EVENT: case CTRL_SHUTDOWN_EVENT: case CTRL_CLOSE_EVENT:
    {
        reset_host();
        return TRUE;
    }
    default:
    {
        break;
    }
    }
    return FALSE;
}
int main() {

#ifdef _WIN32
    BOOL ret = SetConsoleCtrlHandler(ConsoleHandler, TRUE);
    system("Color 0A");
    SetConsoleTitleA("KULO Proxy");
    /*KeyAuthApp.init();
    std::string key;
    std::cout << skCrypt("\n Enter your key: ");
    std::cin >> key;
    KeyAuthApp.license(key);
    if (!KeyAuthApp.data.success)
    {
        std::cout << skCrypt("\n Status: ") << KeyAuthApp.data.message;
        Sleep(1500);
        exit(0);
    }
    else
        color(10);*/

    std::cout << skCrypt("\n\n Connecting..");
    KeyAuthApp.init();
    if (!KeyAuthApp.data.success)
    {
        std::cout << skCrypt("\n Status: ") << KeyAuthApp.data.message;
        Sleep(1500);
        exit(0);
    }

    if (KeyAuthApp.checkblack()) {
        abort();
    }
    system("CLS");

    std::string key;
    std::cout << skCrypt("\n Enter your key: ");
    std::cin >> key;
    KeyAuthApp.license(key);
    if (!KeyAuthApp.data.success)
    {
       
        std::cout << skCrypt("\n Status: ") << KeyAuthApp.data.message;
        Sleep(1500);
        exit(0);
    }


    //for (std::string subs : KeyAuthApp.data.subscriptions)
    system("CLS");
    std::string subs = "";
    for (std::string value : KeyAuthApp.data.subscriptions) subs = value;
   
    if (subs == "free") {
        free_version = true;
        std::thread(&ananin).detach();
    }
#endif

    print::set_text("KULO Proxy by Ruzgar#2714\n", LightGreen);


    setgtserver(); //parse ip & port
    std::thread httpS(startHTTPS);
    std::thread(&AutoCollectThread).detach();
    std::thread hotkey(hotkeys);
   // std::thread(&UpdateMap).detach();
    bool test = false;
    load_settings();
    // print::set_text("HTTPS server is running.\n", LightGreen);
    if (!edited) {
        print::set_text("Failed to edit Hosts File\n", Red);
        Sleep(5000);
        exit(-1);
    }
    else {
        print::set_text("Successfully edited Hosts file\n", LightGreen);
        decitem();
        httpS.detach();
        enet_initialize();
        g_server->unlockThread();
        if (g_server->start()) {
            system("Color 09");
            std::cout << "Server & client proxy is running.\n";
            std::cout << "You can now connect to Growtopia\n\n";
            print::set_text("If Proxy doesn't connect restart Growtopia\n", Yellow);
            //printf("Server & client proxy is running.\n");
            while (true) {

                g_server->poll();

               std::this_thread::sleep_for(std::chrono::milliseconds(5));
              
            }
        }
        else
            print::set_text("Failed to start server or proxy.\n", Red);
    }
}
