#pragma once

#include <Image.hpp>

#ifndef BPS_IMAGEFILTER_OILPAINT_RADIUS
#define BPS_IMAGEFILTER_OILPAINT_RADIUS 5
#endif

#ifndef BPS_IMAGEFILTER_OILPAINT_INTENSITY
#define BPS_IMAGEFILTER_OILPAINT_INTENSITY 20
#endif

namespace bps::ImageFilter {
void Grayscale(Image& image);
void BlackAndWhite(Image& image);
void Invert(Image& image);
void Merge(Image& image1, Image& image2);
void FlipHorizontally(Image& image);
void FlipVertically(Image& image);
void Rotate(Image& image);
void DrawRectangle(Image& image, int x, int y, int width, int height, int thickness, unsigned char *color);
void Frame(Image& image, int fanciness, unsigned int color);
void Crop(Image& image, int x, int y, int w, int h);
void Resize(Image& image, int w, int h);
void ChangeBrightness(Image& image, int factor);
void DetectEdges(Image& image);
void Blur(Image& image, int level);
void Sunlight(Image& image);
void OilPaint(Image& image);
void Purple(Image& image);
void Infrared(Image& image);
void Skew(Image& image, int degree);
void Glasses3D(Image& image, int intensity);
void MotionBlur(Image& image, int level);
void Emboss(Image& image);
} // namespace bps::ImageFilter
