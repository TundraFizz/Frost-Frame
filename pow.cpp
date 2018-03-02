#include <nan.h>
#include <cmath>
#include <windows.h>
#include <windowsx.h>
#include <iostream>
#include <ole2.h>
#include <olectl.h>
#include <gdiplus.h>
#pragma comment (lib,"Gdiplus.lib")

int selectX1   = 0;
int selectY1   = 0;
int selectX2   = 0;
int selectY2   = 0;
int mouseStep  = 0;
bool mouseDown = false;

void DrawRectangle(HWND hwnd, int x1, int y1, int x2, int y2){
  HDC hdc = GetDC(hwnd);

  int upperLeftX  = 0;
  int upperLeftY  = 0;
  int lowerRightX = 0;
  int lowerRightY = 0;

  if(x1 < x2) upperLeftX = x1;
  else        upperLeftX = x2;

  if(y1 < y2) upperLeftY = y1;
  else        upperLeftY = y2;

  if(x1 > x2) lowerRightX = x1;
  else        lowerRightX = x2;

  if(y1 > y2) lowerRightY = y1;
  else        lowerRightY = y2;

  Rectangle(hdc, upperLeftX, upperLeftY, lowerRightX, lowerRightY);
  ReleaseDC(hwnd, hdc);
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid){
  UINT num  = 0; // number of image encoders
  UINT size = 0; // size of the image encoder array in bytes

  Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

  Gdiplus::GetImageEncodersSize(&num, &size);

  if(size == 0)
    return -1; // Failure

  pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));

  if(pImageCodecInfo == NULL)
    return -1; // Failure

  GetImageEncoders(num, size, pImageCodecInfo);

  for(UINT j = 0; j < num; ++j){
    if(wcscmp(pImageCodecInfo[j].MimeType, format) == 0){
      *pClsid = pImageCodecInfo[j].Clsid;
      free(pImageCodecInfo);
      return j; // Success
    }
  }

  free(pImageCodecInfo);
  return -1; // Failure
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){

  // HBRUSH hBrush = CreateSolidBrush(RGB(200,200,200));
  PAINTSTRUCT ps;
  HDC hdc;

  RECT qwe;
  qwe.left   = 100;
  qwe.top    = 100;
  qwe.right  = 400;
  qwe.bottom = 250;

  // ps.hdc     = GetDC(hwnd);
  // ps.rcPaint = qwe;
  // ps.fErase  = true;

  switch(msg){

    case WM_CLOSE:{
      DestroyWindow(hwnd);
      break;
    }

    case WM_DESTROY:{
      PostQuitMessage(0);
      break;
    }

    case WM_SETCURSOR:{
      if(LOWORD(lParam) == HTCLIENT){
        HINSTANCE instance;
        LPCTSTR   cursor;

        instance = NULL;
        cursor   = IDC_CROSS;

        SetCursor(LoadCursor(instance, cursor));

        return true;
      }
      break;
    }

    case WM_LBUTTONDOWN:{
      selectX1 = GET_X_LPARAM(lParam);
      selectY1 = GET_Y_LPARAM(lParam);
      mouseStep = 1;
      break;
    }

    case WM_LBUTTONUP:{
      // int xPos = GET_X_LPARAM(lParam);
      // int yPos = GET_Y_LPARAM(lParam);
      mouseStep = 0;
      // CloseWindow(hwnd);
      break;
    }

    case WM_MOUSEMOVE:{
      if(mouseStep == 1 || mouseStep == 2){
        selectX2 = GET_X_LPARAM(lParam);
        selectY2 = GET_Y_LPARAM(lParam);
        mouseStep = 2;
        InvalidateRect(hwnd, NULL, false);
        // DrawRectangle(hwnd, selectX1, selectY1, selectX2, selectY2);
        // UpdateWindow(hwnd);
      }
      break;
    }

   case WM_PAINT:{
      if(mouseStep == 2){
        // mDC = BeginPaint(hwnd, &ps);
        std::cout << "Painting\n";
        hdc = BeginPaint(hwnd, &ps);

        DrawRectangle(hwnd, selectX1, selectY1, selectX2, selectY2);
        // FillRect(hdc, &qwe, hBrush);

        // TextOut(hdc, 0, 0, "Hello, Windows!", 15);

        // DrawRectangle(hwnd, selectX1, selectY1, selectX2, selectY2);
        // InvalidateRect(hwnd, NULL, true);
        // UpdateWindow(hwnd);

        EndPaint(hwnd, &ps);
        // return 0;
      }

      break;
    }

    default:
      return DefWindowProc(hwnd, msg, wParam, lParam);
  }
  return 0;
}

