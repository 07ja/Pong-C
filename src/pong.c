#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define FPS 60
#define FRAME_TARGET_TIME (1000 / FPS)

int isRunning = 0;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

int lastFrameTime = 0;
float delta = 0;
int p1Score = 0;
int p2Score = 0;
float paddleSpeed = 600.0f;
float ballSpeed = 500.0f;
float pongVelX = 0.0f;
float pongVelY = 0.0f;

typedef struct {
    float x, y, width, height;
} Paddle;

typedef struct {
    float x, y, width, height;
} Pong;

Paddle p1, p2;
Pong pong;

int initialize_window(void);
void setup(void);
void process_input(void);
void update(void);
void render(void);
void destroy_window(void);
void reset_ball(void);

int main() {
    printf("\033[H\033[J");
    printf("Welcome to Pong made using C and SDL3\n");

    printf("\nPlayer 1: Up (W), Down (S)\n");
    printf("Player 2: Up (O), Down (L)\n");
    printf("Pong: Start Game (G)\n");

    printf("\nScore: Player 1    Player 2\n");

    isRunning = initialize_window();
    setup();

    while(isRunning) {
        process_input();
        update();
        render();
    }

    destroy_window();
    printf("\n");
    return 0;
}

int initialize_window(void) {
    // Initialize Video
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
        return 0;
    }

    // Create Window
    window = SDL_CreateWindow("Pong", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
    if (!window) {
        fprintf(stderr, "Error creating SDL window: %s\n", SDL_GetError());
        return 0;
    }

    // Set Window Opacity
    if (!SDL_SetWindowOpacity(window, 0.90f)) {
        fprintf(stderr, "Failed to set opacity: %s\n", SDL_GetError());
    }

    // Create Renderer
    renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        fprintf(stderr, "Error creating SDL renderer: %s\n", SDL_GetError());
        return 0;
    }
    return 1;
}

void setup(void) {
    p1.x = 20;
    p1.y = 20;
    p1.width = 10;
    p1.height = 50;

    p2.x = WINDOW_WIDTH - 30;
    p2.y = WINDOW_HEIGHT - 70;
    p2.width = 10;
    p2.height = 50;

    pong.x = WINDOW_WIDTH / 2;
    pong.y = WINDOW_HEIGHT / 2;
    pong.width = 10;
    pong.height = 10;
}

void process_input(void) {
    // If 'X' at Top Left is Pressed
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            isRunning = 0;
        }
    }

    // Quit if Escape is Pressed
    const bool* keystate = SDL_GetKeyboardState(NULL);
    if (keystate[SDL_SCANCODE_ESCAPE]) {
        isRunning = 0;
    }

    // Player 1 Presses 'W' Move Up
    if (keystate[SDL_SCANCODE_W]) {
        p1.y -= paddleSpeed * delta;
    }

    // Player 1 Presses 'S' Move Down
    if (keystate[SDL_SCANCODE_S]) {
        p1.y += paddleSpeed * delta;
    }

    // Player 2 Presses 'O' Move Up
    if (keystate[SDL_SCANCODE_O]) {
        p2.y -= paddleSpeed * delta;
    }

    // Player 2 Presses 'L' Move Down
    if (keystate[SDL_SCANCODE_L]) {
        p2.y += paddleSpeed * delta;
    }

    // Launch Pong
    if (keystate[SDL_SCANCODE_G] && pongVelX == 0 && pongVelY == 0) {
        float angle = (rand() % 360) * (M_PI / 180.0f);
        pongVelX = ballSpeed * cosf(angle);
        pongVelY = ballSpeed * sinf(angle);
    }
}

void update(void) {
    // Capping frame rate
    int timeToWait = FRAME_TARGET_TIME - (SDL_GetTicks() - lastFrameTime);
    if (timeToWait > 0 && timeToWait <= FRAME_TARGET_TIME) {
        SDL_Delay(timeToWait);
    }

    // Consistent Movement 
    delta = (SDL_GetTicks() - lastFrameTime) / 1000.0f;
    if (delta > 0.033f) delta = 0.033f;
    lastFrameTime = SDL_GetTicks();

    // Bound p1 to Window
    if (p1.y < 0) p1.y = 0;
    if (p1.y + p1.height > WINDOW_HEIGHT) p1.y = WINDOW_HEIGHT - p1.height;

    // Bound p2 to Window
    if (p2.y < 0) p2.y = 0;
    if (p2.y + p2.height > WINDOW_HEIGHT) p2.y = WINDOW_HEIGHT - p2.height;

    // Pong Movement
    pong.x += pongVelX * delta;
    pong.y += pongVelY * delta;

    // Bounce Off Top and Bottom
    if (pong.y <= 0 || pong.y + pong.height >= WINDOW_HEIGHT) {
        pongVelY *= -1;
    }

    // Collision With p1
    if (pong.x < p1.x + p1.width && pong.x + pong.width > p1.x &&
        pong.y < p1.y + p1.height && pong.y + pong.height > p1.y) {
        
        pong.x = p1.x + p1.width;
        pongVelX *= -1;
    
        float hitPos = (pong.y + pong.height / 2) - (p1.y + p1.height / 2);
        float norm = hitPos / (p1.height / 2);
        pongVelY = norm * ballSpeed;
    }
    
    // Collision With p2
    if (pong.x + pong.width > p2.x && pong.x < p2.x + p2.width &&
        pong.y < p2.y + p2.height && pong.y + pong.height > p2.y) {
        
        pong.x = p2.x - pong.width;
        pongVelX *= -1;
    
        float hitPos = (pong.y + pong.height / 2) - (p2.y + p2.height / 2);
        float norm = hitPos / (p2.height / 2);
        pongVelY = norm * ballSpeed;
    }

    // Player 1 Scores
    if (pong.x > WINDOW_WIDTH) {
        p1Score += 1;
        reset_ball();
    }
    
    // Player 2 Scores
    if (pong.x < 0) {
        p2Score += 1;
        reset_ball();
    }
}

void render(void) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Render Player 1
    SDL_FRect p1Rect = {p1.x, p1.y, p1.width, p1.height};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &p1Rect);

    // Render Player 2
    SDL_FRect p2Rect = {p2.x, p2.y, p2.width, p2.height};
    SDL_RenderFillRect(renderer, &p2Rect);

    // Render Pong
    SDL_FRect pongRect = {pong.x, pong.y, pong.width, pong.height};
    SDL_RenderFillRect(renderer, &pongRect);

    SDL_RenderPresent(renderer);
}

void destroy_window(void) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}


void reset_ball(void) {
    pong.x = WINDOW_WIDTH / 2;
    pong.y = WINDOW_HEIGHT / 2;
    pongVelX = 0;
    pongVelY = 0;
    printf("\r\033[K");
    printf("%8d %11d", p1Score, p2Score);
    fflush(stdout);
}
