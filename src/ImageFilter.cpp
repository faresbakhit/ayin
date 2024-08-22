#include <algorithm>
#include <cstring>
#define _USE_MATH_DEFINES
#include <cmath>

#include "ImageFilter.hpp"

using namespace ayin;

void ImageFilter::Grayscale(Image &image) {
	for (int i = 0; i < image.width; i++) {
		for (int j = 0; j < image.height; j++) {
			int avg = (image(i, j, 0) + image(i, j, 1) + image(i, j, 2)) / 3;
			for (int k = 0; k < 3; k++) {
				image(i, j, k) = avg;
			}
		}
	}
}

void ImageFilter::BlackAndWhite(Image &image) {
	for (int i = 0; i < image.width; ++i) {
		for (int j = 0; j < image.height; ++j) {
			int r = image(i, j, 0);
			int g = image(i, j, 1);
			int b = image(i, j, 2);
			int grayScale = (r + g + b) / 3;
			if (grayScale > 128) {
				image(i, j, 0) = 255;
				image(i, j, 1) = 255;
				image(i, j, 2) = 255;
			} else {
				image(i, j, 0) = 0;
				image(i, j, 1) = 0;
				image(i, j, 2) = 0;
			}
		}
	}
}

void ImageFilter::Invert(Image &image) {
	for (int i = 0; i < image.width; ++i) {
		for (int j = 0; j < image.height; ++j) {
			for (int k = 0; k < 3; ++k) {
				image(i, j, k) = 0xFF - image(i, j, k);
			}
		}
	}
}

void ImageFilter::Rotate(Image &image) {
	Image flipped_image(image.height, image.width, image.channels);
	for (int i = 0; i < image.width; ++i) {
		for (int j = 0; j < image.height; ++j) {
			for (int k = 0; k < image.channels; ++k) {
				flipped_image(image.height - j, i, k) = image(i, j, k);
			}
		}
	}
	std::swap(image.data, flipped_image.data);
	std::swap(image.width, image.height);
}

void ImageFilter::DrawRectangle(Image &image, int x, int y, int width, int height, int thickness,
								unsigned char *color) {
	for (int i = 0; i < width; ++i) {
		for (int j = 0; j < thickness; ++j) {
			for (int k = 0; k < 3; ++k) {
				image(x + i, y + j, k) = color[k];
				image(x + i, y + j + height - thickness, k) = color[k];
			}
		}
	}
	for (int i = 0; i < thickness; ++i) {
		for (int j = thickness; j < (height - thickness); ++j) {
			for (int k = 0; k < 3; ++k) {
				image(x + i, y + j, k) = color[k];
				image(x + i + width - thickness, y + j, k) = color[k];
			}
		}
	}
}

static void DrawFilledRectangle(Image &image, int x, int y, int width, int height, unsigned char *color) {
	for (int i = 0; i < width; ++i) {
		for (int j = 0; j < height; ++j) {
			for (int k = 0; k < 3; ++k) {
				image(x + i, y + j, k) = color[k];
			}
		}
	}
}

