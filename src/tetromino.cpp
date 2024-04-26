#include <optional>
#include <stdexcept>

#define FIELD_W 10
#define FIELD_H 24


// =================================================
// [ Плитка (1 плитка, не фигура!!!) и поле плиток ]

enum TetroTile {
    red,
    orange,
    yellow,
    green,
    blue,
    violet,
    //
    white,
    black
};
const TetroTile BASE_TILES[6] = {red, orange, yellow, green, blue, violet };

class TetroField {
private:
    std::optional<TetroTile> tiles[FIELD_H][FIELD_W];

public:
    std::optional<std::optional<TetroTile>*> get(int x, int y) {
        if (x < 0 || x >= FIELD_W) { return std::optional<std::optional<TetroTile>*>(); }
        if (y < 0 || y >= FIELD_H) { return std::optional<std::optional<TetroTile>*>(); }
        return std::optional<std::optional<TetroTile>*>(&this->tiles[x][y]);
    }

    std::optional<TetroTile>* getAssured(int x, int y) {
        auto r = this->get(x, y);
        if(r.has_value()) {
            return r.value();
        } else {
            throw std::out_of_range("");
        }
    }

    void set(int x, int y, std::optional<TetroTile> tile) {
        auto memCell = this->getAssured(x, y);
        *memCell = tile;
    }

};

// ===================================
// [ Падающая фигура из нескольких плиток! ]

enum TetroRotateMode {
    rotate, flip, none
};

enum TetroShapeClass {
    I, L, J, T, S, Z, O
};


// Center:
// .  .  .  .
// .  .  .  .
// .  C  .  .
// .  .  .  .

class TetroShapePrototype {
public:
    int tilesCount;
    int offsetsX[16];
    int offsetsY[16];
    TetroRotateMode rotateMode;
    TetroTile tile;
};

class TetroActiveShape {
public:
    int x, y;
    TetroShapePrototype prototype;


};
