#include "minesweeper.h"

internal bool
IsValidIndex(tilemap *Tilemap, int Row, int Column)
{
    bool Result = false;
    
    if((Row >= 0 && Row < Tilemap->Rows) &&
       (Column >= 0 && Column < Tilemap->Columns))
    {
        Result = true;
    }
    
    return Result;
}

internal local_grid
GetLocalGrid(tilemap *Tilemap, int CellIndex)
{
    // This function ensures that the local grid only includes cells within the tilemap itself
    
    int Row = CellIndex / Tilemap->Columns;
    int Column = CellIndex - (Tilemap->Columns * Row);
    local_grid Result = {};
    
    if(IsValidIndex(Tilemap, Row - 1, Column - 1))
    {
        Result.Indices[Result.CellCount++] = { Row - 1, Column - 1 };
    }
    if(IsValidIndex(Tilemap, Row - 1, Column))
    {
        Result.Indices[Result.CellCount++] = { Row - 1, Column };
    }
    if(IsValidIndex(Tilemap, Row - 1, Column + 1))
    {
        Result.Indices[Result.CellCount++] = { Row - 1, Column + 1 };
    }
    if(IsValidIndex(Tilemap, Row, + Column - 1))
    {
        Result.Indices[Result.CellCount++] = { Row, Column - 1 };
    }
    if(IsValidIndex(Tilemap, Row, Column + 1))
    {
        Result.Indices[Result.CellCount++] = { Row, Column + 1 };
    }
    if(IsValidIndex(Tilemap, Row + 1, Column - 1))
    {
        Result.Indices[Result.CellCount++] = { Row + 1, Column - 1 };
    }
    if(IsValidIndex(Tilemap, Row + 1, Column))
    {
        Result.Indices[Result.CellCount++] = { Row + 1, Column };
    }
    if(IsValidIndex(Tilemap, Row + 1, Column + 1))
    {
        Result.Indices[Result.CellCount++] = { Row + 1, Column + 1 };
    }
    
    return Result;
}

internal void
UpdateDigits(int Number, int *Digits, int DigitCount)
{
    if(Number >= (pow(10, DigitCount) - 1))
    {
        for(int DigitIndex = 0; DigitIndex < DigitCount; ++DigitIndex)
        {
            Digits[DigitIndex] = 9;
        }
    }
    else
    {
        for(int DigitIndex = DigitCount; DigitIndex > 0; --DigitIndex)
        {
            Digits[DigitIndex - 1] = Number % 10;
            Number /= 10;
        }
    }
}

internal int
CheckTileForBomb(tilemap *Tilemap, int Row, int Column)
{
    int Result = 0;
    
    if(IsValidIndex(Tilemap, Row, Column))
    {
        Result = Tilemap->Tiles[(Row * Tilemap->Columns) + Column].Type == BOMB_TYPE ? 1 : 0;
    }
    
    return Result;
}

internal void
UncoverAdjacentTiles(minesweeper_state *GameState, tilemap *Tilemap, int Row, int Column)
{
    int TilemapIndex = (Row * Tilemap->Columns) + Column;
    
    if(!IsValidIndex(Tilemap, Row, Column))
    {
        return;
    }
    
    if(Tilemap->Tiles[TilemapIndex].TextureState == UNCOVERED_STATE)
    {
        return;
    }
    
    if(GameState->GameOver)
    {
        return;
    }
    
    if(Tilemap->Tiles[TilemapIndex].Type == EMPTY_TYPE)
    {
        // Uncover
        Tilemap->Tiles[TilemapIndex].CurrentTextureCoordinates =
            Tilemap->Tiles[TilemapIndex].FinalTextureCoordinates;
        Tilemap->Tiles[TilemapIndex].TextureState = UNCOVERED_STATE;
        --GameState->TilesLeft;
        
        local_grid LocalGrid = GetLocalGrid(Tilemap, TilemapIndex);
        
        // Recurse the local grid to uncover other empty tiles
        for(int LocalGridIndex = 0; LocalGridIndex < LocalGrid.CellCount; ++LocalGridIndex)
        {
            UncoverAdjacentTiles(GameState, Tilemap, 
                                 LocalGrid.Indices[LocalGridIndex].Row, 
                                 LocalGrid.Indices[LocalGridIndex].Column);
        }
        
    }
    else if(Tilemap->Tiles[TilemapIndex].Type == HINT_TYPE)
    {
        // Uncover and return
        Assert(Tilemap->Tiles[TilemapIndex].Type != UNCOVERED_STATE);
        Tilemap->Tiles[TilemapIndex].CurrentTextureCoordinates =
            Tilemap->Tiles[TilemapIndex].FinalTextureCoordinates;
        Tilemap->Tiles[TilemapIndex].TextureState = UNCOVERED_STATE;
        --GameState->TilesLeft;
    }
}

