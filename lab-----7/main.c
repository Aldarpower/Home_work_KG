#include <windows.h>
#include <gl/gl.h>
#include "camera.h"
#include <math.h>

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

void Init_Light(float angleRot)
{
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    GLfloat light_position[] = { 6.0f, 0.0f, 3.0f, 1.0f };
    GLfloat light_spot_direction[] = {0.0, 0.0, -1.0, 1.0};
    GLfloat light_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat light_specular[] = { 0.2f, 0.2f, 0.2f, 32.0f };
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 40);

    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light_spot_direction);
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 8.0);

    glPushMatrix();
        glRotatef(angleRot,0,0,1);
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
            glBegin(GL_QUADS);
            glColor3f(10.0f, 10.0f, 10.0f);
            glVertex3f(light_position[0]-0.5f, light_position[1]-0.5f, light_position[2]);
            glVertex3f(light_position[0]+0.5f, light_position[1]-0.5f, light_position[2]);
            glVertex3f(light_position[0]+0.5f, light_position[1]+0.5f, light_position[2]);
            glVertex3f(light_position[0]-0.5f, light_position[1]+0.5f, light_position[2]);
        glEnd();
    glPopMatrix();

    glEnable(GL_LIGHT0);

}

void Init_Material()
{
    glEnable(GL_COLOR_MATERIAL); //ðàçðåøåíèÿ èñïîëüçîâàíèÿ

    //ìàòåðèàëà
    glShadeModel(GL_SMOOTH); // ñãëàæèâàåò ãðàíèöû
    GLfloat material_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat material_diffuse[] = { 0.0f, 1.0f, 1.0f, 1.0f };
    GLfloat material_specular[] = { 1.0f, 1.0f, 1.0f, 32.0f };
    GLfloat material_shininess[] = { 50.0f }; //áëåñê ìàòåðèàëà

    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void WndResize(int x, int y){
    glViewport(0,0,x,y); //ïåðåñòðàèâàåò ðàçìåðû îêíà
    float k=x/(float)y; //ñîîòíîøåíèå ñòîðîí
    float sz = 0.1; //åäèíèöà ðàçìåðà
    glLoadIdentity(); //çàãðóçêà åäèíè÷íîé ìàòðèöû
    glFrustum(-k*sz, k*sz, -sz, sz, sz*2, 100); //óñòàíîâêà ïåðñïåêòèâíîé ïðîýêöèè
}

void MoveCamera(){
    Camera_MoveDirectional(
        GetKeyState('W')< 0 ? 1 : GetKeyState('S')< 0 ? -1 : 0,
        GetKeyState('D')< 0 ? 1 : GetKeyState('A')< 0 ? -1 : 0, 0.1);
    Camera_AutoMoveByMouse(500,800,0.1);
}

void drawCoords() {
    glPushMatrix();
    glBegin(GL_LINES);
    // X-axis (red)
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-50.0f, 0.0f, 0.0f);
    glVertex3f(50.0f, 0.0f, 0.0f);
    // Y-axis (green)
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, -50.0f, 0.0f);
    glVertex3f(0.0f, 50.0f, 0.0f);
    // Z-axis (blue)
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, -50.0f);
    glVertex3f(0.0f, 0.0f, 50.0f);
    glEnd();
    glPopMatrix();

}

void drawChessboard(int n, float tileSize, float posX, float posY, float PosZ){
    float normal_vert[]={0,0,1, 0,0,1, 0,0,1, 0,0,1};
    // Draw the chessboard using a loop
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            // Set the color of the tile based on its position
            if ((i + j) % 2 == 0) glColor3f(1.0f, 1.0f, 1.0f);
            else glColor3f(0.0f, 0.0f, 0.0f);

            glEnableClientState(GL_NORMAL_ARRAY);
            glNormalPointer(GL_FLOAT,0,&normal_vert);

            // Draw the tile
            glPushMatrix();
                glTranslatef(i * tileSize + posX, j * tileSize + posY, PosZ);
                glBegin(GL_QUADS);
                glVertex3f(0.0f, 0.0f, 0.0f);
                glVertex3f(tileSize, 0.0f, 0.0f);
                glVertex3f(tileSize, tileSize, 0.0f);
                glVertex3f(0.0f, tileSize, 0.0f);
                glEnd();
            glPopMatrix();

            glDisableClientState(GL_NORMAL_ARRAY);
        }
    }
}

