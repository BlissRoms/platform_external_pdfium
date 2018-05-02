// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fxcrt/xml/cfx_xmlparser.h"

#include <memory>

#include "core/fxcrt/cfx_memorystream.h"
#include "core/fxcrt/fx_codepage.h"
#include "core/fxcrt/xml/cfx_xmldocument.h"
#include "core/fxcrt/xml/cfx_xmlelement.h"
#include "core/fxcrt/xml/cfx_xmlinstruction.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/test_support.h"
#include "third_party/base/ptr_util.h"

namespace {

class CFX_XMLTestParser : public CFX_XMLParser {
 public:
  explicit CFX_XMLTestParser(const RetainPtr<IFX_SeekableStream>& pStream)
      : CFX_XMLParser(pStream) {}

  ~CFX_XMLTestParser() override = default;

  FX_XmlSyntaxResult DoSyntaxParse() { return CFX_XMLParser::DoSyntaxParse(); }
  WideString GetTextData() { return CFX_XMLParser::GetTextData(); }
};

RetainPtr<CFX_MemoryStream> MakeProxy(const char* input) {
  auto stream = pdfium::MakeRetain<CFX_MemoryStream>(
      reinterpret_cast<uint8_t*>(const_cast<char*>(input)), strlen(input),
      false);
  return stream;
}

}  // namespace

TEST(CFX_XMLParserTest, CData) {
  const char* input =
      "<script contentType=\"application/x-javascript\">\n"
      "  <![CDATA[\n"
      "    if (a[1] < 3)\n"
      "      app.alert(\"Tclams\");\n"
      "  ]]>\n"
      "</script>";

  const wchar_t* cdata =
      L"\n"
      L"    if (a[1] < 3)\n"
      L"      app.alert(\"Tclams\");\n"
      L"  ";

  auto stream = MakeProxy(input);
  CFX_XMLTestParser parser(stream);
  ASSERT_EQ(FX_XmlSyntaxResult::ElementOpen, parser.DoSyntaxParse());
  ASSERT_EQ(FX_XmlSyntaxResult::TagName, parser.DoSyntaxParse());
  ASSERT_EQ(L"script", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::AttriName, parser.DoSyntaxParse());
  ASSERT_EQ(L"contentType", parser.GetTextData());
  ASSERT_EQ(FX_XmlSyntaxResult::AttriValue, parser.DoSyntaxParse());
  ASSERT_EQ(L"application/x-javascript", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::ElementBreak, parser.DoSyntaxParse());
  ASSERT_EQ(FX_XmlSyntaxResult::Text, parser.DoSyntaxParse());
  ASSERT_EQ(L"\n  ", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::CData, parser.DoSyntaxParse());
  ASSERT_EQ(cdata, parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::Text, parser.DoSyntaxParse());
  ASSERT_EQ(L"\n", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::ElementClose, parser.DoSyntaxParse());
  ASSERT_EQ(L"script", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::EndOfString, parser.DoSyntaxParse());
}

TEST(CFX_XMLParserTest, CDataWithInnerScript) {
  const char* input =
      "<script contentType=\"application/x-javascript\">\n"
      "  <![CDATA[\n"
      "    if (a[1] < 3)\n"
      "      app.alert(\"Tclams\");\n"
      "    </script>\n"
      "  ]]>\n"
      "</script>";

  const wchar_t* cdata =
      L"\n"
      L"    if (a[1] < 3)\n"
      L"      app.alert(\"Tclams\");\n"
      L"    </script>\n"
      L"  ";

  auto stream = MakeProxy(input);
  CFX_XMLTestParser parser(stream);
  ASSERT_EQ(FX_XmlSyntaxResult::ElementOpen, parser.DoSyntaxParse());
  ASSERT_EQ(FX_XmlSyntaxResult::TagName, parser.DoSyntaxParse());
  ASSERT_EQ(L"script", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::AttriName, parser.DoSyntaxParse());
  ASSERT_EQ(L"contentType", parser.GetTextData());
  ASSERT_EQ(FX_XmlSyntaxResult::AttriValue, parser.DoSyntaxParse());
  ASSERT_EQ(L"application/x-javascript", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::ElementBreak, parser.DoSyntaxParse());
  ASSERT_EQ(FX_XmlSyntaxResult::Text, parser.DoSyntaxParse());
  ASSERT_EQ(L"\n  ", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::CData, parser.DoSyntaxParse());
  ASSERT_EQ(cdata, parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::Text, parser.DoSyntaxParse());
  ASSERT_EQ(L"\n", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::ElementClose, parser.DoSyntaxParse());
  ASSERT_EQ(L"script", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::EndOfString, parser.DoSyntaxParse());
}