internal void
UncoverTile(minesweeper_state *GameState, tilemap *Tilemap)
{
    int IndexClicked = GameState->CellIndexClicked;
    if(Tilemap->Tiles[IndexClicked].Type == BOMB_TYPE)
    {
        // Game over
        GameState->GameOver = 1;
        GameState->ActiveFaceTexture = GameState->TextureCoordinates[FROWN];
        GameState->ActiveFaceTextureIndex = FROWN;
        
        // Uncovering all bombs
        for(int Row = 0; Row < Tilemap->Rows; ++Row)
        {
            for(int Column = 0; Column < Tilemap->Columns; ++Column)
            {
                if(Tilemap->Tiles[(Row * Tilemap->Columns) + Column].Type == BOMB_TYPE)
                {
                    Tilemap->Tiles[(Row * Tilemap->Columns) + Column].CurrentTextureCoordinates =
                        Tilemap->Tiles[(Row * Tilemap->Columns) + Column].FinalTextureCoordinates;
                    Tilemap->Tiles[IndexClicked].TextureState = UNCOVERED_STATE;
                }
            }
        }
        
        // Applying exploding bomb texture to bomb tile clicked
        Tilemap->Tiles[IndexClicked].CurrentTextureCoordinates = 
            GameState->TextureCoordinates[BOMB_EXPLODE];
        Tilemap->Tiles[IndexClicked].TextureState = UNCOVERED_STATE;
    }
    else
    {
        int Row = IndexClicked / Tilemap->Columns;
        //int Column = IndexClicked - (Row * Tilemap->Rows);
        int Column = IndexClicked - (Tilemap->Columns * Row);
        UncoverAdjacentTiles(GameState, Tilemap, Row, Column);
    }
}

internal void
Chord(minesweeper_state *GameState, tilemap *Tilemap, int CellIndexClicked)
{
    if(Tilemap->Tiles[CellIndexClicked].TextureState == COVERED_STATE)
    {
        return;
    }
    
    int Row = CellIndexClicked / Tilemap->Columns;
    int Column = CellIndexClicked - (Tilemap->Columns * Row);
    local_grid LocalGrid = GetLocalGrid(Tilemap, CellIndexClicked);
    // There can be fewer than 8 adjacent cells (edges of the tilemap)
    int LocalCellCount = 0;
    int UnflaggedBombCount = 0;
    
    // Check each cell for the bomb and flag status
    int CurrentIndex;
    for(int CellIndex = 0; CellIndex < LocalGrid.CellCount; ++CellIndex)
    {
        CurrentIndex = (LocalGrid.Indices[CellIndex].Row * Tilemap->Columns) + LocalGrid.Indices[CellIndex].Column;
        UnflaggedBombCount += 
            ((Tilemap->Tiles[CurrentIndex].Type == BOMB_TYPE) ^
             (Tilemap->Tiles[CurrentIndex].TextureState == FLAGGED_STATE));
    }
    
    // If the local grid is able to be "chorded" then uncover all the cells
    if(UnflaggedBombCount == 0)
    {
        for(int CellIndex = 0; CellIndex < LocalGrid.CellCount; ++CellIndex)
        {
            UncoverAdjacentTiles(GameState, Tilemap, 
                                 LocalGrid.Indices[CellIndex].Row,
                                 LocalGrid.Indices[CellIndex].Column);
        }
    }
}

internal void
ResetTilemap(minesweeper_state *GameState, tilemap *Tilemap)
{
    if(GameState->TilemapInitialized && !GameState->BombsPlaced)
    {
        return;
    }
    
    // Choose the amount of bombs based on difficulty level
    switch(GameState->Difficulty)
    {
        case BEGINNER:
        {
            Tilemap->Rows = BEGINNER_GRID_ROWS;
            Tilemap->Columns = BEGINNER_GRID_COLUMNS;
            Tilemap->PosX = GameState->BeginnerWindow.TilemapPos.x;
            Tilemap->PosY = GameState->BeginnerWindow.TilemapPos.y;
            GameState->TilesLeft = (BEGINNER_GRID_ROWS * BEGINNER_GRID_COLUMNS) - BEGINNER_BOMBS;
            GameState->FlagsLeft = BEGINNER_BOMBS;
            GameState->BombCount = BEGINNER_BOMBS;
        } break;
        case INTERMEDIATE:
        {
            Tilemap->Rows = INTERMEDIATE_GRID_ROWS;
            Tilemap->Columns = INTERMEDIATE_GRID_COLUMNS;
            Tilemap->PosX = GameState->IntermediateWindow.TilemapPos.x;
            Tilemap->PosY = GameState->IntermediateWindow.TilemapPos.y;
            GameState->TilesLeft = (INTERMEDIATE_GRID_ROWS * INTERMEDIATE_GRID_COLUMNS) - INTERMEDIATE_BOMBS;
            GameState->FlagsLeft = INTERMEDIATE_BOMBS;
            GameState->BombCount = INTERMEDIATE_BOMBS;
        } break;
        case EXPERT:
        {
            Tilemap->Rows = EXPERT_GRID_ROWS;
            Tilemap->Columns = EXPERT_GRID_COLUMNS;
            Tilemap->PosX = GameState->ExpertWindow.TilemapPos.x;
            Tilemap->PosY = GameState->ExpertWindow.TilemapPos.y;
            GameState->TilesLeft = (EXPERT_GRID_ROWS * EXPERT_GRID_COLUMNS) - EXPERT_BOMBS;
            GameState->FlagsLeft = EXPERT_BOMBS;
            GameState->BombCount = EXPERT_BOMBS;
        } break;
        
        default:
        {
            Assert(0);
        }
    }
    
    int TilemapIndex;
    for(int Row = 0; Row < Tilemap->Rows; ++Row)
    {
        for(int Col = 0; Col < Tilemap->Columns; ++Col)
        {
            // The tile map should be visible before being "generated". So the default
            // texture is the covered cell texture
            TilemapIndex = (Row * Tilemap->Columns) + Col;
            Tilemap->Tiles[TilemapIndex].Type = EMPTY_TYPE;
            Tilemap->Tiles[TilemapIndex].TextureState = COVERED_STATE;
            Tilemap->Tiles[TilemapIndex].CurrentTextureCoordinates = 
                GameState->TextureCoordinates[COVERED];
            Tilemap->Tiles[TilemapIndex].FinalTextureCoordinates = 
                GameState->TextureCoordinates[COVERED_PRESSED];
        }
    }
    
    GameState->ActiveFaceTexture = GameState->TextureCoordinates[SMILE];
    GameState->ActiveFaceTextureIndex = SMILE;
    
    UpdateDigits(GameState->FlagsLeft, &(GameState)->FlagDigits[0], ArrayCount(GameState->FlagDigits));
    UpdateDigits(0, &(GameState)->TimerDigits[0], ArrayCount(GameState->TimerDigits));
    
    GameState->SecondsElapsed = 0;
    GameState->TilemapInitialized = 1;
    GameState->BombsPlaced = 0;
    GameState->GameOver = 0;
}

