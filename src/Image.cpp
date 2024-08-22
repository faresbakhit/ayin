#include "Image.hpp"

#include <cstring>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <SDL2/SDL_opengl.h>
#include <stb_image_write.h>

using namespace ayin;

Image::Image(int width, int height, int channels) : width(width), height(height), channels(channels) {
	data = (unsigned char *)malloc(width * height * channels);
}

Image::Image(const Image &image) : Image(image.width, image.height, image.channels) {
	memcpy(data, image.data, width * height * channels);
}

Image::~Image() {
#ifndef NDEBUG
	static int i = 0;
	printf("[DEBUG] %d Image::~Image()\n", ++i);
#endif
	stbi_image_free(data);
	glDeleteTextures(1, &texture);
}

bool Image::load(const char *filename) {
	if (data != nullptr) {
		stbi_image_free(data);
	}
	if (texture) {
		glDeleteTextures(1, &texture);
	}
	data = stbi_load(filename, &width, &height, &channels, 0);
	return data != nullptr;
}

void Image::clear() {
	std::memset(data, 0, width * height * channels);
}

bool Image::save(const char *filename) {
	const char *extension = strrchr(filename, '.');

	if (!extension) {
		return false;
	}

	if (strcmp(extension, ".png") == 0) {
		return stbi_write_png(filename, width, height, STBI_rgb, data, 0);
	} else if (strcmp(extension, ".bmp") == 0) {
		return stbi_write_bmp(filename, width, height, STBI_rgb, data);
	} else if (strcmp(extension, ".tga") == 0) {
		return stbi_write_tga(filename, width, height, STBI_rgb, data);
	} else if (strcmp(extension, ".jpg") == 0 || strcmp(extension, ".jpeg") == 0) {
		return stbi_write_jpg(filename, width, height, STBI_rgb, data, 90);
	}

	return false;
}

void Image::load_texture() {
	glDeleteTextures(1, &texture);

	GLint format = channels == 3 ? GL_RGB : GL_RGBA;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
}

void Image::update_texture() const {
	GLint format = channels == 3 ? GL_RGB : GL_RGBA;
	glBindTexture(GL_TEXTURE_2D, texture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);
}

unsigned char &Image::operator()(int x, int y, int c) { return data[(y * width + x) * channels + c]; }

const unsigned char &Image::operator()(int x, int y, int c) const { return data[(y * width + x) * channels + c]; }
