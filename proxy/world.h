#pragma once
#include "utils.h"
#include "player.h"
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include "gt.hpp"
using namespace std;
#define HashCoord(x, y) (((y) * 100) + (x))

struct TileHeader {
	uint16_t foreground;
	uint16_t background;
	uint16_t data;
	uint8_t flags_1;
	uint8_t flags_2;
};
struct DroppedItem {
	uint16_t itemID;
	vector2_t pos;
	uint8_t count;
	uint8_t flags;
	uint32_t uid;
};

struct Tile {
	TileHeader header;
	vector2i_t pos;
};
class ItemDataContainer {
public:
	ItemDataContainer() = default;
	static std::vector<std::string> arr, itd, itr, itt, itx, ity, ita, ith;
	bool RequiresTileExtra(int id)
	{
		int actionType = stoi(ita.at(id));
		return
			actionType == 2 ||
			actionType == 3 ||
			actionType == 10 || 
			actionType == 13 || 
			actionType == 19 || 
			actionType == 26 || 
			actionType == 33 || 
			actionType == 34 || 
			actionType == 36 || 
			actionType == 38 || 
			actionType == 40 || 
			actionType == 43 || 
			actionType == 46 ||
			actionType == 47 ||
			actionType == 49 ||
			actionType == 50 ||
			actionType == 51 || 
			actionType == 52 ||
			actionType == 53 ||
			actionType == 54 || 
			actionType == 55 || 
			actionType == 56 || 
			id == 2246 ||
			actionType == 57 || 
			actionType == 59 || 
			actionType == 61 ||
			actionType == 62 ||
			actionType == 63 || 
			id == 3760 ||
			actionType == 66 || 
			actionType == 67 || 
			actionType == 73 || 
			actionType == 74 ||
			actionType == 76 || 
			actionType == 78 || 
			actionType == 80 || 
			actionType == 81 ||
			actionType == 83 || 
			actionType == 84 ||
			actionType == 85 || 
			actionType == 86 || 
			actionType == 87 || 
			actionType == 88 || 
			actionType == 89 ||
			actionType == 91 || 
			actionType == 93 || 
			actionType == 97 || 
			actionType == 100 ||
			actionType == 101 ||
			actionType == 111 ||
			actionType == 113 ||
			actionType == 115 ||
			actionType == 116 ||
			actionType == 127 ||
			actionType == -126 ||
			actionType == 130 ||
			(id % 2 == 0 && id >= 5818 && id <= 5932) || false;
	}

};


class world {
public:
	std::string name{};
	std::vector<player> players{};
	player local{};
	bool connected{};