TEST(CFX_XMLParserTest, ArrowBangArrow) {
  const char* input =
      "<script contentType=\"application/x-javascript\">\n"
      "  <!>\n"
      "</script>";

  auto stream = MakeProxy(input);
  CFX_XMLTestParser parser(stream);
  ASSERT_EQ(FX_XmlSyntaxResult::ElementOpen, parser.DoSyntaxParse());
  ASSERT_EQ(FX_XmlSyntaxResult::TagName, parser.DoSyntaxParse());

  ASSERT_EQ(L"script", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::AttriName, parser.DoSyntaxParse());
  ASSERT_EQ(L"contentType", parser.GetTextData());
  ASSERT_EQ(FX_XmlSyntaxResult::AttriValue, parser.DoSyntaxParse());
  ASSERT_EQ(L"application/x-javascript", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::ElementBreak, parser.DoSyntaxParse());
  ASSERT_EQ(FX_XmlSyntaxResult::Text, parser.DoSyntaxParse());
  ASSERT_EQ(L"\n  ", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::Text, parser.DoSyntaxParse());
  ASSERT_EQ(L"\n", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::ElementClose, parser.DoSyntaxParse());
  ASSERT_EQ(L"script", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::EndOfString, parser.DoSyntaxParse());
}

TEST(CFX_XMLParserTest, ArrowBangBracketArrow) {
  const char* input =
      "<script contentType=\"application/x-javascript\">\n"
      "  <![>\n"
      "</script>";

  auto stream = MakeProxy(input);
  CFX_XMLTestParser parser(stream);
  ASSERT_EQ(FX_XmlSyntaxResult::ElementOpen, parser.DoSyntaxParse());
  ASSERT_EQ(FX_XmlSyntaxResult::TagName, parser.DoSyntaxParse());
  ASSERT_EQ(L"script", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::AttriName, parser.DoSyntaxParse());
  ASSERT_EQ(L"contentType", parser.GetTextData());
  ASSERT_EQ(FX_XmlSyntaxResult::AttriValue, parser.DoSyntaxParse());
  ASSERT_EQ(L"application/x-javascript", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::ElementBreak, parser.DoSyntaxParse());
  ASSERT_EQ(FX_XmlSyntaxResult::Text, parser.DoSyntaxParse());
  ASSERT_EQ(L"\n  ", parser.GetTextData());

  // Parser walks to end of input.

  ASSERT_EQ(FX_XmlSyntaxResult::EndOfString, parser.DoSyntaxParse());
}

TEST(CFX_XMLParserTest, IncompleteCData) {
  const char* input =
      "<script contentType=\"application/x-javascript\">\n"
      "  <![CDATA>\n"
      "</script>";

  auto stream = MakeProxy(input);
  CFX_XMLTestParser parser(stream);
  ASSERT_EQ(FX_XmlSyntaxResult::ElementOpen, parser.DoSyntaxParse());
  ASSERT_EQ(FX_XmlSyntaxResult::TagName, parser.DoSyntaxParse());
  ASSERT_EQ(L"script", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::AttriName, parser.DoSyntaxParse());
  ASSERT_EQ(L"contentType", parser.GetTextData());
  ASSERT_EQ(FX_XmlSyntaxResult::AttriValue, parser.DoSyntaxParse());
  ASSERT_EQ(L"application/x-javascript", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::ElementBreak, parser.DoSyntaxParse());
  ASSERT_EQ(FX_XmlSyntaxResult::Text, parser.DoSyntaxParse());
  ASSERT_EQ(L"\n  ", parser.GetTextData());

  // Parser walks to end of input.

  ASSERT_EQ(FX_XmlSyntaxResult::EndOfString, parser.DoSyntaxParse());
}

