// Copyright 2014 The PDFium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXBARCODE_ONED_BC_ONEDUPCAWRITER_H_
#define FXBARCODE_ONED_BC_ONEDUPCAWRITER_H_

#include <memory>

#include "core/fxcrt/fx_string.h"
#include "fxbarcode/BC_Library.h"
#include "fxbarcode/oned/BC_OnedEANWriter.h"

class CBC_OnedEAN13Writer;
class CFX_Matrix;
class CFX_RenderDevice;

class CBC_OnedUPCAWriter final : public CBC_OneDimEANWriter {
 public:
  CBC_OnedUPCAWriter();
  ~CBC_OnedUPCAWriter() override;

  // CBC_OneDimEANWriter:
  DataVector<uint8_t> Encode(const ByteString& contents) override;
  bool CheckContentValidity(WideStringView contents) override;
  WideString FilterContents(WideStringView contents) override;
  void InitEANWriter() override;
  int32_t CalcChecksum(const ByteString& contents) override;

 private:
  bool ShowChars(WideStringView contents,
                 CFX_RenderDevice* device,
                 const CFX_Matrix& matrix,
                 int32_t barWidth) override;

  std::unique_ptr<CBC_OnedEAN13Writer> sub_writer_;
};

#endif  // FXBARCODE_ONED_BC_ONEDUPCAWRITER_H_