	int width = 0, height = 0, tileCount = 0, lastDroppedUid = 0;
	vector2i_t main_door_loc;
	std::unordered_map<uint32_t, Tile> tiles;
	std::unordered_map<uint32_t, TileHeader> wblocks;
	std::unordered_map<uint32_t, DroppedItem> objects;
	void LoadFromMem(gameupdatepacket_t* packet) {
		try
		{
			auto extended = utils::get_extended(packet);
			tiles.clear();
			tileCount = 0;
			height = 0;
			width = 0;
			main_door_loc = vector2i_t(-1, -1);
			int len = 0;
			name.resize(*reinterpret_cast<short*>(extended + 10));
			memcpy(reinterpret_cast<PVOID>(const_cast<char*>(name.data())), extended + 12, name.size());
			if (!name.empty()) {
				std::transform(name.begin(), name.end(), name.begin(), ::tolower);
				std::vector<TileHeader> rawTiles;
				extended += 12 + name.size();
				memcpy(&width, extended, 4);
				memcpy(&height, extended + 4, 4);
				memcpy(&tileCount, extended + 8, 4);
				extended += 12;
				for (int i = 0; i < tileCount; i++) {
					TileHeader header;
					memcpy(&header.foreground, extended, 2);
					memcpy(&header.background, extended + 2, 2);
					memcpy(&header.data, extended + 4, 2);
					memcpy(&header.flags_1, extended + 6, 1);
					memcpy(&header.flags_2, extended + 7, 1);
					extended += 8;
					if (header.data)
						extended += 2;
					if (header.foreground == 3760) {
						extended += 22; // bypass
						rawTiles.push_back(header);
						continue;
					}
					if (itemDataContainer.RequiresTileExtra(header.foreground)) {
						unsigned short len = 0;
						BYTE itemType = *extended;
						extended++;
						switch (itemType)
						{
						case 0:
						{

						}
						break;
						case 1:
						{
							len = (*(uint16_t*)(extended)); extended += 2;
							extended += len + 1;
						}
						break;
						case 0x4a: // Safe vault nothin extended
						break;
						case 2:
						{
							len = (*(uint16_t*)(extended)); extended += 2;
							extended += len + 4;
						}
						break;
						case 3:
						{
							extended++;
							byte adminCount = *(extended + 4);
							extended += (16 + (adminCount * 4));
						}
						break;
						case 4:
						{
							extended += 5;
						}
						break;
						case 0x8:
						{
							extended++;
						}
						break;
						case 0x9:
						{
							extended += 4;
						}
						break;
						case 0xa:
						{
							extended += 5;
						}
						break;
						case 0xb:
						{
							extended += 4;
							len = (*(uint16_t*)(extended)); extended += 2;
							extended += len;
						}
						break;
						case 0xe:
						{
							len = (*(uint16_t*)(extended)); extended += 2;
							extended += len;
							extended += 23;
						}
						break;
						case 0x0f:
						{
							extended++;
						}
						break;
						case 0x10:
						{
							extended++;
						}
						break;
						case 0x12:
						{
							extended += 5;
						}
						break;
						case 0x13:
						{
							extended += 18;
						}
						break;
						case 0x14:
						{
							len = (*(uint16_t*)(extended)); extended += 2;
							extended += len;
						}
						break;
						case 0x15:
						{
							len = (*(uint16_t*)(extended)); extended += 2;
							extended += len;
							extended += 5;
						}
						break;
						case 0x17:
						{
							extended += 4;
						}
						break;
						case 0x18:
						{
							extended += 8;
						}
						break;
						case 0x19:
						{
							extended++;
							int c = *(uint32_t*)(extended); extended += 4;
							extended += 4 * c;
						}
						break;
						case 0x1B:
						{
							extended += 4;
						}
						break;
						case 0x1C:
						{
							extended += 6;
						}
						break;
						case 0x20:
						{
							extended += 4;
						}
						break;
						case 0x21:
						{

							if (header.foreground == 3394)
							{
								len = (*(uint16_t*)(extended)); extended += 2;
								extended += len;
							}
						}
						break;
						case 0x23:
						{
							extended += 4;
							len = (*(uint16_t*)(extended)); extended += 2;
							extended += len;
						}
						break;
						case 0x25:
						{
							len = (*(uint16_t*)(extended)); extended += 2;
							extended += 32 + len;
						}
						break;
						case 0x27:
						{
							extended += 4;
						}
						break;
						case 0x28:
						{
							extended += 4;
						}
						break;
						case 0x2a: // Data Bedrock 
						break;
						case 0x2b:
						{
							extended += 16;
						}
						break;
						case 0x2c:
						{
							extended++;
							extended += 4;
							byte adminCount = *extended;
							extended += 4;
							extended += (adminCount * 4);

						}
						break;
						case 0x2f:
						{
							len = (*(uint16_t*)(extended)); extended += 2;
							extended += 5 + len;
						}
						break;
						case 0x30:
						{
							len = (*(uint16_t*)(extended)); extended += 2;
							extended += 26 + len;
						}
						break;
						case 0x31:
						{
							extended += 9;
						}
						break;
						case 0x32:
						{
							extended += 4;
						}
						break;
						case 0x34:
						{
						}
						break;
						case 0x36:
						{
							short itemsSize = (*(uint16_t*)(extended)); extended += 2;
							extended += itemsSize;
						}
						break;
						case 0x38:
						{
							len = (*(uint16_t*)(extended)); extended += 2;
							extended += 4 + len;
						}
						break;
						case 0x39:
						{
							extended += 4;
						}
						break;
						case 0x3a: // ad
						break;
						case 0x3e:
						{
							extended += 14;
						}
						break;
						case 0x3f:
						{
							uint32_t r = *(uint32_t*)(extended); extended += 4;
							extended += (r * 15);
							extended += 8;
						}
						break;
						case 0x41:
						{
							extended += 17;
						}
						break;
						case 0x42:
						{
							extended++;
						}
						break;
						case 0x49:
						{
							extended += 4;
						}
						break;
						default:
							len = 0;
							break;
						}

					}
					rawTiles.push_back(header);
				}
				for (int y = 0; y < height; y++) {
					for (int x = 0; x < width; x++) {
						int chash = HashCoord(x, y);
						Tile tile;
						tile.header = rawTiles[chash];
						tile.pos.m_x = x;
						tile.pos.m_y = y;
						tiles[chash] = tile;
						if (tile.header.foreground == 6) {
							main_door_loc = tile.pos;
						}
					}
				}

				int droppedCount;
				memcpy(&droppedCount, extended, 4);
				memcpy(&lastDroppedUid, extended + 4, 4);
				extended += 8;
				//gt::send_log("Count: " + to_string(droppedCount) + " last uid: " + to_string(lastDroppedUid));

				for (int i = 0; i < droppedCount; i++) {
					DroppedItem item;
					memcpy(&item.itemID, extended, 2);
					memcpy(&item.pos.m_x, extended + 2, 4);
					memcpy(&item.pos.m_y, extended + 6, 4);
					memcpy(&item.count, extended + 10, 1);
					memcpy(&item.flags, extended + 11, 1);
					memcpy(&item.uid, extended + 12, 4);
					extended += 16;
					objects[item.uid] = item;
				}
				connected = true;
			}
		}
		catch (const std::exception&)
		{
			gt::send_log("`4Unable To serialize this world");
		}
	}

	static ItemDataContainer itemDataContainer;
};