internal void
InitMinesweeperGame(minesweeper_state *GameState, tilemap *Tilemap, window_dimensions WindowDimensions, level Difficulty)
{
    *Tilemap = {};
    *GameState = {};
    
    GameState->WindowWidth = WindowDimensions.Width;
    GameState->WindowHeight = WindowDimensions.Height;
    
    // NOTE: Alternatively I could parse the text file I generate that contains this info but... anyways
    GameState->TextureAtlas = LoadTexture("resources/textures/atlas/atlas.png");
    GameState->TextureCoordinates[BOMB] = { 80, 103, 16, 16 };
    GameState->TextureCoordinates[BOMB_EXPLODE] = { 16, 78, 16, 16 };
    GameState->TextureCoordinates[COVERED] = { 0, 72, 16, 16 };
    GameState->TextureCoordinates[COVERED_PRESSED] = { 79, 71, 16, 16 };
    GameState->TextureCoordinates[FLAG] = { 32, 78, 16, 16 };
    GameState->TextureCoordinates[FROWN] = { 88, 24, 24, 24 };
    GameState->TextureCoordinates[GASP] = { 0, 32, 24, 24 };
    GameState->TextureCoordinates[ICON_BIG] = { 0, 0, 32, 32 };
    GameState->TextureCoordinates[ICON_BIG_CLASSIC] = { 32, 0, 32, 32 };
    GameState->TextureCoordinates[ICON_SMALL] = { 0, 56, 16, 16 };
    GameState->TextureCoordinates[ICON_SMALL_CLASSIC] = { 63, 71, 16, 16 };
    GameState->TextureCoordinates[QUESTION] = { 48, 103, 16, 16 };
    GameState->TextureCoordinates[QUESTION_PRESSED] = { 95, 71, 16, 16 };
    GameState->TextureCoordinates[SMILE] = { 88, 0, 24, 24 };
    GameState->TextureCoordinates[SMILE_PRESSED] = { 64, 24, 24, 24 };
    GameState->TextureCoordinates[WINNER] = { 64, 0, 24, 24 };
    
    // The hint and text texture coordinates are stored this way for easier lookup.
    //  - hint_0 at index 0, hint_1 at index 1, and so on
    // This lets me look up the texture cooridnates of the hint texture based on the corresponding number
    // of bombs found in the tiles local 3x3 grid (see GenerateGameTiles function)
    GameState->HintTextureCoordinates[0] = GameState->TextureCoordinates[COVERED_PRESSED];
    GameState->HintTextureCoordinates[1] = { 64, 103, 16, 16 };
    GameState->HintTextureCoordinates[2] = { 32, 94, 16, 16 };
    GameState->HintTextureCoordinates[3] = { 16, 94, 16, 16 };
    GameState->HintTextureCoordinates[4] = { 0, 88, 16, 16 };
    GameState->HintTextureCoordinates[5] = { 96, 87, 16, 16 };
    GameState->HintTextureCoordinates[6] = { 80, 87, 16, 16 };
    GameState->HintTextureCoordinates[7] = { 64, 87, 16, 16 };
    GameState->HintTextureCoordinates[8] = { 48, 87, 16, 16 };
    
    // I found an pixel artifact while using the digit textures from my atlas. At some point I decided to
    // atlas together just the digit textures and that is working fine now
    GameState->TextTextureAtlas = LoadTexture("resources/textures/text/atlas.png");
    GameState->TextTextureCoordinates[0] = { 0, 0, 13, 23 };
    GameState->TextTextureCoordinates[1] = { 13, 0, 13, 23 };
    GameState->TextTextureCoordinates[2] = { 26, 0, 13, 23 };
    GameState->TextTextureCoordinates[3] = { 39, 0, 13, 23 };
    GameState->TextTextureCoordinates[4] = { 0, 23, 13, 23 };
    GameState->TextTextureCoordinates[5] = { 13, 23, 13, 23 };
    GameState->TextTextureCoordinates[6] = { 26, 23, 13, 23 };
    GameState->TextTextureCoordinates[7] = { 39, 23, 13, 23 };
    GameState->TextTextureCoordinates[8] = { 0, 46, 13, 23 };
    GameState->TextTextureCoordinates[9] = { 13, 46, 13, 23 };
    
    // Window textures
    GameState->BeginnerWindow.Texture = LoadTexture("resources/textures/windows/beginner_window.png");
    GameState->IntermediateWindow.Texture = LoadTexture("resources/textures/windows/intermediate_window.png");
    GameState->ExpertWindow.Texture =
        LoadTexture("resources/textures/windows/expert_window.png");
    
    // The windows will be statically located at the center of the screen
    // The pixel offsets of the positons to inlay into the windows has been precomputed relative
    // to the origin of its window
    float ScreenCenterX = WindowDimensions.Width / 2.0f;
    float ScreenCenterY = WindowDimensions.Height / 2.0f;
    
    GameState->BeginnerWindow.Position.x = ScreenCenterX - (GameState->BeginnerWindow.Texture.width / 2.0f);
    GameState->BeginnerWindow.Position.y =
        ScreenCenterY - (GameState->BeginnerWindow.Texture.height / 2.0f);
    GameState->BeginnerWindow.LeftTextPos.x =       
        GameState->BeginnerWindow.Position.x + 20;
    GameState->BeginnerWindow.LeftTextPos.y =
        GameState->BeginnerWindow.Position.y + 44;
    GameState->BeginnerWindow.FacePos.x =           
        GameState->BeginnerWindow.Position.x + 82;
    GameState->BeginnerWindow.FacePos.y =
        GameState->BeginnerWindow.Position.y + 44;
    GameState->BeginnerWindow.RightTextPos.x =      
        GameState->BeginnerWindow.Position.x + 129;
    GameState->BeginnerWindow.RightTextPos.y =
        GameState->BeginnerWindow.Position.y + 44;
    GameState->BeginnerWindow.TilemapPos.x =
        GameState->BeginnerWindow.Position.x + 15;
    GameState->BeginnerWindow.TilemapPos.y = 
        GameState->BeginnerWindow.Position.y + 83;
    GameState->BeginnerWindow.ExitButtonPos.x =
        GameState->BeginnerWindow.Position.x + 160;
    GameState->BeginnerWindow.ExitButtonPos.y = 
        GameState->BeginnerWindow.Position.y + 5;
    
    GameState->IntermediateWindow.Position.x = ScreenCenterX - (GameState->IntermediateWindow.Texture.width / 2.0f);
    GameState->IntermediateWindow.Position.y =
        ScreenCenterY - (GameState->IntermediateWindow.Texture.height / 2.0f);
    GameState->IntermediateWindow.LeftTextPos.x =
        GameState->IntermediateWindow.Position.x + 20;
    GameState->IntermediateWindow.LeftTextPos.y =
        GameState->IntermediateWindow.Position.y + 44;
    GameState->IntermediateWindow.FacePos.x =
        GameState->IntermediateWindow.Position.x + 131;
    GameState->IntermediateWindow.FacePos.y =
        GameState->IntermediateWindow.Position.y + 44;
    GameState->IntermediateWindow.RightTextPos.x =
        GameState->IntermediateWindow.Position.x + 225;
    GameState->IntermediateWindow.RightTextPos.y =
        GameState->IntermediateWindow.Position.y + 44;
    GameState->IntermediateWindow.TilemapPos.x =
        GameState->IntermediateWindow.Position.x + 15;
    GameState->IntermediateWindow.TilemapPos.y =
        GameState->IntermediateWindow.Position.y + 83;
    GameState->IntermediateWindow.ExitButtonPos.x =
        GameState->IntermediateWindow.Position.x + 256;
    GameState->IntermediateWindow.ExitButtonPos.y =
        GameState->IntermediateWindow.Position.y + 5;
    
    GameState->ExpertWindow.Position.x = ScreenCenterX - (GameState->ExpertWindow.Texture.width / 2.0f);
    GameState->ExpertWindow.Position.y =
        ScreenCenterY - (GameState->ExpertWindow.Texture.height / 2.0f);
    GameState->ExpertWindow.LeftTextPos.x =
        GameState->ExpertWindow.Position.x + 20;
    GameState->ExpertWindow.LeftTextPos.y =
        GameState->ExpertWindow.Position.y + 44;
    GameState->ExpertWindow.FacePos.x =
        GameState->ExpertWindow.Position.x + 243;
    GameState->ExpertWindow.FacePos.y =
        GameState->ExpertWindow.Position.y + 44;
    GameState->ExpertWindow.RightTextPos.x =
        GameState->ExpertWindow.Position.x + 449;
    GameState->ExpertWindow.RightTextPos.y =
        GameState->ExpertWindow.Position.y + 44;
    GameState->ExpertWindow.TilemapPos.x = 
        GameState->ExpertWindow.Position.x + 15;
    GameState->ExpertWindow.TilemapPos.y =
        GameState->ExpertWindow.Position.y + 83;
    GameState->ExpertWindow.ExitButtonPos.x =
        GameState->ExpertWindow.Position.x + 480;
    GameState->ExpertWindow.ExitButtonPos.y =
        GameState->ExpertWindow.Position.y + 5;
    
    GameState->ExitButtonWidth = 21;
    GameState->ExitButtonHeight = 21;
    
    // Background image
    Image BackgroundImage = LoadImage("resources/textures/background/winxp_background.png");
    if((BackgroundImage.width != WindowDimensions.Width) || (BackgroundImage.height != WindowDimensions.Height))
    {
        ImageResize(&BackgroundImage, WindowDimensions.Width, WindowDimensions.Height);
    }
    GameState->BackgroundImage = LoadTextureFromImage(BackgroundImage);
    
    GameState->Difficulty = Difficulty;
    
    Tilemap->CellWidth = GameState->TextureCoordinates[COVERED].width;
    Tilemap->CellHeight = GameState->TextureCoordinates[COVERED].height;
    
    ResetTilemap(GameState, Tilemap);
    
    srand(time(0));
}