void drawCube(float posX, float posY, float PosZ){
    GLfloat vertices[] = {
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        -0.5f, 0.5f, -0.5f,
        -0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f
    };
    GLuint indices[] = {
        0, 1, 2,
        2, 3, 0,
        1, 5, 6,
        6, 2, 1,
        7, 6, 5,
        5, 4, 7,
        4, 0, 3,
        3, 7, 4,
        4, 5, 1,
        1, 0, 4,
        3, 2, 6,
        6, 7, 3
    };
    GLfloat normals[] = {
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };
    glPushMatrix();

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    glTranslatef(posX, posY, PosZ);
    glVertexPointer(3, GL_FLOAT, 0, vertices);

    glNormalPointer(GL_FLOAT, 0, normals);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, indices);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

    glPopMatrix();
}

void drawPrism(int n, int height)
{
     const float pi = acos(-1);
    const int vn = 3*n; // количество вершин

    float vertieces[3*vn]; // Это же и нормали.

    for (int i = 0; i < n; i++) {
        float phi = 2 * pi * (float)i / (float)n;
        float c = cos(phi);
        float s = sin(phi);

        // Вершина в верхнем основании
        vertieces[9*i + 0] = c;
        vertieces[9*i + 1] = s;
        vertieces[9*i + 2] = 1.0;

        // cthtlbyf
        vertieces[9*i + 3] = 0.7*c;
        vertieces[9*i + 4] = 0.7*s;
        vertieces[9*i + 5] = 0.0;

        // Вершина в нижнем основании
        vertieces[9*i + 6] = c;
        vertieces[9*i + 7] = s;
        vertieces[9*i + 8] = -1.0;

    }

    glPushMatrix();

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, vertieces);
    glNormalPointer(GL_FLOAT, 0, vertieces);

    // Боковые стороны
    GLuint side_index[4];
    for (int i = 0; i < n; i++) {
        for (int k = 0; k < 4; k++) {
            side_index[k] = (3*(i + k/2) + k%2) % vn;
        }

        glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, &side_index);
    }

    for (int i = 0; i < n; i++) {
        for (int k = 0; k < 4; k++) {
            side_index[k] = (3*(i + k/2) + k%2 + 1) % vn;
        }

        glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, &side_index);
    }

    // Верхнее основание
    GLuint base_index[n];
    for (int k = 0; k < n; k++) {
        base_index[k] = 3*k;
    }
    glDrawElements(GL_POLYGON, n, GL_UNSIGNED_INT, &base_index);

    // Нижнее основание
    for (int k = 0; k < n; k++) {
        base_index[k] = 3*k + 2;
    }
    glDrawElements(GL_POLYGON, n, GL_UNSIGNED_INT, &base_index);


    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

    glPopMatrix();
}

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;
    float theta = 0.0f;

    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
                          "GLSample",
                          "OpenGL Sample",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          1000,
                          600,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

    RECT rct;
    GetClientRect(hwnd,&rct);
    WndResize(rct.right,rct.bottom);
    glEnable(GL_DEPTH_TEST);

    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            /* OpenGL animation code goes here */
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glPushMatrix();

            if (GetForegroundWindow()==hwnd) MoveCamera();

            Camera_Apply();

            drawChessboard(16, 1, -8,-8,-1);

            glPushMatrix();
            glTranslatef(0.0, 0.0, 0.0);
            float transparency = 0.1;
            for (int i = 0; i < 10; i++)
            {
                glPushMatrix();
                    glRotatef(36*i, 0, 0, 1);
                    glTranslatef(4, 0, 0);
                    glColor4f(0,1,1,transparency);
                    drawPrism(27,2);
                glPopMatrix();
                transparency += 0.1;
            }
            glPopMatrix();

            glPushMatrix();
            glRotatef(theta,0,0,1);

            glPopMatrix();

            glTranslatef(0,0,-1);
            glLineWidth(3);
            drawCoords();
            int ShowCursor(false);
            Init_Material();
            Init_Light(theta);
            Init_Light(theta+5);
            Init_Light(theta+10);
            Init_Light(theta+15);
            Init_Light(theta+20);
            Init_Light(theta+25);
            glPopMatrix();

            SwapBuffers(hDC);

            theta += 1.0f;
            Sleep (1);
        }
    }

    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
        break;

        case WM_DESTROY:
            return 0;

        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case VK_ESCAPE:
                    PostQuitMessage(0);
                break;
            }
        }
        break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}
