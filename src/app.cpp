#include <stdexcept>
#include <SDL2/SDL.h>
#include "render/texture.cpp"
#include "tetromino.cpp"


#define VIEWABLE_FIELD_H 20
#define VIEWABLE_FIELD_X (FIELD_H - VIEWABLE_FIELD_H)

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// [COLORS]
#define COL_WHITE SDL_Color { 255, 255, 255, 255 }
#define COL_BLACK SDL_Color { 0, 0, 0, 255 }
#define COL_GRAY SDL_Color { 128, 128, 128, 255 }
#define COL_GRAY_LIGHT SDL_Color { 192, 192, 192, 255 }
#define COL_GRAY_DARK SDL_Color { 64, 64, 64, 255 }

#define COL_RED SDL_Color { 255, 0, 0, 255 }
#define COL_ORANGE SDL_Color { 255, 128, 0, 255 }
#define COL_YELLOW SDL_Color { 255, 255, 0, 255 }
#define COL_LIME SDL_Color { 128, 255, 0, 255 }
#define COL_GREEN SDL_Color { 0, 255, 0, 255 }
#define COL_GREEN_CYAN SDL_Color { 0, 255, 128, 255 }
#define COL_CYAN SDL_Color { 0, 255, 255, 255 }
#define COL_BLUE_LIGHT SDL_Color { 0, 128, 255, 255 }
#define COL_BLUE SDL_Color { 0, 0, 255, 255 }
#define COL_PURPLE SDL_Color { 128, 0, 255, 255 }
#define COL_PINK SDL_Color { 255, 0, 255, 255 }
#define COL_PINK_DARK SDL_Color { 255, 0, 128, 255 }

// ==============
// Resources

class Resources {
public:
    Texture texBlock;

    Texture menuNewGame;
    Texture menuCleaningLine;
    Texture menuCleaningColor;
    Texture menuExtraTilesOff;
    Texture menuExtraTilesOn;
    Texture menuExit;


    Resources(const Resources&) = delete;
    Resources(Resources&&) = default;
    ~Resources() = default;
};


void modulateSurface(SDL_Surface surface, int r, int g, int b) {

}

Resources loadResources(SDL_Renderer* renderer) {
    auto texBlock = Texture(renderer, "assets/textures/t-block-s.bmp");

    auto menuNewGame = Texture(renderer, "assets/textures/menu/new-game.bmp");
    auto menuCleaningLine = Texture(renderer, "assets/textures/menu/cleaning-line.bmp");
    auto menuCleaningColor = Texture(renderer, "assets/textures/menu/cleaning-color.bmp");
    auto menuExtraTilesOff = Texture(renderer, "assets/textures/menu/extra-tiles-off.bmp");
    auto menuExtraTilesOn = Texture(renderer, "assets/textures/menu/extra-tiles-on.bmp");
    auto menuExit = Texture(renderer, "assets/textures/menu/exit.bmp");

    return Resources {
        std::move(texBlock),

        std::move(menuNewGame),
        std::move(menuCleaningLine),
        std::move(menuCleaningColor),
        std::move(menuExtraTilesOff),
        std::move(menuExtraTilesOn),
        std::move(menuExit)
    };
};

void destroyResources(SDL_Renderer* renderer, Resources* res) {

}

// ===========
// InputState

class KeyState {
private:
    bool f_isDown;
    bool f_isPressed;

public:
    KeyState() {
        this->f_isDown = false;
        this->f_isPressed = false;
    }

    bool isDown() {
        return this->f_isDown;
    }

    bool isUp() {
        return !this->f_isDown;
    }

    bool isPressed() {
        return this->f_isPressed;
    }

    void press() {
        if (this->f_isDown) {
            this->f_isDown = true;
        } else {
            this->f_isDown = true;
            this->f_isPressed = true;
        }
    }

    void release() {
        this->f_isDown = false;
        this->f_isPressed = false;
    }

    void update() {
        if (this->f_isPressed) {
            this->f_isPressed = false;
        }
    }
};

class InputState {
public:
    KeyState keyR; // right
    KeyState keyU; // isUp
    KeyState keyL; // left
    KeyState keyD; // isDown
    KeyState keyAction; // z or space
    KeyState keyBack; // x or esc
    bool exitRequired;

