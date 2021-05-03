#include <windows.h>
#include <strsafe.h>

#include <stdio.h>

LRESULT CALLBACK Windows_EventCallback(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param);

int g_client_w = 600;
int g_client_h = 400;
HINSTANCE g_instance_handle;

int WINAPI
WinMain(HINSTANCE instance_handle,
        HINSTANCE prev_instance_handle,
        LPSTR command_line,
        int show_mode)
{
 g_instance_handle = instance_handle;
 
 srand(time(0));
 
 wchar_t *window_class_name = L"MandarinFlashcards";
 
 WNDCLASSEX window_class = {0};
 window_class.cbSize = sizeof(WNDCLASSEX);
 window_class.style = CS_HREDRAW|CS_VREDRAW;
 window_class.lpfnWndProc = Windows_EventCallback;
 window_class.hInstance = instance_handle;
 window_class.hCursor = LoadCursorW(NULL, IDC_ARROW);
 window_class.hIcon = LoadIconW(instance_handle, L"mainIcon");
 window_class.hbrBackground = (HBRUSH)COLOR_WINDOW;
 window_class.lpszClassName = window_class_name;
 RegisterClassEx(&window_class);
 
 DWORD window_styles = WS_OVERLAPPEDWINDOW;
 int window_x = CW_USEDEFAULT;
 int window_y = CW_USEDEFAULT;
 int window_w = g_client_w;
 int window_h = g_client_h;
 
 RECT window_rect = {0, 0, window_w, window_h};
 AdjustWindowRect(&window_rect, window_styles, FALSE);
 
 HWND window_handle = CreateWindow(window_class_name,
                                   L"Mandarin Flashcards",
                                   window_styles,
                                   window_x, window_y,
                                   window_rect.right-window_rect.left,
                                   window_rect.bottom-window_rect.top,
                                   NULL,
                                   NULL,
                                   instance_handle,
                                   NULL);
 ShowWindow(window_handle, SW_SHOW);
 
 
 int exit_code;
 
 BOOL running = 1;
 while (running)
 {
  MSG msg = {0};
  if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
  {
   if (msg.message  == WM_QUIT)
   {
    running = 0;
    exit_code = msg.wParam;
   }
   TranslateMessage(&msg);
   DispatchMessage(&msg);
  }
 }
 return exit_code;
}

#include "sounds_table.c"

enum { FC_flashcardCount = sizeof(FC_flashcardTable) / sizeof(FC_flashcardTable[0]) };

int FC_currentCard = 0;
int FC_isShowingAnswer = 0;


static void
FC_DrawCard(void)
{
 static int has_card_already_been_picked[FC_flashcardCount] = {0};
 static int already_picked_count = 0;
 
 if (already_picked_count < FC_flashcardCount)
 {
  int i, already_picked = 1;
  do
  {
   i = rand() % FC_flashcardCount;
   already_picked = has_card_already_been_picked[i];
  } while (already_picked);
  
  has_card_already_been_picked[i] = 1;
  already_picked_count += 1;
  
  FC_currentCard = i;
 }
 else
 {
  already_picked_count = 0;
  memset(has_card_already_been_picked, 0, sizeof(has_card_already_been_picked));
 }
}

LRESULT CALLBACK
Windows_EventCallback(HWND window_handle,
                      UINT message,
                      WPARAM w_param,
                      LPARAM l_param)
{
 if (WM_DESTROY == message)
 {
  PostQuitMessage(0);
  return 0;
 }
 else if (WM_PAINT == message)
 {
  PAINTSTRUCT ps;
  HDC device_context_handle = BeginPaint(window_handle, &ps);
  
  FillRect(device_context_handle, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW + 3));
  
  HFONT font_handle = GetStockObject(SYSTEM_FIXED_FONT);
  HFONT old_font_handle;
  if ((old_font_handle = SelectObject(device_context_handle, font_handle))) 
  {
   SetBkColor(device_context_handle, 0x00000000);
   
   {
    wchar_t string[64];
    StringCbPrintfW(string, sizeof(string)/sizeof(string[0]), L"flashcard %d", FC_currentCard + 1);
    SetTextAlign(device_context_handle, TA_CENTER | TA_BASELINE);
    SetTextColor(device_context_handle, 0x004444ee);
    TextOutW(device_context_handle, g_client_w / 2, g_client_h / 2 - 64, string, wcslen(string)); 
   }
   
   {
    wchar_t *string = L"Press <enter> for a random card, <left> and <right> for next or previous";
    SetTextAlign(device_context_handle, TA_CENTER | TA_BASELINE);
    SetTextColor(device_context_handle, 0x00aaaaaa);
    TextOutW(device_context_handle, g_client_w / 2, g_client_h / 2 - 32, string, wcslen(string)); 
   }
   
   {
    wchar_t *string = L"Press <space> to hear the audio and <a> to see the answer";
    SetTextAlign(device_context_handle, TA_CENTER | TA_BASELINE);
    SetTextColor(device_context_handle, 0x00aaaaaa);
    TextOutW(device_context_handle, g_client_w / 2, g_client_h / 2, string, wcslen(string)); 
   }
   
   if (FC_isShowingAnswer)
   {
    {
     wchar_t *string = FC_flashcardTable[FC_currentCard].english;
     SetTextAlign(device_context_handle, TA_CENTER | TA_BASELINE);
     SetTextColor(device_context_handle, 0x0000FF00);
     TextOutW(device_context_handle, g_client_w / 2, g_client_h / 2 + 32, string, wcslen(string)); 
    }
    
    {
     wchar_t *string = FC_flashcardTable[FC_currentCard].mandarin;
     SetTextAlign(device_context_handle, TA_CENTER | TA_BASELINE);
     SetTextColor(device_context_handle, 0x0000FF00);
     TextOutW(device_context_handle, g_client_w / 2, g_client_h / 2 + 64, string, wcslen(string)); 
    }
   }
   
   SelectObject(device_context_handle, old_font_handle); 
  }
  
  return EndPaint(window_handle, &ps);
 }
 else if (WM_SIZE == message)
 {
  RECT rc;
  GetClientRect(window_handle, &rc);
  g_client_w = rc.right - rc.left;
  g_client_h = rc.bottom - rc.top;
  return 0;
 }
 else if (WM_KEYUP == message)
 {
  if (VK_SPACE == w_param)
  {
   PlaySoundW(FC_flashcardTable[FC_currentCard].sound_resource_name,
              g_instance_handle,
              SND_RESOURCE | SND_ASYNC);
  }
  else
  {
   if ('A' == w_param)
   {
    FC_isShowingAnswer= !FC_isShowingAnswer;
   }
   else
   {
    if (VK_LEFT == w_param)
    {
     FC_currentCard -= 1;
     if (FC_currentCard < 0) { FC_currentCard = FC_flashcardCount + FC_currentCard; }
    }
    else if (VK_RIGHT == w_param)
    {
     FC_currentCard = (FC_currentCard + 1) % FC_flashcardCount;
    }
    else if (VK_RETURN == w_param)
    {
     FC_DrawCard();
    }
    FC_isShowingAnswer = 0;
   }
   RedrawWindow(window_handle, NULL, NULL, RDW_INVALIDATE);
  }
  
  return 0;
 }
 else if (WM_LBUTTONDOWN == message)
 {
  FC_DrawCard();
  FC_isShowingAnswer = 0;
  RedrawWindow(window_handle, NULL, NULL, RDW_INVALIDATE);
  return 0;
 }
 else
 {
  return DefWindowProc(window_handle, message, w_param, l_param);
 }
}

