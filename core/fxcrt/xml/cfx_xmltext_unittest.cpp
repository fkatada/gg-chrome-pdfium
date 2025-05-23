// Copyright 2018 The PDFium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fxcrt/xml/cfx_xmltext.h"
#include "core/fxcrt/xml/cfx_xmldocument.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/string_write_stream.h"

TEST(CFXXMLTextTest, GetType) {
  CFX_XMLText text(L"My Text");
  EXPECT_EQ(CFX_XMLNode::Type::kText, text.GetType());
}

TEST(CFXXMLTextTest, GetText) {
  CFX_XMLText data(L"My Data");
  EXPECT_EQ(L"My Data", data.GetText());
}

TEST(CFXXMLTextTest, Clone) {
  CFX_XMLDocument doc;

  CFX_XMLText data(L"My Data");
  CFX_XMLNode* clone = data.Clone(&doc);
  EXPECT_TRUE(clone != nullptr);
  ASSERT_EQ(CFX_XMLNode::Type::kText, clone->GetType());
  EXPECT_EQ(L"My Data", ToXMLText(clone)->GetText());
}

TEST(CFXXMLTextTest, Save) {
  auto stream = pdfium::MakeRetain<StringWriteStream>();
  CFX_XMLText data(L"My Data & this is < and > and ' and \" stuff.");
  data.Save(stream);
  EXPECT_EQ("My Data &amp; this is &lt; and &gt; and &apos; and &quot; stuff.",
            stream->ToString());
}

TEST(CFXXMLTextTest, SetText) {
  CFX_XMLText data(L"My Data");
  EXPECT_EQ(L"My Data", data.GetText());
  data.SetText(L"New Text");
  EXPECT_EQ(L"New Text", data.GetText());
}