internal void
AddBomb(minesweeper_state *GameState, tilemap *Tilemap, local_grid LocalGrid, int IndexClicked)
{
    int BombRow = RandomNumberInRange(0, Tilemap->Rows - 1);
    int BombColumn = RandomNumberInRange(0, Tilemap->Columns - 1);
    int BombIndex = (BombRow * Tilemap->Columns) + BombColumn;
    
    bool ValidBombIndex = true;
    
    // A bomb cannot be placed in the 3x3 grid that the user clicked on to start the game
    for(int Index = 0; Index < 9; ++Index)
    {
        if((BombIndex == (LocalGrid.Indices[Index].Row * Tilemap->Columns) + LocalGrid.Indices[Index].Column) ||
           BombIndex == IndexClicked)
        {
            ValidBombIndex = false;
            break;
        }
    }
    
    // Cannot place a bomb on another bomb
    if(Tilemap->Tiles[BombIndex].Type == BOMB_TYPE)
    {
        ValidBombIndex = false;
    }
    
    if(ValidBombIndex)
    {
        Assert(BombIndex >= 0);
        Tilemap->Tiles[BombIndex].Type = BOMB_TYPE;
        Tilemap->Tiles[BombIndex].FinalTextureCoordinates =
            GameState->TextureCoordinates[BOMB];
    }
    else
    {
        // Try again with new random row and column
        AddBomb(GameState, Tilemap, LocalGrid, IndexClicked);
    }
}

