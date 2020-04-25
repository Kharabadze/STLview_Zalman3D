#pragma once
#include<windows.h>
#include"stl_model.h"
struct engine_t{
    stl_model_t in_stl;
    stl_model_t pr_stl;
    tr_matr_t   tr_mat;

    HDC hDC;
    HGLRC hRC;
    bool open_new_file(char *filename);
    bool on_draw(HWND *hWnd);
    bool freemem(void);
    //---
    bool mouse_pressed;
    int old_x,old_y;
    bool on_mouse_wheel(int msx,int msy,int steps);
    bool on_mouse_move(int msx,int msy,bool pressed);
    bool on_mouse_left_down(int msx,int msy);
    bool on_mouse_left_up(int msx,int msy);
    //---
    engine_t(void);
    ~engine_t(void);
};
extern engine_t engine;

