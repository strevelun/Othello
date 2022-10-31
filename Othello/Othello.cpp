// Othello.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "Othello.h"

#define CELL_SIZE               80
#define BOARD_SIZE              4

#define MAX_LOADSTRING          100
#define NUM_OF_PLAYERS          2
#define MAX_PLACEMENT           BOARD_SIZE * BOARD_SIZE

int player = 2; // 1 : 회색, 2 : 검은색
int playerNumOfPlacement[NUM_OF_PLAYERS] = { 2, 2 };
int total = 4;
int count = 0; // 2이상이 되면 게임 종료 (더 이상 놓을 자리가 없어서 2턴 이상 플레이어가 전환되는 경우)

// https://ko.wikipedia.org/wiki/%EC%98%A4%EB%8D%B8%EB%A1%9C
// 게임 종료 조건
// 1. 64개 놓았을 경우 v
// 2. 어느 한 쪽이 돌을 모두 뒤집은 경우 v
// 3. 한 차례에 양 쪽 모두 서로 차례를 넘겨야 하는 경우 v

typedef struct _pos
{
    _pos() { x = 0; y = 0; }
    _pos(int _x, int _y) : x(_x), y(_y) {}
    int x, y;
} Pos;

Pos cellPos[BOARD_SIZE][BOARD_SIZE];
int board[BOARD_SIZE][BOARD_SIZE];

Pos checkAround[8] = { {0,-1}, {1,-1}, {1,0}, {1,1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1} };

