#include "Engine.h"

#define _USE_MATH_DEFINES
#include <math.h>

enum EBrick_Type
{
    EBT_None,
    EBT_Red,
    EBT_Blue
};

HPEN Highlight_Pen, Brick_Red_Pen, Brick_Blue_Pen, Platform_Circle_Pen, Platform_Inner_Pen;
HBRUSH Brick_Red_Brush, Brick_Blue_Brush, Platform_Circle_Brush, Platform_Inner_Brush;

const int Global_Scale = 3;
const int Brick_Width = 15;
const int Brick_Height = 7;
const int Cell_Width = 16;
const int Cell_Height = 8;
const int Level_X_Offset = 8;
const int Level_Y_Offset = 6;
const int Circle_Size = 7;

int Inner_Width = 21;

char Level_01[14][12] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
//--------------------------------------------------------------------------------------------------------------
void Create_Pen_Brush(unsigned char r, unsigned char g, unsigned char b, HPEN &pen, HBRUSH &brush)
{
    pen = CreatePen(PS_SOLID, 0, RGB(r, g, b));
    brush = CreateSolidBrush(RGB(r, g, b));

}
//--------------------------------------------------------------------------------------------------------------
void Init()
{// start up settings

    Highlight_Pen = CreatePen(PS_SOLID, 0, RGB(255, 255, 255));

    Create_Pen_Brush(255, 85, 85, Brick_Red_Pen, Brick_Red_Brush);
    Create_Pen_Brush(85, 255, 255, Brick_Blue_Pen, Brick_Blue_Brush);
    Create_Pen_Brush(151, 0, 0, Platform_Circle_Pen, Platform_Circle_Brush);
    Create_Pen_Brush(0, 128, 192, Platform_Inner_Pen, Platform_Inner_Brush);
   
}
//--------------------------------------------------------------------------------------------------------------
void Draw_Brick(HDC hdc, int x, int y, EBrick_Type brick_type)
{
    HPEN pen;
    HBRUSH brush;

    switch (brick_type)
    {
    case EBT_None:
        return;

    case EBT_Red:
        pen = Brick_Red_Pen;
        brush = Brick_Red_Brush;
        break;

    case EBT_Blue:
        pen = Brick_Blue_Pen;
        brush = Brick_Blue_Brush;
        break;

    default:
        return;
    }

    SelectObject(hdc, pen);
    SelectObject(hdc, brush);

    RoundRect(hdc, x * Global_Scale, y * Global_Scale, (x + Brick_Width) * Global_Scale, (y + Brick_Height) * Global_Scale, 2 * Global_Scale, 2 * Global_Scale);
}
//--------------------------------------------------------------------------------------------------------------
void Draw_Brick_Letter(HDC hdc, int x, int y, int rotation_step)
{
    
    double offset;
    float rotation_angle = 2.0f * (float)M_PI / 16.0f * (float)rotation_step; //Converting steps to an angle
    int brick_half_height = Brick_Height * Global_Scale / 2;
    int back_part_offset;
    XFORM xform, old_xform;

    if (rotation_step == 4 || rotation_step == 12)
    {
        //The background
        SelectObject(hdc, Brick_Red_Pen);
        SelectObject(hdc, Brick_Red_Brush);

        Rectangle(hdc, x, y + brick_half_height - Global_Scale, x + Brick_Width * Global_Scale, y + brick_half_height);
        
        //The foreground
        SelectObject(hdc, Brick_Blue_Pen);
        SelectObject(hdc, Brick_Blue_Brush);

        Rectangle(hdc, x, y + brick_half_height, x + Brick_Width * Global_Scale, y + brick_half_height + Global_Scale - 1);
    }
    else
    {
        SetGraphicsMode(hdc, GM_ADVANCED);

        //Setting up the Letter Flip Matrix
        xform.eM11 = 1;
        xform.eM12 = 0;
        xform.eM21 = 0;
        xform.eM22 = cosf(rotation_angle);
        xform.eDx = (float)x;
        xform.eDy = (float)y + (float)brick_half_height;
        GetWorldTransform(hdc, &old_xform);
        SetWorldTransform(hdc, &xform);

        //The background
        SelectObject(hdc, Brick_Red_Pen);
        SelectObject(hdc, Brick_Red_Brush);

        offset = 3.0 * (1.0 - fabs(xform.eM22)) * (double)Global_Scale;
        back_part_offset = (int)round(offset);
        Rectangle(hdc, 0, -brick_half_height - back_part_offset, Brick_Width * Global_Scale, brick_half_height - back_part_offset);

        //The foreground
        SelectObject(hdc, Brick_Blue_Pen);
        SelectObject(hdc, Brick_Blue_Brush);

        Rectangle(hdc, 0, -brick_half_height, Brick_Width * Global_Scale, brick_half_height);

        SetWorldTransform(hdc, &old_xform);
    }
}
//--------------------------------------------------------------------------------------------------------------
void Draw_Level(HDC hdc)
{// Draw all bricks on the level
    for (int i = 0; i < 14; i++)
        for (int j = 0; j < 12; j++)
            Draw_Brick(hdc, Level_X_Offset + j * Cell_Width, Level_Y_Offset + i * Cell_Height, (EBrick_Type)Level_01[i][j]);
}
//--------------------------------------------------------------------------------------------------------------
void Draw_Platform(HDC hdc, int x, int y)
{ 

    //1. Draw the side balls
    SelectObject(hdc, Platform_Circle_Pen);
    SelectObject(hdc, Platform_Circle_Brush);

    Ellipse(hdc, x * Global_Scale, y * Global_Scale, (x + Circle_Size) * Global_Scale, (y + Circle_Size) * Global_Scale);
    Ellipse(hdc, (x + Inner_Width) * Global_Scale, y * Global_Scale, (x + Inner_Width + Circle_Size) * Global_Scale, (y + Circle_Size) * Global_Scale);

    //2. Draw a highlight

    SelectObject(hdc, Highlight_Pen);
    Arc(hdc, (x + 1) * Global_Scale, (y + 1) * Global_Scale, (x + Circle_Size - 1) * Global_Scale, (y + Circle_Size - 1) * Global_Scale,
             (x + 1 + 1)* Global_Scale, (y + 1)* Global_Scale, (x + 1) * Global_Scale, (y + 1 + 2) * Global_Scale);

    //3. Draw the middle part of the platform
    SelectObject(hdc, Platform_Inner_Pen);
    SelectObject(hdc, Platform_Inner_Brush);

    RoundRect(hdc, (x + 4) * Global_Scale, (y + 1) * Global_Scale, (x + 4 + Inner_Width - 1) * Global_Scale, (y + 1 + 5) * Global_Scale, 3 * Global_Scale, 3 * Global_Scale);
}
//--------------------------------------------------------------------------------------------------------------
void Draw_Frame(HDC hdc)
{// Draw screen game

    //Draw_Level(hdc);

    //Draw_Platform(hdc, 50, 100);

    for (int i = 0; i < 16; i++)
    {
        Draw_Brick_Letter(hdc, 20 + i * Cell_Width * Global_Scale, 100, i);

    }
}
//--------------------------------------------------------------------------------------------------------------