internal void
GenerateGameTiles(minesweeper_state *GameState, tilemap *Tilemap)
{
    if(!GameState->TilemapInitialized)
    {
        ResetTilemap(GameState, Tilemap);
    }
    
    int BombCount;
    switch(GameState->Difficulty)
    {
        case BEGINNER:
        {
            BombCount = BEGINNER_BOMBS; // 10
        } break;
        
        case INTERMEDIATE:
        {
            BombCount = INTERMEDIATE_BOMBS; // 40
        } break;
        
        case EXPERT:
        {
            BombCount = EXPERT_BOMBS; // 99
        } break;
        
        default:
        {
            Assert(false); // TODO: Remove
            BombCount = 9001;
        } 
    }
    
    int TilemapRows = Tilemap->Rows;
    int TilemapColumns = Tilemap->Columns;
    int Row = GameState->CellIndexClicked / TilemapColumns;
    int Column = GameState->CellIndexClicked - (TilemapColumns * Row);
    
    // This is the local grid whose center is the tile the user clicked on (we're not going to put
    // a bomb in this grid)
    local_grid ReservedCells = GetLocalGrid(Tilemap, GameState->CellIndexClicked);
    for(int BombsAdded = 0; BombsAdded < BombCount; ++BombsAdded)
    {
        AddBomb(GameState, Tilemap, ReservedCells, GameState->CellIndexClicked);
    }
    
    int TilemapIndex;
    for(int Row = 0; Row < TilemapRows; ++Row)
    {
        for(int Column = 0; Column < TilemapColumns; ++Column)
        {
            TilemapIndex = (Row * TilemapColumns) + Column;
            if(Tilemap->Tiles[TilemapIndex].Type != BOMB_TYPE)
            {
                local_grid LocalGrid = GetLocalGrid(Tilemap, TilemapIndex);
                
                int BombsFound = 0;
                for(int LocalCellIndex = 0; LocalCellIndex < LocalGrid.CellCount; ++LocalCellIndex)
                {
                    BombsFound += CheckTileForBomb(Tilemap, LocalGrid.Indices[LocalCellIndex].Row,
                                                   LocalGrid.Indices[LocalCellIndex].Column);
                }
                
                Tilemap->Tiles[TilemapIndex].FinalTextureCoordinates = 
                    GameState->HintTextureCoordinates[BombsFound];
                
                if(BombsFound)
                {
                    Tilemap->Tiles[TilemapIndex].Type = HINT_TYPE;
                }
                else
                {
                    Tilemap->Tiles[TilemapIndex].Type = EMPTY_TYPE;
                }
            }
        }
    }
    
    GameState->BombsPlaced = 1;
}

