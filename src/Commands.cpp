#include "Commands.hpp"
#include "Application.hpp"
#include "ImageFilter.hpp"

#include <imgui.h>
#include <portable-file-dialogs.hpp>

using namespace ayin::Commands;

Info::Info(Type ty) : ty(ty) {}

Info::Info(Type ty, int darkenlighten_factor) : ty(ty), darkenlighten_factor(darkenlighten_factor) {}

Info::Info(Type ty, int frame_fanciness, unsigned int frame_color)
	: ty(ty), frame_fanciness(frame_fanciness), frame_color(frame_color) {}

Info::Info(Type ty, const char *merge_image) : ty(ty), merge_image(merge_image) {}

Info::Info(Type ty, int crop_x, int crop_y, int crop_width, int crop_height)
	: ty(ty), crop_x(crop_x), crop_y(crop_y), crop_width(crop_width), crop_height(crop_height) {}

Info::Info(Type ty, int resize_width, int resize_height)
	: ty(ty), resize_width(resize_width), resize_height(resize_height) {}

Base::~Base() { delete tmpImage; }

void Grayscale::setImage(Image &image) {
	ImageFilter::Grayscale(image);
	image.update_texture();
	done = true;
}

Info Grayscale::getInfo() { return Info(Type_Grayscale); }

void BlackAndWhite::setImage(Image &image) {
	ImageFilter::BlackAndWhite(image);
	image.update_texture();
	done = true;
}

Info BlackAndWhite::getInfo() { return Info(Type_BlackAndWhite); }

void Invert::setImage(Image &image) {
	ImageFilter::Invert(image);
	image.update_texture();
	done = true;
}

Info Invert::getInfo() { return Info(Type_Invert); }

void Merge::setImage(Image &image) {
	auto selection = pfd::OpenFile("Open", "", pfdImageFile, pfd::Option::multiselect).result();

	if (selection.empty()) {
		done = true;
		return;
	}

	m_mergeImageFilename = selection[0].c_str();
	Image *image2 = new Image();
	image2->load(m_mergeImageFilename);
	ImageFilter::Merge(image, *image2);
	image.update_texture();
	delete image2;
	done = true;
}

Info Merge::getInfo() { return Info(Type_Merge, m_mergeImageFilename); }

void FlipHorizontally::setImage(Image &image) {
	ImageFilter::FlipHorizontally(image);
	image.update_texture();
	done = true;
}

Info FlipHorizontally::getInfo() { return Info(Type_FlipHorizontally); }

void FlipVertically::setImage(Image &image) {
	ImageFilter::FlipVertically(image);
	image.update_texture();
	done = true;
}

Info FlipVertically::getInfo() { return Info(Type_FlipVertically); }

void Rotate::setImage(Image &image) {
	ImageFilter::Rotate(image);
	if (image.width == image.height) {
		image.update_texture();
	} else {
		image.load_texture();
	}
}

Info Rotate::getInfo() { return Info(Type_Rotate); }

void DarkenAndLighten::setImage(Image &image) {
	tmpImage = new Image(image.width, image.height, image.channels);
	memcpy(tmpImage->data, image.data, image.width * image.height * image.channels);
	tmpImage->load_texture();
	this->image = &image;
}

bool DarkenAndLighten::hasOptionsMenu() { return true; }

void DarkenAndLighten::showOptionsMenu() {
	if (ImGui::SliderInt("Brightness", &factor, 0, 200)) {
		memcpy(tmpImage->data, image->data, image->width * image->height * image->channels);
		ImageFilter::ChangeBrightness(*tmpImage, factor);
		tmpImage->update_texture();
	}
	if (ImGui::Button("Apply brightness")) {
		std::swap(image->data, tmpImage->data);
		image->update_texture();
		done = true;
	}
}

Info DarkenAndLighten::getInfo() { return Info(Type_DarkenAndLighten, factor); }

void Crop::setImage(Image &image) {
	tmpImage = new Image(image.width, image.height, image.channels);
	memcpy(tmpImage->data, image.data, image.width * image.height * image.channels);
	tmpImage->load_texture();
	this->image = &image;
	m_width = image.width;
	m_height = image.height;
}

bool Crop::hasOptionsMenu() { return true; }

