/* date = July 7th 2021 10:14 am */

#ifndef TYPES_H
#define TYPES_H

#if ASSERTIONS_ENABLED
#define Assert(Expression) if(!(Expression)) {*(int *)0 = 0;}
#else
#define Assert(Expression)
#endif

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

#define internal static
#define global_variable static

typedef Vector2 v2;
typedef Color color;
typedef Font font;
typedef Rectangle rect;
typedef int bool32;

struct window_dimensions
{
    int Width;
    int Height;
};

#endif //TYPES_H