internal void
UpdateMinesweeperGame(minesweeper_state *GameState, tilemap *Tilemap, mouse_input Input, 
                      float FrameTime, int WindowWidth, int WindowHeight)
{
    
    if((GameState->TilesLeft <= 0) && (GameState->GameOver == 0))
    {
        // Set the bomb cell to flags
        for(int Row = 0; Row < Tilemap->Rows; ++Row)
        {
            for(int Column = 0; Column < Tilemap->Columns; ++Column)
            {
                if((Tilemap->Tiles[(Row * Tilemap->Columns) + Column].Type == BOMB_TYPE) &&
                   (Tilemap->Tiles[(Row * Tilemap->Columns) + Column].TextureState != FLAGGED_STATE))
                {
                    Tilemap->Tiles[(Row * Tilemap->Columns) + Column].TextureState =
                        FLAGGED_STATE;
                    Tilemap->Tiles[(Row * Tilemap->Columns) + Column].CurrentTextureCoordinates = GameState->TextureCoordinates[FLAG];
                    --GameState->FlagsLeft;
                }
            }
            
            UpdateDigits(GameState->FlagsLeft, &(GameState)->FlagDigits[0], ArrayCount(GameState->FlagDigits));
        }
        
        GameState->GameOver = 1;
        GameState->ActiveFaceTexture = GameState->TextureCoordinates[WINNER];
        GameState->ActiveFaceTextureIndex = WINNER;
    }
    
    game_window_data WindowData;
    level NewDifficulty;
    // Which window are we viewing?
    switch(GameState->Difficulty)
    {
        case BEGINNER:
        {
            WindowData = GameState->BeginnerWindow;
            NewDifficulty = INTERMEDIATE;
        } break;
        
        case INTERMEDIATE:
        {
            WindowData = GameState->IntermediateWindow;
            NewDifficulty = EXPERT;
        } break;
        
        case EXPERT:
        {
            WindowData = GameState->ExpertWindow;
            NewDifficulty = EXPERT;
        } break;
    }
    
    // Increment timer if game is ongoing
    if(GameState->BombsPlaced && !GameState->GameOver)
    {
        GameState->FrameTimeAccumulation += FrameTime;
        if(GameState->FrameTimeAccumulation > 1)
        {
            ++GameState->SecondsElapsed;
            GameState->FrameTimeAccumulation = 0;
            UpdateDigits(GameState->SecondsElapsed, 
                         &(GameState)->TimerDigits[0], 
                         ArrayCount(GameState->TimerDigits));
        }
    }
    
    // Cell index at current cursor position
    int CellIndexClicked = GetTileIndexAtCursor(Input.CursorPosition,
                                                WindowWidth, WindowHeight);
    
    // Unncommitted press - texture changes
    if(Input.LeftMouseButtonPressed && !Input.RightMouseButtonPressed)
    {
        if(CellIndexClicked >= 0)
        {
            // Currently clicking on a tile
            GameState->CellIndexClicked = CellIndexClicked;
            
            if(Tilemap->Tiles[CellIndexClicked].TextureState == QUESTIONED_STATE)
            {
                // Swap tile texture
                Tilemap->Tiles[CellIndexClicked].CurrentTextureCoordinates = 
                    GameState->TextureCoordinates[QUESTION_PRESSED];
            }
            else if(Tilemap->Tiles[CellIndexClicked].TextureState == COVERED_STATE)
            {
                // Swap tile texture
                Tilemap->Tiles[CellIndexClicked].CurrentTextureCoordinates = 
                    GameState->TextureCoordinates[COVERED_PRESSED];
                
                if(!GameState->GameOver)
                {
                    // Face swaps textures during mouse pressed while the game is ongoing
                    GameState->ActiveFaceTexture = GameState->TextureCoordinates[GASP];
                    GameState->ActiveFaceTextureIndex = GASP;
                }
            }
        }
        else
        {
            // Check if mouse clicked on the exit button
            if((Input.CursorPosition.x > WindowData.ExitButtonPos.x && 
                Input.CursorPosition.x < WindowData.ExitButtonPos.x + GameState->ExitButtonWidth) &&
               (Input.CursorPosition.y > WindowData.ExitButtonPos.y &&
                Input.CursorPosition.y < WindowData.ExitButtonPos.y + GameState->ExitButtonHeight))
            {
                GlobalRunning = false;
            }
            // Check if mouse clicked on the smiley face
            else if((Input.CursorPosition.x > WindowData.FacePos.x && 
                     Input.CursorPosition.x < WindowData.FacePos.x + GameState->TextureCoordinates[SMILE].width) &&
                    (Input.CursorPosition.y > WindowData.FacePos.y &&
                     Input.CursorPosition.y < WindowData.FacePos.y + GameState->TextureCoordinates[SMILE].height))
            {
                
                GameState->ActiveFaceTexture = GameState->TextureCoordinates[SMILE_PRESSED];
                GameState->ActiveFaceTextureIndex = SMILE_PRESSED;
            }
        }
    }
    else if(Input.MiddleMouseButtonPressed)
    {
        if(CellIndexClicked >= 0)
        {
            if(!GameState->GameOver)
            {
                Chord(GameState, Tilemap, CellIndexClicked);
            }
        }
    }
    
    if(Input.RightMouseButtonReleased)
    {
        // Right clicks toggle textures on tiles (default, flags, question marks)
        if(CellIndexClicked >= 0)
        {
            // Cycle the state of the cell that was cliked on between the three states in minesweeper:
            // covered (default), flagged, and questioned (question mark)
            cell_state CurrentCellState = (cell_state)Tilemap->Tiles[CellIndexClicked].TextureState;
            switch(CurrentCellState)
            {
                case COVERED_STATE:
                {
                    if(GameState->FlagsLeft > 0)
                    {
                        Tilemap->Tiles[CellIndexClicked].CurrentTextureCoordinates = 
                            GameState->TextureCoordinates[FLAG];
                        Tilemap->Tiles[CellIndexClicked].TextureState = FLAGGED_STATE;
                        --GameState->FlagsLeft;
                        UpdateDigits(GameState->FlagsLeft, 
                                     &(GameState)->FlagDigits[0], 
                                     ArrayCount(GameState->FlagDigits));
                    }
                    
                } break;
                
                case FLAGGED_STATE:
                {
                    Tilemap->Tiles[CellIndexClicked].CurrentTextureCoordinates = 
                        GameState->TextureCoordinates[QUESTION];
                    Tilemap->Tiles[CellIndexClicked].TextureState = QUESTIONED_STATE;
                    ++GameState->FlagsLeft;
                    UpdateDigits(GameState->FlagsLeft, 
                                 &(GameState)->FlagDigits[0], 
                                 ArrayCount(GameState->FlagDigits));
                } break;
                
                case QUESTIONED_STATE:
                {
                    Tilemap->Tiles[CellIndexClicked].CurrentTextureCoordinates = 
                        GameState->TextureCoordinates[COVERED];
                    Tilemap->Tiles[CellIndexClicked].TextureState = COVERED_STATE;
                } break;
            }
        }
    }
    
    // An intended click was made on a tile
    if(Input.LeftMouseButtonReleased)
    {
        if(CellIndexClicked == GameState->CellIndexClicked) 
        {
            // "Initialized" meaning the size of the current tilemap is defined, and all the current
            // textures are set to covered
            if(!GameState->TilemapInitialized)
            {
                ResetTilemap(GameState, Tilemap);
            }
            
            // The hidden game tiles won't be generated until the player clicks on the first square
            // to ensure that the first square clicked on is never a bomb
            if(!GameState->BombsPlaced)
            {
                GenerateGameTiles(GameState, Tilemap);
            }
            
            cell_state CurrentCellState = (cell_state)Tilemap->Tiles[CellIndexClicked].TextureState;
            switch(CurrentCellState)
            {
                case COVERED_STATE:
                {
                    if(!GameState->GameOver)
                    {
                        UncoverTile(GameState, Tilemap);
                    }
                    
                } break;
                
                case QUESTIONED_STATE:
                {
                    Tilemap->Tiles[CellIndexClicked].CurrentTextureCoordinates =
                        GameState->TextureCoordinates[QUESTION];
                } break;
            }
        }
        else if((Input.CursorPosition.x > WindowData.FacePos.x && 
                 Input.CursorPosition.x < WindowData.FacePos.x + GameState->TextureCoordinates[SMILE].width) &&
                (Input.CursorPosition.y > WindowData.FacePos.y &&
                 Input.CursorPosition.y < WindowData.FacePos.y + GameState->TextureCoordinates[SMILE].height) &&
                GameState->ActiveFaceTextureIndex == SMILE_PRESSED)
        {
            if(GameState->GameOver && (GameState->TilesLeft <= 0))
            {
                // Advance the difficulty
                GameState->Difficulty = NewDifficulty;
            }
            
            // Otherwise reset
            ResetTilemap(GameState, Tilemap);
            
            GameState->ActiveFaceTexture = GameState->TextureCoordinates[SMILE];
            GameState->ActiveFaceTextureIndex = SMILE;
        }
        
        if(Tilemap->Tiles[GameState->CellIndexClicked].TextureState == QUESTIONED_STATE)
        {
            // The last "questioned" cell clicked on had it's texture changed, so change it back
            // regardless of where the cursor was released
            Tilemap->Tiles[GameState->CellIndexClicked].CurrentTextureCoordinates =
                GameState->TextureCoordinates[QUESTION];
        }
        else if(Tilemap->Tiles[GameState->CellIndexClicked].TextureState == COVERED_STATE)
        {
            // The last covered cell had it's texture changed to "covered-pressed", so change it back
            // regardless of where the cursor was released
            Tilemap->Tiles[GameState->CellIndexClicked].CurrentTextureCoordinates = 
                GameState->TextureCoordinates[COVERED];
        }
        
        // Reset the smiley face texture if it was pressed down
        if(GameState->ActiveFaceTextureIndex = SMILE_PRESSED)
        {
            if(GameState->GameOver)
            {
                if(GameState->TilesLeft > 0)
                {
                    GameState->ActiveFaceTexture = GameState->TextureCoordinates[FROWN];
                    GameState->ActiveFaceTextureIndex = FROWN;
                }
                else
                {
                    GameState->ActiveFaceTexture = GameState->TextureCoordinates[WINNER];
                    GameState->ActiveFaceTextureIndex = WINNER;
                }
            }
            else
            {
                GameState->ActiveFaceTexture = GameState->TextureCoordinates[SMILE];
                GameState->ActiveFaceTextureIndex = SMILE;
            }
        }
    }
    
}

