#include "game.h"
#include "raymath.h"
#include "utils/rutils.h"
#define RLIGHTS_IMPLEMENTATION
#include <thread>

#include "impl.h"
#include "rlights.h"
#include "impl/baserom_extractor/baserom_extractor.h"
#include "impl/extractor/extractor.h"
#include "impl/fix_baserom/fix_baserom.h"
#include "utils/mutils.h"

#define TEXT_COLOR Color(0, 0, 0, 160)
#define SCALE(x) Vector3(x, x, x)

Shader shader = { 0 };
Light light = { 0 };
Vector3 lightPos = { -5.0f, 10.0f, 10.0f };
Vector2 dragOffset;
bool isDragging = false;
std::string sohFolder = NULLSTR;
bool extracting = false;
bool rom_ready = false;
bool single_thread = false;
bool hide_second_btn = false;
RomVersion version;
const char* patched_rom = "tmp/rom.z64";
extern bool oldExtractMode;

static std::string currentStep = "None";

void OTRGame::preload() {
	this->LoadTexture("Cartridge", "assets/icons/cartridge.png");
	this->LoadTexture("Frame", "assets/sprites/frame.png");
	this->LoadTexture("Folder", "assets/icons/folder.png");
	this->LoadTexture("Button", "assets/sprites/button.png");
	this->LoadTexture("Exit", "assets/icons/exit.png");
	this->LoadTexture("Title", "assets/sprites/title.png");
	this->LoadTexture("Modal", "assets/sprites/modal.png");
	this->LoadTexture("Info", "assets/icons/info.png");
	Models["Ship"] = LoadModel("assets/models/Hark.glb");
}

void cleanTempData() {
	MoonUtils::rm("tmp/");
	MoonUtils::mkdir("tmp/");
}

void OTRGame::init(){
	camera.position = { 18.0f, 15.0f, 18.0f };
	camera.target = { -0.5f, -0.07f, 0.0f };
	camera.up = { 0.0f, 1.0f, 0.0f };
	camera.fovy = 1.5f;
	camera.projection = CAMERA_PERSPECTIVE;

	shader = LoadShader("assets/shaders/base_lighting.vs", "assets/shaders/lighting.fs");
	shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
	int ambientLoc = GetShaderLocation(shader, "ambient");
	float points[4] = { 0.3f, 0.3f, 0.3f, 1.0f };
	SetShaderValue(shader, ambientLoc, points, SHADER_UNIFORM_VEC4);

	light = CreateLight(LIGHT_DIRECTIONAL, camera.position, camera.target, WHITE, shader);

	for (int i = 0; i < Models["Ship"].materialCount; i++) {
		Material& mat = Models["Ship"].materials[i];
		mat.shader = shader;
	}

	if (fs::exists("soh.exe") || fs::exists("soh.elf")) {
		hide_second_btn = true;
		sohFolder = ".";
	}
}

void ExtractRom()
{
	WriteResult result;

	if (oldExtractMode)
		ExtractBaserom(patched_rom);
	else
		result.error = NULLSTR;

	if (result.error == NULLSTR) {
		if (MoonUtils::exists("oot.otr") && !version.isMQ) MoonUtils::rm("oot.otr");
		if (MoonUtils::exists("oot-mq.otr") && version.isMQ) MoonUtils::rm("oot-mq.otr");
		if (MoonUtils::exists("Extract")) MoonUtils::rm("Extract");

		MoonUtils::mkdir("Extract");
		//MoonUtils::copy("tmp/baserom/Audiobank", "Extract/Audiobank");
		//MoonUtils::copy("tmp/baserom/Audioseq", "Extract/Audioseq");
		//MoonUtils::copy("tmp/baserom/Audiotable", "Extract/Audiotable");
		//MoonUtils::copy("tmp/baserom/version", "Extract/version");

		MoonUtils::copy("assets/game/", "Extract/assets/");

		startWorker(version);
		extracting = true;
	}
}

void OTRGame::update(){
	updateWorker(sohFolder, version);
	this->ModelRotation += 50 * GetFrameTime();
	UpdateLightValues(shader, light);

	float cameraPos[3] = { camera.position.x, camera.position.y, camera.position.z };
	SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);

	if(!extracting && sohFolder != NULLSTR && rom_ready) {
		currentStep = "Extracting rom assets";
		ExtractRom();
	}
}

