#include <nan.h>
#include <cmath>
#include <windows.h>
#include <iostream>
#include <ole2.h>
#include <olectl.h>
#include <gdiplus.h>
#pragma comment (lib,"Gdiplus.lib")


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
  switch(msg){

    case WM_CLOSE:
      DestroyWindow(hwnd);
    break;

    case WM_DESTROY:
      PostQuitMessage(0);
    break;

    case WM_SETCURSOR:
      if(LOWORD(lParam) == HTCLIENT){
        HINSTANCE instance;
        LPCTSTR   cursor;

        instance = NULL;
        cursor   = IDC_CROSS;

        SetCursor(LoadCursor(instance, cursor));

        return true;
      }
    break; // default non-client cursor processing

    default:
      return DefWindowProc(hwnd, msg, wParam, lParam);
  }
  return 0;
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

  HWND hwnd = CreateWindowEx(
    WS_EX_LAYERED,
    "Win Class Name",
    "Hello World",
    WS_POPUP | WS_VISIBLE,
    // WS_POPUP | WS_VISIBLE,
    // WS_OVERLAPPEDWINDOW | WS_VISIBLE,
    0, 0,
    320, 200,
    NULL,
    NULL,
    hInstance,
    NULL);

  if(hwnd == NULL){
    std::cout << "HWND was not created";
    return;
  }

  // Make the window transparent
  SetLayeredWindowAttributes(hwnd, NULL, 128, LWA_ALPHA);

  // Change the background color of the window
  HBRUSH brush = CreateSolidBrush(RGB(255, 255, 255));
  SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG)brush);

  ShowWindow(hwnd, SW_SHOW);
  UpdateWindow(hwnd);

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
