/*
  client.cc
*/

#include "client.h"
#include "v8variant.h"

using namespace v8;
using namespace ole32core;

namespace node_win32ole {

Persistent<FunctionTemplate> Client::clazz;

void Client::Init(Handle<Object> target)
{
  HandleScope scope;
  Local<FunctionTemplate> t = FunctionTemplate::New(New);
  clazz = Persistent<FunctionTemplate>::New(t);
  clazz->InstanceTemplate()->SetInternalFieldCount(1);
  clazz->SetClassName(String::NewSymbol("Client"));
//  NODE_SET_PROTOTYPE_METHOD(clazz, "New", New);
  NODE_SET_PROTOTYPE_METHOD(clazz, "Dispatch", Dispatch);
  NODE_SET_PROTOTYPE_METHOD(clazz, "Finalize", Finalize);
  target->Set(String::NewSymbol("Client"), clazz->GetFunction());
}

Handle<Value> Client::New(const Arguments& args)
{
  HandleScope scope;
  DISPFUNCIN();
  if(!args.IsConstructCall())
    return ThrowException(Exception::TypeError(
      String::New("Use the new operator to create new Client objects")));
  std::string cstr_locale(".ACP"); // default
  if(args.Length() >= 1){
    if(!args[0]->IsString())
      return ThrowException(Exception::TypeError(
        String::New("Argument 1 is not a String")));
    String::Utf8Value u8s_locale(args[0]);
    cstr_locale = std::string(*u8s_locale);
  }
  OLE32core *oc = new OLE32core();
  if(!oc)
    return ThrowException(Exception::TypeError(
      String::New("Can't create new Client object (null OLE32core)")));
  bool cnresult = false;
  try{
    cnresult = oc->connect(cstr_locale);
  }catch(OLE32coreException e){
    std::cerr << e.errorMessage((char *)cstr_locale.c_str());
  }catch(char *e){
    std::cerr << e << cstr_locale.c_str() << std::endl;
  }
  if(!cnresult)
    return ThrowException(Exception::TypeError(
      String::New("May be CoInitialize() is failed.")));
  Local<Object> thisObject = args.This();
  Client *cl = new Client();
  cl->Wrap(thisObject);
  thisObject->SetInternalField(0, External::New(oc));
  Persistent<Object> objectDisposer = Persistent<Object>::New(thisObject);
  objectDisposer.MakeWeak(oc, Dispose);
  DISPFUNCOUT();
  return args.This();
}

Handle<Value> Client::Dispatch(const Arguments& args)
{
  HandleScope scope;
  DISPFUNCIN();
  BEVERIFY(done, args.Length() >= 1);
  BEVERIFY(done, args[0]->IsString());
  wchar_t *wcs;
  {
    String::Utf8Value u8s(args[0]); // must create here
    wcs = u8s2wcs(*u8s);
  }
  BEVERIFY(done, wcs);
#ifdef DEBUG
  char *mbs = wcs2mbs(wcs);
  if(!mbs) free(wcs);
  BEVERIFY(done, mbs);
  fprintf(stderr, "ProgID: %s\n", mbs);
  free(mbs);
#endif
  CLSID clsid;
  HRESULT hr = CLSIDFromProgID(wcs, &clsid);
  free(wcs);
  BEVERIFY(done, !FAILED(hr));
#ifdef DEBUG
  fprintf(stderr, "clsid:"); // 00024500-0000-0000-c000-000000000046 (Excel) ok
  for(int i = 0; i < sizeof(CLSID); ++i)
    fprintf(stderr, " %02x", ((unsigned char *)&clsid)[i]);
  fprintf(stderr, "\n");
#endif
  Handle<Object> vApp = V8Variant::CreateUndefined();
  BEVERIFY(done, !vApp.IsEmpty());
  BEVERIFY(done, !vApp->IsUndefined());
  BEVERIFY(done, vApp->IsObject());
  OCVariant *app = castedInternalField<OCVariant>(vApp);
  if(!app)
    return ThrowException(Exception::TypeError(
      String::New("Can't access to V8Variant object (null OCVariant)")));
  app->v.vt = VT_DISPATCH;
  // When 'CoInitialize(NULL)' is not called first (and on the same instance),
  // next functions will return many errors.
  // (old style) GetActiveObject() returns 0x000036b7
  //   The requested lookup key was not found in any active activation context.
  // (OLE2) CoCreateInstance() returns 0x000003f0
  //   An attempt was made to reference a token that does not exist.
#ifdef DEBUG // obsolete (it needs that OLE target has been already executed)
  IUnknown *pUnk;
  hr = GetActiveObject(clsid, NULL, (IUnknown **)&pUnk);
  BEVERIFY(done, !FAILED(hr));
  hr = pUnk->QueryInterface(IID_IDispatch, (void **)&app->v.pdispVal);
  pUnk->Release();
#else
  // C -> C++ changes types (&clsid -> clsid, &IID_IDispatch -> IID_IDispatch)
  hr = CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER|CLSCTX_LOCAL_SERVER,
    IID_IDispatch, (void **)&app->v.pdispVal);
#endif
  BEVERIFY(done, !FAILED(hr));
  DISPFUNCOUT();
  return scope.Close(vApp);
done:
  DISPFUNCOUT();
  return ThrowException(Exception::TypeError(String::New("Dispatch failed")));
}

Handle<Value> Client::Finalize(const Arguments& args)
{
  HandleScope scope;
  DISPFUNCIN();
#if(0)
  std::cerr << __FUNCTION__ << " Finalizer is called\a" << std::endl;
#endif
  Local<Object> thisObject = args.This();
// Client *cl = ObjectWrap::Unwrap<Client>(thisObject);
#if(1) // now GC will call Disposer automatically
  OLE32core *oc = castedInternalField<OLE32core>(thisObject);
  if(oc){
    try{
      delete oc; // will call oc->disconnect();
    }catch(OLE32coreException e){ std::cerr << e.errorMessage(__FUNCTION__);
    }catch(char *e){ std::cerr << e << __FUNCTION__ << std::endl;
    }
  }
#endif
  thisObject->SetInternalField(0, External::New(NULL));
  DISPFUNCOUT();
  return args.This();
}

void Client::Dispose(Persistent<Value> handle, void *param)
{
  DISPFUNCIN();
#if(1)
  std::cerr << __FUNCTION__ << " Disposer is called\a" << std::endl;
#endif
  OLE32core *p = castedInternalField<OLE32core>(handle->ToObject());
  if(!p){
    std::cerr << __FUNCTION__;
    std::cerr << " InternalField has been already deleted" << std::endl;
  }
//  else{
    OLE32core *oc = static_cast<OLE32core *>(param);
    if(oc){
      try{
        delete oc; // will call oc->disconnect();
      }catch(OLE32coreException e){ std::cerr << e.errorMessage(__FUNCTION__);
      }catch(char *e){ std::cerr << e << __FUNCTION__ << std::endl;
      }
    }
//  }
  BEVERIFY(done, handle->ToObject()->InternalFieldCount() > 0);
  handle->ToObject()->SetInternalField(0, External::New(NULL));
done:
  handle.Dispose();
  DISPFUNCOUT();
}

void Client::Finalize()
{
  assert(!finalized);
  finalized = true;
}

} // namespace node_win32ole
