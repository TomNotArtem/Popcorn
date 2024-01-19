#include "Engine.h"

char Level_01[AsEngine::Level_Height][AsEngine::Level_Width] =
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

AsEngine::AsEngine()
    : Inner_Width(21), Platform_X_Pos(Border_X_Offset), Platform_X_Step(Global_Scale * 2),
      Platform_Width(28), Ball_X_Pos(20), Ball_Y_Pos(170), Ball_Speed(3.0), Ball_Direction(M_PI - M_PI_4)
{
}
//--------------------------------------------------------------------------------------------------------------
void AsEngine::Init_Engine(HWND hwnd)
{// Start up settings

    Hwnd = hwnd;

    Highlight_Pen = CreatePen(PS_SOLID, 0, RGB(255, 255, 255));
    Letter_Pen = CreatePen(PS_SOLID, Global_Scale, RGB(255, 255, 255));

    Create_Pen_Brush(15, 63, 31, BG_Pen, BG_Brush);
    Create_Pen_Brush(255, 85, 85, Brick_Red_Pen, Brick_Red_Brush);
    Create_Pen_Brush(85, 255, 255, Brick_Blue_Pen, Brick_Blue_Brush);
    Create_Pen_Brush(151, 0, 0, Platform_Circle_Pen, Platform_Circle_Brush);
    Create_Pen_Brush(0, 128, 192, Platform_Inner_Pen, Platform_Inner_Brush);
    Create_Pen_Brush(255, 255, 255, Ball_Pen, Ball_Brush);
    Create_Pen_Brush(85, 255, 255, Border_Blue_Pen, Border_Blue_Brush);
    Create_Pen_Brush(255, 255, 255, Border_White_Pen, Border_White_Brush);

    Level_Rect.left = Level_X_Offset * Global_Scale;
    Level_Rect.top = Level_Y_Offset * Global_Scale;
    Level_Rect.right = Level_Rect.left + Cell_Width * Level_Width * Global_Scale;
    Level_Rect.bottom = Level_Rect.top + Cell_Height * Level_Height * Global_Scale;


    Redraw_Platform();

    SetTimer(Hwnd, Timer_ID, 50, 0);

}
//--------------------------------------------------------------------------------------------------------------

void AsEngine::Draw_Frame(HDC hdc, RECT& paint_area)
{// Draw screen game

    RECT intersection_rect;

    // Draw only where there are changes
    if (IntersectRect(&intersection_rect, &paint_area, &Level_Rect))
        Draw_Level(hdc);

    if (IntersectRect(&intersection_rect, &paint_area, &Platform_Rect))
        Draw_Platform(hdc, Platform_X_Pos, Platform_Y_Pos);

    //for (int i = 0; i < 16; i++)
    //{
    //    Draw_Brick_Letter(hdc, 20 + i * Cell_Width * Global_Scale, 100, EBT_Blue, ELT_O, i);
    //    Draw_Brick_Letter(hdc, 20 + i * Cell_Width * Global_Scale, 130, EBT_Red, ELT_O, i);
    //}

    if (IntersectRect(&intersection_rect, &paint_area, &Ball_Rect))
        Draw_Ball(hdc, paint_area);

    Draw_Bounds(hdc, paint_area);
}
//--------------------------------------------------------------------------------------------------------------

int AsEngine::On_Key_Down(EKey_Type key_type)
{
    switch (key_type)
    {
    case EKT_Left:
        Platform_X_Pos -= Platform_X_Step;

        if (Platform_X_Pos <= Border_X_Offset)
            Platform_X_Pos = Border_X_Offset;

        Redraw_Platform();
        break;
    case EKT_Right:
        Platform_X_Pos += Platform_X_Step;

        if (Platform_X_Pos >= Max_X_Pos - Platform_Width + 1)
            Platform_X_Pos = Max_X_Pos - Platform_Width + 1;

        Redraw_Platform();
        break;

    case EKT_Space:
        break;
    }
    return 0;
}
//--------------------------------------------------------------------------------------------------------------
int AsEngine::On_Timer()
{
    Move_Ball();
    return 0;
}
//--------------------------------------------------------------------------------------------------------------

void AsEngine::Create_Pen_Brush(unsigned char r, unsigned char g, unsigned char b, HPEN& pen, HBRUSH& brush)
{
    pen = CreatePen(PS_SOLID, 0, RGB(r, g, b));
    brush = CreateSolidBrush(RGB(r, g, b));

}//--------------------------------------------------------------------------------------------------------------

