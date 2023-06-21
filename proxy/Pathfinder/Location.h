#pragma once
struct Location {
    int posx;
    int posy;
    bool operator==(const Location& other) const {
        return posx == other.posx && posy == other.posy;
    }
};



