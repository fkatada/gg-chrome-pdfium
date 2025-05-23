// Copyright 2014 The PDFium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/cjs_runtime.h"

#include <math.h>

#include <algorithm>

#include "core/fxcrt/check_op.h"
#include "fpdfsdk/cpdfsdk_formfillenvironment.h"
#include "fxjs/cfx_globaldata.h"
#include "fxjs/cjs_annot.h"
#include "fxjs/cjs_app.h"
#include "fxjs/cjs_border.h"
#include "fxjs/cjs_color.h"
#include "fxjs/cjs_console.h"
#include "fxjs/cjs_display.h"
#include "fxjs/cjs_document.h"
#include "fxjs/cjs_event.h"
#include "fxjs/cjs_event_context.h"
#include "fxjs/cjs_field.h"
#include "fxjs/cjs_font.h"
#include "fxjs/cjs_global.h"
#include "fxjs/cjs_globalarrays.h"
#include "fxjs/cjs_globalconsts.h"
#include "fxjs/cjs_highlight.h"
#include "fxjs/cjs_icon.h"
#include "fxjs/cjs_object.h"
#include "fxjs/cjs_position.h"
#include "fxjs/cjs_publicmethods.h"
#include "fxjs/cjs_scalehow.h"
#include "fxjs/cjs_scalewhen.h"
#include "fxjs/cjs_style.h"
#include "fxjs/cjs_timerobj.h"
#include "fxjs/cjs_util.h"
#include "fxjs/cjs_zoomtype.h"
#include "fxjs/fxv8.h"
#include "fxjs/js_define.h"
#include "v8/include/v8-context.h"
#include "v8/include/v8-exception.h"
#include "v8/include/v8-isolate.h"

CJS_Runtime::CJS_Runtime(CPDFSDK_FormFillEnvironment* pFormFillEnv)
    : form_fill_env_(pFormFillEnv) {
  v8::Isolate* pIsolate = nullptr;
  IPDF_JSPLATFORM* pPlatform = form_fill_env_->GetFormFillInfo()->m_pJsPlatform;
  if (pPlatform->version <= 2) {
    // Backwards compatibility - JS now initialized earlier in more modern
    // JSPLATFORM versions.
    unsigned int embedderDataSlot = 0;
    v8::Isolate* pExternalIsolate = nullptr;
    if (pPlatform->version == 2) {
      pExternalIsolate = static_cast<v8::Isolate*>(pPlatform->m_isolate);
      embedderDataSlot = pPlatform->m_v8EmbedderSlot;
    }
    FXJS_Initialize(embedderDataSlot, pExternalIsolate);
  }
  isolate_managed_ = FXJS_GetIsolate(&pIsolate);
  SetIsolate(pIsolate);

  v8::Isolate::Scope isolate_scope(pIsolate);
  v8::HandleScope handle_scope(pIsolate);
  if (isolate_managed_ || FXJS_GlobalIsolateRefCount() == 0) {
    DefineJSObjects();
  }

  ScopedEventContext pContext(this);
  InitializeEngine();
  SetFormFillEnvToDocument();
}

CJS_Runtime::~CJS_Runtime() {
  NotifyObservers();
  ReleaseEngine();
  if (isolate_managed_) {
    DisposeIsolate();
  }
}

void CJS_Runtime::DefineJSObjects() {
  v8::Isolate::Scope isolate_scope(GetIsolate());
  v8::HandleScope handle_scope(GetIsolate());
  v8::Local<v8::Context> context = v8::Context::New(GetIsolate());
  v8::Context::Scope context_scope(context);

  // The call order determines the "ObjDefID" assigned to each class.
  // ObjDefIDs 0 - 2
  CJS_Border::DefineJSObjects(this);
  CJS_Display::DefineJSObjects(this);
  CJS_Font::DefineJSObjects(this);

  // ObjDefIDs 3 - 5
  CJS_Highlight::DefineJSObjects(this);
  CJS_Position::DefineJSObjects(this);
  CJS_ScaleHow::DefineJSObjects(this);

  // ObjDefIDs 6 - 8
  CJS_ScaleWhen::DefineJSObjects(this);
  CJS_Style::DefineJSObjects(this);
  CJS_Zoomtype::DefineJSObjects(this);

  // ObjDefIDs 9 - 11
  CJS_App::DefineJSObjects(this);
  CJS_Color::DefineJSObjects(this);
  CJS_Console::DefineJSObjects(this);

  // ObjDefIDs 12 - 14
  CJS_Document::DefineJSObjects(this);
  CJS_Event::DefineJSObjects(this);
  CJS_Field::DefineJSObjects(this);

  // ObjDefIDs 15 - 17
  CJS_Global::DefineJSObjects(this);
  CJS_Icon::DefineJSObjects(this);
  CJS_Util::DefineJSObjects(this);

  // ObjDefIDs 18 - 20 (these can't fail, return void).
  CJS_PublicMethods::DefineJSObjects(this);
  CJS_GlobalConsts::DefineJSObjects(this);
  CJS_GlobalArrays::DefineJSObjects(this);

  // ObjDefIDs 21 - 22.
  CJS_TimerObj::DefineJSObjects(this);
  CJS_Annot::DefineJSObjects(this);
}