TEST(CFX_XMLParserTest, UnClosedCData) {
  const char* input =
      "<script contentType=\"application/x-javascript\">\n"
      "  <![CDATA[\n"
      "</script>";

  auto stream = MakeProxy(input);
  CFX_XMLTestParser parser(stream);
  ASSERT_EQ(FX_XmlSyntaxResult::ElementOpen, parser.DoSyntaxParse());
  ASSERT_EQ(FX_XmlSyntaxResult::TagName, parser.DoSyntaxParse());
  ASSERT_EQ(L"script", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::AttriName, parser.DoSyntaxParse());
  ASSERT_EQ(L"contentType", parser.GetTextData());
  ASSERT_EQ(FX_XmlSyntaxResult::AttriValue, parser.DoSyntaxParse());
  ASSERT_EQ(L"application/x-javascript", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::ElementBreak, parser.DoSyntaxParse());
  ASSERT_EQ(FX_XmlSyntaxResult::Text, parser.DoSyntaxParse());
  ASSERT_EQ(L"\n  ", parser.GetTextData());

  // Parser walks to end of input.

  ASSERT_EQ(FX_XmlSyntaxResult::EndOfString, parser.DoSyntaxParse());
}

TEST(CFX_XMLParserTest, EmptyCData) {
  const char* input =
      "<script contentType=\"application/x-javascript\">\n"
      "  <![CDATA[]]>\n"
      "</script>";

  auto stream = MakeProxy(input);
  CFX_XMLTestParser parser(stream);
  ASSERT_EQ(FX_XmlSyntaxResult::ElementOpen, parser.DoSyntaxParse());
  ASSERT_EQ(FX_XmlSyntaxResult::TagName, parser.DoSyntaxParse());
  ASSERT_EQ(L"script", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::AttriName, parser.DoSyntaxParse());
  ASSERT_EQ(L"contentType", parser.GetTextData());
  ASSERT_EQ(FX_XmlSyntaxResult::AttriValue, parser.DoSyntaxParse());
  ASSERT_EQ(L"application/x-javascript", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::ElementBreak, parser.DoSyntaxParse());
  ASSERT_EQ(FX_XmlSyntaxResult::Text, parser.DoSyntaxParse());
  ASSERT_EQ(L"\n  ", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::CData, parser.DoSyntaxParse());
  ASSERT_EQ(L"", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::Text, parser.DoSyntaxParse());
  ASSERT_EQ(L"\n", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::ElementClose, parser.DoSyntaxParse());
  ASSERT_EQ(L"script", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::EndOfString, parser.DoSyntaxParse());
}

TEST(CFX_XMLParserTest, Comment) {
  const char* input =
      "<script contentType=\"application/x-javascript\">\n"
      "  <!-- A Comment -->\n"
      "</script>";

  auto stream = MakeProxy(input);
  CFX_XMLTestParser parser(stream);
  ASSERT_EQ(FX_XmlSyntaxResult::ElementOpen, parser.DoSyntaxParse());
  ASSERT_EQ(FX_XmlSyntaxResult::TagName, parser.DoSyntaxParse());
  ASSERT_EQ(L"script", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::AttriName, parser.DoSyntaxParse());
  ASSERT_EQ(L"contentType", parser.GetTextData());
  ASSERT_EQ(FX_XmlSyntaxResult::AttriValue, parser.DoSyntaxParse());
  ASSERT_EQ(L"application/x-javascript", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::ElementBreak, parser.DoSyntaxParse());
  ASSERT_EQ(FX_XmlSyntaxResult::Text, parser.DoSyntaxParse());
  ASSERT_EQ(L"\n  ", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::Text, parser.DoSyntaxParse());
  ASSERT_EQ(L"\n", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::ElementClose, parser.DoSyntaxParse());
  ASSERT_EQ(L"script", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::EndOfString, parser.DoSyntaxParse());
}

TEST(CFX_XMLParserTest, IncorrectCommentStart) {
  const char* input =
      "<script contentType=\"application/x-javascript\">\n"
      "  <!- A Comment -->\n"
      "</script>";

  auto stream = MakeProxy(input);
  CFX_XMLTestParser parser(stream);
  ASSERT_EQ(FX_XmlSyntaxResult::ElementOpen, parser.DoSyntaxParse());
  ASSERT_EQ(FX_XmlSyntaxResult::TagName, parser.DoSyntaxParse());
  ASSERT_EQ(L"script", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::AttriName, parser.DoSyntaxParse());
  ASSERT_EQ(L"contentType", parser.GetTextData());
  ASSERT_EQ(FX_XmlSyntaxResult::AttriValue, parser.DoSyntaxParse());
  ASSERT_EQ(L"application/x-javascript", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::ElementBreak, parser.DoSyntaxParse());
  ASSERT_EQ(FX_XmlSyntaxResult::Text, parser.DoSyntaxParse());
  ASSERT_EQ(L"\n  ", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::Text, parser.DoSyntaxParse());
  ASSERT_EQ(L"\n", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::ElementClose, parser.DoSyntaxParse());
  ASSERT_EQ(L"script", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::EndOfString, parser.DoSyntaxParse());
}

