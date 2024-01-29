#include "draw.h"
#include "picsimlab.h"

// Draw Functions

void DrawSlider(void (*CanvasCmd)(CanvasCmd_t), const output_t* output, const unsigned char pos, const std::string val,
                const int FontPointSize) {
    float dy = pos / 1.66;
    (*CanvasCmd)({CC_SETFGCOLOR, .SetFgColor{255, 255, 255}});
    (*CanvasCmd)({CC_SETBGCOLOR, .SetBgColor{89, 89, 89}});
    (*CanvasCmd)(
        {CC_RECTANGLE, .Rectangle{1, output->x1, output->y1, output->x2 - output->x1, output->y2 - output->y1}});
    (*CanvasCmd)({CC_SETCOLOR, .SetColor{0, 0, 200}});
    (*CanvasCmd)({CC_RECTANGLE, .Rectangle{1, output->x1 + 1, output->y1 + 1 + dy, output->x2 - output->x1 - 2,
                                           output->y2 - output->y1 - 2 - dy}});

    (*CanvasCmd)({CC_SETFGCOLOR, .SetFgColor{0, 0, 0}});
    (*CanvasCmd)({CC_SETBGCOLOR, .SetBgColor{46, 46, 46}});
    (*CanvasCmd)({CC_RECTANGLE, .Rectangle{1, output->x1, output->y1 + pos / 1.66f, 32, 19}});
    (*CanvasCmd)({CC_SETCOLOR, .SetColor{250, 250, 250}});
    (*CanvasCmd)({CC_SETFONTSIZE, .SetFontSize{FontPointSize}});
    (*CanvasCmd)({CC_ROTATEDTEXT, .RotatedText{val.c_str(), output->x1 + 1, output->y1 + 5 + pos / 1.66f, 0}});
}

void DrawPotentiometer(void (*CanvasCmd)(CanvasCmd_t), const output_t* output, const unsigned char pos,
                       const std::string val, const int FontPointSize) {
    (*CanvasCmd)({CC_SETCOLOR, .SetColor{179, 179, 179}});
    (*CanvasCmd)(
        {CC_RECTANGLE, .Rectangle{1, output->x1, output->y1, output->x2 - output->x1, output->y2 - output->y1}});
    (*CanvasCmd)({CC_SETFGCOLOR, .SetFgColor{0, 0, 0}});
    (*CanvasCmd)({CC_SETBGCOLOR, .SetBgColor{96, 96, 96}});
    (*CanvasCmd)({CC_RECTANGLE, .Rectangle{1, output->x1 + 9, output->y1 + 9, output->x2 - output->x1 - 18,
                                           output->y2 - output->y1 - 18}});
    (*CanvasCmd)({CC_SETBGCOLOR, .SetBgColor{46, 46, 46}});
    (*CanvasCmd)({CC_RECTANGLE, .Rectangle{1, output->x1, output->y1 + (200 - pos) / 1.66f, 32, 19}});
    (*CanvasCmd)({CC_SETCOLOR, .SetColor{250, 250, 250}});
    (*CanvasCmd)({CC_SETFONTSIZE, .SetFontSize{FontPointSize}});
    (*CanvasCmd)({CC_ROTATEDTEXT, .RotatedText{val.c_str(), output->x1 + 4, output->y1 + 5 + (200 - pos) / 1.66f, 0}});
}

void DrawLED(void (*CanvasCmd)(CanvasCmd_t), const output_t* output) {
    unsigned int r1, g1, b1;
    (*CanvasCmd)({CC_GETBGCOLOR, .GetBgColor{&r1, &g1, &b1}});

    unsigned int r2 = r1 - 120;
    unsigned int g2 = g1 - 120;
    unsigned int b2 = b1 - 120;
    if (r2 > 255)
        r2 = 0;
    if (g2 > 255)
        g2 = 0;
    if (b2 > 255)
        b2 = 0;
    (*CanvasCmd)({CC_SETBGCOLOR, .SetBgColor{r2, g2, b2}});
    (*CanvasCmd)({CC_CIRCLE, .Circle{1, output->x1, output->y1, output->r + 1}});
    (*CanvasCmd)({CC_SETBGCOLOR, .SetBgColor{r1, g1, b1}});
    (*CanvasCmd)({CC_CIRCLE, .Circle{1, output->x1, output->y1, output->r - 2}});
}