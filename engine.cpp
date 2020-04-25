#include"engine.h"

#include<gl\gl.h>
#include<gl\glu.h>
#include<math.h>

engine_t engine;
bool engine_t::open_new_file(char *filename){
    if(!in_stl.open(filename))return false;
    if(!in_stl.correct())return false;
    if(!pr_stl.new_empty(in_stl.number))return false;
    if(!pr_stl.convert_from(&in_stl,&tr_mat))return false;
    return true;
}
bool engine_t::on_draw(HWND *hWnd){
	//---
	RECT myrect;
	GetClientRect(*hWnd,&myrect);
	int Width = myrect.right-myrect.left;
	int Height = myrect.bottom-myrect.top;

	POINT main_point;
	main_point.x=0;main_point.y=0;
	ClientToScreen(*hWnd,&main_point);


    hDC=GetDC(*hWnd);
	//---------------------------------------------------

/*
	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory( &pfd, sizeof( pfd ) );
	pfd.nSize = sizeof( pfd );
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	//pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_DEPTH_DONTCARE;
	//pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_DEPTH_DONTCARE;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 24; //16;
	pfd.iLayerType = PFD_MAIN_PLANE;
	int iFormat = ChoosePixelFormat( hDC, &pfd );
	SetPixelFormat( hDC, iFormat, &pfd );
*/
    int m_GLPixelIndex;
    PIXELFORMATDESCRIPTOR pfd;
    pfd.nSize       = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion    = 1;
    pfd.dwFlags   = PFD_DRAW_TO_WINDOW |
                    PFD_SUPPORT_OPENGL |
                    PFD_DOUBLEBUFFER;
    pfd.iPixelType     = PFD_TYPE_RGBA;
    pfd.cColorBits     = 32;
    pfd.cRedBits       = 8;
    pfd.cRedShift      = 16;
    pfd.cGreenBits     = 8;
    pfd.cGreenShift    = 8;
    pfd.cBlueBits      = 8;
    pfd.cBlueShift     = 0;
    pfd.cAlphaBits     = 0;
    pfd.cAlphaShift    = 0;
    pfd.cAccumBits     = 64;
    pfd.cAccumRedBits  = 16;
    pfd.cAccumGreenBits   = 16;
    pfd.cAccumBlueBits    = 16;
    pfd.cAccumAlphaBits   = 0;
    pfd.cDepthBits        = 32;
    pfd.cStencilBits      = 8;
    pfd.cAuxBuffers       = 0;
    pfd.iLayerType        = PFD_MAIN_PLANE;
    pfd.bReserved         = 0;
    pfd.dwLayerMask       = 0;
    pfd.dwVisibleMask     = 0;
    pfd.dwDamageMask      = 0;
    m_GLPixelIndex = ChoosePixelFormat( hDC, &pfd);
    if(m_GLPixelIndex==0){ // Let's choose a default index.
        m_GLPixelIndex = 1;
        if(DescribePixelFormat(hDC,m_GLPixelIndex,sizeof(PIXELFORMATDESCRIPTOR),&pfd)==0){
            MessageBox( NULL, "Can't describe pixel format.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
            exit(0);
        }
    }
    if (SetPixelFormat( hDC, m_GLPixelIndex, &pfd)==FALSE){
        MessageBox( NULL, "Can't set pixel format.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
        exit(0);
    }

	//--- Context
	hRC = wglCreateContext( hDC );
	if(!hRC){
        MessageBox( NULL, "Can't Create A GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
        exit(0);
    }
	wglMakeCurrent( hDC, hRC );

	//glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |GL_ACCUM_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
	//--- Special features
	//glEnable(GL_POLYGON_STIPPLE);

	//=================================================================
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_RGBA);
    glEnable(GL_DOUBLE);

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |GL_ACCUM_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
	glEnable(GL_POLYGON_STIPPLE);


    float razr = 1050.0/300.0;//pix/mm
    float eye_x = 31.0*razr;//31 mm -> pixels
    float eye_z = 600.0*razr;//600 mm -> pixels
    //0 = Left channel, 1=Right channel
	//=============================================================== process model
	pr_stl.convert_from(&in_stl,&tr_mat);
    //===============================================================

    glDisable(GL_TEXTURE_2D);

	unsigned int maska[2][32]={{
		0xffffffff,0,0xffffffff,0,0xffffffff,0,0xffffffff,0,
		0xffffffff,0,0xffffffff,0,0xffffffff,0,0xffffffff,0,
		0xffffffff,0,0xffffffff,0,0xffffffff,0,0xffffffff,0,
		0xffffffff,0,0xffffffff,0,0xffffffff,0,0xffffffff,0
	},{
		0,0xffffffff,0,0xffffffff,0,0xffffffff,0,0xffffffff,
		0,0xffffffff,0,0xffffffff,0,0xffffffff,0,0xffffffff,
		0,0xffffffff,0,0xffffffff,0,0xffffffff,0,0xffffffff,
		0,0xffffffff,0,0xffffffff,0,0xffffffff,0,0xffffffff
	}};

	for(int channel=0;channel<2;channel++){
        double Sh = (channel==0)?eye_x:-eye_x;
        double L =eye_z;//pixel
        double Wid2= Width + 2*eye_x;
        double Matrix[16]={
            2.0/Wid2        ,0.0       ,0.0    ,0.0    ,
            0.0             ,2.0/Height,0.0    ,0.0    ,
            0.0             ,0.0       ,-1.0/L ,-1.0/L ,
            2.0*Sh/Wid2     ,0.0       ,0.0    ,1.0
        };
        glLoadMatrixd(Matrix);
        if(channel==0)glViewport(-eye_x*2,0,Wid2,Height);
        else glViewport(0,0,Wid2,Height);
        //glViewport(-Sh,0,Wid2,Height);
        //glViewport(-eye_x*0.5-Sh,0,Wid2,Height);

		int mask_num=(channel+main_point.y+Height)%2;
		glPolygonStipple((GLubyte*)maska[mask_num]);//STEREO_MASK

        glColor3d(0.5,0.5,0.5);
        glBegin(GL_TRIANGLES);

        for(int i=0;i<pr_stl.number;i++){
            float color[3];
            //Расчёт освещённости
            for(int vn=0;vn<3;vn++){
                float observer[3]={-Sh,0.0,L};
                float obs_len2 = 0.0f;
                for(int cc=0;cc<3;cc++){
                    observer[cc]-=pr_stl.triangles[i].vert[vn][cc];
                    obs_len2 += observer[cc]*observer[cc];
                }
                double obs_len = sqrt(obs_len2);

                double cosangle = 0.0;
                for(int cc=0;cc<3;cc++){
                    cosangle += observer[cc] * pr_stl.triangles[i].vert[3][cc];;
                }


                cosangle = fabs( cosangle / obs_len );

                color[vn] = 0.5*(1.0+cosangle);// * pow(2.0,-obs_len/eye_z);
                 //0.5+0.001*pr_stl.triangles[i].vert[vn][2];
            }

            //Рисование
            for(int vn=0;vn<3;vn++){
                glColor3d(color[vn],color[vn],color[vn]);
                glVertex3f(
                    pr_stl.triangles[i].vert[vn][0],
                    pr_stl.triangles[i].vert[vn][1],
                    pr_stl.triangles[i].vert[vn][2]);
            }
        }
        glEnd();
    }

	//=================================================================

	SwapBuffers(hDC);

	//wglMakeCurrent( NULL, NULL );
    wglDeleteContext( hRC );

	ReleaseDC(*hWnd,hDC);

	//MessageBox(0,"good","Ondraw finished",0);//Debug
    return true;
}

bool engine_t::on_mouse_wheel(int msx,int msy,int steps){
    if(steps!=0)
        tr_mat.scale(steps);
    return (steps!=0);
}

bool engine_t::on_mouse_left_down(int msx,int msy){
    mouse_pressed=true;
    old_x = msx;
    old_y = msy;
    return true;
}
bool engine_t::on_mouse_left_up(int msx,int msy){
    mouse_pressed=false;
    old_x = msx;
    old_y = msy;
    return true;
}

bool engine_t::on_mouse_move(int msx,int msy,bool pressed){
    int difx = msx-old_x;
    int dify = msy-old_y;
    old_x = msx;
    old_y = msy;

    if(!pressed){
        mouse_pressed=false;
        return false;
    }

    if(mouse_pressed){
        if((difx!=0)||(dify!=0))
            tr_mat.rotate_xy(difx,dify);
        //MessageBox( NULL, "Rotation of matrix", "Good", MB_OK | MB_ICONEXCLAMATION);
        return true;
    }
    return false;
}


bool engine_t::freemem(void){
    in_stl.freemem();
    pr_stl.freemem();
    return true;
}

engine_t::engine_t(void){
    mouse_pressed = false;
    return;
}
engine_t::~engine_t(void){
    return;
}
