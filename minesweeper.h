/* date = July 6th 2021 11:59 am */

#ifndef MINESWEEPER_H
#define MINESWEEPER_H

#include "time.h"

enum level
{
    BEGINNER = 0,
    INTERMEDIATE,
    EXPERT
};

enum cell_state
{
    COVERED_STATE = 0,
    FLAGGED_STATE,
    QUESTIONED_STATE,
    UNCOVERED_STATE
};

enum cell_type
{
    EMPTY_TYPE = 0,
    BOMB_TYPE,
    HINT_TYPE
};

struct grid_cell
{
    int Row; 
    int Column;
};

struct local_grid
{
    grid_cell Indices[8];
    int CellCount;
};

#define BEGINNER_BOMBS 10 // 10
#define INTERMEDIATE_BOMBS 40 // 40
#define EXPERT_BOMBS 99

#define BEGINNER_GRID_ROWS 10
#define BEGINNER_GRID_COLUMNS 10
#define INTERMEDIATE_GRID_ROWS 16
#define INTERMEDIATE_GRID_COLUMNS 16
#define EXPERT_GRID_ROWS 16
#define EXPERT_GRID_COLUMNS 30

// Cell texture indices
#define BOMB 0
#define BOMB_EXPLODE 1
#define COVERED 2
#define COVERED_PRESSED 3 // NOTE: Also considered an empty cell
#define QUESTION 11
#define QUESTION_PRESSED 12
#define FLAG 4

// Face texture indices
#define FROWN 5
#define GASP 6
#define SMILE 13
#define SMILE_PRESSED 14
#define WINNER 15

// Icon texture indices
#define ICON_BIG 7
#define ICON_BIG_CLASSIC 8
#define ICON_SMALL 9
#define ICON_SMALL_CLASSIC 10

struct game_window_data
{
    Texture2D Texture;
    Vector2 Position;
    Vector2 LeftTextPos;
    Vector2 FacePos;
    Vector2 RightTextPos;
    Vector2 TilemapPos;
    Vector2 ExitButtonPos;
};

struct minesweeper_state
{
    int WindowWidth;
    int WindowHeight;
    int CellIndexClicked;
    int FlagsLeft;
    int SecondsElapsed;
    float FrameTimeAccumulation;
    int ExitButtonWidth;
    int ExitButtonHeight;
    int TilesLeft;
    int BombCount;
    bool32 GameOver;
    bool32 BombsPlaced;
    bool32 TilemapInitialized;
    level Difficulty;
    Texture2D TextureAtlas;
    Texture2D TextTextureAtlas;
    rect ActiveFaceTexture;
    int ActiveFaceTextureIndex;
    Texture2D BackgroundImage;
    game_window_data BeginnerWindow;
    game_window_data IntermediateWindow;
    game_window_data ExpertWindow;
    int TimerDigits[3];
    int FlagDigits[3];
    rect HintTextureCoordinates[9];
    rect TextTextureCoordinates[10];
    rect TextureCoordinates[16];
};

inline int
RandomNumberInRange(int Lower, int Upper)
{
    return (rand() % (Upper - Lower + 1)) + Lower;
}

#endif //MINESWEEPER_H