void Crop::showOptionsMenu() {
	bool update_frame = false;
	if (ImGui::SliderInt("Width", &m_width, 0, image->width - m_x)) {
		update_frame = true;
	}
	if (ImGui::SliderInt("Height", &m_height, 0, image->height - m_y)) {
		update_frame = true;
	}
	if (ImGui::SliderInt("X", &m_x, 0, image->width - m_width)) {
		update_frame = true;
	}
	if (ImGui::SliderInt("Y", &m_y, 0, image->height - m_height)) {
		update_frame = true;
	}
	if (update_frame) {
		unsigned char color[3] = {255, 0, 0};
		memcpy(tmpImage->data, image->data, image->width * image->height * image->channels);
		ImageFilter::DrawRectangle(*tmpImage, m_x, m_y, m_width, m_height, 10, color);
		tmpImage->update_texture();
	}
	if (ImGui::Button("Apply crop")) {
		ImageFilter::Crop(*image, m_x, m_y, m_width, m_height);
		image->load_texture();
		done = true;
	}
}

Info Crop::getInfo() { return Info(Type_Crop, m_x, m_y, m_width, m_height); }

void Frame::setImage(Image &image) {
	tmpImage = new Image(image.width, image.height, image.channels);
	memcpy(tmpImage->data, image.data, image.width * image.height * image.channels);
	ImU32 pcolor = ImGui::ColorConvertFloat4ToU32(color);
	ImageFilter::Frame(*tmpImage, fanciness, pcolor);
	tmpImage->load_texture();
	this->image = &image;
}

bool Frame::hasOptionsMenu() { return true; }

void Frame::showOptionsMenu() {
	bool update_frame = false;
	if (ImGui::RadioButton("Simple", &fanciness, 1)) {
		update_frame = true;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Fancy", &fanciness, 2)) {
		update_frame = true;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Very Fancy", &fanciness, 3)) {
		update_frame = true;
	}
	if (ImGui::ColorPicker3("Frame Color", (float *)&color, ImGuiColorEditFlags_DisplayRGB)) {
		update_frame = true;
	}
	if (ImGui::Button("Apply frame")) {
		std::swap(image->data, tmpImage->data);
		image->update_texture();
		done = true;
	}
	if (update_frame) {
		memcpy(tmpImage->data, image->data, image->width * image->height * image->channels);
		ImU32 pcolor = ImGui::ColorConvertFloat4ToU32(color);
		ImageFilter::Frame(*tmpImage, fanciness, pcolor);
		tmpImage->load_texture();
	}
}

Info Frame::getInfo() {
	ImU32 pcolor = ImGui::ColorConvertFloat4ToU32(color);
	return Info(Type_Frame, fanciness, pcolor);
}

void DetectEdges::setImage(Image &image) {
	ImageFilter::DetectEdges(image);
	image.update_texture();
	done = true;
}

Info DetectEdges::getInfo() { return Info(Type_DetectEdges); }

void Resize::setImage(Image &image) {
	this->image = &image;
	m_width = image.width;
	m_height = image.height;
}

bool Resize::hasOptionsMenu() { return true; }

void Resize::showOptionsMenu() {
	bool update_frame = false;
	if (ImGui::SliderInt("Width", &m_width, 1, image->width * 2, "%d", ImGuiSliderFlags_AlwaysClamp)) {
		update_frame = true;
	}
	if (ImGui::SliderInt("Height", &m_height, 1, image->height * 2, "%d", ImGuiSliderFlags_AlwaysClamp)) {
		update_frame = true;
	}
	if (update_frame) {
		delete tmpImage;
		tmpImage = new Image(image->width, image->height, image->channels);
		memcpy(tmpImage->data, image->data, image->width * image->height * image->channels);
		ImageFilter::Resize(*tmpImage, m_width, m_height);
		tmpImage->load_texture();
	}
	if (ImGui::Button("Apply resize")) {
		image->width = tmpImage->width;
		image->height = tmpImage->height;
		std::swap(image->data, tmpImage->data);
		std::swap(image->texture, tmpImage->texture);
		done = true;
	}
}

Info Resize::getInfo() { return Info(Type_Resize, m_width, m_height); }

void Blur::setImage(Image &image) {
	tmpImage = new Image(image.width, image.height, image.channels);
	memcpy(tmpImage->data, image.data, image.width * image.height * image.channels);
	ImageFilter::Blur(*tmpImage, m_blurLevel);
	tmpImage->load_texture();
	this->image = &image;
}

bool Blur::hasOptionsMenu() { return true; }

