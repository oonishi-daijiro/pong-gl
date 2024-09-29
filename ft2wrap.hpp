#pragma once

#include "freetype/fttypes.h"
#include <cstdint>
#include <iostream>
#include <mutex>
#include <string>
#include <string_view>

#include <freetype2/ft2build.h>
#include FT_FREETYPE_H
// macro for include freetype2

namespace Ft2Wrap {

class freetype2 {
public:
  static inline bool init() {
    bool err;
    std::call_once(onceflag, [&]() { err = FT_Init_FreeType(&ftlib); });
    return err;
  }

protected:
  static inline FT_Library ftlib;
  static inline std::once_flag onceflag;
  friend class Face;
};

class Ft2Obj {
public:
  Ft2Obj() {}

  virtual int error() final { return err; };
  int err;
};

// pt expression of freetype
// The first 26 bits expresses integer part of pt.
// The last 6 bits expresses decimal part of pt.
// pt -> 0.3528mm

inline long getPoint(long integerPart, unsigned long decimalPart) {
  return (integerPart << 6) + decimalPart;
}

class Face : public Ft2Obj {
public:
  Face(std::string_view fontPath) {
    err = FT_New_Face(ftlib, fontPath.data(), 0, &face);
    FT_Select_Charmap(face, FT_ENCODING_UNICODE);
  }

  ~Face() { FT_Done_Face(face); }

  auto setCharSize(FT_F26Dot6 charWidthPt, FT_F26Dot6 charHeightPt,
                   FT_UInt horizonalRes, FT_UInt verticalRes) {
    return FT_Set_Char_Size(face, charWidthPt, charHeightPt, horizonalRes,
                            verticalRes);
  }

  auto setPixelSizes(FT_UInt pixelWidth, FT_UInt pixelHeight) {
    return FT_Set_Pixel_Sizes(face, pixelWidth, pixelHeight);
  }

  auto loadGlypth(FT_UInt glyphIndex, FT_Int32 loadFlags) {
    return FT_Load_Glyph(face, glyphIndex, loadFlags);
  }

  auto getCharIndex(auto &&character) {
    return FT_Get_Char_Index(face, character);
  }

  auto renderGlyph(FT_Render_Mode mode) {
    return FT_Render_Glyph(face->glyph, mode);
  }

  const FT_Face &operator->() const { return face; }

private:
  FT_Face face;
  FT_Library &ftlib = freetype2::ftlib;
};

} // namespace Ft2Wrap
