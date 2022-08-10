#pragma once

#include <map>
#include <vector>
#include <string>
#include <functional>

#include "Lib/ImGui/imgui.h"
#define NOGDI
#define WIN32_LEAN_AND_MEAN
#include "spdlog/spdlog.h"

namespace Ship {
	#define CMD_SUCCESS true
	#define CMD_FAILED false
	#define MAX_BUFFER_SIZE 255
	#define NULLSTR "None"

	typedef std::function<bool(std::vector<std::string> args)> CommandHandler;

	enum class ArgumentType {
		TEXT, NUMBER, PLAYER_POS, PLAYER_ROT
	};

	struct CommandArgument {
		std::string info;
		ArgumentType type = ArgumentType::NUMBER;
		bool optional = false;
	};

	struct CommandEntry {
		CommandHandler handler;
		std::string description;
		std::vector<CommandArgument> arguments;
	};

	struct ConsoleLine {
		std::string text;
		spdlog::level::level_enum priority = spdlog::level::info;
		std::string channel = "Main";
	};

	class Console {
		int selectedId = -1;
		std::vector<int> selectedEntries;
		std::string filter;
		spdlog::level::level_enum level_filter = spdlog::level::trace;
		std::vector<std::string> log_channels = { "Console", "Logs" };
		std::vector<spdlog::level::level_enum> priority_filters = { spdlog::level::off, spdlog::level::critical, spdlog::level::err, spdlog::level::warn, spdlog::level::info, spdlog::level::debug, spdlog::level::trace };
		std::vector<ImVec4> priority_colors = {
			ImVec4(0.8f, 0.8f, 0.8f, 1.0f),     // TRACE
			ImVec4(0.9f, 0.9f, 0.9f, 1.0f),     // DEBUG
			ImVec4(1.0f, 1.0f, 1.0f, 1.0f),     // INFO
			ImVec4(1.0f, 0.875f, 0.125f, 1.0f), // WARN
			ImVec4(0.65f, 0.18f, 0.25, 1.0f),   // ERROR
			ImVec4(0.95f, 0.11f, 0.25, 1.0f),   // CRITICAL
			ImVec4(0.0f, 0.0f, 0.0f, 0.0f)      // OFF
		};
	protected:
		void Append(const std::string& channel, spdlog::level::level_enum priority, const char* fmt, va_list args);

	public:
		std::map<std::string, std::vector<ConsoleLine>> Log;
		std::map<std::string, CommandEntry> Commands;
		std::vector<std::string> Autocomplete;
		std::vector<std::string> History;
		std::string CMDHint = NULLSTR;
		char* FilterBuffer = nullptr;
		char* InputBuffer = nullptr;
		bool OpenAutocomplete = false;
		int HistoryIndex = -1;
		std::string selected_channel = "Console";
		bool opened = false;
		void Init();
		void Update();
		void Draw();
		void Dispatch(const std::string& line);
		static int CallbackStub(ImGuiInputTextCallbackData* data);
		void SendInfoMessage(const char* fmt, ...);
		void SendErrorMessage(const char* fmt, ...);
		void SendInfoMessage(const std::string& str);
		void SendErrorMessage(const std::string& str);
		void Append(const std::string& channel, spdlog::level::level_enum priority, const char* fmt, ...);
	};
}