    InputState():
            keyR(KeyState()),
            keyU(KeyState()),
            keyL(KeyState()),
            keyD(KeyState()),
            keyAction(KeyState()),
            keyBack(KeyState()),
            exitRequired(false) {}
};

// =============
// Application



SDL_Point point(int x, int y) {
    return SDL_Point { x, y };
}

enum AppState { menu = 0, game = 1 };

enum MenuElement { newGame = 0, cleaningMode = 1, extraTiles = 2, quit = 3 };

enum CleaningMode { line = 0, color = 1 };

enum ExtraTilesMode { off = 0, on = 1 };

class App {
public:

    SDL_Window* window;
    SDL_Renderer* renderer;
    Resources resources;

    AppState state;
    InputState input;

    // =================
    // [menu state part]

    MenuElement menuElement;
    CleaningMode cleaningMode;
    ExtraTilesMode extraTilesMode;

    // [menu state part]
    // =================
    // [game state part]

    // [game state part]
    // =================

    App() = delete;
    App(const App&) = delete;
    App(App&&) = default;
    ~App() = default;

    App(SDL_Window* window, SDL_Renderer* renderer, Resources res):
            window(window),
            renderer(renderer),
            resources(std::move(res)),
            state(AppState::menu),

            // [menu]
            menuElement(MenuElement::newGame),
            cleaningMode(CleaningMode::line),
            extraTilesMode(ExtraTilesMode::off)
    { }

    void drawTextureCopyColored(Texture& texture, SDL_Point point, SDL_Color color) {
        auto textureRect = texture.rect();
        SDL_Rect destRect = SDL_Rect { point.x, point.y, textureRect.w, textureRect.h };

        SDL_SetTextureColorMod(texture.sldHandle(), color.r, color.g, color.b);
        SDL_RenderCopy(this->renderer, texture.sldHandle(), &textureRect, &destRect);
    }

    void drawTextureCopy(Texture& texture, SDL_Point point) {
        drawTextureCopyColored(texture, point, SDL_Color { 255, 255, 255, 255});
    }

