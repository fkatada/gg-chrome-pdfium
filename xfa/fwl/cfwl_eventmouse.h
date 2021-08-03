// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CFWL_EVENTMOUSE_H_
#define XFA_FWL_CFWL_EVENTMOUSE_H_

#include "xfa/fwl/cfwl_event.h"
#include "xfa/fwl/cfwl_messagemouse.h"

class CFWL_EventMouse final : public CFWL_Event {
 public:
  CFWL_EventMouse(CFWL_Widget* pSrcTarget,
                  CFWL_Widget* pDstTarget,
                  CFWL_MessageMouse::MouseCommand cmd);
  ~CFWL_EventMouse() override;

  CFWL_MessageMouse::MouseCommand GetCommand() const { return m_dwCmd; }

 private:
  const CFWL_MessageMouse::MouseCommand m_dwCmd;
};

#endif  // XFA_FWL_CFWL_EVENTMOUSE_H_
