// Copyright 2014 The PDFium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_CXFA_FFWIDGET_H_
#define XFA_FXFA_CXFA_FFWIDGET_H_

#include <stdint.h>

#include "core/fpdfdoc/cpdf_formfield.h"
#include "core/fxcodec/fx_codec_def.h"
#include "core/fxcrt/mask.h"
#include "core/fxcrt/retain_ptr.h"
#include "core/fxge/cfx_graphstatedata.h"
#include "fxjs/gc/heap.h"
#include "v8/include/cppgc/garbage-collected.h"
#include "v8/include/cppgc/visitor.h"
#include "xfa/fwl/cfwl_app.h"
#include "xfa/fwl/cfwl_messagemouse.h"
#include "xfa/fwl/cfwl_widget.h"
#include "xfa/fwl/fwl_widgetdef.h"
#include "xfa/fxfa/cxfa_eventparam.h"
#include "xfa/fxfa/cxfa_ffapp.h"
#include "xfa/fxfa/fxfa.h"
#include "xfa/fxfa/layout/cxfa_contentlayoutitem.h"

class CFGAS_GEGraphics;
class CFX_DIBitmap;
class CXFA_Box;
class CXFA_FFApp;
class CXFA_FFDoc;
class CXFA_FFDocView;
class CXFA_FFField;
class CXFA_FFPageView;
class CXFA_FFWidgetHandler;
class CXFA_Margin;
class IFX_SeekableReadStream;

namespace pdfium {
enum class FWL_WidgetHit;
}  // namespace pdfium

inline float XFA_UnitPx2Pt(float fPx, float fDpi) {
  return fPx * 72.0f / fDpi;
}

constexpr float kXFAWidgetPrecision = 0.001f;

void XFA_DrawImage(CFGAS_GEGraphics* pGS,
                   const CFX_RectF& rtImage,
                   const CFX_Matrix& matrix,
                   RetainPtr<CFX_DIBitmap> pDIBitmap,
                   XFA_AttributeValue iAspect,
                   const CFX_Size& dpi,
                   XFA_AttributeValue iHorzAlign,
                   XFA_AttributeValue iVertAlign);

RetainPtr<CFX_DIBitmap> XFA_LoadImageFromBuffer(
    RetainPtr<IFX_SeekableReadStream> pImageFileRead,
    FXCODEC_IMAGE_TYPE type,
    int32_t& iImageXDpi,
    int32_t& iImageYDpi);

void XFA_RectWithoutMargin(CFX_RectF* rt, const CXFA_Margin* margin);

