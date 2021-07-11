/* date = July 1st 2021 3:28 am */

#ifndef TILEMAP_H
#define TILEMAP_H

struct tile
{
    int Type;
    int TextureState;
    rect CurrentTextureCoordinates; // Current texture coordinates being drawn
    rect FinalTextureCoordinates; // When tiles are uncovered, they have a final texture to display
};

struct tilemap
{
    int Columns; // Columns
    int Rows; // Rows
    
    int CellWidth;
    int CellHeight;
    
    Color OnColor = WHITE;
    Color OffColor = RED;
    
    // Origin (top-left)
    float PosX = 0;
    float PosY = 0;
    
    tile Tiles[1024] = {};
}; tilemap DefaultTilemap; 

internal int
GetTileIndexAtCursor(Vector2 GameCursor, int WindowWidth, int WindowHeight)
{
    int Result = -1;
    
    float CellWidth = DefaultTilemap.CellWidth;
    float CellHeight = DefaultTilemap.CellHeight;
    
    float TilemapLeftX = DefaultTilemap.PosX;// * ScaleFactor;
    float TilemapRightX = TilemapLeftX + (DefaultTilemap.Columns * CellWidth);
    float TilemapTopY = DefaultTilemap.PosY;// * ScaleFactor;
    float TilemapBottomY = TilemapTopY + (DefaultTilemap.Rows * CellHeight);
    
    // Is the cursor inside the tilemap?
    if((GameCursor.x > TilemapLeftX && GameCursor.x < TilemapRightX) &&
       (GameCursor.y > TilemapTopY && GameCursor.y < TilemapBottomY))
    {
        float CellOffsetX;
        float CellOffsetY;
        
        // Is the viewport inside the tilemap? (Tilemap bigger than the screen)
        int TilemapWidth = TilemapRightX - TilemapLeftX;
        int TilemapHeight = TilemapBottomY - TilemapTopY;
        int TilemapArea = TilemapWidth * TilemapHeight;
        int ScreenArea = WindowWidth * WindowHeight;
        if((TilemapLeftX <= 0) && (TilemapTopY <= 0) && (TilemapArea > ScreenArea))
        {
            // Distance from the tilemap (-x, -y) to the viewport origin (0, 0), plus the distance from
            // the viewport origin (0, 0), to the game cursor (mouse.x, mouse.y)
            CellOffsetX = GameCursor.x + -TilemapLeftX;
            CellOffsetY = GameCursor.y + -TilemapTopY;
        }
        else // Or is the tilemap inside the viewport?
        {
            // Distance from viewport origin (0, 0) to the tilemap (x, y), plus the distance from
            // the tilemap to the cursor (mouse.x, mouse.y).
            CellOffsetX = GameCursor.x - TilemapLeftX;
            CellOffsetY = GameCursor.y - TilemapTopY;
        }
        
        CellOffsetX /= CellWidth;
        CellOffsetY /= CellHeight;
        
        Result = ((int)CellOffsetY * DefaultTilemap.Columns) + (int)CellOffsetX;
        //DefaultTilemap.Tiles[(DefaultTilemap.Rows * (int)CellOffsetY) + (int)CellOffsetX].Active ^= 1;
    }
    
    return Result;
}

#endif //TILEMAP_H
