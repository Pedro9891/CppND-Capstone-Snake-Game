#ifndef UTILS_H
#define UTILS_H

#include <SDL.h>
#include <vector>
#include <algorithm>

inline bool operator==(const SDL_Point &a, const SDL_Point &b) {
    return a.x == b.x && a.y == b.y;
}

inline bool operator!=(const SDL_Point &a, const SDL_Point &b) {
    return a.x != b.x || a.y != b.y;
}

inline bool CheckCollision(const std::vector<SDL_Point> &pointList, const SDL_Point &otherPoint) {
    for (const SDL_Point &point : pointList){
        if (point == otherPoint)
            return true;
    }
        
    return false;
}

#endif