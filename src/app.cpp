#include <stdexcept>
#include <SDL2/SDL.h>
#include "render/texture.cpp"
#include "tetromino.cpp"


#define VIEWABLE_FIELD_H 20
#define VIEWABLE_FIELD_X (FIELD_H - VIEWABLE_FIELD_H)

// ==============
// Resources

class Resources {
public:
    Texture texBlock;

    Resources(const Resources&) = delete;
    Resources(Resources&&) = default;
    ~Resources() = default;
};


void modulateSurface(SDL_Surface surface, int r, int g, int b) {

}

Resources loadResources(SDL_Renderer* renderer) {
    auto texBlock = Texture(renderer, "assets/textures/t-block-s.bmp");

    return Resources {
        std::move(texBlock)
    };
};

void destroyResources(SDL_Renderer* renderer, Resources* res) {

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
    return SDL_Point { x, y };
}

class App {
public:

    SDL_Window* window;
    SDL_Renderer* renderer;
    Resources resources;

    App() = delete;
    App(const App&) = delete;
    App(App&&) = default;
    ~App() = default;

    App(SDL_Window* window, SDL_Renderer* renderer, Resources res):
            window(window),
            renderer(renderer),
            resources(std::move(res))
    { }

    void drawTextureCopy(Texture& texture, SDL_Point point) {
        auto textureRect = texture.rect();
        SDL_Rect destRect = SDL_Rect { point.x, point.y, textureRect.w, textureRect.h };
        SDL_RenderCopy(this->renderer, texture.texture(), &textureRect, &destRect);
    }

    bool handleEvents() {
        bool continueLoop = true;

        SDL_Event event; // handle window closing
        if (SDL_PollEvent(&event) && (SDL_QUIT==event.type || (SDL_KEYDOWN==event.type && SDLK_ESCAPE==event.key.keysym.sym))) {
            continueLoop = false;
        }

        return continueLoop;
    }

    void drawState() {
        SDL_RenderClear(this->renderer);

        drawTextureCopy(this->resources.texBlock, point(0, 0));
        drawTextureCopy(this->resources.texBlock, point(100, 0));
        drawTextureCopy(this->resources.texBlock, point(200, 0));
        drawTextureCopy(this->resources.texBlock, point(0, 100));
        drawTextureCopy(this->resources.texBlock, point(100, 100));

        SDL_RenderPresent(this->renderer);
    }

    // While true: Do game loop
    bool tick() {
        bool continueLoop = handleEvents();
        drawState();

        auto error = SDL_GetError();
        if (error && strcmp(error, "") != 0) {
            printf("> SDL ERROR: %s\n", error);
        }

        return continueLoop;
    }
};

App Tetris_initApplication() {
    if(SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        throw std::runtime_error("Unable init SDL");
    }

    SDL_Window* window;
    SDL_Renderer* renderer;
    auto createWindow = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN, &window, &renderer);
    if(createWindow) {
        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        throw std::runtime_error("Unable create Window");
    }
    SDL_SetWindowTitle(window, "TetrisSDL");
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    auto resources = loadResources(renderer);

    return App(window, renderer, std::move(resources));
}

void Tetris_closeApplication(App* app) {
    destroyResources(app->renderer, &app->resources);

    SDL_DestroyWindow(app->window);
    app->window = NULL;

    SDL_DestroyRenderer(app->renderer);
    app->renderer = NULL;

    SDL_Quit();
}
