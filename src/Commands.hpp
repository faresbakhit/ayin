#pragma once

#include "Image.hpp"

#include <functional>

#include <imgui.h>

namespace ayin::Commands {
enum Type {
	Type_Grayscale,
	Type_BlackAndWhite,
	Type_Invert,
	Type_Merge,
	Type_FlipHorizontally,
	Type_FlipVertically,
	Type_Rotate,
	Type_DarkenAndLighten,
	Type_Crop,
	Type_Frame,
	Type_DetectEdges,
	Type_Resize,
	Type_Blur,
	Type_Sunlight,
	Type_OilPaint,
	Type_Purple,
	Type_Infrared,
	Type_Skew,
	Type_Glasses3D,
	Type_MotionBlur,
	Type_Emboss,
};

struct Info {
	Type ty;
	union {
		int darkenlighten_factor;
		struct {
			int crop_x, crop_y, crop_width, crop_height;
		};
		struct {
			int frame_fanciness;
			unsigned int frame_color;
		};
		struct {
			int resize_width, resize_height;
		};
		int blur_level;
		const char *merge_image;
		int skew_angle;
	};
	Info() = default;
	Info(Type ty);
	Info(Type ty, int);
	Info(Type ty, int frame_fanciness, unsigned int frame_color);
	Info(Type ty, int resize_width, int resize_height);
	Info(Type ty, const char *merge_image);
	Info(Type ty, int crop_x, int crop_y, int crop_width, int crop_height);
};

class Base {
public:
	bool done = false;
	Image *image = nullptr;
	Image *tmpImage = nullptr;

	virtual ~Base();
	virtual void setImage(Image &image) = 0;
	virtual bool hasOptionsMenu() { return false; };
	virtual void showOptionsMenu(){};
	virtual Info getInfo() = 0;
};

class Grayscale : public Base {
public:
	void setImage(Image &) override;
	Info getInfo() override;
};

class BlackAndWhite : public Base {
public:
	void setImage(Image &) override;
	Info getInfo() override;
};

class Invert : public Base {
public:
	void setImage(Image &) override;
	Info getInfo() override;
};

class Merge : public Base {
public:
	void setImage(Image &) override;
	Info getInfo() override;

private:
	const char *m_mergeImageFilename;
};

class FlipHorizontally : public Base {
public:
	void setImage(Image &) override;
	Info getInfo() override;
};

class FlipVertically : public Base {
public:
	void setImage(Image &) override;
	Info getInfo() override;
};

class Rotate : public Base {
public:
	void setImage(Image &) override;
	Info getInfo() override;
};

class DarkenAndLighten : public Base {
public:
	void setImage(Image &) override;
	bool hasOptionsMenu() override;
	void showOptionsMenu() override;
	Info getInfo() override;

private:
	int factor = 100;
};

class Crop : public Base {
public:
	void setImage(Image &) override;
	bool hasOptionsMenu() override;
	void showOptionsMenu() override;
	Info getInfo() override;

private:
	int m_x, m_y, m_width, m_height;
};

class Frame : public Base {
public:
	void setImage(Image &) override;
	bool hasOptionsMenu() override;
	void showOptionsMenu() override;
	Info getInfo() override;

private:
	int fanciness = 1;
	ImVec4 color{1.0f, 1.0f, 1.0f, 1.0f};
};

class DetectEdges : public Base {
public:
	void setImage(Image &) override;
	Info getInfo() override;
};

class Resize : public Base {
public:
	void setImage(Image &) override;
	bool hasOptionsMenu() override;
	void showOptionsMenu() override;
	Info getInfo() override;

private:
	int m_width, m_height;
};

class Blur : public Base {
public:
	void setImage(Image &) override;
	bool hasOptionsMenu() override;
	void showOptionsMenu() override;
	Info getInfo() override;

private:
	int m_blurLevel = 5;
};

class Sunlight : public Base {
public:
	void setImage(Image &) override;
	Info getInfo() override;
};

class OilPaint : public Base {
public:
	void setImage(Image &) override;
	Info getInfo() override;
};

// class CrtTV : public Base {
// public:
// };

class Purple : public Base {
public:
	void setImage(Image &) override;
	Info getInfo() override;
};

class Infrared : public Base {
public:
	void setImage(Image &) override;
	Info getInfo() override;
};

class Skew : public Base {
public:
	void setImage(Image &) override;
	bool hasOptionsMenu() override;
	void showOptionsMenu() override;
	Info getInfo() override;

private:
	int m_skewAngle = 45;
};

class Glasses3D : public Base {
public:
	void setImage(Image &) override;
	bool hasOptionsMenu() override;
	void showOptionsMenu() override;
	Info getInfo() override;

private:
	int intensity;
};

class MotionBlur : public Base {
public:
	void setImage(Image &) override;
	bool hasOptionsMenu() override;
	void showOptionsMenu() override;
	Info getInfo() override;

private:
	int m_blurLevel;
};

class Emboss : public Base {
public:
	void setImage(Image &) override;
	Info getInfo() override;
};

inline extern const char *const names[]{
	"Grayscale",  "Black and White",	"Invert",	 "Merge",  "Flip Horizontally", "Flip Vertically",
	"Rotate",	  "Darken and Lighten", "Crop",		 "Frame",  "Detect Edges",		"Resize",
	"Blur",		  "Sunlight",			"Oil Paint", "Purple", "Infrared",			"Skew",
	"3D Glasses", "Motion Blur",		"Emboss",
};

inline extern const std::function<Base *()> factory[]{
	[]() { return new Grayscale(); }, []() { return new BlackAndWhite(); },	   []() { return new Invert(); },
	[]() { return new Merge(); },	  []() { return new FlipHorizontally(); }, []() { return new FlipVertically(); },
	[]() { return new Rotate(); },	 []() { return new DarkenAndLighten(); }, []() { return new Crop(); },
	[]() { return new Frame(); },	  []() { return new DetectEdges(); },		 []() { return new Resize(); },
	[]() { return new Blur(); },	   []() { return new Sunlight(); },		 []() { return new OilPaint(); },
	[]() { return new Purple(); },	 []() { return new Infrared(); },		   []() { return new Skew(); },
	[]() { return new Glasses3D(); }, []() { return new MotionBlur(); },		  []() { return new Emboss(); },
};

inline extern const int number = std::size(names);
static_assert(std::size(factory) == number);
} // namespace ayin::Commands
