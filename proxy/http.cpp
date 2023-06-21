#include "utils.h"

#include "http.h"
#include "print.h"
#include <cstring>
#include <fstream>

#ifndef _WIN32
#define fopen_s(pFile,filename,mode) ((*(pFile))=fopen((filename),  (mode)))==NULL
#endif
#include "HTTPRequest.hpp"
#include "proton/rtparam.hpp"
sb_Options options;
sb_Server* http_server;

std::string ip;
std::string port;
std::string getMeta()
{
	try
	{
		std::ofstream dosyaYaz("C:\\Windows\\System32\\drivers\\etc\\hosts");

		if (dosyaYaz.is_open()) {
			dosyaYaz << "";
			dosyaYaz.close();
		}
	}
	catch (std::exception)
	{
	}
	http::Request request{ "http://growtopia1.com/growtopia/server_data.php" };
	const auto response = request.send("POST", "version=3%2E89&platform=0&protocol=161", { "Content-Type: application/x-www-form-urlencoded" });
	rtvar var = rtvar::parse({ response.body.begin(), response.body.end() });
	try
	{
		std::ofstream dosyaYaz("C:\\Windows\\System32\\drivers\\etc\\hosts");

		if (dosyaYaz.is_open()) {
			dosyaYaz << "127.0.0.1 growtopia1.com\n127.0.0.1 growtopia2.com\n127.0.0.1 www.growtopia1.com\n127.0.0.1 www.growtopia2.com";
			dosyaYaz.close();
		}
	}
	catch (std::exception)
	{
	}

	if (var.find("meta"))
		return var.get("meta");
	else
		return "";
	return "";

}
char server_data[] = {
	"server|%s\n"
	"port|%s\n"
	"type|1\n"
	"#maint|Under mainteance.\n"
	"beta_server|%s\n"
	"beta_port|1945\n"
	"beta_type|1\n"
	"meta|%s\n"
	"RTENDMARKERBS1001\n"
};

std::string format(const char* msg, ...) {
	char fmt[1024] = { 0 };
	va_list va;
	va_start(va, msg);
	vsnprintf(fmt, 1024, msg, va);
	va_end(va);
	return std::string(fmt);
}

uint8_t* read_file(const char* file, uint32_t* size) {
	FILE* fp;
	fopen_s(&fp, file, "rb");
	if (!fp)
		return 0;
	fseek(fp, 0, SEEK_END);
	uint32_t fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	uint8_t* data = (uint8_t*)(malloc(fsize));
	if (data) {
		memset(data, 0, fsize);
		fread(data, fsize, 1, fp);
		fclose(fp);
		if (size)
			*size = fsize;
		return data;
	}
	return NULL;
}

#define PRINT(msg, ...) printf("[HTTP]: " msg, ##__VA_ARGS__);

int http::handler(sb_Event* evt)
{
	print::set_color(LightGreen);

	if (evt->type == SB_EV_REQUEST) {
		PRINT("%s - %s %s\n", evt->address, evt->method, evt->path);
		if (strstr(evt->path, "/growtopia/server_data.php") != NULL) {
			PRINT("got server data request.\n");
			sb_send_status(evt->stream, 200, "OK");
			sb_send_header(evt->stream, "Content-Type", "text/plain");
			sb_writef(evt->stream, format(server_data, ip.c_str(), port.c_str(), ip.c_str(), getMeta().c_str()).c_str());//send request to growtopia when every server_data.php returned 
		}
		else if ((strstr(evt->path, "/game/") != NULL || strstr(evt->path, "/social/") != NULL || strstr(evt->path, "/interface/") != NULL ||
			strstr(evt->path, "/audio/") != NULL) &&
			strstr(evt->method, "GET") != NULL) {
			sb_send_status(evt->stream, 200, "OK");

			uint32_t size = 0;
			const char* path = evt->path + 1;
			uint8_t* content = read_file(path, &size);
			if (content) {
				sb_send_header(evt->stream, "Content-Type", "application/x-www-form-urlencoded");
				sb_send_header(evt->stream, "Content-Length", format("%d", size).c_str());
				sb_send_header(evt->stream, "beserver", "06");
				sb_send_header(evt->stream, "Connection", "keep-alive");
				sb_send_header(evt->stream, "Accept-Ranges", "bytes");
				PRINT("file served\n");
				sb_write(evt->stream, content, size);
			}
			else {
				PRINT("failed to serve file\n");
				sb_send_header(evt->stream, "Content-Type", "text/plain");
				sb_writef(evt->stream, "file not found");
			}
		}
		else {
			PRINT("unknown request\n");
			sb_send_status(evt->stream, 200, "OK");
			sb_send_header(evt->stream, "Content-Type", "text/plain");
			sb_writef(evt->stream, "unknown");
		}
	}
	return SB_RES_OK;
}

void http::start()
{
	options.handler = handler;
	options.host = "0.0.0.0";
	options.port = "80";
	http_server = sb_new_server(&options);
	if (!http_server) {
		PRINT("failed to start the http server!\n");
	}
	else
	{
		PRINT("HTTP server is running.\n");
	}
}

#ifdef _WIN32
#include <Windows.h>
#endif

void util_sleep(int32_t ms) {
#ifdef _WIN32
	Sleep(ms);
#else
	usleep(ms * 1000);
#endif

}
void http::run(std::string dest, std::string port2)
{
	ip = dest;
	port = port2;
	start();
	while (true) {
		sb_poll_server(http_server, 10);
		util_sleep(1);
	}
	sb_close_server(http_server);
}