void AsEngine::Redraw_Platform()
{
    Prev_Patform_Rect = Platform_Rect;

    Platform_Rect.left = Platform_X_Pos * Global_Scale;
    Platform_Rect.top = Platform_Y_Pos * Global_Scale;
    Platform_Rect.right = Platform_Rect.left + Platform_Width * Global_Scale;
    Platform_Rect.bottom = Platform_Rect.top + Platform_Height * Global_Scale;


    InvalidateRect(Hwnd, &Prev_Patform_Rect, FALSE);
    InvalidateRect(Hwnd, &Platform_Rect, FALSE);
}
//--------------------------------------------------------------------------------------------------------------

void AsEngine::Draw_Brick(HDC hdc, int x, int y, EBrick_Type brick_type)
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

void AsEngine::Set_Brick_Letter_Color(bool is_switch_color, HPEN &front_pen, HBRUSH &front_brush, HPEN &back_pen, HBRUSH &back_brush)
{
    if (is_switch_color)
    {
        front_pen = Brick_Red_Pen;
        front_brush = Brick_Red_Brush;

        back_pen = Brick_Blue_Pen;
        back_brush = Brick_Blue_Brush;
    }
    else
    {
        front_pen = Brick_Blue_Pen;
        front_brush = Brick_Blue_Brush;

        back_pen = Brick_Red_Pen;
        back_brush = Brick_Red_Brush;
    }
}
//--------------------------------------------------------------------------------------------------------------

void AsEngine::Draw_Brick_Letter(HDC hdc, int x, int y, EBrick_Type brick_type, ELetter_Type letter_type, int rotation_step)
{
    bool switch_color;
    double offset;
    float rotation_angle; // Converting steps to an angle
    int brick_half_height = Brick_Height * Global_Scale / 2;
    int back_part_offset;
    HPEN front_pen, back_pen;
    HBRUSH front_brush, back_brush;
    XFORM xform, old_xform;

    if (!(brick_type == EBT_Blue || brick_type == EBT_Red))
        return; // Falling letters can only be made of bricks of this color

    // Correction of rotation pitch and rotation angle
    rotation_step = rotation_step % 16;

    if (rotation_step < 8)
        rotation_angle = 2.0f * (float)M_PI / 16.0f * (float)rotation_step;
    else
        rotation_angle = 2.0f * (float)M_PI / 16.0f * (float)(8 - rotation_step);

    if (rotation_step > 4 && rotation_step <= 12)
    {
        switch_color = brick_type == EBT_Blue;
    }
    else
    { 
        switch_color = brick_type == EBT_Red;
    }

    AsEngine::Set_Brick_Letter_Color(switch_color, front_pen, front_brush, back_pen, back_brush);



    if (rotation_step == 4 || rotation_step == 12)
    {
        // Draw background
        SelectObject(hdc, back_pen);
        SelectObject(hdc, back_brush);

        Rectangle(hdc, x, y + brick_half_height - Global_Scale, x + Brick_Width * Global_Scale, y + brick_half_height);
        
        // Draw foreground
        SelectObject(hdc, front_pen);
        SelectObject(hdc, front_brush);

        Rectangle(hdc, x, y + brick_half_height, x + Brick_Width * Global_Scale, y + brick_half_height + Global_Scale - 1);
    }
    else
    {
        SetGraphicsMode(hdc, GM_ADVANCED);

        // Setting up the Letter Flip Matrix
        xform.eM11 = 1;
        xform.eM12 = 0;
        xform.eM21 = 0;
        xform.eM22 = cosf(rotation_angle);
        xform.eDx = (float)x;
        xform.eDy = (float)y + (float)brick_half_height;
        GetWorldTransform(hdc, &old_xform);
        SetWorldTransform(hdc, &xform);

        // Draw background
        SelectObject(hdc, back_pen);
        SelectObject(hdc, back_brush);

        offset = 3.0 * (1.0 - fabs(xform.eM22)) * (double)Global_Scale;
        back_part_offset = (int)round(offset);
        Rectangle(hdc, 0, -brick_half_height - back_part_offset, Brick_Width * Global_Scale, brick_half_height - back_part_offset);

        // Draw foreground
        SelectObject(hdc, front_pen);
        SelectObject(hdc, front_brush);

        Rectangle(hdc, 0, -brick_half_height, Brick_Width * Global_Scale, brick_half_height);

        if (rotation_step > 4 && rotation_step <= 12)
        {
            if (letter_type == ELT_O)
            { 
            SelectObject(hdc, Letter_Pen);
            Ellipse(hdc, 0 + 5 * Global_Scale, (-5 * Global_Scale) / 2, 0 + 10 * Global_Scale, 5 * Global_Scale / 2);
            }
        }


        SetWorldTransform(hdc, &old_xform);
    }
}
//--------------------------------------------------------------------------------------------------------------

