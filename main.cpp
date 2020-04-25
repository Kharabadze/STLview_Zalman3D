#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <tchar.h>
#include <windows.h>

#include "engine.h" //D.K.

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("CodeBlocksWindowsApp");

int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           _T("STL viewer (for Zalman 3D)"),       /* Title Text */
           WS_OVERLAPPEDWINDOW, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           544,                 /* The programs width */
           375,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);
    //--- Maximize
    SendMessage(hwnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);


    if(lpszArgument[0]==0){
        MessageBox(hwnd,"Use filename as argment.","Error",MB_OK);
        return 0;
    }
    if(!engine.open_new_file(lpszArgument)){//L"oct.stl"
        MessageBox(hwnd,"Can't open file.","Error",MB_OK);
        return 0;
    }
    //engine.open_new_file("Boat.stl");
    //engine.open_new_file("octopus-v5-5-spiral.stl");
    //engine.open_new_file("oct.stl");

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    engine.freemem();

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//RECT myrect;//Size of window
	//GetClientRect(hwnd,&myrect);
	//int maxx=myrect.right-myrect.left;
	//int maxy=myrect.bottom-myrect.top;
	POINT a;
	GetCursorPos(&a);
	ScreenToClient(hwnd,&a);
	int msx=a.x,msy=a.y;


    switch (message)                  /* handle the messages */
    {
        case WM_DESTROY:
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        case WM_PAINT:
            engine.on_draw(&hwnd);
            break;
        case WM_SIZE:
        case WM_MOVE:
            InvalidateRect(hwnd,NULL,false);
            break;
        //-------------------------------------------------------
		case WM_LBUTTONDOWN:
            engine.on_mouse_left_down(msx,msy);
		    break;
		case WM_LBUTTONUP:
            engine.on_mouse_left_up(msx,msy);
		    break;
		case WM_MOUSEMOVE:
			{
				if(engine.on_mouse_move(msx,msy,(wParam&MK_LBUTTON) != 0))
					InvalidateRect(hwnd,NULL,false);
			}
			break;
		case WM_MOUSEWHEEL:
			{
				short steps = ((wParam>>16) & 0xffff);
				steps /= WHEEL_DELTA;
				engine.on_mouse_wheel(msx,msy,steps);
				InvalidateRect(hwnd,NULL,false);
			}
			break;
        //-------------------------------------------------------

        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }


    return DefWindowProc (hwnd, message, wParam, lParam);;
}
