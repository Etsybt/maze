#include "SDL.h"
#include <iostream>
#include <cmath>

const int MAP_WIDTH = 10;
const int MAP_HEIGHT = 10;
int map[MAP_WIDTH][MAP_HEIGHT] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 1, 1, 1, 1, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 1, 0, 1, 1, 0, 1, 0, 1},
    {1, 0, 1, 0, 1, 1, 0, 1, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 1, 1, 1, 1, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

void castRays(SDL_Renderer* renderer, float playerX, float playerY, float playerAngle) {
    int screenWidth = 600;
    int screenHeight = 200;
    float FOV = M_PI / 3; // Field of View
    float lightAngle = M_PI / 4; // Angle of sunlight (adjust as needed)
    float wallThickness = 20.0f; // Thickness of walls

    // Define floor
    SDL_SetRenderDrawColor(renderer, 0, 128, 0, 255);
    SDL_RenderClear(renderer);

    // Draw sky
    SDL_SetRenderDrawColor(renderer, 0, 191, 255, 255); // Sky blue
    SDL_Rect skyRect = { 0, 0, screenWidth, screenHeight / 2 };
    SDL_RenderFillRect(renderer, &skyRect);

    for (int x = 0; x < screenWidth; x++) {
        float rayAngle = (playerAngle - FOV / 2.0f) + ((float)x / (float)screenWidth) * FOV;
        float distanceToWall = 0;
        bool hitWall = false;
        float rayX = playerX;
        float rayY = playerY;

        float angleSin = sin(rayAngle);
        float angleCos = cos(rayAngle);

        while (!hitWall && distanceToWall < 20) {
            distanceToWall += 0.1f; // Increment the step of the ray
            int testX = (int)(rayX + distanceToWall * angleCos);
            int testY = (int)(rayY + distanceToWall * angleSin);

            // Check if ray is out of bounds
            if (testX < 0 || testX >= MAP_WIDTH || testY < 0 || testY >= MAP_HEIGHT) {
                hitWall = true;
                distanceToWall = 20;
            }
            else {
                if (map[testX][testY] == 1) { // Check if ray hit a wall
                    hitWall = true;
                }
            }
        }

        // Calculate distance to ceiling and floor
        int ceiling = (float)(screenHeight / 2.0) - screenHeight / ((float)distanceToWall);
        int floor = screenHeight - ceiling;

        // Calculate brightness based on the angle between the wall and the sunlight
        float brightness = cos(rayAngle - lightAngle);
        if (brightness < 0) brightness = 0; // Ensure brightness is non-negative

        // Adjust wall color based on brightness
        int wallColor = 128 + 127 * brightness;
        SDL_SetRenderDrawColor(renderer, wallColor, wallColor, wallColor, 255);

        // Draw the walls with rectangles
        int wallHeight = floor - ceiling;
        SDL_Rect wallRect = { x, ceiling, (int)wallThickness, wallHeight };
        SDL_RenderFillRect(renderer, &wallRect);
    }
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window = SDL_CreateWindow("Maze", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 400, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    float playerX = 3.0f;
    float playerY = 3.0f;
    float playerAngle = 0.0f;
    float moveSpeed = 0.1f;

    bool running = true;
    while (running) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        castRays(renderer, playerX, playerY, playerAngle);

        SDL_RenderPresent(renderer);

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                case SDLK_LEFT: // Rotate left
                    playerAngle -= 0.1f;
                    break;
                case SDLK_RIGHT: // Rotate right
                    playerAngle += 0.1f;
                    break;
                case SDLK_UP: // Move forward
                    playerX += moveSpeed * cos(playerAngle);
                    playerY += moveSpeed * sin(playerAngle);
                    break;
                case SDLK_DOWN: // Move backward
                    playerX -= moveSpeed * cos(playerAngle);
                    playerY -= moveSpeed * sin(playerAngle);
                    break;
                default:
                    break;
                }
            }
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

