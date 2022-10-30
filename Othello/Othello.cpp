// Othello.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "Othello.h"

#define CELL_SIZE               80
#define CIRCLE_SIZE             80
#define GAP                     5
#define BOARD_SIZE              8

#define MAX_LOADSTRING          100
#define NUM_OF_PLAYERS          2
#define MAX_PLACEMENT           BOARD_SIZE * BOARD_SIZE

int player = 2; // 1 : 회색, 2 : 검은색
int playerNumOfPlacement[NUM_OF_PLAYERS] = { 2, 2 };

// https://ko.wikipedia.org/wiki/%EC%98%A4%EB%8D%B8%EB%A1%9C
// 매 턴마다 

typedef struct _pos
{
    _pos() { x = 0; y = 0; }
    _pos(int _x, int _y) : x(_x), y(_y) {}
    int x, y;
} Pos;

Pos cellPos[BOARD_SIZE][BOARD_SIZE];
int board[BOARD_SIZE][BOARD_SIZE];

Pos checkAround[8] = { {0,-1}, {1,-1}, {1,0}, {1,1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1} };

bool CheckAround(int x, int y);

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_OTHELLO, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OTHELLO));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OTHELLO));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_OTHELLO);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    static int x, y;
    static bool moving = false;
    HBRUSH brush, oldBrush;

    switch (message)
    {
    case WM_CREATE:
        board[3][3] = 1;
        board[4][4] = 1;
        board[3][4] = 2;
        board[4][3] = 2;
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            hdc = BeginPaint(hWnd, &ps);



            for (int i = 0; i < BOARD_SIZE; i++)
            {
                for (int j = 0; j < BOARD_SIZE; j++)
                {
                    Rectangle(hdc, j * CELL_SIZE, i * CELL_SIZE, (j+1) * CELL_SIZE, (i+1) * CELL_SIZE);
                    cellPos[i][j] = Pos(j * CELL_SIZE, i * CELL_SIZE);
                    
                    if (board[i][j] == 1)
                    {
                        brush = CreateSolidBrush(RGB(200, 200, 200));
                        oldBrush = (HBRUSH)SelectObject(hdc, brush);
                        Ellipse(hdc, cellPos[i][j].x, cellPos[i][j].y, cellPos[i][j].x + CELL_SIZE, cellPos[i][j].y + CELL_SIZE);
                        SelectObject(hdc, oldBrush);
                        DeleteObject(brush);
                    }
                    else if (board[i][j] == 2)
                    {
                        brush = CreateSolidBrush(RGB(0, 0, 0));
                        oldBrush = (HBRUSH)SelectObject(hdc, brush);
                        Ellipse(hdc, cellPos[i][j].x, cellPos[i][j].y, cellPos[i][j].x + CELL_SIZE, cellPos[i][j].y + CELL_SIZE);
                        SelectObject(hdc, oldBrush);
                        DeleteObject(brush);
                    }
                }
            }
          
           // wchar_t scoreStr[100];
           // swprintf_s(scoreStr, L"%d, %d", mousePos.x, mousePos.y);
            if (moving)
            {
                if (board[y][x] == 0)
                    Ellipse(hdc, cellPos[y][x].x, cellPos[y][x].y, cellPos[y][x].x + CELL_SIZE, cellPos[y][x].y + CELL_SIZE);
            }
            else
            {
                if(board[y][x] == 0)
                    Rectangle(hdc, cellPos[y][x].x, cellPos[y][x].y, cellPos[y][x].x + CELL_SIZE, cellPos[y][x].y + CELL_SIZE);
            }

            EndPaint(hWnd, &ps);
        }
        break;

    case WM_LBUTTONUP:

        POINT mousePos;
        GetCursorPos(&mousePos);
        ScreenToClient(hWnd, &mousePos);

        if (BOARD_SIZE * CELL_SIZE > mousePos.x && BOARD_SIZE * CELL_SIZE > mousePos.y)
        {
            x = mousePos.x / CELL_SIZE;
            y = mousePos.y / CELL_SIZE;

            if (board[y][x] == 0)
            {
                if (CheckAround(x, y) == false)
                    break;
                board[y][x] = player;
                player = (player % 2) + 1;
                InvalidateRect(hWnd, NULL, FALSE);
            }
        }

        break;

    case WM_MOUSEMOVE:
    {
        POINT mousePos;
        GetCursorPos(&mousePos);
        ScreenToClient(hWnd, &mousePos);

        if (BOARD_SIZE * CELL_SIZE > mousePos.x && BOARD_SIZE * CELL_SIZE > mousePos.y)
        {
            x = mousePos.x / CELL_SIZE;
            y = mousePos.y / CELL_SIZE;

            moving = true;
            InvalidateRect(hWnd, NULL, FALSE);
        }
        else
            moving = false;
        
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

bool CheckAround(int x, int y)
{
    bool placeable = false;

 
    // 모든 방향 체크
    for (int k = 0; k < 8; k++)
    {
        int checkX = checkAround[k].x + x;
        int checkY = checkAround[k].y + y;

        if (0 < checkX && checkX < BOARD_SIZE && 0 < checkY && checkY < BOARD_SIZE)
        {
            int opponent = (player % 2) + 1;

            if (board[checkY][checkX] == opponent) // 체크할 방향에 적이 있다면 
            {
                Pos path[BOARD_SIZE];
                int p = 0;

                do 
                {
                    path[p++] = Pos(checkX, checkY);
                    checkX += checkAround[k].x;
                    checkY += checkAround[k].y;
                    
                    if (board[checkY][checkX] == player)
                        break;
                } while (board[checkY][checkX] != 0);

                int idx = 0;
                if (board[checkY][checkX] != 0)
                {
                    placeable = true;
                    while (idx < p)
                    {
                        board[path[idx].y][path[idx].x] = player;
                        idx++;
                    }
                }
            }
        }
    }
        
    
    return placeable;
}