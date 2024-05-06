#include <optional>
#include <stdexcept>

#define FIELD_W 10
#define FIELD_H 24


// =================================================
// [ Плитка (1 плитка, не фигура!!!) и поле плиток ]

enum TetroColor {
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
const TetroColor BASE_TILES[6] = {red, orange, yellow, green, blue, violet };

// TODO: Add class
using TetroTile = TetroColor;

class TetroField {
private:
    std::optional<TetroColor> tiles[FIELD_W][FIELD_H];

public:
    std::optional<std::optional<TetroColor>*> get(int x, int y) {
        if (x < 0 || x >= FIELD_W) { return std::optional<std::optional<TetroColor>*>(); }
        if (y < 0 || y >= FIELD_H) { return std::optional<std::optional<TetroColor>*>(); }
        return std::optional<std::optional<TetroColor>*>(&this->tiles[x][y]);
    }

    std::optional<TetroColor>* getAssured(int x, int y) {
        auto r = this->get(x, y);
        if(r.has_value()) {
            return r.value();
        } else {
            throw std::out_of_range("Out of field range");
        }
    }

    void set(int x, int y, std::optional<TetroColor> tile) {
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
    I, L, J, T, S, Z, O,
    // Extra
    Dot
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
    TetroColor color;

    TetroShapePrototype(const TetroShapePrototype& other) {
        this->tilesCount = other.tilesCount;
        std::copy(std::begin(other.offsetsX), std::end(other.offsetsX), this->offsetsX);
        std::copy(std::begin(other.offsetsY), std::end(other.offsetsY), this->offsetsY);
        this->rotateMode = other.rotateMode;
        this->color = other.color;
    }

    TetroShapePrototype(TetroShapeClass clazz, TetroColor color) {
        this->color = color;
        switch(clazz) {
            case TetroShapeClass::I:
                this->tilesCount = 4;
                this->rotateMode = TetroRotateMode::flip;
                this->offsetsX[0] = 0;
                this->offsetsY[0] = -2;
                this->offsetsX[1] = 0;
                this->offsetsY[1] = -1;
                this->offsetsX[2] = 0;
                this->offsetsY[2] = 0;
                this->offsetsX[3] = 0;
                this->offsetsY[3] = 1;
                break;
            case TetroShapeClass::J:
                this->tilesCount = 4;
                this->rotateMode = TetroRotateMode::rotate;
                this->offsetsX[0] = 0;
                this->offsetsY[0] = -1;
                this->offsetsX[1] = 0;
                this->offsetsY[1] = 0;
                this->offsetsX[2] = 0;
                this->offsetsY[2] = 1;
                this->offsetsX[3] = -1;
                this->offsetsY[3] = 1;
                break;
            case TetroShapeClass::L:
                this->tilesCount = 4;
                this->rotateMode = TetroRotateMode::rotate;
                this->offsetsX[0] = 0;
                this->offsetsY[0] = -1;
                this->offsetsX[1] = 0;
                this->offsetsY[1] = 0;
                this->offsetsX[2] = 0;
                this->offsetsY[2] = 1;
                this->offsetsX[3] = 1;
                this->offsetsY[3] = 1;
                break;
            case TetroShapeClass::O:
                this->tilesCount = 4;
                this->rotateMode = TetroRotateMode::none;
                this->offsetsX[0] = 0;
                this->offsetsY[0] = 0;
                this->offsetsX[1] = 0;
                this->offsetsY[1] = 1;
                this->offsetsX[2] = 1;
                this->offsetsY[2] = 0;
                this->offsetsX[3] = 1;
                this->offsetsY[3] = 1;
                break;
            case TetroShapeClass::S:
                this->tilesCount = 4;
                this->rotateMode = TetroRotateMode::rotate;
                this->offsetsX[0] = 1;
                this->offsetsY[0] = 0;
                this->offsetsX[1] = 0;
                this->offsetsY[1] = 0;
                this->offsetsX[2] = 0;
                this->offsetsY[2] = 1;
                this->offsetsX[3] = -1;
                this->offsetsY[3] = 1;
                break;
            case TetroShapeClass::Z:
                this->tilesCount = 4;
                this->rotateMode = TetroRotateMode::rotate;
                this->offsetsX[0] = -1;
                this->offsetsY[0] = -1;
                this->offsetsX[1] = -1;
                this->offsetsY[1] = 0;
                this->offsetsX[2] = 0;
                this->offsetsY[2] = 0;
                this->offsetsX[3] = 0;
                this->offsetsY[3] = 1;
                break;
            case TetroShapeClass::T:
                this->tilesCount = 4;
                this->rotateMode = TetroRotateMode::rotate;
                this->offsetsX[0] = 0;
                this->offsetsY[0] = 0;
                this->offsetsX[1] = -1;
                this->offsetsY[1] = 0;
                this->offsetsX[2] = 1;
                this->offsetsY[2] = 0;
                this->offsetsX[3] = 0;
                this->offsetsY[3] = 1;
                break;

            // EXTRA
            case TetroShapeClass::Dot:
                this->tilesCount = 1;
                this->rotateMode = TetroRotateMode::none;
                this->offsetsX[0] = 0;
                this->offsetsY[0] = 0;
                break;
        }
    }
};

class TetroActiveShape {
public:
    int x, y;
    TetroShapePrototype prototype;

    TetroActiveShape(const TetroActiveShape& other): x(other.x), y(other.y), prototype(other.prototype) {}
    TetroActiveShape(int x, int y, TetroShapePrototype prototype): x(x), y(y), prototype(prototype) {}

};
