// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_connectionset.h"

namespace {

constexpr wchar_t kName[] = L"connectionSet";

}  // namespace

CXFA_ConnectionSet::CXFA_ConnectionSet(CXFA_Document* doc,
                                       XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                XFA_XDPPACKET_ConnectionSet,
                XFA_ObjectType::ModelNode,
                XFA_Element::ConnectionSet,
                nullptr,
                nullptr,
                kName) {}

CXFA_ConnectionSet::~CXFA_ConnectionSet() {}