int smallestLeft  = 0;
int smallestTop   = 0;
int largestRight  = 0;
int largestBottom = 0;
bool firstRun     = true;

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData){
  MONITORINFO mi;
  mi.cbSize = sizeof(mi);
  GetMonitorInfo(hMonitor, &mi);
  RECT r = mi.rcMonitor;

  if(firstRun){
    firstRun      = false;
    smallestLeft  = r.left;
    smallestTop   = r.top;
    largestRight  = r.right;
    largestBottom = r.bottom;
  }else{
    if(r.left   < smallestLeft)  smallestLeft  = r.left;
    if(r.top    < smallestTop)   smallestTop   = r.top;
    if(r.right  > largestRight)  largestRight  = r.right;
    if(r.bottom > largestBottom) largestBottom = r.bottom;
  }

  return true;
}

void Reeeeeee(const Nan::FunctionCallbackInfo<v8::Value>& info){
  std::cout << "Enter: Reeeeeee\n";
  HINSTANCE hInstance = GetModuleHandle(NULL);
  MSG Msg;

  WNDCLASS winclass;
  winclass.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
  winclass.lpfnWndProc = WindowProc;
  winclass.cbClsExtra = 0;
  winclass.cbWndExtra = 0;
  winclass.hInstance = hInstance;
  winclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  winclass.hCursor = LoadCursor(NULL, IDC_ARROW);
  winclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
  winclass.lpszMenuName = NULL;
  winclass.lpszClassName = "Win Class Name";

  if(!RegisterClass(&winclass)){
    std::cout << "Class not registered\n";
    return;
  }

  EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, 0);
  int maskWidth  = largestRight  - smallestLeft;
  int maskHeight = largestBottom - smallestTop;
  std::cout << "========== Monitor Information ==========\n";
  std::cout << "smallestLeft : " << smallestLeft  << "\n";
  std::cout << "smallestTop  : " << smallestTop   << "\n";
  std::cout << "largestRight : " << largestRight  << "\n";
  std::cout << "largestBottom: " << largestBottom << "\n";
  std::cout << "maskWidth    : " << maskWidth     << "\n";
  std::cout << "maskHeight   : " << maskHeight    << "\n";

  smallestLeft = 100;
  smallestTop  = 100;
  maskWidth    = 300;
  maskHeight   = 300;

  HWND hwnd = CreateWindowEx(
    // 1. Allows better window functionality
    // 2. Makes sure the that window is always on top
    // 3. Hides the program when the user alt-tabs
    WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
    "Win Class Name",
    "Hello World",
    WS_POPUP | WS_VISIBLE,
    smallestLeft,
    smallestTop,
    maskWidth,
    maskHeight,
    NULL,
    NULL,
    hInstance,
    NULL);

  if(hwnd == NULL){
    std::cout << "HWND was not created";
    return;
  }

  // Make the window transparent
  int barelyVisible = 255; // Change this to 1 later
  SetLayeredWindowAttributes(hwnd, NULL, barelyVisible, LWA_ALPHA);

  // Change the background color of the window
  HBRUSH brush = CreateSolidBrush(RGB(255, 255, 255));
  SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG)brush);

  ShowWindow(hwnd, SW_SHOW);
  // UpdateWindow(hwnd);
  // DrawRectangle(hwnd, 0, 0);

  return;

  std::cout << "1\n";
  // The Message Loop
  while(GetMessage(&Msg, NULL, 0, 0) > 0){
    std::cout << "2\n";
    TranslateMessage(&Msg);
    std::cout << "3\n";
    DispatchMessage(&Msg);
    std::cout << "4\n";
  }

  std::cout << "5\n";

  // return Msg.wParam
  return;
}

void Pow(const Nan::FunctionCallbackInfo<v8::Value>& info) {

    if (info.Length() < 2) {
        Nan::ThrowTypeError("Wrong number of arguments");
        return;
    }

    if (!info[0]->IsNumber() || !info[1]->IsNumber()) {
        Nan::ThrowTypeError("Both arguments should be numbers");
        return;
    }

    double arg0 = info[0]->NumberValue();
    double arg1 = info[1]->NumberValue();
    v8::Local<v8::Number> num = Nan::New(pow(arg0, arg1)+5);

    info.GetReturnValue().Set(num);
}

void Init(v8::Local<v8::Object> exports) {
    exports->Set(Nan::New("pow").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(Pow)->GetFunction());
    exports->Set(Nan::New("Reeeeeee").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(Reeeeeee)->GetFunction());
}

NODE_MODULE(pow, Init)