void OTRGame::draw() {
	Vector2 windowSize(GetScreenWidth(), GetScreenHeight());
	Rectangle titlebar = Rectangle(0, 0, windowSize.x - 50, 35);
	Vector2 mousePos = GetMousePosition();
	Vector2 mouseDelta = GetMouseDelta();

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !isDragging &&
		mousePos.x >= titlebar.x && mousePos.y >= titlebar.y && mousePos.x <= titlebar.x + titlebar.width && mousePos.y <= titlebar.y + titlebar.height) {
		isDragging = true;
		dragOffset = mousePos;
	}
	else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && isDragging) {
		isDragging = false;
		dragOffset = Vector2(0, 0);
	}

	if (isDragging && (mouseDelta.x != 0.0f || mouseDelta.y != 0.0f)) {
		Vector2 wndPos = GetWindowPosition();
		wndPos = Vector2(wndPos.x + (mousePos.x - dragOffset.x), wndPos.y + (mousePos.y - dragOffset.y));

		// Calculate virtual screen total size in case there are multiple monitors

		int vsX1 = 0, vsY1 = 0, vsX2 = 0, vsY2 = 0;
		int monitorCount = GetMonitorCount();

		for (int m = 0; m < monitorCount; m++) {
			Vector2 monitorPos = GetMonitorPosition(m);
			Vector2 monitorSize = Vector2(GetMonitorWidth(m), GetMonitorHeight(m));

			if (monitorPos.x < vsX1) vsX1 = monitorPos.x;
			if (monitorPos.y < vsY1) vsY1 = monitorPos.y;
			if (monitorPos.x + monitorSize.x > vsX2) vsX2 = monitorPos.x + monitorSize.x;
			if (monitorPos.y + monitorSize.y > vsY2) vsY2 = monitorPos.y + monitorSize.y;
		}

		// Clamp the window to the borders of the monitors
		if (wndPos.x < vsX1) wndPos.x = vsX1;
		if (wndPos.x < vsX1) wndPos.x = vsX1;
		if (wndPos.y < vsY1) wndPos.y = vsY1;
		if (wndPos.x + windowSize.x > vsX2) wndPos.x = vsX2 - windowSize.x;
		if (wndPos.y + windowSize.y > vsY2) wndPos.y = vsY2 - windowSize.y;

		SetWindowPosition(wndPos.x, wndPos.y);
	}

	BeginDrawing();
	ClearBackground(Color(40, 40, 40, 255));

	DrawTexture(Textures["Frame"], 0, 0, WHITE);

	Texture2D titleTex = Textures["Title"];
	DrawTexture(titleTex, windowSize.x / 2 - titleTex.width / 2, titlebar.height / 2 - titleTex.height / 2, WHITE);

	if (UIUtils::GuiIcon("Exit", windowSize.x - 36, titlebar.height / 2 - 10) && (extracting && currentStep.find("Done") != std::string::npos || !extracting)) {
		closeRequested = true;
	}

	BeginMode3D(camera);
	DrawModelEx(Models["Ship"], Vector3Zero(), Vector3(.0f, 1.0f, .0f), this->ModelRotation, SCALE(1.0f), WHITE);
	EndMode3D();

	constexpr float text_y = 125.f;
	UIUtils::GuiShadowText(("Rom Type: " + version.version).c_str(), 32, text_y, 10, WHITE, BLACK);
	UIUtils::GuiShadowText("Tool Version: 1.0", 32, text_y + 15, 10, WHITE, BLACK);
	UIUtils::GuiShadowText("OTR Version: 1.0", 32, text_y + 30, 10, WHITE, BLACK);

	if (oldExtractMode)
		UIUtils::GuiToggle(&single_thread, "Single Thread", 32, text_y + 40, currentStep != NULLSTR);

	if (!hide_second_btn && UIUtils::GuiIconButton("Folder", "Open\nShip Folder", 109, 50, currentStep != NULLSTR, "Select your Ship of Harkinian Folder\n\nYou could use another folder\nfor development purposes")) {
		const std::string path = NativeFS->LaunchFileExplorer(LaunchType::FOLDER);
		sohFolder = path;
	}

	if (UIUtils::GuiIconButton("Cartridge", "Open\nOoT Rom", 32, 50, currentStep != NULLSTR, "Select an Ocarina of Time\nGameCube PAL or Vanilla Debug Rom\n\nYou can dump it or lend one from Nintendo")) {
		const std::string path = NativeFS->LaunchFileExplorer(LaunchType::FILE);
		if (path != NULLSTR) {
			const std::string patched_n64 = std::string(patched_rom);
			MoonUtils::rm(patched_n64);
			version = GetVersion(fopen(path.c_str(), "r"));
			if (version.version != NULLSTR) {
				MoonUtils::copy(path, patched_n64);
				rom_ready = true;
				return;
			}
			fix_baserom(path.c_str(), patched_rom);
			version = GetVersion(fopen(patched_rom, "r"));
			if (version.version != NULLSTR) rom_ready = true;
		}
	}

	if (currentStep != NULLSTR) {
		DrawRectangle(0, 0, windowSize.x, windowSize.y, Color(0, 0, 0, 160));
		DrawTexture(Textures["Modal"], windowSize.x / 2 - Textures["Modal"].width / 2, windowSize.y / 2 - Textures["Modal"].height / 2, WHITE);
		UIUtils::GuiShadowText(currentStep.c_str(), 0, windowSize.y / 2, 10, WHITE, BLACK, windowSize.x, true);
	}

	EndDrawing();
}

void setCurrentStep(const std::string& step) {
	currentStep = step;
}

void OTRGame::exit(){

}