class CXFA_FFWidget : public cppgc::GarbageCollected<CXFA_FFWidget>,
                      public CFWL_Widget::AdapterIface {
 public:
  enum FocusOption { kDoNotDrawFocus = 0, kDrawFocus };
  enum HighlightOption { kNoHighlight = 0, kHighlight };

  class IteratorIface {
   public:
    virtual ~IteratorIface() = default;

    virtual CXFA_FFWidget* MoveToFirst() = 0;
    virtual CXFA_FFWidget* MoveToLast() = 0;
    virtual CXFA_FFWidget* MoveToNext() = 0;
    virtual CXFA_FFWidget* MoveToPrevious() = 0;
    virtual CXFA_FFWidget* GetCurrentWidget() = 0;
    virtual bool SetCurrentWidget(CXFA_FFWidget* hWidget) = 0;
  };

  static CXFA_FFWidget* FromLayoutItem(CXFA_LayoutItem* pLayoutItem);

  CONSTRUCT_VIA_MAKE_GARBAGE_COLLECTED;
  ~CXFA_FFWidget() override;

  void Trace(cppgc::Visitor* visitor) const override;

  // CFWL_Widget::AdapterIface:
  CFX_Matrix GetRotateMatrix() override;
  void DisplayCaret(bool bVisible, const CFX_RectF* pRtAnchor) override;
  void GetBorderColorAndThickness(FX_ARGB* cr, float* fWidth) override;

  virtual CXFA_FFField* AsField();
  virtual CFX_RectF GetBBox(FocusOption focus);
  virtual void RenderWidget(CFGAS_GEGraphics* pGS,
                            const CFX_Matrix& matrix,
                            HighlightOption highlight);
  virtual bool IsLoaded();
  virtual bool LoadWidget();
  virtual void PerformLayout();
  virtual void UpdateFWLData();
  virtual void UpdateWidgetProperty();
  // |command| must be LeftButtonDown or RightButtonDown.
  virtual bool AcceptsFocusOnButtonDown(
      Mask<XFA_FWL_KeyFlag> dwFlags,
      const CFX_PointF& point,
      CFWL_MessageMouse::MouseCommand command);

  // Caution: Returning false from an On* method may mean |this| is destroyed.
  [[nodiscard]] virtual bool OnMouseEnter();
  [[nodiscard]] virtual bool OnMouseExit();
  [[nodiscard]] virtual bool OnLButtonDown(Mask<XFA_FWL_KeyFlag> dwFlags,
                                           const CFX_PointF& point);
  [[nodiscard]] virtual bool OnLButtonUp(Mask<XFA_FWL_KeyFlag> dwFlags,
                                         const CFX_PointF& point);
  [[nodiscard]] virtual bool OnLButtonDblClk(Mask<XFA_FWL_KeyFlag> dwFlags,
                                             const CFX_PointF& point);
  [[nodiscard]] virtual bool OnMouseMove(Mask<XFA_FWL_KeyFlag> dwFlags,
                                         const CFX_PointF& point);
  [[nodiscard]] virtual bool OnMouseWheel(Mask<XFA_FWL_KeyFlag> dwFlags,
                                          const CFX_PointF& point,
                                          const CFX_Vector& delta);
  [[nodiscard]] virtual bool OnRButtonDown(Mask<XFA_FWL_KeyFlag> dwFlags,
                                           const CFX_PointF& point);
  [[nodiscard]] virtual bool OnRButtonUp(Mask<XFA_FWL_KeyFlag> dwFlags,
                                         const CFX_PointF& point);
  [[nodiscard]] virtual bool OnRButtonDblClk(Mask<XFA_FWL_KeyFlag> dwFlags,
                                             const CFX_PointF& point);
  [[nodiscard]] virtual bool OnSetFocus(CXFA_FFWidget* pOldWidget);
  [[nodiscard]] virtual bool OnKillFocus(CXFA_FFWidget* pNewWidget);
  [[nodiscard]] virtual bool OnKeyDown(XFA_FWL_VKEYCODE dwKeyCode,
                                       Mask<XFA_FWL_KeyFlag> dwFlags);
  [[nodiscard]] virtual bool OnChar(uint32_t dwChar,
                                    Mask<XFA_FWL_KeyFlag> dwFlags);

  virtual pdfium::FWL_WidgetHit HitTest(const CFX_PointF& point);
  virtual bool CanUndo();
  virtual bool CanRedo();
  virtual bool CanCopy();
  virtual bool CanCut();
  virtual bool CanPaste();
  virtual bool CanSelectAll();
  virtual bool CanDelete();
  virtual bool CanDeSelect();
  virtual bool Undo();
  virtual bool Redo();
  virtual std::optional<WideString> Copy();
  virtual std::optional<WideString> Cut();
  virtual bool Paste(const WideString& wsPaste);
  virtual void SelectAll();
  virtual void Delete();
  virtual void DeSelect();
  virtual WideString GetText();
  virtual FormFieldType GetFormFieldType();

  CXFA_Node* GetNode() const { return node_; }
  CXFA_ContentLayoutItem* GetLayoutItem() const { return layout_item_; }
  void SetLayoutItem(CXFA_ContentLayoutItem* pItem) { layout_item_ = pItem; }
  CXFA_FFPageView* GetPageView() const { return page_view_; }
  void SetPageView(CXFA_FFPageView* pPageView) { page_view_ = pPageView; }
  CXFA_FFDocView* GetDocView() const { return doc_view_; }
  void SetDocView(CXFA_FFDocView* pDocView) { doc_view_ = pDocView; }

  CXFA_FFWidget* GetNextFFWidget() const;
  const CFX_RectF& GetWidgetRect() const;
  const CFX_RectF& RecacheWidgetRect() const;
  void ModifyStatus(Mask<XFA_WidgetStatus> dwAdded,
                    Mask<XFA_WidgetStatus> dwRemoved);

  CXFA_FFDoc* GetDoc();
  CXFA_FFApp* GetApp();
  CXFA_FFApp::CallbackIface* GetAppProvider();
  CFWL_App* GetFWLApp() const;
  void InvalidateRect();
  bool IsFocused() const {
    return GetLayoutItem()->TestStatusBits(XFA_WidgetStatus::kFocused);
  }
  CFX_PointF Rotate2Normal(const CFX_PointF& point);
  bool IsLayoutRectEmpty();
  CXFA_LayoutItem* GetParent();
  bool IsAncestorOf(CXFA_FFWidget* pWidget);
  bool HasEventUnderHandler(XFA_EVENTTYPE eEventType,
                            CXFA_FFWidgetHandler* pHandler);
  bool ProcessEventUnderHandler(CXFA_EventParam* params,
                                CXFA_FFWidgetHandler* pHandler);

 protected:
  explicit CXFA_FFWidget(CXFA_Node* pNode);
  virtual bool PtInActiveRect(const CFX_PointF& point);

  void DrawBorder(CFGAS_GEGraphics* pGS,
                  CXFA_Box* box,
                  const CFX_RectF& rtBorder,
                  const CFX_Matrix& matrix);
  void DrawBorderWithFlag(CFGAS_GEGraphics* pGS,
                          CXFA_Box* box,
                          const CFX_RectF& rtBorder,
                          const CFX_Matrix& matrix,
                          bool forceRound);

  CFX_RectF GetRectWithoutRotate();
  bool HasVisibleStatus() const;
  void EventKillFocus();
  bool IsButtonDown();
  void SetButtonDown(bool bSet);

  cppgc::Member<CXFA_ContentLayoutItem> layout_item_;
  cppgc::Member<CXFA_FFDocView> doc_view_;
  cppgc::Member<CXFA_FFPageView> page_view_;
  cppgc::Member<CXFA_Node> const node_;
  mutable CFX_RectF widget_rect_;
};

inline CXFA_FFField* ToField(CXFA_FFWidget* widget) {
  return widget ? widget->AsField() : nullptr;
}

#endif  // XFA_FXFA_CXFA_FFWIDGET_H_
