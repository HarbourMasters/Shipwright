#include "GameOverlay.h"

#include "Cvar.h"
#include "File.h"
#include "Archive.h"
#include "ResourceMgr.h"
#include "SohConsole.h"
#include "SohImGuiImpl.h"
#include "TextureMod.h"
#include "Lib/ImGui/imgui_internal.h"

void Ship::GameOverlay::LoadFont(const std::string& name, const std::string& path, float fontSize) {
	ImGuiIO& io = ImGui::GetIO();
	std::shared_ptr<Archive> base = GlobalCtx2::GetInstance()->GetResourceManager()->GetArchive();
	std::shared_ptr<File> font = std::make_shared<File>();
	base->LoadFile(path, false, font);
	if (font->bIsLoaded) {
		char* font_data = new char[font->dwBufferSize];
		memcpy(font_data, font->buffer.get(), font->dwBufferSize);
		Fonts[name] = io.Fonts->AddFontFromMemoryTTF(font_data, font->dwBufferSize, fontSize);
	}
}

void Ship::GameOverlay::TextDraw(float x, float y, bool shadow, ImVec4 color, const char* fmt, ...) {
	char buf[1024];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
	buf[IM_ARRAYSIZE(buf) - 1] = 0;
	va_end(args);

	ImGui::PushStyleColor(ImGuiCol_Text, color);
	ImGui::PushFont(Fonts[this->CurrentFont]);
	if (shadow) {
		ImGui::SetCursorPos(ImVec2(x + 1, y + 1));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(.0f, .0f, .0f, color.w));
		ImGui::Text(buf, args);
	}
	ImGui::PopStyleColor();
	ImGui::SetCursorPos(ImVec2(x, y));
	ImGui::Text(buf, args);
	ImGui::PopFont();
	ImGui::PopStyleColor();
}

void Ship::GameOverlay::TextDrawNotification(float duration, bool shadow, const char* fmt, ...) {
	char buf[1024];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
	buf[IM_ARRAYSIZE(buf) - 1] = 0;
	va_end(args);

	this->RegisteredOverlays[fmt] = new Overlay({ OverlayType::NOTIFICATION, ImStrdup(buf), duration, duration });
}


float Ship::GameOverlay::GetScreenWidth() {
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	return viewport->Size.x;
}

float Ship::GameOverlay::GetScreenHeight() {
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	return viewport->Size.y;
}

float Ship::GameOverlay::GetStringWidth(const char* text) {
	return CalculateTextSize(text).x;
}

ImVec2 Ship::GameOverlay::CalculateTextSize(const char* text, const char* text_end, bool hide_text_after_double_hash, float wrap_width) {
	ImGuiContext& g = *GImGui;

	const char* text_display_end;
	if (hide_text_after_double_hash)
		text_display_end = ImGui::FindRenderedTextEnd(text, text_end);      // Hide anything after a '##' string
	else
		text_display_end = text_end;

	GameOverlay* overlay = SohImGui::overlay;

	ImFont* font = overlay->CurrentFont == "Default" ? g.Font : overlay->Fonts[overlay->CurrentFont];
	const float font_size = font->FontSize;
	if (text == text_display_end)
		return ImVec2(0.0f, font_size);
	ImVec2 text_size = font->CalcTextSizeA(font_size, FLT_MAX, wrap_width, text, text_display_end, NULL);

	// Round
	// FIXME: This has been here since Dec 2015 (7b0bf230) but down the line we want this out.
	// FIXME: Investigate using ceilf or e.g.
	// - https://git.musl-libc.org/cgit/musl/tree/src/math/ceilf.c
	// - https://embarkstudios.github.io/rust-gpu/api/src/libm/math/ceilf.rs.html
	text_size.x = IM_FLOOR(text_size.x + 0.99999f);

	return text_size;
}