bool CheckAround(int x, int y, bool changeBoard = true);

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
    if (!InitInstance(hInstance, nCmdShow))
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
            // 총 둔 돌의 수가 64개일 때 또는 어느 한 쪽이 전부 뒤집힐 때 체크
            if (total >= MAX_PLACEMENT || playerNumOfPlacement[0] <= 0 || playerNumOfPlacement[1] <= 0 || count >= 2)
            {
                if (playerNumOfPlacement[0] > playerNumOfPlacement[1])
                    MessageBox(NULL, L"회색 플레이어 승!", L"결과", MB_OK);
                else if (playerNumOfPlacement[0] < playerNumOfPlacement[1])
                    MessageBox(NULL, L"검은색 플레이어 승!", L"결과", MB_OK);
                else
                    MessageBox(NULL, L"무승부", L"결과", MB_OK);
                break;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OTHELLO));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_OTHELLO);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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
    static bool placeable = true; // 게임 시작할때는 무조건 둘 수 있음




    switch (message)
    {
    case WM_CREATE:
        //board[3][3] = 1;
        //board[4][4] = 1;
        //board[3][4] = 2;
        //board[4][3] = 2;
        board[1][1] = 1;
        board[2][2] = 1;
        board[1][2] = 2;
        board[2][1] = 2;
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

        wchar_t scoreStr[100];
        swprintf_s(scoreStr, L"                                             ");
        TextOut(hdc, 700, 0, scoreStr, _tcslen(scoreStr));
        swprintf_s(scoreStr, L"회:%d vs 흑:%d, 전체 수:%d", playerNumOfPlacement[0], playerNumOfPlacement[1], total);
        TextOut(hdc, 700, 0, scoreStr, _tcslen(scoreStr));

        wchar_t curPlayer[10];
        swprintf_s(curPlayer, L"플레이어 %d", player);
        TextOut(hdc, 700, 400, curPlayer, _tcslen(curPlayer));

        // 만약 둘 수 있는 곳이 없다면 
        if (placeable == false && count < 2)
        {
            TextOut(hdc, 700, 200, L"                                                                     ", _tcslen(L"                                                                     "));
            TextOut(hdc, 700, 200, L"둘 수 있는 곳이 없기 때문에 다음 플레이어의 차례가 됩니다!", _tcslen(L"둘 수 있는 곳이 없기 때문에 다음 플레이어의 차례가 됩니다!"));
            //placeable = true;
        }
        else
        {
            TextOut(hdc, 700, 200, L"                                                                                                             ", 
                _tcslen(L"                                                                                                             "));
        }

        for (int i = 0; i < BOARD_SIZE; i++)
        {
            for (int j = 0; j < BOARD_SIZE; j++)
            {
                Rectangle(hdc, j * CELL_SIZE, i * CELL_SIZE, (j + 1) * CELL_SIZE, (i + 1) * CELL_SIZE);
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

        if (moving)
        {
            if (board[y][x] == 0)
            {
                HPEN pen;
                HBRUSH brush, oldBrush;

                if (player == 1)
                {
                    pen = CreatePen(PS_DOT, 2, RGB(200, 200, 200));
                    brush = CreateSolidBrush(RGB(200, 200, 200));
                }
                else
                {
                    pen = CreatePen(PS_DOT, 2, RGB(0, 0, 0));
                    brush = CreateSolidBrush(RGB(0, 0, 0));
                }

                HGDIOBJ oldPen = SelectObject(hdc, pen);
                Ellipse(hdc, cellPos[y][x].x, cellPos[y][x].y, cellPos[y][x].x + CELL_SIZE, cellPos[y][x].y + CELL_SIZE);
                oldBrush = (HBRUSH)SelectObject(hdc, brush);
                Ellipse(hdc, cellPos[y][x].x + (CELL_SIZE/3), cellPos[y][x].y + (CELL_SIZE / 3), cellPos[y][x].x + CELL_SIZE / 2, cellPos[y][x].y + CELL_SIZE / 2);
                SelectObject(hdc, oldBrush);
                SelectObject(hdc, oldPen);
                DeleteObject(pen);
            }
        }
        else
        {
            if (board[y][x] == 0)
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
                placeable = false; // 일단 둘 수 없다고 가정
                if (CheckAround(x, y) == true)
                {
                    total++;
                    board[y][x] = player;
                    player = (player % 2) + 1; // 다음 차례
                }

                // 상대방이 클릭해서 돌을 두면 그 다음 차례가 보드에서 둘 곳이 있는지 확인
                for (int i = 0; i < BOARD_SIZE; i++)
                {
                    for (int j = 0; j < BOARD_SIZE; j++)
                    {
                        if (board[i][j] == 0)
                        {
                            if (CheckAround(j, i, false) == true)// 돌다가 둘 수 있는 곳이 하나라도 있으면
                                placeable = true;
                        }
                    }
                }

                if (!placeable) // 둘 곳이 없으면 둘 곳이 없다고 출력하고 상대방 차례
                {
                    player = (player % 2) + 1;
                    count++;
                }
                InvalidateRect(hWnd, NULL, FALSE);
                //placeable = true;
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

bool CheckAround(int x, int y, bool changeBoard)
{
    bool placeable = false;

    // 모든 방향 체크
    for (int k = 0; k < 8; k++)
    {
        int checkX = checkAround[k].x + x;
        int checkY = checkAround[k].y + y;

        if (0 <= checkX && checkX < BOARD_SIZE && 0 <= checkY && checkY < BOARD_SIZE)
        {
            int opponent = (player % 2) + 1;

            if (board[checkY][checkX] == opponent) // 체크할 방향에 적이 있다면 
            {
                Pos path[BOARD_SIZE];
                int p = 0;

                bool playerFound = false;

                while (p < BOARD_SIZE)
                {
                    path[p++] = Pos(checkX, checkY);

                    // 다음 칸이 보드를 빠져나가는 위치라면 그대로 탐색 종료
                    if (0 > checkX + checkAround[k].x || checkX + checkAround[k].x >= BOARD_SIZE ||
                        0 > checkY + checkAround[k].y || checkY + checkAround[k].y >= BOARD_SIZE)
                        break;

                    // 다음 칸이 플레이어라면 
                    if (board[checkY + checkAround[k].y][checkX + checkAround[k].x] == player)
                    {
                        playerFound = true;
                        break;
                    }

                    // 다음 칸이 0이라면
                    if (board[checkY + checkAround[k].y][checkX + checkAround[k].x] == 0)
                        break;

                    // 다음 칸이 상대방 돌이라면 
                    checkX += checkAround[k].x;
                    checkY += checkAround[k].y;
                }

                int idx = 0;
                if (playerFound)
                {
                    placeable = true;
                    count = 0;

                    if (changeBoard == false)
                        break;

                    while (idx < p)
                    {
                        playerNumOfPlacement[player - 1]++;
                        playerNumOfPlacement[opponent - 1]--;
                        board[path[idx].y][path[idx].x] = player;
                        idx++;
                    }
                }
            }
        }
    }

    if (placeable && changeBoard)
        playerNumOfPlacement[player - 1]++;

    return placeable;
}