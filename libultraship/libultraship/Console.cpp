#include "Console.h"

#include "Cvar.h"
#include "ImGuiImpl.h"
#include "Lib/ImGui/imgui.h"
#include "Utils/StringHelper.h"
#include "Lib/ImGui/imgui_internal.h"
#include "Utils.h"
#include <sstream>

namespace Ship {
	std::string BuildUsage(const CommandEntry& entry) {
		std::string usage;
		for (const auto& arg : entry.arguments)
			usage += StringHelper::Sprintf(arg.optional ? "[%s] " : "<%s> ", arg.info.c_str());
		return usage;
	}

	bool Console::HelpCommand(std::shared_ptr<Console> Console, const std::vector<std::string>& args) {
		Console->SendInfoMessage("SoH Commands:");
		for (const auto& cmd : Console->Commands) {
			Console->SendInfoMessage(" - " + cmd.first);
		}
		return CMD_SUCCESS;
	}

	bool Console::ClearCommand(std::shared_ptr<Console> Console, const std::vector<std::string>& args) {
		Console->ClearLogs(Console->GetCurrentChannel());
		return CMD_SUCCESS;
	}

	bool Console::BindCommand(std::shared_ptr<Console> Console, const std::vector<std::string>& args) {
		if (args.size() > 2) {
			const ImGuiIO* io = &ImGui::GetIO();;
			for (size_t k = 0; k < std::size(io->KeysData); k++) {
				std::string key(ImGui::GetKeyName(k));

				if (toLowerCase(args[1]) == toLowerCase(key)) {
					std::vector<std::string> tmp;
					const char* const delim = " ";
					std::ostringstream imploded;
					std::copy(args.begin() + 2, args.end(), std::ostream_iterator<std::string>(imploded, delim));
					Console->Bindings[k] = imploded.str();
					Console->SendInfoMessage("Binding '%s' to %s", args[1].c_str(), Console->Bindings[k].c_str());
					break;
				}
			}
		}
		return CMD_SUCCESS;
	}

	bool Console::BindToggleCommand(std::shared_ptr<Console> Console, const std::vector<std::string>& args) {
		if (args.size() > 2) {
			const ImGuiIO* io = &ImGui::GetIO();;
			for (size_t k = 0; k < std::size(io->KeysData); k++) {
				std::string key(ImGui::GetKeyName(k));

				if (toLowerCase(args[1]) == toLowerCase(key)) {
					Console->BindingToggle[k] = args[2];
					Console->SendInfoMessage("Binding toggle '%s' to %s", args[1].c_str(), Console->BindingToggle[k].c_str());
					break;
				}
			}
		}
		return CMD_SUCCESS;
	}

	void Console::Init() {
		this->inputBuffer = new char[MAX_BUFFER_SIZE];
		strcpy(this->inputBuffer, "");
		this->filterBuffer = new char[MAX_BUFFER_SIZE];
		strcpy(this->filterBuffer, "");
		AddCommand("help", { HelpCommand, "Shows all the commands" });
		AddCommand("clear", { ClearCommand, "Clear the console history" });
		AddCommand("bind", { BindCommand, "Binds key to commands" });
		AddCommand("bind-toggle", { BindToggleCommand, "Bind key as a bool toggle" });
	}

	void Console::Update() {
		for (auto [key, cmd] : Bindings) {
			if (ImGui::IsKeyPressed(key)) Dispatch(cmd);
		}
		for (auto [key, var] : BindingToggle) {
			if (ImGui::IsKeyPressed(key)) {
				CVar* cvar = CVar_Get(var.c_str());
				Dispatch("set " + var + " " + std::to_string(cvar == nullptr ? 0 : !static_cast<bool>(cvar->value.valueS32)));
			}
		}
	}

	void Console::Draw() {
		if (!this->opened) {
			CVar_SetS32("gConsoleEnabled", 0);
			return;
		}

		bool input_focus = false;

		ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
		ImGui::Begin("Console", &this->opened, ImGuiWindowFlags_NoFocusOnAppearing);
		const ImVec2 pos = ImGui::GetWindowPos();
		const ImVec2 size = ImGui::GetWindowSize();
		// SohImGui::ShowCursor(ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows | ImGuiHoveredFlags_RectOnly), SohImGui::Dialogues::dConsole);

		// Renders autocomplete window
		if (this->openAutocomplete) {
			ImGui::SetNextWindowSize(ImVec2(350, std::min(static_cast<int>(this->Autocomplete.size()), 3) * 20.f), ImGuiCond_Once);
			ImGui::SetNextWindowPos(ImVec2(pos.x + 8, pos.y + size.y - 1));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(3, 3));
			ImGui::Begin("##WndAutocomplete", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
			ImGui::BeginChild("AC_Child", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(.3f, .3f, .3f, 1.0f));
			if (ImGui::BeginTable("AC_History", 1)) {
				for (const auto& cmd : this->Autocomplete) {
					std::string usage = BuildUsage(this->Commands[cmd]);
					std::string preview = cmd + " - " + this->Commands[cmd].description;
					std::string autocomplete = (usage == NULLSTR ? cmd : usage);
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					if (ImGui::Selectable(preview.c_str())) {
						memset(this->inputBuffer, 0, MAX_BUFFER_SIZE);
						memcpy(this->inputBuffer, autocomplete.c_str(), sizeof(char) * autocomplete.size());
						this->openAutocomplete = false;
						input_focus = true;
					}
				}
				ImGui::EndTable();
			}
			if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
				this->openAutocomplete = false;
			}
			ImGui::PopStyleColor();
			ImGui::EndChild();
			ImGui::End();
			ImGui::PopStyleVar();
		}