TEST(CFX_XMLParserTest, CommentEmpty) {
  const char* input =
      "<script contentType=\"application/x-javascript\">\n"
      "  <!---->\n"
      "</script>";

  auto stream = MakeProxy(input);
  CFX_XMLTestParser parser(stream);
  ASSERT_EQ(FX_XmlSyntaxResult::ElementOpen, parser.DoSyntaxParse());
  ASSERT_EQ(FX_XmlSyntaxResult::TagName, parser.DoSyntaxParse());
  ASSERT_EQ(L"script", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::AttriName, parser.DoSyntaxParse());
  ASSERT_EQ(L"contentType", parser.GetTextData());
  ASSERT_EQ(FX_XmlSyntaxResult::AttriValue, parser.DoSyntaxParse());
  ASSERT_EQ(L"application/x-javascript", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::ElementBreak, parser.DoSyntaxParse());
  ASSERT_EQ(FX_XmlSyntaxResult::Text, parser.DoSyntaxParse());
  ASSERT_EQ(L"\n  ", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::Text, parser.DoSyntaxParse());
  ASSERT_EQ(L"\n", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::ElementClose, parser.DoSyntaxParse());
  ASSERT_EQ(L"script", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::EndOfString, parser.DoSyntaxParse());
}

TEST(CFX_XMLParserTest, CommentThreeDash) {
  const char* input =
      "<script contentType=\"application/x-javascript\">\n"
      "  <!--->\n"
      "</script>";

  auto stream = MakeProxy(input);
  CFX_XMLTestParser parser(stream);
  ASSERT_EQ(FX_XmlSyntaxResult::ElementOpen, parser.DoSyntaxParse());
  ASSERT_EQ(FX_XmlSyntaxResult::TagName, parser.DoSyntaxParse());
  ASSERT_EQ(L"script", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::AttriName, parser.DoSyntaxParse());
  ASSERT_EQ(L"contentType", parser.GetTextData());
  ASSERT_EQ(FX_XmlSyntaxResult::AttriValue, parser.DoSyntaxParse());
  ASSERT_EQ(L"application/x-javascript", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::ElementBreak, parser.DoSyntaxParse());
  ASSERT_EQ(FX_XmlSyntaxResult::Text, parser.DoSyntaxParse());
  ASSERT_EQ(L"\n  ", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::EndOfString, parser.DoSyntaxParse());
}

TEST(CFX_XMLParserTest, CommentTwoDash) {
  const char* input =
      "<script contentType=\"application/x-javascript\">\n"
      "  <!-->\n"
      "</script>";

  auto stream = MakeProxy(input);
  CFX_XMLTestParser parser(stream);
  ASSERT_EQ(FX_XmlSyntaxResult::ElementOpen, parser.DoSyntaxParse());
  ASSERT_EQ(FX_XmlSyntaxResult::TagName, parser.DoSyntaxParse());
  ASSERT_EQ(L"script", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::AttriName, parser.DoSyntaxParse());
  ASSERT_EQ(L"contentType", parser.GetTextData());
  ASSERT_EQ(FX_XmlSyntaxResult::AttriValue, parser.DoSyntaxParse());
  ASSERT_EQ(L"application/x-javascript", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::ElementBreak, parser.DoSyntaxParse());
  ASSERT_EQ(FX_XmlSyntaxResult::Text, parser.DoSyntaxParse());
  ASSERT_EQ(L"\n  ", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::EndOfString, parser.DoSyntaxParse());
}

