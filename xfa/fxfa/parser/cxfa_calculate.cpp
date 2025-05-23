// Copyright 2017 The PDFium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_calculate.h"

#include "fxjs/xfa/cjx_node.h"
#include "xfa/fxfa/parser/cxfa_document.h"
#include "xfa/fxfa/parser/cxfa_message.h"
#include "xfa/fxfa/parser/cxfa_script.h"
#include "xfa/fxfa/parser/cxfa_text.h"

namespace {

const CXFA_Node::PropertyData kCalculatePropertyData[] = {
    {XFA_Element::Message, 1, {}},
    {XFA_Element::Script, 1, {}},
    {XFA_Element::Extras, 1, {}},
};

const CXFA_Node::AttributeData kCalculateAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Override, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Error},
};

}  // namespace

CXFA_Calculate::CXFA_Calculate(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                {XFA_XDPPACKET::kTemplate, XFA_XDPPACKET::kForm},
                XFA_ObjectType::Node,
                XFA_Element::Calculate,
                kCalculatePropertyData,
                kCalculateAttributeData,
                cppgc::MakeGarbageCollected<CJX_Node>(
                    doc->GetHeap()->GetAllocationHandle(),
                    this)) {}

CXFA_Calculate::~CXFA_Calculate() = default;

XFA_AttributeValue CXFA_Calculate::GetOverride() {
  return JSObject()
      ->TryEnum(XFA_Attribute::Override, false)
      .value_or(XFA_AttributeValue::Error);
}

CXFA_Script* CXFA_Calculate::GetScriptIfExists() {
  return GetChild<CXFA_Script>(0, XFA_Element::Script, false);
}

WideString CXFA_Calculate::GetMessageText() const {
  const auto* pNode = GetChild<CXFA_Message>(0, XFA_Element::Message, false);
  if (!pNode) {
    return WideString();
  }

  const auto* text = pNode->GetChild<CXFA_Text>(0, XFA_Element::Text, false);
  return text ? text->GetContent() : WideString();
}
