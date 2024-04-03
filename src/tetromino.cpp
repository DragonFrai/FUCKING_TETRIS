#include <optional>
#include <stdexcept>

#define FIELD_W 10
#define FIELD_H 24


enum TetroColor {
    white,
    red,
    yellow,
    green,
    blue,
    violet,
    black
};



class TetroTile {
public:
    TetroColor color;

};


enum TetroRotateMode {
    rotate
};

class TetroField {
    std::optional<TetroTile> tiles[FIELD_H][FIELD_W];

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

class TetroShape {
public:
    bool mask[4][4];
    int x;
    int y;

};

class TetroState {
    TetroField field;
    std::optional<TetroShape> activeShape;
};