void Blur::showOptionsMenu() {
	if (ImGui::SliderInt("Blur Level", &m_blurLevel, 1, 10)) {
		memcpy(tmpImage->data, image->data, image->width * image->height * image->channels);
		ImageFilter::Blur(*tmpImage, m_blurLevel);
		tmpImage->update_texture();
	}
	if (ImGui::Button("Apply blur")) {
		std::swap(image->data, tmpImage->data);
		image->update_texture();
		done = true;
	}
}

Info Blur::getInfo() { return Info(Type_Blur, m_blurLevel); }

void Sunlight::setImage(Image &image) {
	ImageFilter::Sunlight(image);
	image.update_texture();
	done = true;
}

Info Sunlight::getInfo() { return Info(Type_Sunlight); }

void OilPaint::setImage(Image &image) {
	ImageFilter::OilPaint(image);
	image.update_texture();
	done = true;
}

Info OilPaint::getInfo() { return Info(Type_OilPaint); }

void Purple::setImage(Image &image) {
	ImageFilter::Purple(image);
	image.update_texture();
	done = true;
}

Info Purple::getInfo() { return Info(Type_Purple); }

void Infrared::setImage(Image &image) {
	ImageFilter::Infrared(image);
	image.update_texture();
	done = true;
}

Info Infrared::getInfo() { return Info(Type_Infrared); }

void Skew::setImage(Image &image) {
	tmpImage = new Image(image.width, image.height, image.channels);
	memcpy(tmpImage->data, image.data, image.width * image.height * image.channels);
	ImageFilter::Skew(*tmpImage, m_skewAngle);
	tmpImage->load_texture();
	this->image = &image;
}

bool Skew::hasOptionsMenu() { return true; }

void Skew::showOptionsMenu() {
	if (ImGui::SliderInt("Angle", &m_skewAngle, -89, 89, "%d", ImGuiSliderFlags_AlwaysClamp)) {
		delete tmpImage;
		tmpImage = new Image(image->width, image->height, image->channels);
		memcpy(tmpImage->data, image->data, image->width * image->height * image->channels);
		if (m_skewAngle != 0) {
			ImageFilter::Skew(*tmpImage, m_skewAngle);
		}
		tmpImage->load_texture();
	}
	if (ImGui::Button("Apply skew")) {
		std::swap(image->data, tmpImage->data);
		image->width = tmpImage->width;
		image->load_texture();
		done = true;
	}
}

Info Skew::getInfo() { return Info(Type_Skew, m_skewAngle); }

void Glasses3D::setImage(Image &image) {
	tmpImage = new Image(image.width, image.height, image.channels);
	memcpy(tmpImage->data, image.data, image.width * image.height * image.channels);
	intensity = 10;
	ImageFilter::Glasses3D(*tmpImage, intensity);
	tmpImage->load_texture();
	this->image = &image;
}

bool Glasses3D::hasOptionsMenu() { return true; }

void Glasses3D::showOptionsMenu() {
	if (ImGui::SliderInt("Intensity", &intensity, 0, 50)) {
		memcpy(tmpImage->data, image->data, image->width * image->height * image->channels);
		ImageFilter::Glasses3D(*tmpImage, intensity);
		tmpImage->update_texture();
	}
	if (ImGui::Button("Apply effect")) {
		std::swap(image->data, tmpImage->data);
		image->update_texture();
		done = true;
	}
}

Info Glasses3D::getInfo() { return Info(Type_Glasses3D, intensity); }

void MotionBlur::setImage(Image &image) {
	tmpImage = new Image(image.width, image.height, image.channels);
	memcpy(tmpImage->data, image.data, image.width * image.height * image.channels);
	m_blurLevel = 9;
	ImageFilter::MotionBlur(*tmpImage, m_blurLevel);
	tmpImage->load_texture();
	this->image = &image;
}

bool MotionBlur::hasOptionsMenu() { return true; }

void MotionBlur::showOptionsMenu() {
	if (ImGui::SliderInt("Blur Level", &m_blurLevel, 1, 21)) {
		memcpy(tmpImage->data, image->data, image->width * image->height * image->channels);
		ImageFilter::MotionBlur(*tmpImage, m_blurLevel);
		tmpImage->update_texture();
	}
	if (ImGui::Button("Apply blur")) {
		std::swap(image->data, tmpImage->data);
		image->update_texture();
		done = true;
	}
}

Info MotionBlur::getInfo() { return Info(Type_MotionBlur, m_blurLevel); }

void Emboss::setImage(Image &image) {
	ImageFilter::Emboss(image);
	image.update_texture();
	done = true;
}

Info Emboss::getInfo() { return Info(Type_Emboss); }
