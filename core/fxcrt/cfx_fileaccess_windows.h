// Copyright 2014 The PDFium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_CFX_FILEACCESS_WINDOWS_H_
#define CORE_FXCRT_CFX_FILEACCESS_WINDOWS_H_

#include <stddef.h>
#include <stdint.h>

#include "build/build_config.h"
#include "core/fxcrt/fileaccess_iface.h"
#include "core/fxcrt/fx_types.h"
#include "core/fxcrt/unowned_ptr_exclusion.h"

#if !BUILDFLAG(IS_WIN)
#error "Included on the wrong platform"
#endif

class CFX_FileAccess_Windows final : public FileAccessIface {
 public:
  CFX_FileAccess_Windows();
  ~CFX_FileAccess_Windows() override;

  // FileAccessIface
  bool Open(ByteStringView fileName) override;
  void Close() override;
  FX_FILESIZE GetSize() const override;
  FX_FILESIZE GetPosition() const override;
  FX_FILESIZE SetPosition(FX_FILESIZE pos) override;
  size_t Read(pdfium::span<uint8_t> buffer) override;
  size_t Write(pdfium::span<const uint8_t> buffer) override;
  size_t ReadPos(pdfium::span<uint8_t> buffer, FX_FILESIZE pos) override;
  bool Flush() override;
  bool Truncate(FX_FILESIZE szFile) override;

 private:
  UNOWNED_PTR_EXCLUSION void* file_ = nullptr;  // void type incompatible.
};

#endif  // CORE_FXCRT_CFX_FILEACCESS_WINDOWS_H_
