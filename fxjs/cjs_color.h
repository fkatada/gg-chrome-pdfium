// Copyright 2014 The PDFium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_CJS_COLOR_H_
#define FXJS_CJS_COLOR_H_

#include "core/fxcrt/span.h"
#include "core/fxge/cfx_color.h"
#include "fxjs/cjs_object.h"
#include "fxjs/js_define.h"

class CJS_Color final : public CJS_Object {
 public:
  static uint32_t GetObjDefnID();
  static void DefineJSObjects(CFXJS_Engine* pEngine);
  static v8::Local<v8::Array> ConvertPWLColorToArray(CJS_Runtime* pRuntime,
                                                     const CFX_Color& color);
  static CFX_Color ConvertArrayToPWLColor(CJS_Runtime* pRuntime,
                                          v8::Local<v8::Array> array);

  CJS_Color(v8::Local<v8::Object> pObject, CJS_Runtime* pRuntime);
  ~CJS_Color() override;

  JS_STATIC_PROP(black, black, CJS_Color)
  JS_STATIC_PROP(blue, blue, CJS_Color)
  JS_STATIC_PROP(cyan, cyan, CJS_Color)
  JS_STATIC_PROP(dkGray, dark_gray, CJS_Color)
  JS_STATIC_PROP(gray, gray, CJS_Color)
  JS_STATIC_PROP(green, green, CJS_Color)
  JS_STATIC_PROP(ltGray, light_gray, CJS_Color)
  JS_STATIC_PROP(magenta, magenta, CJS_Color)
  JS_STATIC_PROP(red, red, CJS_Color)
  JS_STATIC_PROP(transparent, transparent, CJS_Color)
  JS_STATIC_PROP(white, white, CJS_Color)
  JS_STATIC_PROP(yellow, yellow, CJS_Color)

  JS_STATIC_METHOD(convert, CJS_Color)
  JS_STATIC_METHOD(equal, CJS_Color)

 private:
  static uint32_t ObjDefnID;
  static const char kName[];
  static const JSPropertySpec PropertySpecs[];
  static const JSMethodSpec MethodSpecs[];

  CJS_Result get_black(CJS_Runtime* pRuntime);
  CJS_Result set_black(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_blue(CJS_Runtime* pRuntime);
  CJS_Result set_blue(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_cyan(CJS_Runtime* pRuntime);
  CJS_Result set_cyan(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_dark_gray(CJS_Runtime* pRuntime);
  CJS_Result set_dark_gray(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_gray(CJS_Runtime* pRuntime);
  CJS_Result set_gray(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_green(CJS_Runtime* pRuntime);
  CJS_Result set_green(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_light_gray(CJS_Runtime* pRuntime);
  CJS_Result set_light_gray(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_magenta(CJS_Runtime* pRuntime);
  CJS_Result set_magenta(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_red(CJS_Runtime* pRuntime);
  CJS_Result set_red(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_transparent(CJS_Runtime* pRuntime);
  CJS_Result set_transparent(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_white(CJS_Runtime* pRuntime);
  CJS_Result set_white(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_yellow(CJS_Runtime* pRuntime);
  CJS_Result set_yellow(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result convert(CJS_Runtime* pRuntime,
                     pdfium::span<v8::Local<v8::Value>> params);
  CJS_Result equal(CJS_Runtime* pRuntime,
                   pdfium::span<v8::Local<v8::Value>> params);

  CJS_Result GetPropertyHelper(CJS_Runtime* pRuntime, CFX_Color* val);
  CJS_Result SetPropertyHelper(CJS_Runtime* pRuntime,
                               v8::Local<v8::Value> vp,
                               CFX_Color* val);

  CFX_Color transparent_;
  CFX_Color black_;
  CFX_Color white_;
  CFX_Color red_;
  CFX_Color green_;
  CFX_Color blue_;
  CFX_Color cyan_;
  CFX_Color magenta_;
  CFX_Color yellow_;
  CFX_Color dkgray_;
  CFX_Color gray_;
  CFX_Color ltgray_;
};

#endif  // FXJS_CJS_COLOR_H_
