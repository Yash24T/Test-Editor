#include <Windows.h>
#include <assert.h>
#include <stdio.h>

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")

TCHAR szTextToPrintT[] = TEXT("My Text Editor\n   <Press ECS to return to Home>\n");
TCHAR szTEMPTEXTTOPRINT[100][100];
int count = 0, i_line = 1;
int check_center = 1;
FILE* file_to_write = NULL;
int msg_to_print_flag = 0;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
char* glb_lpCmd_Line;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    glb_lpCmd_Line = lpCmdLine;
    static TCHAR szClassName[] = TEXT("Text Editor Application");
    static TCHAR szWindowName[] = TEXT("My Text Editor");

    WNDCLASSEX wndEX;
    MSG msg;

    ZeroMemory(&wndEX, sizeof(WNDCLASSEX));
    ZeroMemory(&msg, sizeof(MSG));

    HWND hWnd = NULL;
    HBRUSH hBrush = NULL;
    HCURSOR hCursor = NULL;
    HICON hIcon = NULL;
    HICON hIconSm = NULL;

    hBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
    assert(hBrush);

    hCursor = LoadCursor(NULL, IDC_ARROW);
    assert(hCursor);
    
    hIcon = LoadIcon(NULL, IDI_WINLOGO);
    assert(hIcon);
    
    hIconSm = LoadIcon(NULL, IDI_WINLOGO);
    assert(hIconSm);

    wndEX.cbSize = sizeof(WNDCLASSEX);
    wndEX.cbClsExtra = 0;
    wndEX.cbWndExtra = 0;
    wndEX.hbrBackground = hBrush;
    wndEX.hCursor = hCursor;
    wndEX.hIcon = hIcon;
    wndEX.hIcon = hIconSm;
    wndEX.lpszClassName = szClassName;
    wndEX.lpfnWndProc = WndProc;
    wndEX.lpszMenuName = NULL;
    wndEX.style = CS_HREDRAW | CS_VREDRAW ;

    ATOM bRet = RegisterClassEx(&wndEX);
    assert(bRet);

    hWnd = CreateWindowEx(WS_EX_APPWINDOW, szClassName, szWindowName, WS_OVERLAPPEDWINDOW, 
                            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                            NULL, NULL, hInstance, NULL);
    int i, j;
    for( i = 0; i < 100; i++)
    {
        for(j = 0; j < 100; j++)
        {
            szTEMPTEXTTOPRINT[i][j] = '\0';
        }
    }

    for(i = 0; i < 100; i++)
                {
                    szTEMPTEXTTOPRINT[i_line - 1][i] = szTextToPrintT[i];
                    if(szTextToPrintT[i] == '\0')
                        break;
                }
    ShowWindow(hWnd, nShowCmd);
    UpdateWindow(hWnd);


    while(GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        UpdateWindow(hWnd);
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static int cxChar, cyChar, cxCaps, cySize, cxSize; 
    
    TCHAR szBuffer[16]; 
    int i, j, iLen; 

    HDC hdc = NULL; 

    TEXTMETRIC tm; 
    PAINTSTRUCT ps; 

    switch(uMsg)
    {
        case WM_SIZE:
            cySize = HIWORD(lParam);
            cxSize = LOWORD(lParam);

        case WM_CREATE: 
        {
            hdc = GetDC(hWnd); 
            GetTextMetrics(hdc, &tm); 
            cxChar = tm.tmAveCharWidth; 
            cyChar = tm.tmHeight; 
            cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2; 
            ReleaseDC(hWnd, hdc); 
            hdc = NULL; 
            break; 
        }

        case WM_PAINT: 
        {
            hdc = BeginPaint(hWnd, &ps);
            if(check_center == 1)
            {
                TextOut(hdc, cxSize/2, cySize/2, "My Text Editor ", lstrlen("My Text Editor "));
                SetTextAlign(hdc, TA_CENTER);
                TextOut(hdc, cxSize/2, cySize/2 + cyChar*2, "<Press ESC To return HOME>", lstrlen("<Press ESC To return HOME>"));
                SetTextAlign(hdc, TA_CENTER);
                if(msg_to_print_flag == 0)
                {
                    TextOut(hdc, cxSize/2, cySize/2 + 3*cyChar, "<Press SHIFT to Save file>", lstrlen("<Press SHIFT To Save FILE>"));
                    SetTextAlign(hdc, TA_CENTER);
                }
                else if(msg_to_print_flag == 1)
                {
                    TextOut(hdc, cxSize/2, cySize/2 + 3*cyChar, "<File saved successfully>", lstrlen("<File save successfully to >"));
                    SetTextAlign(hdc, TA_CENTER);
                }
                else if(msg_to_print_flag == 2)
                {
                    TextOut(hdc, cxSize/2, cySize/2 + 3*cyChar, "No such file to Save!!", lstrlen("No such file to save!!"));
                    SetTextAlign(hdc, TA_CENTER);
                }
            }
            else
            {
                for(i = 0; i < i_line; ++i)
                {
                    TextOut(hdc, 0, i * cyChar, &szTEMPTEXTTOPRINT[i][0], lstrlen(&szTEMPTEXTTOPRINT[i][0])); 
                    SetTextAlign(hdc, TA_TOP | TA_LEFT);   
                }
            }
            EndPaint(hWnd, &ps); 
            break; 
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_SYSKEYDOWN:
        case WM_KEYDOWN:
        {
            if(wParam == VK_SHIFT)
            {
                file_to_write = fopen(glb_lpCmd_Line, "w");
                if(file_to_write == NULL)
                {
                    msg_to_print_flag = 2;
                }
                else
                {
                    for(i = 0; i < i_line; i++)
                    {
                        for(j = 0; j < 100; j++)
                            {
                                if(szTEMPTEXTTOPRINT[i][j] == '\0')
                                    break;
                                fputc(szTEMPTEXTTOPRINT[i][j], file_to_write);
                                msg_to_print_flag = 1;
                            }
                            msg_to_print_flag = 1;

                    }
                    fclose(file_to_write);
                }
                check_center = 1;
                
            }

            else if(wParam == VK_ESCAPE)
            {
                check_center = 1;
                count = 0;
                i_line = 1;
            }

            else if(wParam == VK_BACK)
            {
                if(count < 0)
                {
                    i_line--;
                    if(i_line <= 0)
                    {
                        i_line = 1;
                    }
                    count = lstrlen(&szTEMPTEXTTOPRINT[i_line - 1][0]);
                    szTEMPTEXTTOPRINT[i_line - 1][count] = '\0';
                }
                
                    
                else
                {
                    szTEMPTEXTTOPRINT[i_line - 1][count - 1] = '\0';
                    count--;
                }
                
            }

            else if(wParam == VK_RETURN)
            {
                szTEMPTEXTTOPRINT[i_line - 1][count] = '\n';
                szTEMPTEXTTOPRINT[i_line - 1][count + 1] = '\0';
                i_line++;
                count = 0;
            }

            else
            {   
                check_center = 0;
                if(count == 0)
                {
                    for(i = 0; i < 100; i++)
                    {
                        szTEMPTEXTTOPRINT[i_line - 1][i] = '\0';
                    }
                }
                szTEMPTEXTTOPRINT[i_line - 1][count] = (char)wParam;
                count++;
            }

            InvalidateRect(hWnd, NULL, TRUE);
        }

    }

    return(DefWindowProc(hWnd, uMsg, wParam, lParam));
}