void AsEngine::Draw_Level(HDC hdc)
{// Draw all bricks on the level
    for (int i = 0; i < Level_Height; i++)
        for (int j = 0; j < Level_Width; j++)
            Draw_Brick(hdc, Level_X_Offset + j * Cell_Width, Level_Y_Offset + i * Cell_Height, (EBrick_Type)Level_01[i][j]);
}
//--------------------------------------------------------------------------------------------------------------

void AsEngine::Draw_Platform(HDC hdc, int x, int y)
{ 
    SelectObject(hdc, BG_Pen);
    SelectObject(hdc, BG_Brush);

    Rectangle(hdc, Prev_Patform_Rect.left, Prev_Patform_Rect.top, Prev_Patform_Rect.right, Prev_Patform_Rect.bottom);

    // 1. Draw the side balls
    SelectObject(hdc, Platform_Circle_Pen);
    SelectObject(hdc, Platform_Circle_Brush);

    Ellipse(hdc, x * Global_Scale, y * Global_Scale, (x + Circle_Size) * Global_Scale, (y + Circle_Size) * Global_Scale);
    Ellipse(hdc, (x + Inner_Width) * Global_Scale, y * Global_Scale, (x + Inner_Width + Circle_Size) * Global_Scale, (y + Circle_Size) * Global_Scale);

    // 2. Draw a highlight

    SelectObject(hdc, Highlight_Pen);
    Arc(hdc, (x + 1) * Global_Scale, (y + 1) * Global_Scale, (x + Circle_Size - 1) * Global_Scale, (y + Circle_Size - 1) * Global_Scale,
             (x + 1 + 1)* Global_Scale, (y + 1)* Global_Scale, (x + 1) * Global_Scale, (y + 1 + 2) * Global_Scale);

    // 3. Draw the middle part of the platform
    SelectObject(hdc, Platform_Inner_Pen);
    SelectObject(hdc, Platform_Inner_Brush);

    RoundRect(hdc, (x + 4) * Global_Scale, (y + 1) * Global_Scale, (x + 4 + Inner_Width - 1) * Global_Scale, (y + 1 + 5) * Global_Scale, 3 * Global_Scale, 3 * Global_Scale);
}
//--------------------------------------------------------------------------------------------------------------

void AsEngine::Draw_Ball(HDC hdc, RECT &paint_area)
{
    // 1. Clean BG
    SelectObject(hdc, BG_Pen);
    SelectObject(hdc, BG_Brush);

    Ellipse(hdc, Prev_Ball_Rect.left, Prev_Ball_Rect.top, Prev_Ball_Rect.right - 1, Prev_Ball_Rect.bottom - 1);
    
    // 2. Draw ball
    SelectObject(hdc, Ball_Pen);
    SelectObject(hdc, Ball_Brush);

    Ellipse(hdc, Ball_Rect.left, Ball_Rect.top, Ball_Rect.right - 1, Ball_Rect.bottom - 1);
}
//--------------------------------------------------------------------------------------------------------------

void AsEngine::Draw_Border(HDC hdc, int x, int y, bool top_border)
{ //Draw element of border level
    
    // Main line
    SelectObject(hdc, Border_Blue_Pen);
    SelectObject(hdc, Border_Blue_Brush);

    if (top_border)
    Rectangle(hdc, x * Global_Scale, (y + 1) * Global_Scale, (x + 4) * Global_Scale, (y + 4) * Global_Scale);
    else
    Rectangle(hdc, (x + 1) * Global_Scale, y * Global_Scale, (x + 4) * Global_Scale, (y + 4) * Global_Scale);

    // White part of the line
    SelectObject(hdc, Border_White_Pen);
    SelectObject(hdc, Border_White_Brush);

    if (top_border)
    Rectangle(hdc, x * Global_Scale, y * Global_Scale, (x + 4) * Global_Scale, (y + 1) * Global_Scale);
    else
    Rectangle(hdc, x * Global_Scale, y * Global_Scale, (x + 1) * Global_Scale, (y + 4) * Global_Scale);

    // Perforation on line
    SelectObject(hdc, BG_Pen);
    SelectObject(hdc, BG_Brush);

    if (top_border)
    Rectangle(hdc, (x + 2) * Global_Scale, (y + 2) * Global_Scale, (x + 3) * Global_Scale, (y + 3) * Global_Scale);
    else
    Rectangle(hdc, (x + 2) * Global_Scale, (y + 1) * Global_Scale, (x + 3) * Global_Scale, (y + 2) * Global_Scale);

}
//--------------------------------------------------------------------------------------------------------------

