#include <stdexcept>
#include <SDL2/SDL.h>
#include "tetromino.cpp"


#define VIEWABLE_FIELD_H 20
#define VIEWABLE_FIELD_X (FIELD_H - VIEWABLE_FIELD_H)

// ==============
// Resources

struct Resources {
public:
    SDL_Surface* texBlock;
};

SDL_Surface* loadTexture(const char* path) {
    auto tex = SDL_LoadBMP(path);
    if (tex == NULL) {
        printf("Unable load texture: %s!\nSDL Error: %s\n", path, SDL_GetError());
        throw std::runtime_error("Error on load texture");
    }
    return tex;
}

void modulateSurface(SDL_Surface surface, int r, int g, int b) {

}

Resources loadResources() {
    auto texBlock = loadTexture("assets/textures/t-block-s.bmp");

    SDL_DuplicateSurface()

    return Resources {
        texBlock
    };
};

void destroyResources(Resources* res) {
    SDL_FreeSurface(res->texBlock);
    res->texBlock = NULL;
}

// ===========
// InputState

class KeyState {
public:
    bool pressed;
    bool justPressed;

    KeyState() {
        this->pressed = false;
        this->justPressed = false;
    }

    void update(bool nowPressed) {
        if (nowPressed) {
            if (!this->pressed) {
                this->pressed = true;
                this->justPressed = true;
            } else if (this->justPressed) {
                this->justPressed = false;
            }
        } else {
            this->pressed = false;
            this->justPressed = false;
        }
    }
};

class InputState {
public:
    KeyState r_state; // right
    KeyState u_state; // up
    KeyState l_state; // left
    KeyState d_state; // down
    KeyState action_state;
};

// =============
// Application

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Point point(int x, int y) {
    return SDL_Point { x, y};
}

class App {
public:

    SDL_Window* window;
    SDL_Surface* windowSurface;
    Resources resources;

    App(SDL_Window* window, SDL_Surface* windowSurface, Resources res) {
        this->window = window;
        this->windowSurface = windowSurface;
        this->resources = res;
    }

    void blitSurface(SDL_Surface* src, SDL_Point point) {
        int destW = std::min(src->clip_rect.w, this->windowSurface->clip_rect.w - point.x);
        int destH = std::min(src->clip_rect.h, this->windowSurface->clip_rect.h - point.y);
        SDL_Rect destRect = SDL_Rect { point.x, point.y, destW, destH };
        SDL_BlitSurface(src, NULL, this->windowSurface, &destRect);
    }

    void updateSurface() {
        SDL_UpdateWindowSurface(this->window);
    }

    void handleEvents() {
        blitSurface(this->resources.texBlock, point(0, 0));
        blitSurface(this->resources.texBlock, point(10, 0));
        blitSurface(this->resources.texBlock, point(20, 0));
        blitSurface(this->resources.texBlock, point(0, 10));
        blitSurface(this->resources.texBlock, point(10, 10));
    }

    void drawState() {

    }

    // While true: Do game loop
    bool tick() {
        handleEvents();
        drawState();
        updateSurface();
        return true;
    }
};

App Tetris_initApplication() {
    if(SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        throw std::runtime_error("Unable init SDL");
    }

    auto window = SDL_CreateWindow("TetrisSDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    if( window == NULL ) {
        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        throw std::runtime_error("Unable create Window");
    }

    auto windowSurface = SDL_GetWindowSurface(window);

    auto resources = loadResources();

    return App(window, windowSurface, resources);
}

void Tetris_closeApplication(App* app) {
    destroyResources(&app->resources);

    SDL_DestroyWindow(app->window);
    app->windowSurface = NULL;
    app->window = NULL;

    SDL_Quit();
}
