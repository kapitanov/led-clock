#include "font.h"

#include "font.h"
#include "FS.h"

using namespace fs;

void font_init()
{
  SPIFFS.begin();
}

const char *font_get_name(font_id id);

int font_draw_glyph(led_matrix_t &m, File &file, int x, int y);
int font_draw_undefined(led_matrix_t &m, int x, int y);

int font_draw(led_matrix_t &m, font_id font, char c, int x, int y)
{
  char filename[64];
  sprintf(filename, "/font/%s/%d.dat\0", font_get_name(font), c);

  if (!SPIFFS.exists(filename))
  {
    Serial.printf("[font] no such file: '%s'\r\n", filename);
    return font_draw_undefined(m, x, y);
  }

  fs::File f = SPIFFS.open(filename, "r");
  int w = font_draw_glyph(m, f, x, y);
  f.close();
  return w;
}

const char *font_get_name(font_id id)
{
  switch (id)
  {
  case FONT_DEFAULT:
    return "default";
  case FONT_MONOSPACE:
    return "monospace";
  case FONT_SPECIAL:
    return "special";
  default:
    return "default";
  }
}

int font_draw_glyph(led_matrix_t &m, File &file, int x, int y)
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

int font_draw_undefined(led_matrix_t &m, int x, int y)
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