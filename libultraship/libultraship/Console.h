#pragma once

#include <map>
#include <vector>
#include <string>
#include <functional>

#include "Lib/ImGui/imgui.h"
#include "spdlog/spdlog.h"

namespace Ship {
	#define CMD_SUCCESS true
	#define CMD_FAILED false
	#define MAX_BUFFER_SIZE 255
	#define NULLSTR "None"

	class Console;
	typedef std::function<bool(std::shared_ptr<Console> Console, std::vector<std::string> args)> CommandHandler;

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
		std::string channel = "Console";
	};

	class Console : public std::enable_shared_from_this<Console> {
	private:
		static int CallbackStub(ImGuiInputTextCallbackData* data);
		static bool ClearCommand(std::shared_ptr<Console> Console, const std::vector<std::string>& args);
		static bool HelpCommand(std::shared_ptr<Console> Console, const std::vector<std::string>& args);
		static bool BindCommand(std::shared_ptr<Console> Console, const std::vector<std::string>& args);
		static bool BindToggleCommand(std::shared_ptr<Console> Console, const std::vector<std::string>& args);

		bool opened = false;
		int selectedId = -1;
		int historyIndex = -1;
		std::vector<int> selectedEntries;
		std::string filter;
		std::string currentChannel = "Console";
		bool openAutocomplete = false;
		char* inputBuffer = nullptr;
		char* filterBuffer = nullptr;
		std::string cmdHint = NULLSTR;
		spdlog::level::level_enum levelFilter = spdlog::level::trace;

		std::vector<std::string> History;
		std::vector<std::string> Autocomplete;
		std::map<ImGuiKey, std::string> Bindings;
		std::map<ImGuiKey, std::string> BindingToggle;
		std::map<std::string, CommandEntry> Commands;
		std::map<std::string, std::vector<ConsoleLine>> Log;
		const std::vector<std::string> LogChannels = { "Console", "Logs" };
		const std::vector<spdlog::level::level_enum> PriorityFilters = { spdlog::level::off, spdlog::level::critical, spdlog::level::err, spdlog::level::warn, spdlog::level::info, spdlog::level::debug, spdlog::level::trace };
		const std::vector<ImVec4> PriorityColours = {
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
		void ClearLogs(std::string channel);
		void ClearLogs();
		void Init();
		void Update();
		void Draw();
		void Dispatch(const std::string& line);
		void SendInfoMessage(const char* fmt, ...);
		void SendErrorMessage(const char* fmt, ...);
		void SendInfoMessage(const std::string& str);
		void SendErrorMessage(const std::string& str);
		void Append(const std::string& channel, spdlog::level::level_enum priority, const char* fmt, ...);
		bool HasCommand(const std::string& command);
		void AddCommand(const std::string& command, CommandEntry entry);

		std::string GetCurrentChannel() { return currentChannel; }
		bool IsOpened() { return opened;  }
		void Close() { opened = false; }
		void Open() { opened = true;  }
	};
}