		if (ImGui::BeginPopupContextWindow("Context Menu")) {
			if (ImGui::MenuItem("Copy Text")) {
				ImGui::SetClipboardText(this->Log[this->currentChannel][this->selectedId].text.c_str());
				this->selectedId = -1;
			}
			ImGui::EndPopup();
		}
		if (this->selectedId != -1 && ImGui::IsMouseClicked(1)) {
			ImGui::OpenPopup("##WndAutocomplete");
		}

		// Renders top bar filters
		if (ImGui::Button("Clear")) this->Log[this->currentChannel].clear();

		if (CVar_GetS32("gSinkEnabled", 0)) {
			ImGui::SameLine();
			ImGui::SetNextItemWidth(150);
			if (ImGui::BeginCombo("##channel", this->currentChannel.c_str())) {
				for (const auto& channel : LogChannels) {
					const bool is_selected = (channel == std::string(this->currentChannel));
					if (ImGui::Selectable(channel.c_str(), is_selected))
						this->currentChannel = channel;
					if (is_selected) ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
		} else {
			this->currentChannel = "Console";
		}
		ImGui::SameLine();
		ImGui::SetNextItemWidth(150);

		if (this->currentChannel != "Console") {
			if (ImGui::BeginCombo("##level", spdlog::level::to_string_view(this->levelFilter).data())) {
				for (const auto& priority_filter : PriorityFilters) {
					const bool is_selected = priority_filter == this->levelFilter;
					if (ImGui::Selectable(spdlog::level::to_string_view(priority_filter).data(), is_selected))
					{
						this->levelFilter = priority_filter;
						if (is_selected) ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
		} else {
			this->levelFilter = spdlog::level::trace;
		}
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		if (ImGui::InputTextWithHint("##input", "Filter", this->filterBuffer, MAX_BUFFER_SIZE))this->filter = std::string(this->filterBuffer);
		ImGui::PopItemWidth();

		// Renders console history
		const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
		ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(.3f, .3f, .3f, 1.0f));
		if (ImGui::BeginTable("History", 1)) {

			if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_DownArrow)))
				if (this->selectedId < (int)this->Log.size() - 1)++this->selectedId;
			if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_UpArrow)))
				if (this->selectedId > 0)--this->selectedId;

			const std::vector<ConsoleLine> channel = this->Log[this->currentChannel];
			for (int i = 0; i < static_cast<int>(channel.size()); i++) {
				ConsoleLine line = channel[i];
				if (!this->filter.empty() && line.text.find(this->filter) == std::string::npos) continue;
				if (this->levelFilter > line.priority) continue;
				std::string id = line.text + "##" + std::to_string(i);
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				const bool is_selected = (this->selectedId == i) || std::find(this->selectedEntries.begin(), this->selectedEntries.end(), i) != this->selectedEntries.end();
				ImGui::PushStyleColor(ImGuiCol_Text, this->PriorityColours[line.priority]);
				if (ImGui::Selectable(id.c_str(), is_selected)) {
					if (ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_LeftCtrl)) && !is_selected)
						this->selectedEntries.push_back(i);

					else this->selectedEntries.clear();
					this->selectedId = is_selected ? -1 : i;
				}
				ImGui::PopStyleColor();
				if (is_selected) ImGui::SetItemDefaultFocus();
			}
			ImGui::EndTable();
		}
		ImGui::PopStyleColor();
		if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
			ImGui::SetScrollHereY(1.0f);
		ImGui::EndChild();

		if (this->currentChannel == "Console") {
			// Renders input textfield
			constexpr ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackEdit |
				ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
		#ifdef __WIIU__
			ImGui::PushItemWidth(-53.0f * 2.0f);
		#else
			ImGui::PushItemWidth(-53.0f);
		#endif
			if (ImGui::InputTextWithHint("##CMDInput", ">", this->inputBuffer, MAX_BUFFER_SIZE, flags, &Console::CallbackStub, this)) {
				input_focus = true;
				if (this->inputBuffer[0] != '\0' && this->inputBuffer[0] != ' ')
					this->Dispatch(std::string(this->inputBuffer));
				memset(this->inputBuffer, 0, MAX_BUFFER_SIZE);
			}

			if (this->cmdHint != NULLSTR) {
				if (ImGui::IsItemFocused()) {
					ImGui::SetNextWindowPos(ImVec2(pos.x, pos.y + size.y));
					ImGui::SameLine();
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted(this->cmdHint.c_str());
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}
			}

			ImGui::SameLine();
		#ifdef __WIIU__
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - 50 * 2.0f);
		#else
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - 50);
		#endif
			if (ImGui::Button("Submit") && !input_focus && this->inputBuffer[0] != '\0' && this->inputBuffer[0] != ' ') {
				this->Dispatch(std::string(this->inputBuffer));
				memset(this->inputBuffer, 0, MAX_BUFFER_SIZE);
			}

			ImGui::SetItemDefaultFocus();
			if (input_focus) ImGui::SetKeyboardFocusHere(-1);
			ImGui::PopItemWidth();
		}
		ImGui::End();
	}

	void Console::Dispatch(const std::string& line) {
		this->cmdHint = NULLSTR;
		this->History.push_back(line);
		SendInfoMessage("> " + line);
		const std::vector<std::string> cmd_args = StringHelper::Split(line, " ");
		if (this->Commands.contains(cmd_args[0])) {
			const CommandEntry entry = this->Commands[cmd_args[0]];
			if (!entry.handler(shared_from_this(), cmd_args) && !entry.arguments.empty()) {
				SendErrorMessage("[SOH] Usage: " + cmd_args[0] + " " + BuildUsage(entry));
			}

			return;
		}
		SendErrorMessage("[SOH] Command not found");
	}

	int Console::CallbackStub(ImGuiInputTextCallbackData* data) {
		const auto instance = static_cast<Console*>(data->UserData);
		const bool empty_history = instance->History.empty();
		const int  history_index = instance->historyIndex;
		std::string history;

		switch (data->EventKey) {
		case ImGuiKey_Tab:
			instance->Autocomplete.clear();
			for (auto& [cmd, entry] : instance->Commands)
				if (cmd.find(std::string(data->Buf)) != std::string::npos) instance->Autocomplete.push_back(cmd);
			instance->openAutocomplete = !instance->Autocomplete.empty();
			instance->cmdHint = NULLSTR;
			break;
		case ImGuiKey_UpArrow:
			if (empty_history) break;
			if (history_index < static_cast<int>(instance->History.size()) - 1) instance->historyIndex += 1;
			data->DeleteChars(0, data->BufTextLen);
			data->InsertChars(0, instance->History[instance->historyIndex].c_str());
			instance->cmdHint = NULLSTR;
			break;
		case ImGuiKey_DownArrow:
			if (empty_history) break;
			if (history_index > -1) instance->historyIndex -= 1;
			data->DeleteChars(0, data->BufTextLen);
			if (history_index >= 0)
				data->InsertChars(0, instance->History[history_index].c_str());
			instance->cmdHint = NULLSTR;
			break;
		case ImGuiKey_Escape:
			instance->historyIndex = -1;
			data->DeleteChars(0, data->BufTextLen);
			instance->openAutocomplete = false;
			instance->cmdHint = NULLSTR;
			break;
		default:
			instance->openAutocomplete = false;
			for (auto& [cmd, entry] : instance->Commands) {
				const std::vector<std::string> cmd_args = StringHelper::Split(std::string(data->Buf), " ");
				if (data->BufTextLen > 2 && !cmd_args.empty() && cmd.find(cmd_args[0]) != std::string::npos) {
					instance->cmdHint = cmd + " " + BuildUsage(entry);
					break;
				}
				instance->cmdHint = NULLSTR;
			}
		}
		return 0;
	}

	void Console::Append(const std::string& channel, spdlog::level::level_enum priority, const char* fmt, va_list args) {
		char buf[2048];
		vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
		buf[IM_ARRAYSIZE(buf) - 1] = 0;
		this->Log[channel].push_back({ std::string(buf), priority });
	}

	void Console::Append(const std::string& channel, spdlog::level::level_enum priority, const char* fmt, ...) {
		va_list args;
		va_start(args, fmt);
		Append(channel, priority, fmt, args);
		va_end(args);
	}

	void Console::SendInfoMessage(const char* fmt, ...) {
		va_list args;
		va_start(args, fmt);
		Append("Console", spdlog::level::info, fmt, args);
		va_end(args);
	}

	void Console::SendErrorMessage(const char* fmt, ...) {
		va_list args;
		va_start(args, fmt);
		Append("Console", spdlog::level::err, fmt, args);
		va_end(args);
	}

	void Console::SendInfoMessage(const std::string& str) {
		Append("Console", spdlog::level::info, str.c_str());
	}

	void Console::SendErrorMessage(const std::string& str) {
		Append("Console", spdlog::level::err, str.c_str());
	}

	void Console::ClearLogs(std::string channel) {
		Log[channel].clear();
	}

	void Console::ClearLogs() {
		for (auto [key, var] : Log) {
			var.clear();
		}
	}

	bool Console::HasCommand(const std::string& command) {
		for (const auto& Command : Commands) {
			if (Command.first == command) {
				return true;
			}
		}

		return false;
	}

	void Console::AddCommand(const std::string& command, CommandEntry entry) {
		if (!HasCommand(command)) {
			Commands[command] = entry;
		}
	}

	std::string Console::GetCurrentChannel() {
		return currentChannel;
	}

	bool Console::IsOpened() {
		return opened;
	}

	void Console::Close() {
		opened = false;
	}

	void Console::Open() {
		opened = true;
	}
}