void Ship::GameOverlay::Init() {
	this->LoadFont("Press Start 2P", "assets/ship_of_harkinian/fonts/PressStart2P-Regular.ttf", 12.0f);
	this->LoadFont("Fipps", "assets/ship_of_harkinian/fonts/Fipps-Regular.otf", 32.0f);
	const std::string DefaultFont = this->Fonts.begin()->first;
	if(!this->Fonts.empty()) {
		const std::string font = CVar_GetString("gOverlayFont", ImStrdup(DefaultFont.c_str()));
		for (auto& [name, _] : this->Fonts) {
			if (font.starts_with(name)) {
				this->CurrentFont = name;
				break;
			}
			this->CurrentFont = DefaultFont;
		}
	}
	SohImGui::console->Commands["overlay"] = { OverlayCommand, "Draw an overlay using a cvar value" };
}

void Ship::GameOverlay::DrawSettings() {
	ImGui::Text("Overlays Text Font");
	if (ImGui::BeginCombo("##TextFont", this->CurrentFont.c_str())) {
		for (auto& [name, font] : this->Fonts) {
			if (ImGui::Selectable(name.c_str(), name == this->CurrentFont)) {
				this->CurrentFont = name;
				CVar_SetString("gOverlayFont", ImStrdup(name.c_str()));
				SohImGui::needs_save = true;
			}

		}
		ImGui::EndCombo();
	}
}


void Ship::GameOverlay::Draw() {
	const ImGuiViewport* viewport = ImGui::GetMainViewport();

	ImGui::SetNextWindowPos(viewport->Pos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(viewport->Size, ImGuiCond_Always);
	ImGui::Begin("SoHOverlay", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs);

	float textY = 50;
	float notY = 0;

	for (auto &[key, overlay] : this->RegisteredOverlays) {

		if (overlay->type == OverlayType::TEXT) {
			const char* text = ImStrdup(overlay->value);
			const CVar* var = CVar_Get(text);
			ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

			switch (var->type) {
			case CVAR_TYPE_FLOAT:
				this->TextDraw(30, textY, true, color, "%s %.2f", text, var->value.valueFloat);
				break;
			case CVAR_TYPE_S32:
				this->TextDraw(30, textY, true, color, "%s %d", text, var->value.valueS32);
				break;
			case CVAR_TYPE_STRING:
				this->TextDraw(30, textY, true, color, "%s %s", text, var->value.valueStr);
				break;
			}

			free((void*) text);
			textY += 30;
		}

		if (overlay->type == OverlayType::NOTIFICATION && overlay->duration > 0) {
			const char* text = overlay->value;
			const float duration = overlay->duration / overlay->fadeTime;

			const ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, duration);
			const float textWidth = this->GetStringWidth(overlay->value);

			this->TextDraw(GetScreenWidth() - textWidth - 40, GetScreenHeight() - 40 - notY, true, color, text);
			notY += 30;
			overlay->duration -= .05f;
		}
	}

	ImGui::End();
}


bool Ship::OverlayCommand(const std::vector<std::string>& args) {
	if (args.size() < 3) {
		return CMD_FAILED;
	}

	if (CVar_Get(args[2].c_str()) != nullptr) {
		const char* key = args[2].c_str();
		GameOverlay* overlay = SohImGui::overlay;
		if (args[1] == "add") {
			if (!overlay->RegisteredOverlays.contains(key)) {
				overlay->RegisteredOverlays[key] = new Overlay({ OverlayType::TEXT, ImStrdup(key), -1.0f });
				INFO("Added overlay: %s ", key);
			} else {
				ERROR("Overlay already exists: %s", key);
			}
		} else if (args[1] == "remove") {
			if (overlay->RegisteredOverlays.contains(key)) {
				overlay->RegisteredOverlays.erase(key);
				INFO("Removed overlay: %s ", key);
			} else {
				ERROR("Overlay not found: %s ", key);
			}
		}
	} else {
		ERROR("CVar %s does not exist", args[2].c_str());
	}

	return CMD_SUCCESS;
}