TEST(CFX_XMLParserTest, Entities) {
  const char* input =
      "<script contentType=\"application/x-javascript\">"
      "&#66;"                     // B
      "&#x54;"                    // T
      "&#x6a;"                    // j
      "&#x00000000000000000048;"  // H
      "&#x0000000000000000AB48;"  // \xab48
      "&#x0000000000000000000;"
      "&amp;"
      "&lt;"
      "&gt;"
      "&apos;"
      "&quot;"
      "&something_else;"
      "</script>";

  auto stream = MakeProxy(input);
  CFX_XMLTestParser parser(stream);
  ASSERT_EQ(FX_XmlSyntaxResult::ElementOpen, parser.DoSyntaxParse());
  ASSERT_EQ(FX_XmlSyntaxResult::TagName, parser.DoSyntaxParse());
  ASSERT_EQ(L"script", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::AttriName, parser.DoSyntaxParse());
  ASSERT_EQ(L"contentType", parser.GetTextData());
  ASSERT_EQ(FX_XmlSyntaxResult::AttriValue, parser.DoSyntaxParse());
  ASSERT_EQ(L"application/x-javascript", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::ElementBreak, parser.DoSyntaxParse());
  ASSERT_EQ(FX_XmlSyntaxResult::Text, parser.DoSyntaxParse());
  ASSERT_EQ(L"BTjH\xab48&<>'\"", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::ElementClose, parser.DoSyntaxParse());
  ASSERT_EQ(L"script", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::EndOfString, parser.DoSyntaxParse());
}

TEST(CFX_XMLParserTest, EntityOverflowHex) {
  const char* input =
      "<script contentType=\"application/x-javascript\">"
      "&#xaDBDFFFFF;"
      "&#xafffffffffffffffffffffffffffffffff;"
      "</script>";

  auto stream = MakeProxy(input);
  CFX_XMLTestParser parser(stream);
  ASSERT_EQ(FX_XmlSyntaxResult::ElementOpen, parser.DoSyntaxParse());
  ASSERT_EQ(FX_XmlSyntaxResult::TagName, parser.DoSyntaxParse());
  ASSERT_EQ(L"script", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::AttriName, parser.DoSyntaxParse());
  ASSERT_EQ(L"contentType", parser.GetTextData());
  ASSERT_EQ(FX_XmlSyntaxResult::AttriValue, parser.DoSyntaxParse());
  ASSERT_EQ(L"application/x-javascript", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::ElementBreak, parser.DoSyntaxParse());
  ASSERT_EQ(FX_XmlSyntaxResult::Text, parser.DoSyntaxParse());
  ASSERT_EQ(L"  ", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::ElementClose, parser.DoSyntaxParse());
  ASSERT_EQ(L"script", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::EndOfString, parser.DoSyntaxParse());
}

TEST(CFX_XMLParserTest, EntityOverflowDecimal) {
  const char* input =
      "<script contentType=\"application/x-javascript\">"
      "&#2914910205;"
      "&#29149102052342342134521341234512351234213452315;"
      "</script>";

  auto stream = MakeProxy(input);
  CFX_XMLTestParser parser(stream);
  ASSERT_EQ(FX_XmlSyntaxResult::ElementOpen, parser.DoSyntaxParse());
  ASSERT_EQ(FX_XmlSyntaxResult::TagName, parser.DoSyntaxParse());
  ASSERT_EQ(L"script", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::AttriName, parser.DoSyntaxParse());
  ASSERT_EQ(L"contentType", parser.GetTextData());
  ASSERT_EQ(FX_XmlSyntaxResult::AttriValue, parser.DoSyntaxParse());
  ASSERT_EQ(L"application/x-javascript", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::ElementBreak, parser.DoSyntaxParse());
  ASSERT_EQ(FX_XmlSyntaxResult::Text, parser.DoSyntaxParse());
  ASSERT_EQ(L"  ", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::ElementClose, parser.DoSyntaxParse());
  ASSERT_EQ(L"script", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::EndOfString, parser.DoSyntaxParse());
}

