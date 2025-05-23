// Copyright 2014 The PDFium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCODEC_JBIG2_JBIG2_HUFFMANDECODER_H_
#define CORE_FXCODEC_JBIG2_JBIG2_HUFFMANDECODER_H_

#include "core/fxcodec/jbig2/JBig2_BitStream.h"
#include "core/fxcodec/jbig2/JBig2_HuffmanTable.h"
#include "core/fxcrt/unowned_ptr.h"

class CJBig2_HuffmanDecoder {
 public:
  explicit CJBig2_HuffmanDecoder(CJBig2_BitStream* pStream);
  ~CJBig2_HuffmanDecoder();

  int DecodeAValue(const CJBig2_HuffmanTable* pTable, int* nResult);

 private:
  UnownedPtr<CJBig2_BitStream> const stream_;
};

#endif  // CORE_FXCODEC_JBIG2_JBIG2_HUFFMANDECODER_H_