void ImageFilter::Frame(Image &image, int fanciness, unsigned int pcolor) {
	unsigned char color[3] = {
		(unsigned char)((pcolor >> 0) & 0xFF),
		(unsigned char)((pcolor >> 8) & 0xFF),
		(unsigned char)((pcolor >> 16) & 0xFF),
	};
	unsigned char white[3] = {255, 255, 255};
	int outer_frame_box_width = image.height / 32;		  // px
	int outer_frame_thickness = image.height / 64;		  // px
	int inner_frame_thickness = image.height / 96;		  // px
	int inner_inner_frame_thickness = image.height / 128; // px

	DrawRectangle(image, 0, 0,				 // x, y
				  image.width, image.height, // width, height
				  outer_frame_thickness,	 // thickness
				  color);

	if (fanciness <= 1) {
		return;
	}

	DrawRectangle(image, outer_frame_thickness, outer_frame_thickness, // x, y
				  image.width - 2 * outer_frame_thickness,			   // width
				  image.height - 2 * outer_frame_thickness,			   // height
				  inner_frame_thickness,							   // thickness
				  white);

	DrawRectangle(image, outer_frame_thickness + 2 * inner_frame_thickness,
				  outer_frame_thickness + 2 * inner_frame_thickness,					// x, y
				  image.width - 2 * outer_frame_thickness - 4 * inner_frame_thickness,	// width
				  image.height - 2 * outer_frame_thickness - 4 * inner_frame_thickness, // height
				  inner_inner_frame_thickness,											// thickness
				  white);

	if (fanciness <= 2) {
		return;
	}

	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < 2; ++j) {
			DrawFilledRectangle(image,
								(1 - 2 * i) * (outer_frame_thickness + 2 * inner_frame_thickness) +
									i * (image.width - outer_frame_box_width), // x
								(1 - 2 * j) * (outer_frame_thickness + 2 * inner_frame_thickness) +
									j * (image.height - outer_frame_box_width), // y
								outer_frame_box_width,
								outer_frame_box_width, // width, height
								white);
			DrawRectangle(image,
						  (1 - 2 * i) * (outer_frame_thickness + 2 * inner_frame_thickness) +
							  i * (image.width - 2.4 * outer_frame_box_width), // x
						  (1 - 2 * j) * (outer_frame_thickness + 2 * inner_frame_thickness) +
							  j * (image.height - 2.4 * outer_frame_box_width), // y
						  2.4 * outer_frame_box_width,							// width
						  2.4 * outer_frame_box_width,							// height
						  inner_frame_thickness,								// thickness
						  white);
			DrawRectangle(image,
						  (1 - 2 * i) * (outer_frame_thickness + 2 * inner_frame_thickness) +
							  i * (image.width - 1.7 * outer_frame_box_width), // x
						  (1 - 2 * j) * (outer_frame_thickness + 2 * inner_frame_thickness) +
							  j * (image.height - 1.7 * outer_frame_box_width), // y
						  1.7 * outer_frame_box_width,							// width
						  1.7 * outer_frame_box_width,							// height
						  inner_inner_frame_thickness / 2,						// thickness
						  white);
			DrawFilledRectangle(
				image,
				i * ((1 - 2 * j) * outer_frame_thickness + j * (image.width - inner_frame_thickness)),	   // x
				!i * ((1 - 2 * j) * (outer_frame_thickness) + j * (image.height - inner_frame_thickness)), // y
				i * inner_frame_thickness + !i * image.width,											   // width
				i * image.height + !i * inner_frame_thickness,											   // height
				white);
		}
	}
}

void ImageFilter::FlipHorizontally(Image &image) {
	int width = image.width;
	int middle = width / 2;
	bool isEven = width % 2 == 0;
	int end = isEven ? middle - 2 : middle - 1;
	for (int i = 0; i < end; i++) {
		for (int j = 0; j < image.height; j++) {
			for (int c = 0; c < 3; c++) {
				int temp = image(i, j, c);
				image(i, j, c) = image(width - i - 1, j, c);
				image(width - i - 1, j, c) = temp;
			}
		}
	}
	if (isEven) {
		for (int j = 0; j < image.height; j++) {
			for (int c = 0; c < 3; c++) {
				int temp = image(middle, j, c);
				image(middle, j, c) = image(middle - 1, j, c);
				image(middle - 1, j, c) = temp;
			}
		}
	}
}

void ImageFilter::FlipVertically(Image &image) {
	int height = image.height;
	int middle = height / 2;
	bool isEven = height % 2 == 0;
	int end = isEven ? middle - 2 : middle - 1;
	for (int j = 0; j < end; j++) {
		for (int i = 0; i < image.width; ++i) {
			for (int c = 0; c < 3; c++) {
				int temp = image(i, j, c);
				image(i, j, c) = image(i, height - j - 1, c);
				image(i, height - j - 1, c) = temp;
			}
		}
	}
	if (isEven) {
		for (int i = 0; i < image.width; ++i) {
			for (int c = 0; c < 3; c++) {
				int temp = image(i, middle, c);
				image(i, middle, c) = image(i, middle - 1, c);
				image(i, middle - 1, c) = temp;
			}
		}
	}
}

void ImageFilter::Crop(Image &image, int x, int y, int width, int height) {
	Image cropped_image(width, height, image.channels);
	for (int i = x; i < x + width; i++) {
		for (int j = y; j < y + height; j++) {
			for (int k = 0; k < image.channels; k++) {
				cropped_image(i - x, j - y, k) = image(i, j, k);
			}
		}
	}
	std::swap(image.data, cropped_image.data);
	image.width = width;
	image.height = height;
}

void ImageFilter::Resize(Image &image, int w, int h) {
	Image result(w, h, image.channels);
	bool w_bigger = w > image.width;
	bool h_bigger = h > image.height;
	float ratio_w = w_bigger ? (float)image.width / (float)w : (float)w / (float)image.width;
	float ratio_h = h_bigger ? (float)image.height / (float)h : (float)h / (float)image.height;
	float end_w = w_bigger ? w : image.width;
	float end_h = h_bigger ? h : image.height;
	for (int i = 0; i < end_w; i++) {
		for (int j = 0; j < end_h; j++) {
			for (int c = 0; c < 3; c++) {
				result(w_bigger ? i : (int)floor(i * ratio_w), h_bigger ? j : (int)floor(j * ratio_h), c) =
					image(w_bigger ? (int)floor(i * ratio_w) : i, h_bigger ? (int)floor(j * ratio_h) : j, c);
			}
		}
	}
	std::swap(image.data, result.data);
	image.width = w;
	image.height = h;
}

