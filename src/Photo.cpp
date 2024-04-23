#include "Photo.hpp"
#include "ImageFilter.hpp"

using namespace ayin;

static void doCommand(Image &image, Commands::Info cmd) {
	switch (cmd.ty) {
	case Commands::Type_Grayscale:
		ImageFilter::Grayscale(image);
		break;
	case Commands::Type_BlackAndWhite:
		ImageFilter::BlackAndWhite(image);
		break;
	case Commands::Type_Invert:
		ImageFilter::Invert(image);
		break;
	case Commands::Type_Merge: {
		Image *image2 = new Image();
		image2->load(cmd.merge_image);
		ImageFilter::Merge(image, *image2);
		delete image2;
		break;
	}
	case Commands::Type_FlipHorizontally:
		ImageFilter::FlipHorizontally(image);
		break;
	case Commands::Type_FlipVertically:
		ImageFilter::FlipVertically(image);
		break;
	case Commands::Type_Rotate:
		ImageFilter::Rotate(image);
		break;
	case Commands::Type_DarkenAndLighten:
		ImageFilter::ChangeBrightness(image, cmd.darkenlighten_factor);
		break;
	case Commands::Type_Crop:
		ImageFilter::Crop(image, cmd.crop_x, cmd.crop_y, cmd.crop_width,
						  cmd.crop_height);
		break;
	case Commands::Type_Frame:
		ImageFilter::Frame(image, cmd.frame_fanciness, cmd.frame_color);
		break;
	case Commands::Type_DetectEdges:
		ImageFilter::DetectEdges(image);
		break;
	case Commands::Type_Resize:
		ImageFilter::Resize(image, cmd.resize_width, cmd.resize_height);
		break;
	case Commands::Type_Blur:
		ImageFilter::Blur(image, cmd.blur_level);
		break;
	case Commands::Type_Sunlight:
		ImageFilter::Sunlight(image);
		break;
	case Commands::Type_OilPaint:
		ImageFilter::OilPaint(image);
		break;
	case Commands::Type_Purple:
		ImageFilter::Purple(image);
		break;
	case Commands::Type_Infrared:
		ImageFilter::Infrared(image);
		break;
	case Commands::Type_Skew:
		ImageFilter::Skew(image, cmd.skew_angle);
		break;
	case Commands::Type_Glasses3D:
		ImageFilter::Glasses3D(image, cmd.darkenlighten_factor);
		break;
	case Commands::Type_MotionBlur:
		ImageFilter::MotionBlur(image, cmd.blur_level);
		break;
	case Commands::Type_Emboss:
		ImageFilter::Emboss(image);
		break;
	}
}

void Photo::reset() {
	bool newDataSize =
		image->width != origImage->width || image->height != origImage->height;
	if (newDataSize) {
		newDataSize = true;
		delete image;
		image =
			new Image(origImage->width, origImage->height, origImage->channels);
	}
	memcpy(image->data, origImage->data,
		   origImage->width * origImage->height * origImage->channels);
	if (newDataSize) {
		image->load_texture();
	} else {
		image->update_texture();
	}
	m_undoPos = 0;
	m_undoStack.clear();
}

void Photo::soft_reset() {
	m_undoPos = 0;
	m_undoStack.clear();
}

void Photo::push_change(Commands::Info info) {
	if (m_undoPos != 0) {
		m_undoStack.resize(m_undoStack.size() - m_undoPos);
		m_undoPos = 0;
	}
	m_undoStack.push_back(info);
}

void Photo::undo_change() {
	++m_undoPos;

	bool newDataSize =
		image->width != origImage->width || image->height != origImage->height;
	if (newDataSize) {
		newDataSize = true;
		delete image;
		image =
			new Image(origImage->width, origImage->height, origImage->channels);
	}

	memcpy(image->data, origImage->data,
		   origImage->width * origImage->height * origImage->channels);

	for (size_t i = 0; i < m_undoStack.size() - m_undoPos; ++i) {
		doCommand(*image, m_undoStack[i]);
	}

	if (newDataSize) {
		image->load_texture();
	} else {
		image->update_texture();
	}
}

bool Photo::can_undo_change() {
	return m_undoPos <= (int)m_undoStack.size() - 1;
}

void Photo::redo_change() {
	--m_undoPos;
	int width = image->width, height = image->height;
	doCommand(*image, m_undoStack[m_undoStack.size() - m_undoPos - 1]);
	if (width == image->width && height == image->height) {
		image->update_texture();
	} else {
		image->load_texture();
	}
}

bool Photo::can_redo_change() { return m_undoPos != 0; }
