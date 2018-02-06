// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCODEC_BMP_FX_BMP_H_
#define CORE_FXCODEC_BMP_FX_BMP_H_

#include <setjmp.h>

#include <memory>
#include <vector>

#include "core/fxcodec/codec/ccodec_bmpmodule.h"

#define BMP_WIDTHBYTES(width, bitCount) ((width * bitCount) + 31) / 32 * 4
#define BMP_PAL_ENCODE(a, r, g, b) \
  (((uint32_t)(a) << 24) | ((r) << 16) | ((g) << 8) | (b))
#define BMP_D_STATUS_HEADER 0x01
#define BMP_D_STATUS_PAL 0x02
#define BMP_D_STATUS_DATA_PRE 0x03
#define BMP_D_STATUS_DATA 0x04
#define BMP_D_STATUS_TAIL 0x00
#define BMP_SIGNATURE 0x4D42
#define BMP_PAL_OLD 1
#define RLE_MARKER 0
#define RLE_EOL 0
#define RLE_EOI 1
#define RLE_DELTA 2
#define BMP_RGB 0L
#define BMP_RLE8 1L
#define BMP_RLE4 2L
#define BMP_BITFIELDS 3L
// Limit width to (MAXINT32 - 31) / 32
#define BMP_MAX_WIDTH 67108863
#pragma pack(1)
typedef struct tagBmpFileHeader {
  uint16_t bfType;
  uint32_t bfSize;
  uint16_t bfReserved1;
  uint16_t bfReserved2;
  uint32_t bfOffBits;
} BmpFileHeader;
typedef struct tagBmpCoreHeader {
  uint32_t bcSize;
  uint16_t bcWidth;
  uint16_t bcHeight;
  uint16_t bcPlanes;
  uint16_t bcBitCount;
} BmpCoreHeader;
typedef struct tagBmpInfoHeader {
  uint32_t biSize;
  int32_t biWidth;
  int32_t biHeight;
  uint16_t biPlanes;
  uint16_t biBitCount;
  uint32_t biCompression;
  uint32_t biSizeImage;
  int32_t biXPelsPerMeter;
  int32_t biYPelsPerMeter;
  uint32_t biClrUsed;
  uint32_t biClrImportant;
} BmpInfoHeader;
#pragma pack()

class CFX_BmpDecompressor {
 public:
  CFX_BmpDecompressor();
  ~CFX_BmpDecompressor();

  void Error();
  int32_t DecodeImage();
  int32_t ReadHeader();
  void SetInputBuffer(uint8_t* src_buf, uint32_t src_size);
  uint32_t GetAvailInput(uint8_t** avail_buf);

  jmp_buf jmpbuf_;

  void* context_ptr_;

  std::vector<uint8_t> out_row_buffer_;
  std::vector<uint32_t> palette_;
  uint8_t* next_in_;

  uint32_t header_offset_;
  uint32_t width_;
  uint32_t height_;
  uint32_t compress_flag_;
  int32_t components_;
  size_t src_row_bytes_;
  size_t out_row_bytes_;
  uint16_t bit_counts_;
  uint32_t color_used_;
  bool imgTB_flag_;
  int32_t pal_num_;
  int32_t pal_type_;
  uint32_t data_size_;
  uint32_t img_data_offset_;
  uint32_t img_ifh_size_;
  size_t row_num_;
  size_t col_num_;
  int32_t dpi_x_;
  int32_t dpi_y_;
  uint32_t mask_red_;
  uint32_t mask_green_;
  uint32_t mask_blue_;

  uint32_t avail_in_;
  uint32_t skip_size_;
  int32_t decode_status_;

 private:
  bool GetDataPosition(uint32_t cur_pos);
  void ReadScanline(uint32_t row_num, const std::vector<uint8_t>& row_buf);
  int32_t DecodeRGB();
  int32_t DecodeRLE8();
  int32_t DecodeRLE4();
  uint8_t* ReadData(uint8_t** des_buf, uint32_t data_size);
  void SaveDecodingStatus(int32_t status);
  bool ValidateColorIndex(uint8_t val);
  bool ValidateFlag() const;
  void SetHeight(int32_t signed_height);
};

class CFX_BmpContext : public CCodec_BmpModule::Context {
 public:
  CFX_BmpContext(CCodec_BmpModule* pModule,
                 CCodec_BmpModule::Delegate* pDelegate);
  ~CFX_BmpContext() override;

  CFX_BmpDecompressor m_Bmp;
  UnownedPtr<CCodec_BmpModule> const m_pModule;
  UnownedPtr<CCodec_BmpModule::Delegate> const m_pDelegate;
};

#endif  // CORE_FXCODEC_BMP_FX_BMP_H_
