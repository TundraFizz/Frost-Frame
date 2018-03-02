#include <nan.h>
#include <cmath>
#include <windows.h>
#include <windowsx.h>
#include <iostream>
#include <ole2.h>
#include <olectl.h>
#include <gdiplus.h>
#pragma comment (lib,"Gdiplus.lib")

HWND hwndTop;
HWND hwndBot;

int selectX1   = 0;
int selectY1   = 0;
int selectX2   = 0;
int selectY2   = 0;
int mouseStep  = 0;
bool mouseDown = false;

int smallestLeft  = 0;
int smallestTop   = 0;
int largestRight  = 0;
int largestBottom = 0;
bool firstRun     = true;

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

bool saveBitmap(LPCSTR filename, HBITMAP bmp, HPALETTE pal){
    bool result = false;
    PICTDESC pd;

    pd.cbSizeofstruct   = sizeof(PICTDESC);
    pd.picType      = PICTYPE_BITMAP;
    pd.bmp.hbitmap  = bmp;
    pd.bmp.hpal     = pal;

    LPPICTURE picture;
    HRESULT res = OleCreatePictureIndirect(&pd, IID_IPicture, false,
                       reinterpret_cast<void**>(&picture));

    if (!SUCCEEDED(res))
    return false;

    LPSTREAM stream;
    res = CreateStreamOnHGlobal(0, true, &stream);

    if (!SUCCEEDED(res))
    {
    picture->Release();
    return false;
    }

    LONG bytes_streamed;
    res = picture->SaveAsFile(stream, true, &bytes_streamed);

    HANDLE file = CreateFile(filename, GENERIC_WRITE, FILE_SHARE_READ, 0,
                 CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

    if (!SUCCEEDED(res) || !file)
    {
    stream->Release();
    picture->Release();
    return false;
    }

    HGLOBAL mem = 0;
    GetHGlobalFromStream(stream, &mem);
    LPVOID data = GlobalLock(mem);

    DWORD bytes_written;

    result   = !!WriteFile(file, data, bytes_streamed, &bytes_written, 0);
    result  &= (bytes_written == static_cast<DWORD>(bytes_streamed));

    GlobalUnlock(mem);
    CloseHandle(file);

    stream->Release();
    picture->Release();

    return result;
}

bool screenCapturePart(int x, int y, int w, int h, LPCSTR fname){
    HDC hdcSource = GetDC(NULL);
    HDC hdcMemory = CreateCompatibleDC(hdcSource);

    int capX = GetDeviceCaps(hdcSource, HORZRES);
    int capY = GetDeviceCaps(hdcSource, VERTRES);

    HBITMAP hBitmap = CreateCompatibleBitmap(hdcSource, w, h);
    HBITMAP hBitmapOld = (HBITMAP)SelectObject(hdcMemory, hBitmap);

    BitBlt(hdcMemory, 0, 0, w, h, hdcSource, x, y, SRCCOPY);
    hBitmap = (HBITMAP)SelectObject(hdcMemory, hBitmapOld);

    DeleteDC(hdcSource);
    DeleteDC(hdcMemory);

    HPALETTE hpal = NULL;
    if(saveBitmap(fname, hBitmap, hpal)) return true;
    return false;
}

int ConvertBmpToPng(){
  Gdiplus::GdiplusStartupInput gdiplusStartupInput;
  ULONG_PTR gdiplusToken;
  GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

  CLSID  encoderClsid;
  Gdiplus::Status stat;
  Gdiplus::Image* image = new Gdiplus::Image(L"testing.bmp");

  // Get the CLSID of the PNG encoder.
  GetEncoderClsid(L"image/png", &encoderClsid);

  stat = image->Save(L"testing.png", &encoderClsid, NULL);

  if(stat == Gdiplus::Ok)
    printf("testing.png was saved successfully\n");
  else
    printf("Failure: stat = %d\n", stat);

  // delete image;
  Gdiplus::GdiplusShutdown(gdiplusToken);
  return 0;
}

LRESULT CALLBACK WindowProcTop(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
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
      selectX2 = GET_X_LPARAM(lParam);
      selectY2 = GET_Y_LPARAM(lParam);
      mouseStep = 0;
      CloseWindow(hwndTop);
      CloseWindow(hwndBot);

      int width  = selectX2 - selectX1;
      int height = selectY2 - selectY1;

      int ssX = selectX1 + smallestLeft; // Subtract from the smallest top-left corner
      int ssY = selectY1 + smallestTop;  // Subtract from the smallest top-left corner

      std::cout << selectX1 << ", " << selectY1 << ", " << width << ", " << height << "\n";
      screenCapturePart(ssX, ssY, width, height, "testing.bmp");
      ConvertBmpToPng();

      break;
    }

    case WM_MOUSEMOVE:{
      if(mouseStep == 1 || mouseStep == 2){
        selectX2 = GET_X_LPARAM(lParam);
        selectY2 = GET_Y_LPARAM(lParam);
        mouseStep = 2;

        int upperLeftX  = 0;
        int upperLeftY  = 0;
        int lowerRightX = 0;
        int lowerRightY = 0;

        if(selectX1 < selectX2) upperLeftX = selectX1;
        else                    upperLeftX = selectX2;

        if(selectY1 < selectY2) upperLeftY = selectY1;
        else        upperLeftY = selectY2;

        if(selectX1 > selectX2) lowerRightX = selectX1;
        else        lowerRightX = selectX2;

        if(selectY1 > selectY2) lowerRightY = selectY1;
        else        lowerRightY = selectY2;

        HRGN WinRgn1;
        HRGN WinRgn2;
        WinRgn1 = CreateRectRgn(upperLeftX, upperLeftY, lowerRightX, lowerRightY);
        WinRgn2 = CreateRectRgn(-2560, 0, 2560, 1440);
        CombineRgn(WinRgn1, WinRgn1, WinRgn2, RGN_XOR);
        SetWindowRgn(hwndBot, WinRgn1, true);
        UpdateWindow(hwndBot);
      }
      break;
    }

    default:
      return DefWindowProc(hwnd, msg, wParam, lParam);
  }
  return 0;
}

