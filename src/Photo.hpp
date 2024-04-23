#pragma once

#include "Commands.hpp"
#include "Image.hpp"

#include <string>

namespace ayin {
class Photo {
  public:
	Image *image = nullptr;
	Image *origImage = nullptr;
	std::string filename{};
	std::string filepath{};
	float x = 0.0f, y = 0.0f, zoom = 1.0f;

	Photo() = default;
	void reset();
	void soft_reset();
	void push_change(Commands::Info info);
	void undo_change();
	bool can_undo_change();
	void redo_change();
	bool can_redo_change();

  private:
	std::vector<Commands::Info> m_undoStack{};
	int m_undoPos = 0;
};
} // namespace ayin
