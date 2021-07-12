#include <cstdio>
#include <string.h>
#include <cstdlib>
#include <math.h>
#include "include/raylib.h"
#include "include/types.h"
#include "tilemap.h"

//1366x768

#define DEFAULT_WINDOW_WIDTH 1366
#define DEFAULT_WINDOW_HEIGHT 768

global_variable bool GlobalRunning;

struct mouse_input
{
    bool32 LeftMouseButtonPressed;
    bool32 RightMouseButtonPressed;
    bool32 LeftMouseButtonReleased;
    bool32 RightMouseButtonReleased;
    bool32 MiddleMouseButtonPressed;
    Vector2 CursorPosition;
};

#include "minesweeper.cpp"

internal void
PollMouseInput(mouse_input *Input)
{
    *Input = {};
    
    Input->CursorPosition = GetMousePosition();
    
    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Input->LeftMouseButtonPressed = 1;
    }
    
    if(IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
    {
        Input->RightMouseButtonPressed = 1;
    }
    
    if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
    {
        Input->LeftMouseButtonReleased = 1;
    }
    
    if(IsMouseButtonReleased(MOUSE_RIGHT_BUTTON))
    {
        Input->RightMouseButtonReleased = 1;
    }
    
    if(IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON))
    {
        Input->MiddleMouseButtonPressed = 1;
    }
}

int main()
{
    window_dimensions WindowDimensions;
    WindowDimensions.Width = DEFAULT_WINDOW_WIDTH;
    WindowDimensions.Height = DEFAULT_WINDOW_HEIGHT;
    InitWindow(WindowDimensions.Width, WindowDimensions.Height, "Minesweeper");
    int Monitor = GetCurrentMonitor();
    //SetWindowState(FLAG_FULLSCREEN_MODE);
    
    int TargetFramesPerSecond = GetMonitorRefreshRate(Monitor);
    SetTargetFPS(TargetFramesPerSecond);
    
    if(IsWindowReady())
    {
        SetMousePosition(WindowDimensions.Width / 2, WindowDimensions.Height / 2);
        Texture2D CursorTexture = LoadTexture("resources/textures/cursor/winxp_cursor.png");
        int CursorWidth = CursorTexture.width;
        int CursorHeight = CursorTexture.height;
        Vector2 CursorPosition = GetMousePosition();
        Vector2 PrevCursorPosition = CursorPosition;
        
        minesweeper_state GameState;
        InitMinesweeperGame(&GameState, &DefaultTilemap, WindowDimensions, BEGINNER);
        mouse_input Input = {};
        GlobalRunning = true;
        
        // Game loop
        while(GlobalRunning)
        {
            // Global exit case
            if((IsKeyDown(KEY_LEFT_ALT) && IsKeyDown(KEY_F4)) ||
               WindowShouldClose())
            {
                GlobalRunning = false;
            }
            
            PollMouseInput(&Input);
            float FrameTime = GetFrameTime();
            UpdateMinesweeperGame(&GameState, &DefaultTilemap, Input, 
                                  FrameTime, WindowDimensions.Width, WindowDimensions.Height);
            
            // NOTE: Draw
            BeginDrawing();
            ClearBackground(BLACK);
            
            // Draw game tilemap
            DrawMinesweeperGame(&DefaultTilemap, &GameState);
            
            EndDrawing();
            
        } // Game loop end
        
        CloseWindow();
    }
    else
    {
        // TODO: Logging
    }
}