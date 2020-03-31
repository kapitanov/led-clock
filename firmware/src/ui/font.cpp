#include "font.h"
#include <FS.h>

using namespace fs;

void font_init()
{
  SPIFFS.begin();
}

const char *font_get_name(ui_font_id id);

int font_measure_glyph(ui_led_matrix &m, File &file);
int font_draw_glyph(ui_led_matrix &m, File &file, int x, int y);
int font_draw_undefined(ui_led_matrix &m, int x, int y);

String font_utf8_to_win1251(const String &source)
{
  int i, k;
  String target;
  unsigned char n;
  char m[2] = {'0', '\0'};

  k = source.length();
  i = 0;

  while (i < k)
  {
    n = source[i];
    i++;

    if (n >= 0xC0)
    {
      switch (n)
      {
      case 0xD0:
      {
        n = source[i];
        i++;
        if (n == 0x81)
        {
          n = 0xA8;
          break;
        }
        if (n >= 0x90 && n <= 0xBF)
          n = n + 0x30;
        break;
      }
      case 0xD1:
      {
        n = source[i];
        i++;
        if (n == 0x91)
        {
          n = 0xB8;
          break;
        }
        if (n >= 0x80 && n <= 0x8F)
          n = n + 0x70;
        break;
      }
      }
    }
    m[0] = n;
    target = target + String(m);
  }
  return target;
}

int font_measure(ui_led_matrix &m, ui_font_id font, char c)
{
  char filename[64];
  sprintf(filename, "/font/%s/%d.dat\0", font_get_name(font), c);

  if (!SPIFFS.exists(filename))
  {
    Serial.printf("[font] no such file: '%s' (char '%c' 0x%02X)\r\n", filename, c, c);
    return 0;
  }

  fs::File f = SPIFFS.open(filename, "r");
  int w = font_measure_glyph(m, f);
  f.close();
  return w;
}

int font_draw(ui_led_matrix &m, ui_font_id font, char c, int x, int y)
{
  char filename[64];
  sprintf(filename, "/font/%s/%d.dat\0", font_get_name(font), c);

  if (!SPIFFS.exists(filename))
  {
    Serial.printf("[font] no such file: '%s' (char '%c' 0x%02X)\r\n", filename, c, c);
    return font_draw_undefined(m, x, y);
  }

  fs::File f = SPIFFS.open(filename, "r");
  int w = font_draw_glyph(m, f, x, y);
  f.close();
  return w;
}

const char *font_get_name(ui_font_id id)
{
  switch (id)
  {
  case UI_FONT_DEFAULT:
    return "default";
  case UI_FONT_MONOSPACE:
    return "monospace";
  case UI_FONT_SPECIAL:
    return "special";
  case UI_FONT_CLOCK:
    return "clock";
  default:
    return "default";
  }
}

int font_measure_glyph(ui_led_matrix &m, File &file)
{
  int cx = 0;
  int w = 0;

  int c = 0;
  while ((c = file.read()) != -1)
  {
    switch (c)
    {
    case '.':
      cx++;
      break;
    case '#':
      cx++;
      break;
    case '\n':
      if (w < cx)
      {
        w = cx;
      }
      cx = 0;
      break;
    }
  }

  return w;
}

int font_draw_glyph(ui_led_matrix &m, File &file, int x, int y)
{
  int cx = 0;
  int cy = 0;
  int w = 0;

  int c = 0;
  while ((c = file.read()) != -1)
  {
    switch (c)
    {
    case '.':
      m.set(x + cx, y + cy, false);
      cx++;
      break;
    case '#':
      m.set(x + cx, y + cy, true);
      cx++;
      break;
    case '\n':
      if (w < cx)
      {
        w = cx;
      }
      cx = 0;
      cy++;
      break;
    }
  }

  return w;
}

int font_draw_undefined(ui_led_matrix &m, int x, int y)
{
  for (int cx = 0; cx < 4; cx++)
  {
    m.set(cx + x, y + 0, true);
    m.set(cx + x, y + 7, true);
  }

  for (int cy = 0; cy < 8; cy++)
  {
    m.set(0 + x, y + cy, true);
    m.set(4 + x, y + cy, true);
  }

  return 5;
}