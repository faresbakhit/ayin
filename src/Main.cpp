#include "Application.hpp"
#include "Commands.hpp"
#include "Image.hpp"
#include "fonts/MaterialIcons.hpp"

#include <cmath>
#include <filesystem>
#include <vector>

using namespace ayin;

const ImGuiWindowFlags window_flags =
	ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;

const ImGuiTabBarFlags tabbar_flags = ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_FittingPolicyScroll;

int main(int, char **) {
	Commands::Base *cmd = nullptr;
	Application app;
	app.init("Ayin");

	while (!app.done) {
		InputRequest input_req = app.handle_input();
		app.new_frame();

		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Open", "Ctrl+O")) {
					app.open_file_dialog();
				}
				if (ImGui::MenuItem("Save", "Ctrl+S")) {
					input_req.ty = InputRequest_Save;
				};
				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) {
					input_req.ty = InputRequest_SaveAs;
				};
				ImGui::Separator();
				ImGui::MenuItem("Quit", "Ctrl-Q", &app.done);
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit")) {
				if (ImGui::MenuItem("Undo", "Ctrl+Z")) {
					input_req.ty = InputRequest_Undo;
				}
				if (ImGui::MenuItem("Redo", "Ctrl+Y")) {
					input_req.ty = InputRequest_Redo;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		if (app.photos.empty()) {
			app.render();
			continue;
		}

		Photo *photo;
		ImGui::SetNextWindowSize(ImVec2(app.io->DisplaySize.x * 4.0f / 5.0f, (app.io->DisplaySize.y - 23.0f)));
		ImGui::SetNextWindowPos(ImVec2(app.io->DisplaySize.x - app.io->DisplaySize.x * 4.0f / 5.0f, 23.0f));
		if (ImGui::Begin("Photos", NULL, window_flags)) {
			if (ImGui::BeginTabBar("Photos Tab Bar", tabbar_flags)) {
				int i = 1;
				for (auto it = app.photos.begin(); it != app.photos.end(); ++it) {
					auto tab_flags = ImGuiTabItemFlags_None;
					if (input_req.ty == InputRequest_SwitchTab && input_req.tab_number == i++ && !cmd) {
						tab_flags = ImGuiTabItemFlags_SetSelected;
					}
					if (ImGui::BeginTabItem((*it)->filename.c_str(), NULL, tab_flags)) {
						// photo = *it;
						if (ImGui::BeginChild("Image")) {
							ImVec2 cursor_pos = ImGui::GetCursorPos();
							ImVec2 content_region = ImGui::GetContentRegionAvail();
							ImVec2 content_pos = ImGui::GetWindowPos();
							if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && app.io->MousePos.x >= content_pos.x &&
								app.io->MousePos.y >= content_pos.y) {
								photo->x += app.io->MouseDelta.x;
								photo->y += app.io->MouseDelta.y;
							}
							if ((app.io->KeyCtrl && app.io->MouseWheel > 0) || input_req.ty == InputRequest_ZoomIn) {
								photo->zoom = std::min(photo->zoom * 1.03f, 10.0f);
							} else if ((app.io->KeyCtrl && app.io->MouseWheel < 0) ||
									   input_req.ty == InputRequest_ZoomOut) {
								photo->zoom = std::max(photo->zoom / 1.03f, 0.1f);
							}
							ImGui::SetCursorPos(ImVec2(
								photo->x + cursor_pos.x + (content_region.x - photo->image->width * photo->zoom) * 0.5f,
								photo->y + cursor_pos.y +
									(content_region.y - photo->image->height * photo->zoom) * 0.5f));
							if (cmd && cmd->tmpImage) {
								ImGui::Image(
									(void *)(intptr_t)cmd->tmpImage->texture,
									ImVec2(cmd->tmpImage->width * photo->zoom, cmd->tmpImage->height * photo->zoom));
							} else {
								ImGui::Image(
									(void *)(intptr_t)photo->image->texture,
									ImVec2(photo->image->width * photo->zoom, photo->image->height * photo->zoom));
							}
						}
						ImGui::EndChild();
						ImGui::EndTabItem();
					}
				}
				ImGui::EndTabBar();
			}
			ImGui::End();
		}

		ImVec2 button_size(app.io->DisplaySize.x * 1.0f / 5.0f - 15, 0);
		ImGui::SetNextWindowSize(ImVec2(button_size.x + 15, (app.io->DisplaySize.y - 23.0f)));
		ImGui::SetNextWindowPos(ImVec2(0, app.io->DisplaySize.y - (app.io->DisplaySize.y - 23.0f)));
		if (ImGui::Begin("Filters", NULL, window_flags)) {
			if (cmd && cmd->done) {
				photo->push_change(cmd->getInfo());
				delete cmd;
				cmd = nullptr;
			} else if (cmd) {
				cmd->showOptionsMenu();
			} else {
				ImGui::BeginDisabled(!photo->can_undo_change());
				if (ImGui::Button(ICON_MD_UNDO " Undo")) {
					input_req.ty = InputRequest_Undo;
				}
				ImGui::EndDisabled();
				ImGui::SameLine();
				ImGui::BeginDisabled(!photo->can_redo_change());
				if (ImGui::Button(ICON_MD_REDO " Redo")) {
					input_req.ty = InputRequest_Redo;
				};
				ImGui::EndDisabled();
				ImGui::SameLine();
				ImGui::BeginDisabled(!photo->can_undo_change());
				if (ImGui::Button(ICON_MD_HISTORY " Reset")) {
					photo->reset();
				}
				ImGui::EndDisabled();
				ImGui::Text("Filters");
				// Filters buttons
				for (size_t i = 0; i < Commands::number; i++) {
					if (ImGui::Button(Commands::names[i], button_size)) {
						cmd = Commands::factory[i]();
						cmd->setImage(*photo->image);
						if (!cmd->hasOptionsMenu()) {
							photo->push_change(cmd->getInfo());
							delete cmd;
							cmd = nullptr;
						}
					}
				}
			}
		}
		ImGui::End();

		if (input_req.ty == InputRequest_SaveAs) {
			app.save_file_dialog(*photo);
		} else if (input_req.ty == InputRequest_Save) {
			photo->image->save(photo->filepath.c_str());
			delete photo->origImage;
			photo->origImage = new Image(photo->image->width, photo->image->height, photo->image->channels);
			memcpy(photo->origImage->data, photo->image->data,
				   photo->image->width * photo->image->height * photo->image->channels);
			photo->soft_reset();
		} else if (input_req.ty == InputRequest_Undo && photo->can_undo_change()) {
			photo->undo_change();
		} else if (input_req.ty == InputRequest_Redo && photo->can_redo_change()) {
			photo->redo_change();
		} else if (input_req.ty == InputRequest_CloseTab) {
			for (auto it = app.photos.begin(); it != app.photos.end(); ++it) {
				if ((*it)->filename == photo->filename) {
					app.photos.erase(it);
					break;
				}
			}
		}

		app.render();
	}

	if (cmd != nullptr) {
		delete cmd;
	}
	return EXIT_SUCCESS;
}
