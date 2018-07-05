// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_PAGE_CPDF_CONTENTMARK_H_
#define CORE_FPDFAPI_PAGE_CPDF_CONTENTMARK_H_

#include <memory>
#include <vector>

#include "core/fpdfapi/page/cpdf_contentmarkitem.h"
#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/retain_ptr.h"

class CPDF_Dictionary;

class CPDF_ContentMark {
 public:
  CPDF_ContentMark();
  ~CPDF_ContentMark();

  std::unique_ptr<CPDF_ContentMark> Clone();
  int GetMarkedContentID() const;
  size_t CountItems() const;
  const CPDF_ContentMarkItem& GetItem(size_t i) const;

  void AddMark(ByteString name, const CPDF_Dictionary* pDict, bool bDirect);
  void DeleteLastMark();

 private:
  class MarkData : public Retainable {
   public:
    MarkData();
    MarkData(const MarkData& src);
    ~MarkData() override;

    size_t CountItems() const;
    const CPDF_ContentMarkItem& GetItem(size_t index) const;

    int GetMarkedContentID() const;
    void AddMark(ByteString name,
                 const CPDF_Dictionary* pDict,
                 bool bDictNeedClone);
    void DeleteLastMark();

   private:
    std::vector<CPDF_ContentMarkItem> m_Marks;
  };

  RetainPtr<MarkData> m_pMarkData;
};

#endif  // CORE_FPDFAPI_PAGE_CPDF_CONTENTMARK_H_