    void updateInput() {
        {
            this->input.keyAction.update();
            this->input.keyBack.update();
            this->input.keyR.update();
            this->input.keyU.update();
            this->input.keyL.update();
            this->input.keyD.update();
        }

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                this->input.exitRequired = true;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_z || event.key.keysym.sym == SDLK_SPACE || event.key.keysym.sym == SDLK_RETURN) { this->input.keyAction.press(); }
                if (event.key.keysym.sym == SDLK_x || event.key.keysym.sym == SDLK_ESCAPE) { this->input.keyBack.press(); }
                if (event.key.keysym.sym == SDLK_RIGHT) { this->input.keyR.press(); }
                if (event.key.keysym.sym == SDLK_UP) { this->input.keyU.press(); }
                if (event.key.keysym.sym == SDLK_LEFT) { this->input.keyL.press(); }
                if (event.key.keysym.sym == SDLK_DOWN) { this->input.keyD.press(); }
            } else if (event.type == SDL_KEYUP) {
                if (event.key.keysym.sym == SDLK_z || event.key.keysym.sym == SDLK_SPACE || event.key.keysym.sym == SDLK_RETURN) { this->input.keyAction.release(); }
                if (event.key.keysym.sym == SDLK_x || event.key.keysym.sym == SDLK_ESCAPE) { this->input.keyBack.release(); }
                if (event.key.keysym.sym == SDLK_RIGHT) { this->input.keyR.release(); }
                if (event.key.keysym.sym == SDLK_UP) { this->input.keyU.release(); }
                if (event.key.keysym.sym == SDLK_LEFT) { this->input.keyL.release(); }
                if (event.key.keysym.sym == SDLK_DOWN) { this->input.keyD.release(); }
            } else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
                this->input.keyAction.release();
                this->input.keyBack.release();
                this->input.keyR.release();
                this->input.keyU.release();
                this->input.keyL.release();
                this->input.keyD.release();
            }
        }
    }

    void updateStateMenu() {
        if (this->input.keyBack.isPressed()) {
            this->input.exitRequired = true;
        }

        if (this->input.keyAction.isPressed()) {
            switch (this->menuElement) {
                case MenuElement::newGame:
                    this->state = AppState::game;
                    break;
                case MenuElement::cleaningMode:
                    this->cleaningMode = static_cast<CleaningMode>((this->cleaningMode + 1) % 2);
                    break;
                case MenuElement::extraTiles:
                    this->extraTilesMode = static_cast<ExtraTilesMode>((this->extraTilesMode + 1) % 2);
                    break;
                case MenuElement::quit:
                    this->input.exitRequired = true;
                    break;
                default:
                    printf("UNREACHABLE\n");
                    exit(1);
            }
        }

        if (this->input.keyD.isPressed()) {
            this->menuElement = static_cast<MenuElement>(this->menuElement < 3 ? this->menuElement + 1 : 3);
        }
        if (this->input.keyU.isPressed()) {
            this->menuElement = static_cast<MenuElement>(this->menuElement > 0 ? this->menuElement - 1 : 0);
        }
    }

    void updateStateGame() {
        if (this->input.keyBack.isPressed()) {
            this->state = AppState::menu;
        }
    }

    // Game logic
    void updateState() {
        switch (this->state) {
            case AppState::menu:
                this->updateStateMenu();
                break;
            case AppState::game:
                this->updateStateGame();
                break;
//            default:
//                printf("UNREACHABLE\n");
//                exit(1);
        }
    }

    void drawMenu() {
        auto activeColor = COL_WHITE;
        auto unActiveColor = COL_GRAY;

        auto baseX = (SCREEN_WIDTH - 256) / 2;
        auto baseY = (SCREEN_HEIGHT - 32 * 4) / 2;

        {
            auto color = this->menuElement == MenuElement::newGame ? activeColor : unActiveColor;
            this->drawTextureCopyColored(this->resources.menuNewGame, point(baseX, baseY + 32*0), color);
        }

        {
            auto color = this->menuElement == MenuElement::cleaningMode ? activeColor : unActiveColor;
            if (this->cleaningMode == CleaningMode::line) {
                this->drawTextureCopyColored(this->resources.menuCleaningLine, point(baseX, baseY + 32*1), color);
            }
            if (this->cleaningMode == CleaningMode::color) {
                this->drawTextureCopyColored(this->resources.menuCleaningColor, point(baseX, baseY + 32*1), color);
            }
        }

        {
            auto color = this->menuElement == MenuElement::extraTiles ? activeColor : unActiveColor;
            if (this->extraTilesMode == ExtraTilesMode::off) {
                this->drawTextureCopyColored(this->resources.menuExtraTilesOff, point(baseX, baseY + 32*2), color);
            }
            if (this->extraTilesMode == CleaningMode::color) {
                this->drawTextureCopyColored(this->resources.menuExtraTilesOn, point(baseX, baseY + 32*2), color);
            }
        }

        {
            auto color = this->menuElement == MenuElement::quit ? activeColor : unActiveColor;
            this->drawTextureCopyColored(this->resources.menuExit, point(baseX, baseY + 32*3), color);
        }
    }

    void drawGame() {
        SDL_Color colors[13] {
                COL_WHITE,
                COL_RED,
                COL_ORANGE,
                COL_YELLOW,
                COL_LIME,
                COL_GREEN,
                COL_GREEN_CYAN,
                COL_CYAN,
                COL_BLUE_LIGHT,
                COL_BLUE,
                COL_PURPLE,
                COL_PINK,
                COL_PINK_DARK
        };

        int x = 0;
        for(int i = 0; i < 13; i++) {
            drawTextureCopyColored(this->resources.texBlock, point(x, 0), colors[i]);
            x += 16;
        }
    }

    void drawState() {
        SDL_RenderClear(this->renderer);

        switch (this->state) {
            case AppState::menu:
                this->drawMenu();
                break;
            case AppState::game:
                this->drawGame();
                break;
        }

        SDL_RenderPresent(this->renderer);
    }

    // While true: Do game loop
    bool tick() {
        updateInput();
        updateState();
        drawState();

        auto error = SDL_GetError();
        if (error && strcmp(error, "") != 0) {
            printf("> SDL ERROR: %s\n", error);
        }

        return !this->input.exitRequired;
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