void AsEngine::Draw_Bounds(HDC hdc, RECT &paint_area)
{//Draw border of level
    

    // 1. Line on the left
    for (int i = 0; i < 50; i++)
        Draw_Border(hdc, 2, 1 + i * 4, false);

    // 2. Line on the right
    for (int i = 0; i < 50; i++)
        Draw_Border(hdc, 201, 1 + i * 4, false);

    // 3. Line on the top
    for (int i = 0; i < 50; i++)
        Draw_Border(hdc, 3 + i * 4, 0, true);
}
//--------------------------------------------------------------------------------------------------------------

void AsEngine::Check_Level_Brick_Hit(int &next_y_pos)
{
    // Correction the position when reflected of the brick
    int brick_y_pos = Level_Y_Offset + Level_Height * Cell_Height;

    for (int i = Level_Height - 1; i >= 0; i--)
    {
        for (int j = 0; j < Level_Width; j++)
        {
            if (Level_01[i][j] == 0)
                continue;

            if (next_y_pos < brick_y_pos)
            {
                next_y_pos = brick_y_pos - (next_y_pos - brick_y_pos);
                Ball_Direction = -Ball_Direction;
            }
        }
        brick_y_pos -= Cell_Height;
    }
}
//--------------------------------------------------------------------------------------------------------------

void AsEngine::Move_Ball()
{
    int next_x_pos, next_y_pos;
    int max_x_pos = Max_X_Pos - Ball_Size;
    int platform_y_pos = Platform_Y_Pos - Ball_Size;

    Prev_Ball_Rect = Ball_Rect;

    next_x_pos = Ball_X_Pos + (int)(Ball_Speed * cos(Ball_Direction));
    next_y_pos = Ball_Y_Pos - (int)(Ball_Speed * sin(Ball_Direction));

    // Correction the position when reflected of the border
    if (next_x_pos < Border_X_Offset)
    {
        next_x_pos = Level_X_Offset - (next_x_pos - Level_X_Offset);
        Ball_Direction = M_PI - Ball_Direction;
    }

    if (next_y_pos < Border_Y_Offset)
    {
        next_y_pos = Border_Y_Offset - (next_y_pos - Border_Y_Offset);
        Ball_Direction = -Ball_Direction;
    }

    if (next_x_pos > max_x_pos)
    {
        next_x_pos = max_x_pos - (next_x_pos - max_x_pos);
        Ball_Direction = M_PI - Ball_Direction;
    }

    if (next_y_pos > Max_Y_Pos)
    {
        next_y_pos = Max_Y_Pos - (next_y_pos - Max_Y_Pos);
        Ball_Direction = M_PI + (M_PI - Ball_Direction);
    }

    // Correction the position when reflected of the platform
    if (next_y_pos > platform_y_pos)
    {
        if (next_x_pos >= Platform_X_Pos && next_x_pos <= Platform_X_Pos + Platform_Width)
        {
            next_y_pos = platform_y_pos - (next_y_pos - platform_y_pos);
            Ball_Direction = M_PI + (M_PI - Ball_Direction);
        }
    }

    // Correction the position when reflected of the brick
    Check_Level_Brick_Hit(next_y_pos);

    // Moving the ball
    Ball_X_Pos = next_x_pos;
    Ball_Y_Pos = next_y_pos;

    Ball_Rect.left = Ball_X_Pos * Global_Scale;
    Ball_Rect.top = Ball_Y_Pos * Global_Scale;
    Ball_Rect.right = Ball_Rect.left + Ball_Size * Global_Scale;
    Ball_Rect.bottom = Ball_Rect.top + Ball_Size * Global_Scale;

    InvalidateRect(Hwnd, &Prev_Ball_Rect, FALSE);
    InvalidateRect(Hwnd, &Ball_Rect, FALSE);
}
//--------------------------------------------------------------------------------------------------------------