void ImageFilter::Merge(Image &image1, Image &image2) {
	int minWidth = std::min(image1.width, image2.width);
	int minHeight = std::min(image1.height, image2.height);
	for (int i = 0; i < minWidth; i++) {
		for (int j = 0; j < minHeight; j++) {
			image1(i, j, 0) = (image1(i, j, 0) + image2(i, j, 0)) / 2;
			image1(i, j, 1) = (image1(i, j, 1) + image2(i, j, 1)) / 2;
			image1(i, j, 2) = (image1(i, j, 2) + image2(i, j, 2)) / 2;
		}
	}
}

void ImageFilter::ChangeBrightness(Image &image, int factor) {
	float m = factor / 100.0f;
	for (int i = 0; i < image.width; i++) {
		for (int j = 0; j < image.height; j++) {
			for (int k = 0; k < 3; k++) {
				image(i, j, k) = std::clamp((int)(image(i, j, k) * m), 0, 255);
			}
		}
	}
}

void ImageFilter::DetectEdges(Image &image) {
	int sobel_x[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
	int sobel_y[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
	Image edges(image.width, image.height, image.channels);
	for (int y = 1; y < image.height - 1; y++) {
		for (int x = 1; x < image.width - 1; x++) {
			int x_gradient = 0, y_gradient = 0;
			for (int j = -1; j <= 1; j++) {
				for (int i = -1; i <= 1; i++) {
					x_gradient += sobel_x[j + 1][i + 1] * image(x + i, y + j, 0);
					y_gradient += sobel_y[j + 1][i + 1] * image(x + i, y + j, 0);
				}
			}
			int magnitude = static_cast<int>(sqrt(x_gradient * x_gradient + y_gradient * y_gradient));
			magnitude = std::max(0, std::min(255, magnitude));
			for (int c = 0; c < 3; ++c) {
				edges(x, y, c) = magnitude;
			}
		}
	}
	for (int y = 0; y < image.height; y++) {
		for (int x = 0; x < image.width; x++) {
			for (int c = 0; c < 3; c++) {
				image(x, y, c) = edges(x, y, c);
			}
		}
	}
}

void ImageFilter::Blur(Image &image, int level) {
	Image blurred_image(image.width, image.height, image.channels);
	int n = pow(2 * level + 1, 2);
	int sum = 0;
	for (int i = level; i < image.width - level; ++i) {
		for (int j = level; j < image.height - level; ++j) {
			for (int k = 0; k < image.channels; ++k) {
				for (int m = -level; m <= level; ++m) {
					for (int n = -level; n <= level; ++n) {
						sum += image(i + m, j + n, k);
					}
				}
				blurred_image(i, j, k) = sum / n;
				sum = 0;
			}
		}
	}
	std::swap(image.data, blurred_image.data);
}

void ImageFilter::Sunlight(Image &image) {
	for (int i = 0; i < image.width; i++) {
		for (int j = 0; j < image.height; j++) {
			image(i, j, 2) /= 2;
		}
	}
}

void ImageFilter::OilPaint(Image &image) {
	int intensityCount[AYIN_IMAGEFILTER_OILPAINT_INTENSITY]{}, averageR[AYIN_IMAGEFILTER_OILPAINT_INTENSITY]{},
		averageG[AYIN_IMAGEFILTER_OILPAINT_INTENSITY]{}, averageB[AYIN_IMAGEFILTER_OILPAINT_INTENSITY]{};
	Image oil_image(image.width, image.height, image.channels);
	for (int i = AYIN_IMAGEFILTER_OILPAINT_RADIUS; i < image.width - AYIN_IMAGEFILTER_OILPAINT_RADIUS; ++i) {
		for (int j = AYIN_IMAGEFILTER_OILPAINT_RADIUS; j < image.height - AYIN_IMAGEFILTER_OILPAINT_RADIUS; ++j) {
			for (int m = -AYIN_IMAGEFILTER_OILPAINT_RADIUS; m <= AYIN_IMAGEFILTER_OILPAINT_RADIUS; ++m) {
				for (int n = -AYIN_IMAGEFILTER_OILPAINT_RADIUS; n <= AYIN_IMAGEFILTER_OILPAINT_RADIUS; ++n) {
					int r = image(i + m, j + n, 0), g = image(i + m, j + n, 1), b = image(i + m, j + n, 2);
					int curIntensity = (int)((((double)(r + g + b) / 3) * AYIN_IMAGEFILTER_OILPAINT_INTENSITY) / 255.0);
					intensityCount[curIntensity]++;
					averageR[curIntensity] += r;
					averageG[curIntensity] += g;
					averageB[curIntensity] += b;
				}
			}
			int curMax = 0;
			int maxIndex = 0;
			for (int t = 0; t < AYIN_IMAGEFILTER_OILPAINT_INTENSITY; t++) {
				if (intensityCount[t] > curMax) {
					curMax = intensityCount[t];
					maxIndex = t;
				}
			}
			oil_image(i, j, 0) = averageR[maxIndex] / curMax;
			oil_image(i, j, 1) = averageG[maxIndex] / curMax;
			oil_image(i, j, 2) = averageB[maxIndex] / curMax;
			std::memset(intensityCount, 0, sizeof(intensityCount));
			std::memset(averageR, 0, sizeof(averageR));
			std::memset(averageG, 0, sizeof(averageG));
			std::memset(averageB, 0, sizeof(averageB));
		}
	}
	std::swap(image.data, oil_image.data);
}

void ImageFilter::Purple(Image &image) {
	for (int i = 0; i < image.width; ++i) {
		for (int j = 0; j < image.height; ++j) {
			image(i, j, 0) = std::clamp((int)((float)image(i, j, 0) * 1.25f), 0, 255);
			image(i, j, 1) = (int)((float)image(i, j, 1) * 0.5f);
			image(i, j, 2) = std::clamp((int)((float)image(i, j, 2) * 1.25f), 0, 255);
		}
	}
}

void ImageFilter::Infrared(Image &image) {
	for (int i = 0; i < image.width; ++i) {
		for (int j = 0; j < image.height; ++j) {
			int r = image(i, j, 0);
			image(i, j, 0) = 255;
			image(i, j, 1) = 255 - r;
			image(i, j, 2) = 255 - r;
		}
	}
}

void ImageFilter::Skew(Image &image, int angle) {
	bool neg = angle < 0;
	double tangent = std::tan(std::abs(angle) * M_PI / 180.0);
	Image skew_image(image.width + image.height * tangent, image.height, image.channels);

	skew_image.clear();
	for (int i = 0; i < image.width; i++) {
		for (int j = 0; j < image.height; j++) {
			for (int k = 0; k < 3; k++) {
				skew_image(i + (neg * image.height + (1 - 2 * neg) * j) * tangent, j, k) = image(i, j, k);
			}
		}
	}

	std::swap(image.data, skew_image.data);
	image.width = skew_image.width;
}

void ImageFilter::Glasses3D(Image &image, int intensity) {
	Image helper_image(image.width, image.height, image.channels);
	for (int i = intensity; i < image.width - intensity - 1; i++) {
		for (int j = 0; j < image.height; j++) {
			helper_image(i - intensity, j, 0) = image(i, j, 0);
			helper_image(i + intensity, j, 2) = image(i, j, 2);
		}
	}
	for (int i = intensity; i < image.width - intensity - 1; i++) {
		for (int j = 0; j < image.height; j++) {
			image(i, j, 0) = (image(i, j, 0) + helper_image(i, j, 0)) / 2;
			image(i, j, 2) = (image(i, j, 2) + helper_image(i, j, 2)) / 2;
		}
	}
}

void ImageFilter::MotionBlur(Image &image, int level) {
	Image blurred_image(image.width, image.height, image.channels);
	int n = 2 * level + 1;
	int sum = 0;
	for (int i = level; i < image.width - level; ++i) {
		for (int j = level; j < image.height - level; ++j) {
			for (int k = 0; k < image.channels; ++k) {
				for (int m = -level; m <= level; ++m) {
					sum += image(i + m, j + m, k);
				}
				blurred_image(i, j, k) = sum / n;
				sum = 0;
			}
		}
	}
	std::swap(image.data, blurred_image.data);
}

void ImageFilter::Emboss(Image &image) {
	Image emboss_image(image.width, image.height, image.channels);
	int sum = 0;
	for (int i = 3; i < image.width - 3; ++i) {
		for (int j = 3; j < image.height - 3; ++j) {
			for (int k = 0; k < image.channels; ++k) {
				sum = image(i, j - 1, k) * -1 + 128 + image(i - 1, j - 1, k) * -1 + 128 + image(i - 1, j, k) * -1 +
					  128 + image(i, j + 1, k) + 128 + image(i + 1, j + 1, k) + 128 + image(i + 1, j, k) + 128;
				emboss_image(i, j, k) = std::clamp(sum / 6, 0, 255);
			}
		}
	}
	std::swap(image.data, emboss_image.data);
}
