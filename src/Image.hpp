#pragma once

#include <vector>

namespace ayin {
class Image {
  public:
	int width = 0;
	int height = 0;
	int channels = 0;
	unsigned char *data = nullptr;
	unsigned int texture = 0;

	Image() = default;
	Image(int width, int height, int channels);
	~Image();

	bool load(const char *filename);
	bool save(const char *filename);

	void load_texture();
	void update_texture() const;

	unsigned char &operator()(int x, int y, int c);
	const unsigned char &operator()(int x, int y, int c) const;
};
} // namespace ayin