internal void
DrawMinesweeperGame(tilemap *Tilemap, minesweeper_state *GameState)
{
    // Draw background
    DrawTexture(GameState->BackgroundImage, 0, 0, WHITE);
    
    game_window_data WindowData;
    switch(GameState->Difficulty)
    {
        case BEGINNER:
        {
            WindowData = GameState->BeginnerWindow;
        } break;
        
        case INTERMEDIATE:
        {
            WindowData = GameState->IntermediateWindow;
        } break;
        
        case EXPERT:
        {
            WindowData = GameState->ExpertWindow;
        } break;
    }
    
    // Draw window
    DrawTexture(WindowData.Texture, WindowData.Position.x, WindowData.Position.y, WHITE);
    
    // Draw text
    rect TextTextureCoordinates;
    rect TextDrawRect;
    for(int TextOffset = 0; TextOffset < 3; ++TextOffset)
    {
        // Timer text (left)
        TextTextureCoordinates = GameState->TextTextureCoordinates[GameState->TimerDigits[TextOffset]];
        TextDrawRect.x = WindowData.LeftTextPos.x + (TextOffset * TextTextureCoordinates.width);
        TextDrawRect.y = WindowData.LeftTextPos.y;
        TextDrawRect.width = TextTextureCoordinates.width;
        TextDrawRect.height = TextTextureCoordinates.height;
        DrawTexturePro(GameState->TextTextureAtlas, TextTextureCoordinates, TextDrawRect, {0, 0}, 0, WHITE);
        
        // Flag count text (right)
        TextTextureCoordinates = GameState->TextTextureCoordinates[GameState->FlagDigits[TextOffset]];
        TextDrawRect.x = WindowData.RightTextPos.x + (TextOffset * TextTextureCoordinates.width);
        TextDrawRect.y = WindowData.RightTextPos.y;
        TextDrawRect.width = TextTextureCoordinates.width;
        TextDrawRect.height = TextTextureCoordinates.height;
        DrawTexturePro(GameState->TextTextureAtlas, TextTextureCoordinates, TextDrawRect, {0, 0}, 0, WHITE);
    }
    
    // Draw face
    rect FaceRect;
    FaceRect.x = WindowData.FacePos.x;
    FaceRect.y = WindowData.FacePos.y;
    FaceRect.width = GameState->ActiveFaceTexture.width;
    FaceRect.height = GameState->ActiveFaceTexture.height;
    DrawTexturePro(GameState->TextureAtlas, GameState->ActiveFaceTexture, FaceRect, {0, 0}, 0, WHITE);
    
    // Draw tilemap
    rect TextureRect;
    rect TileDimensions;
    float TilemapPosX = Tilemap->PosX;
    float TilemapPosY = Tilemap->PosY;
    
    int TileOffsetY = 0;
    for(int Row = 0; Row < DefaultTilemap.Rows; ++Row)
    {
        int TileOffsetX = 0;
        for(int Col = 0; Col < DefaultTilemap.Columns; ++Col)
        {
            TextureRect = Tilemap->Tiles[(Row * Tilemap->Columns) + Col].CurrentTextureCoordinates;
            TileDimensions.x = TilemapPosX + (TileOffsetX);
            TileDimensions.y = TilemapPosY + (TileOffsetY);
            TileDimensions.width = DefaultTilemap.CellWidth;
            TileDimensions.height = DefaultTilemap.CellHeight;
            
            DrawTexturePro(GameState->TextureAtlas, TextureRect, TileDimensions, {0, 0}, 0, WHITE);
            
            TileOffsetX += DefaultTilemap.CellWidth;
        }
        
        TileOffsetY += DefaultTilemap.CellHeight;
    }
    
}

