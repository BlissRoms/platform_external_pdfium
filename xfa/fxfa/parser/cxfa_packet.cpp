// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_packet.h"

namespace {

constexpr wchar_t kName[] = L"packet";

}  // namespace

CXFA_Packet::CXFA_Packet(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                XFA_XDPPACKET_XDP,
                XFA_ObjectType::NodeC,
                XFA_Element::Packet,
                nullptr,
                nullptr,
                kName) {}

CXFA_Packet::~CXFA_Packet() {}