IJS_EventContext* CJS_Runtime::NewEventContext() {
  event_context_array_.push_back(std::make_unique<CJS_EventContext>(this));
  return event_context_array_.back().get();
}

void CJS_Runtime::ReleaseEventContext(IJS_EventContext* pContext) {
  DCHECK_EQ(pContext, event_context_array_.back().get());
  event_context_array_.pop_back();
}

CJS_EventContext* CJS_Runtime::GetCurrentEventContext() const {
  return event_context_array_.empty() ? nullptr
                                      : event_context_array_.back().get();
}

CFX_Timer::HandlerIface* CJS_Runtime::GetTimerHandler() const {
  return form_fill_env_ ? form_fill_env_->GetTimerHandler() : nullptr;
}

void CJS_Runtime::SetFormFillEnvToDocument() {
  v8::Isolate::Scope isolate_scope(GetIsolate());
  v8::HandleScope handle_scope(GetIsolate());
  v8::Local<v8::Context> context = GetV8Context();
  v8::Context::Scope context_scope(context);

  v8::Local<v8::Object> pThis = GetThisObj();
  if (pThis.IsEmpty()) {
    return;
  }

  auto pJSDocument = JSGetObject<CJS_Document>(GetIsolate(), pThis);
  if (!pJSDocument) {
    return;
  }

  pJSDocument->SetFormFillEnv(form_fill_env_.Get());
}

CPDFSDK_FormFillEnvironment* CJS_Runtime::GetFormFillEnv() const {
  return form_fill_env_.Get();
}

std::optional<IJS_Runtime::JS_Error> CJS_Runtime::ExecuteScript(
    const WideString& script) {
  return Execute(script);
}

bool CJS_Runtime::AddEventToSet(const FieldEvent& event) {
  return field_event_set_.insert(event).second;
}

void CJS_Runtime::RemoveEventFromSet(const FieldEvent& event) {
  field_event_set_.erase(event);
}

CJS_Runtime* CJS_Runtime::AsCJSRuntime() {
  return this;
}

v8::Local<v8::Value> CJS_Runtime::GetValueByNameFromGlobalObject(
    ByteStringView utf8Name) {
  v8::Isolate::Scope isolate_scope(GetIsolate());
  v8::Local<v8::Context> context = GetV8Context();
  v8::Context::Scope context_scope(context);
  v8::Local<v8::String> str = fxv8::NewStringHelper(GetIsolate(), utf8Name);
  v8::MaybeLocal<v8::Value> maybe_value = context->Global()->Get(context, str);
  if (maybe_value.IsEmpty()) {
    return v8::Local<v8::Value>();
  }
  return maybe_value.ToLocalChecked();
}

bool CJS_Runtime::SetValueByNameInGlobalObject(ByteStringView utf8Name,
                                               v8::Local<v8::Value> pValue) {
  if (utf8Name.IsEmpty() || pValue.IsEmpty()) {
    return false;
  }

  v8::Isolate* pIsolate = GetIsolate();
  v8::Isolate::Scope isolate_scope(pIsolate);
  v8::Local<v8::Context> context = GetV8Context();
  v8::Context::Scope context_scope(context);
  v8::Local<v8::String> str = fxv8::NewStringHelper(pIsolate, utf8Name);
  v8::Maybe<bool> result = context->Global()->Set(context, str, pValue);
  return result.IsJust() && result.FromJust();
}

v8::Local<v8::Value> CJS_Runtime::MaybeCoerceToNumber(
    v8::Local<v8::Value> value) {
  bool bAllowNaN = false;
  if (value->IsString()) {
    ByteString bstr = fxv8::ToByteString(GetIsolate(), value.As<v8::String>());
    if (bstr.IsEmpty()) {
      return value;
    }
    if (bstr == "NaN") {
      bAllowNaN = true;
    }
  }

  v8::TryCatch try_catch(GetIsolate());
  v8::MaybeLocal<v8::Number> maybeNum =
      value->ToNumber(GetIsolate()->GetCurrentContext());
  if (maybeNum.IsEmpty()) {
    return value;
  }

  v8::Local<v8::Number> num = maybeNum.ToLocalChecked();
  if (isnan(num->Value()) && !bAllowNaN) {
    return value;
  }

  return num;
}
