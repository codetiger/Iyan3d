
#ifndef TILE_H_
#define TILE_H_

struct Tile {
    int x, y;
    int width, height;
    double distance;
};

bool sortByDisance(const Tile &lhs, const Tile &rhs) {
	return lhs.distance < rhs.distance;
}

#endif
