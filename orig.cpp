#include "qwe/node_api.h"
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

napi_value MyFunction(napi_env env, napi_callback_info info){
  napi_status status;
  size_t argc = 1;
  int number = 0;
  napi_value argv[1];
  status = napi_get_cb_info(env, info, &argc, argv, NULL, NULL);

  if(status != napi_ok)
    napi_throw_error(env, NULL, "Failed to parse arguments");

  status = napi_get_value_int32(env, argv[0], &number);

  if(status != napi_ok)
    napi_throw_error(env, NULL, "Invalid number was passed as argument");

  std::cout << "IT WORKS!\n";

  napi_value myNumber;
  number = number * 2;
  status = napi_create_int32(env, number, &myNumber);

  if(status != napi_ok)
    napi_throw_error(env, NULL, "Unable to create return value");

  return myNumber;
}

napi_value HelloWorld(napi_env env, napi_callback_info info){
  std::cout << "Hello, world\n";
  return NULL;
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

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
  switch(msg){
    case WM_CLOSE:
      DestroyWindow(hwnd);
    break;
    case WM_DESTROY:
      PostQuitMessage(0);
    break;
    default:
      return DefWindowProc(hwnd, msg, wParam, lParam);
  }
  return 0;
}

napi_value Reeeeeee(napi_env env, napi_callback_info info){
  HINSTANCE hInstance = GetModuleHandle(NULL);
  WNDCLASS winclass;
  HWND hwnd;
  MSG Msg;

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
    std::cout << "Class not registered";
    return(0);
  }

  hwnd = CreateWindowA(
    "Win Class Name",
    "Hello Dave",
    WS_OVERLAPPEDWINDOW | WS_VISIBLE,
    0, 0,
    320, 200,
    NULL,
    NULL,
    hInstance,
    NULL);

  if(hwnd == NULL){
    std::cout << "HWND was not created";
    return(0);
  }

  // ShowWindow(hwnd, nCmdShow);
  ShowWindow(hwnd, NULL);
  UpdateWindow(hwnd);

  // The Message Loop
  while(GetMessage(&Msg, NULL, 0, 0) > 0){
    TranslateMessage(&Msg);
    DispatchMessage(&Msg);
  }

  // return Msg.wParam
  return 0;
}

napi_value Test1(napi_env env, napi_callback_info info){

  screenCapturePart(50, 50, 600, 400, "testing.bmp");
  ConvertBmpToPng();
  return NULL;
}

napi_value Init(napi_env env, napi_value exports){
  //////////////////////////////////////////////////////////////////////////////
  napi_status status;
  napi_value fn;

  status = napi_create_function(env, NULL, 0, MyFunction, NULL, &fn);
  if (status != napi_ok)
    napi_throw_error(env, NULL, "Unable to wrap native function");

  status = napi_set_named_property(env, exports, "swag", fn);
  if (status != napi_ok)
    napi_throw_error(env, NULL, "Unable to populate exports");
  //////////////////////////////////////////////////////////////////////////////

  napi_create_function(env, NULL, 0, HelloWorld, NULL, &fn);
  napi_set_named_property(env, exports, "HelloWorld", fn);

  napi_create_function(env, NULL, 0, Test1, NULL, &fn);
  napi_set_named_property(env, exports, "Test1", fn);

  napi_create_function(env, NULL, 0, Reeeeeee, NULL, &fn);
  napi_set_named_property(env, exports, "Reeeeeee", fn);

  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
