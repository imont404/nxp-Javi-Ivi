#ifndef ST7789_H_
#define ST7789_H_

#include <stdint.h>

#define PORTRAIT 1
#define LANDSCAPE 2

#ifndef CONFIG_DISPLAY_ORIENTATION
#define CONFIG_DISPLAY_ORIENTATION LANDSCAPE
#endif

#define LITTLE_ENDIAN 1
#define BIG_ENDIAN 2

#ifndef CONFIG_DISPLAY_ENDIANESS
#define CONFIG_DISPLAY_ENDIANESS LITTLE_ENDIAN
#endif

void st7789__init(void);
void st7789__set_window(uint32_t x0, uint32_t x1, uint32_t y0, uint32_t y1);
void st7789__begin_pixels(void);

#endif /* ST7789_H_ */