TEST(CFX_XMLParserTest, IsXMLNameChar) {
  EXPECT_FALSE(CFX_XMLTestParser::IsXMLNameChar(L'-', true));
  EXPECT_TRUE(CFX_XMLTestParser::IsXMLNameChar(L'-', false));

  EXPECT_FALSE(CFX_XMLTestParser::IsXMLNameChar(0x2069, true));
  EXPECT_TRUE(CFX_XMLTestParser::IsXMLNameChar(0x2070, true));
  EXPECT_TRUE(CFX_XMLTestParser::IsXMLNameChar(0x2073, true));
  EXPECT_TRUE(CFX_XMLTestParser::IsXMLNameChar(0x218F, true));
  EXPECT_FALSE(CFX_XMLTestParser::IsXMLNameChar(0x2190, true));

  EXPECT_FALSE(CFX_XMLTestParser::IsXMLNameChar(0xFDEF, true));
  EXPECT_TRUE(CFX_XMLTestParser::IsXMLNameChar(0xFDF0, true));
  EXPECT_TRUE(CFX_XMLTestParser::IsXMLNameChar(0xFDF1, true));
  EXPECT_TRUE(CFX_XMLTestParser::IsXMLNameChar(0xFFFD, true));
  EXPECT_FALSE(CFX_XMLTestParser::IsXMLNameChar(0xFFFE, true));
}

TEST(CFX_XMLParserTest, BadElementClose) {
  const char* input = "</endtag>";

  auto stream = MakeProxy(input);
  CFX_XMLTestParser parser(stream);
  ASSERT_EQ(FX_XmlSyntaxResult::Error, parser.DoSyntaxParse());
}

TEST(CFX_XMLParserTest, DoubleElementClose) {
  const char* input = "<p></p></p>";

  auto stream = MakeProxy(input);
  CFX_XMLTestParser parser(stream);
  ASSERT_EQ(FX_XmlSyntaxResult::ElementOpen, parser.DoSyntaxParse());
  ASSERT_EQ(FX_XmlSyntaxResult::TagName, parser.DoSyntaxParse());
  ASSERT_EQ(L"p", parser.GetTextData());
  ASSERT_EQ(FX_XmlSyntaxResult::ElementBreak, parser.DoSyntaxParse());
  ASSERT_EQ(L"", parser.GetTextData());
  ASSERT_EQ(FX_XmlSyntaxResult::ElementClose, parser.DoSyntaxParse());
  ASSERT_EQ(L"p", parser.GetTextData());
  ASSERT_EQ(FX_XmlSyntaxResult::Error, parser.DoSyntaxParse());
}

TEST(CFX_XMLParserTest, ParseInstruction) {
  const char* input =
      "<?originalXFAVersion http://www.xfa.org/schema/xfa-template/3.3/ ?>"
      "<form></form>";

  auto stream = MakeProxy(input);
  CFX_XMLTestParser parser(stream);

  auto doc = parser.Parse();
  ASSERT_TRUE(doc.get() != nullptr);

  CFX_XMLElement* root = doc->GetRoot();
  ASSERT_TRUE(root->GetFirstChild() != nullptr);
  ASSERT_EQ(FX_XMLNODE_Instruction, root->GetFirstChild()->GetType());

  CFX_XMLInstruction* instruction =
      static_cast<CFX_XMLInstruction*>(root->GetFirstChild());
  EXPECT_TRUE(instruction->IsOriginalXFAVersion());
}

TEST(CFX_XMLParserTest, BadEntity) {
  const char* input =
      "<script>"
      "Test &<p>; thing"
      "</script>";

  auto stream = MakeProxy(input);
  CFX_XMLTestParser parser(stream);
  ASSERT_EQ(FX_XmlSyntaxResult::ElementOpen, parser.DoSyntaxParse());
  ASSERT_EQ(FX_XmlSyntaxResult::TagName, parser.DoSyntaxParse());
  ASSERT_EQ(L"script", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::ElementBreak, parser.DoSyntaxParse());
  ASSERT_EQ(FX_XmlSyntaxResult::Text, parser.DoSyntaxParse());
  ASSERT_EQ(L"Test &", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::ElementOpen, parser.DoSyntaxParse());
  ASSERT_EQ(FX_XmlSyntaxResult::TagName, parser.DoSyntaxParse());
  ASSERT_EQ(L"p", parser.GetTextData());
  ASSERT_EQ(FX_XmlSyntaxResult::ElementBreak, parser.DoSyntaxParse());

  ASSERT_EQ(FX_XmlSyntaxResult::Text, parser.DoSyntaxParse());
  ASSERT_EQ(L"; thing", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::ElementClose, parser.DoSyntaxParse());
  ASSERT_EQ(L"script", parser.GetTextData());

  ASSERT_EQ(FX_XmlSyntaxResult::EndOfString, parser.DoSyntaxParse());
}