LRESULT CALLBACK WindowProcBot(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
  switch(msg){
    case WM_CLOSE:{
      DestroyWindow(hwnd);
      break;
    }

    case WM_DESTROY:{
      PostQuitMessage(0);
      break;
    }

    default:{
      return DefWindowProc(hwnd, msg, wParam, lParam);
    }
  }
  return 0;
}

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
  HINSTANCE hInstance = GetModuleHandle(NULL);
  MSG Msg;

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

  WNDCLASS winClassTop;
  winClassTop.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
  winClassTop.lpfnWndProc = WindowProcTop;
  winClassTop.cbClsExtra = 0;
  winClassTop.cbWndExtra = 0;
  winClassTop.hInstance = hInstance;
  winClassTop.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  winClassTop.hCursor = LoadCursor(NULL, IDC_ARROW);
  winClassTop.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
  winClassTop.lpszMenuName = NULL;
  winClassTop.lpszClassName = "winClassTop";

  WNDCLASS winClassBot;
  winClassBot.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
  winClassBot.lpfnWndProc = WindowProcBot;
  winClassBot.cbClsExtra = 0;
  winClassBot.cbWndExtra = 0;
  winClassBot.hInstance = hInstance;
  winClassBot.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  winClassBot.hCursor = LoadCursor(NULL, IDC_ARROW);
  winClassBot.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
  winClassBot.lpszMenuName = NULL;
  winClassBot.lpszClassName = "winClassBot";

  RegisterClass(&winClassTop);
  RegisterClass(&winClassBot);

  // IMPORTANT! The order that these HWND variables are defined determines what order
  // the windows appear. The first HWND variable will appear beneath the other HWNDs
  // and the last HWND variable will appear above all other HWMDs.

  hwndBot = CreateWindowEx(
    // 1. Allows better window functionality
    // 2. Makes sure the that window is always on top
    // 3. Hides the program when the user alt-tabs
    WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
    "winClassBot",
    "Bot",
    // 1. Make it a popup window which removes all borders/menu items from it
    // 2. Set the window to initially be visible
    WS_POPUP | WS_VISIBLE,
    smallestLeft,
    smallestTop,
    maskWidth,
    maskHeight,
    NULL,
    NULL,
    hInstance,
    NULL);

  hwndTop = CreateWindowEx(
    // 1. Allows better window functionality
    // 2. Makes sure the that window is always on top
    // 3. Hides the program when the user alt-tabs
    WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
    "winClassTop",
    "Top",
    // 1. Make it a popup window which removes all borders/menu items from it
    // 2. Set the window to initially be visible
    WS_POPUP | WS_VISIBLE,
    smallestLeft,
    smallestTop,
    maskWidth,
    maskHeight,
    NULL,
    NULL,
    hInstance,
    NULL);

  SetLayeredWindowAttributes(hwndTop, NULL, 1,   LWA_ALPHA);
  SetLayeredWindowAttributes(hwndBot, NULL, 120, LWA_ALPHA);

  // Change background colors of the windows
  HBRUSH brushTop = CreateSolidBrush(RGB(255, 255, 255));
  HBRUSH brushBot = CreateSolidBrush(RGB(0, 0, 0));
  SetClassLongPtr(hwndTop, GCLP_HBRBACKGROUND, (LONG)brushTop);
  SetClassLongPtr(hwndBot, GCLP_HBRBACKGROUND, (LONG)brushBot);

  ShowWindow(hwndTop, SW_SHOW);
  ShowWindow(hwndBot, SW_SHOW);

  info.GetReturnValue().Set(123456);
}

void Pow(const Nan::FunctionCallbackInfo<v8::Value>& info){
  if(info.Length() < 2){
    Nan::ThrowTypeError("Wrong number of arguments");
    return;
  }

  if(!info[0]->IsNumber() || !info[1]->IsNumber()){
    Nan::ThrowTypeError("Both arguments should be numbers");
    return;
  }

  double arg0 = info[0]->NumberValue();
  double arg1 = info[1]->NumberValue();
  v8::Local<v8::Number> num = Nan::New(pow(arg0, arg1)+5);

  info.GetReturnValue().Set(num);
}

void Init(v8::Local<v8::Object> exports) {
    exports->Set(Nan::New("pow").ToLocalChecked(),      Nan::New<v8::FunctionTemplate>(Pow)->GetFunction());
    exports->Set(Nan::New("Reeeeeee").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(Reeeeeee)->GetFunction());
}

NODE_MODULE(pow, Init)
