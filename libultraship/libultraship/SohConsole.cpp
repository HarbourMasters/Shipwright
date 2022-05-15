#include "SohConsole.h"

#include <iostream>
#include <sstream>

#include "Cvar.h"
#include "GlobalCtx2.h"
#include "SohImGuiImpl.h"
#include "Lib/ImGui/imgui.h"
#include "Utils/StringHelper.h"
#include "Lib/ImGui/imgui_internal.h"

std::map<ImGuiKey, std::string> Bindings;
std::map<ImGuiKey, std::string> BindingToggle;

static bool HelpCommand(const std::vector<std::string>&) {
	INFO("SoH Commands:");
	for(const auto& cmd : SohImGui::console->Commands) {
		INFO("%s", (" - " + cmd.first).c_str());
	}
	return CMD_SUCCESS;
}

static bool ClearCommand(const std::vector<std::string>&) {
	SohImGui::console->Log[SohImGui::console->selected_channel].clear();
	return CMD_SUCCESS;
}

std::string toLowerCase(std::string in) {
	std::string cpy(in);
	std::ranges::transform(cpy, cpy.begin(), [](unsigned char c) { return std::tolower(c); });
	return cpy;
}

static bool BindCommand(const std::vector<std::string>& args) {
	if(args.size() > 2) {
		const ImGuiIO* io = &ImGui::GetIO();;
		for (size_t k = 0; k < std::size(io->KeysData); k++) {
			std::string key(ImGui::GetKeyName(k));

			if(toLowerCase(args[1]) == toLowerCase(key)) {
				std::vector<std::string> tmp;
				const char* const delim = " ";
				std::ostringstream imploded;
				std::copy(args.begin() + 2, args.end(), std::ostream_iterator<std::string>(imploded, delim));
				Bindings[k] = imploded.str();
				INFO("Binding '%s' to %s", args[1].c_str(), Bindings[k].c_str());
				break;
			}
		}
	}
	return CMD_SUCCESS;
}

static bool BindToggleCommand(const std::vector<std::string>& args) {
	if (args.size() > 2) {
		const ImGuiIO* io = &ImGui::GetIO();;
		for (size_t k = 0; k < std::size(io->KeysData); k++) {
			std::string key(ImGui::GetKeyName(k));

			if (toLowerCase(args[1]) == toLowerCase(key)) {
				BindingToggle[k] = args[2];
				INFO("Binding toggle '%s' to %s", args[1].c_str(), BindingToggle[k].c_str());
				break;
			}
		}
	}
	return CMD_SUCCESS;
}

std::string BuildUsage(const CommandEntry& entry) {
	std::string usage;
	for (const auto& arg : entry.arguments)
		usage += StringHelper::Sprintf(arg.optional ? "[%s] " : "<%s> ", arg.info.c_str());
	return usage;
}

void Console::Init() {
	this->InputBuffer = new char[MAX_BUFFER_SIZE];
	strcpy(this->InputBuffer, "");
	this->FilterBuffer = new char[MAX_BUFFER_SIZE];
	strcpy(this->FilterBuffer, "");
	this->Commands["help"]  = { HelpCommand, "Shows all the commands" };
	this->Commands["clear"] = { ClearCommand, "Clear the console history" };
	this->Commands["bind"]  = { BindCommand, "Binds key to commands" };
	this->Commands["bind-toggle"] = { BindToggleCommand, "Bind key as a bool toggle" };
}

void Console::Update() {
	for(auto [key, cmd] : Bindings) {
		if (ImGui::IsKeyPressed(key)) Dispatch(cmd);
	}
	for (auto [key, var] : BindingToggle) {
		if (ImGui::IsKeyPressed(key)) {
			CVar* cvar = CVar_Get(var.c_str());
			Dispatch("set " + var + " " + std::to_string(cvar == nullptr ? 0 : !static_cast<bool>(cvar->value.valueS32)));
		}
	}
}

extern "C" uint8_t __enableGameInput;

void Console::Draw() {
	bool input_focus = false;
	__enableGameInput = true;
	if (!this->opened) return;

	ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
	ImGui::Begin("Console", nullptr, ImGuiWindowFlags_NoFocusOnAppearing);
		const ImVec2 pos = ImGui::GetWindowPos();
		const ImVec2 size = ImGui::GetWindowSize();

		__enableGameInput = !ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

		// SohImGui::ShowCursor(ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows | ImGuiHoveredFlags_RectOnly), SohImGui::Dialogues::dConsole);

		// Renders autocomplete window
		if(this->OpenAutocomplete) {
			ImGui::SetNextWindowSize(ImVec2(350, std::min(static_cast<int>(this->Autocomplete.size()), 3) * 20.f), ImGuiCond_Once);
			ImGui::SetNextWindowPos(ImVec2(pos.x + 8, pos.y + size.y - 1));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(3, 3));
			ImGui::Begin("##WndAutocomplete", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove );
			ImGui::BeginChild("AC_Child", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(.3f, .3f, .3f, 1.0f));
			if (ImGui::BeginTable("AC_History", 1)) {
				for (const auto &cmd : this->Autocomplete) {
					std::string usage   = BuildUsage(this->Commands[cmd]);
					std::string preview = cmd + " - " + this->Commands[cmd].description;
					std::string autocomplete = (usage == NULLSTR ? cmd : usage);
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					if (ImGui::Selectable(preview.c_str())) {
						memset(this->InputBuffer, 0, MAX_BUFFER_SIZE);
						memcpy(this->InputBuffer, autocomplete.c_str(), sizeof(char) * autocomplete.size());
						this->OpenAutocomplete = false;
						input_focus = true;
					}
				}
				ImGui::EndTable();
			}
			if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
				this->OpenAutocomplete = false;
			}
			ImGui::PopStyleColor();
			ImGui::EndChild();
			ImGui::End();
			ImGui::PopStyleVar();
		}

		if (ImGui::BeginPopupContextWindow("Context Menu")) {
			if (ImGui::MenuItem("Copy Text")) {
				ImGui::SetClipboardText(this->Log[this->selected_channel][this->selectedId].text.c_str());
				this->selectedId = -1;
			}
			ImGui::EndPopup();
		}
		if (this->selectedId != -1 && ImGui::IsMouseClicked(1)) {
			ImGui::OpenPopup("##WndAutocomplete");
		}

		// Renders top bar filters
		if (ImGui::Button("Clear")) this->Log[this->selected_channel].clear();
		ImGui::SameLine();
		ImGui::SetNextItemWidth(150);
		if (ImGui::BeginCombo("##channel", this->selected_channel.c_str())) {
			for (const auto& channel : log_channels) {
				const bool is_selected = (channel == std::string(this->selected_channel));
				if (ImGui::Selectable(channel.c_str(), is_selected))
					this->selected_channel = channel;
				if (is_selected) ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		ImGui::SameLine();
		ImGui::SetNextItemWidth(150);
		if (ImGui::BeginCombo("##level", this->level_filter.c_str())) {
			for (const auto& filter : priority_filters) {
				const bool is_selected = (filter == std::string(this->level_filter));
				if (ImGui::Selectable(filter.c_str(), is_selected))
				{
					this->level_filter = filter;
					if (is_selected) ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		if (ImGui::InputTextWithHint("##input", "Filter", this->FilterBuffer, MAX_BUFFER_SIZE))this->filter = std::string(this->FilterBuffer);
		ImGui::PopItemWidth();

		// Renders console history
		const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
		ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(.3f, .3f, .3f, 1.0f));
			if (ImGui::BeginTable("History", 1)) {

				if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_DownArrow)))
					if (this->selectedId < (int)this->Log.size() - 1) ++this->selectedId;
				if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_UpArrow)))
					if (this->selectedId > 0) --this->selectedId;

				const std::vector<ConsoleLine> channel = this->Log[this->selected_channel];
				for (int i = 0; i < static_cast<int>(channel.size()); i++) {
					ConsoleLine line = channel[i];
					if(!this->filter.empty() && line.text.find(this->filter) == std::string::npos) continue;
					if(this->level_filter != NULLSTR && line.priority != (std::ranges::find(priority_filters, this->level_filter) - priority_filters.begin()) - 1) continue;
					std::string id = line.text + "##" + std::to_string(i);
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					const bool is_selected = (this->selectedId == i) || std::ranges::find(this->selectedEntries, i) != this->selectedEntries.end();
					ImGui::PushStyleColor(ImGuiCol_Text, this->priority_colors[line.priority]);
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

		// Renders input textfield
		constexpr ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackEdit |
			                                  ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
		ImGui::PushItemWidth(-1);
		if(ImGui::InputTextWithHint("CMDInput", ">", this->InputBuffer, MAX_BUFFER_SIZE, flags, &Console::CallbackStub, this)) {
			input_focus = true;
			if(this->InputBuffer[0] != '\0' && this->InputBuffer[0] != ' ')
				this->Dispatch(std::string(this->InputBuffer));
			memset(this->InputBuffer, 0, MAX_BUFFER_SIZE);
		}

		if(this->CMDHint != NULLSTR) {
			if (ImGui::IsItemFocused()) {
				ImGui::SetNextWindowPos(ImVec2(pos.x, pos.y + size.y));
				ImGui::SameLine();
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
				ImGui::TextUnformatted(this->CMDHint.c_str());
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}
		}

		ImGui::SetItemDefaultFocus();
		if (input_focus) ImGui::SetKeyboardFocusHere(-1);
		ImGui::PopItemWidth();
	ImGui::End();
}

void Console::Dispatch(const std::string& line) {
	this->CMDHint = NULLSTR;
	this->History.push_back(line);
	this->Log[this->selected_channel].push_back({ "> " + line } );
	const std::vector<std::string> cmd_args = StringHelper::Split(line, " ");
	if (this->Commands.contains(cmd_args[0])) {
		const CommandEntry entry = this->Commands[cmd_args[0]];
		if(!entry.handler(cmd_args) && !entry.arguments.empty())
			this->Log[this->selected_channel].push_back({ "[SOH] Usage: " + cmd_args[0] + " " + BuildUsage(entry), ERROR_LVL});
		return;
	}
	this->Log[this->selected_channel].push_back({ "[SOH] Command not found", ERROR_LVL });
}

int Console::CallbackStub(ImGuiInputTextCallbackData* data) {
	const auto instance = static_cast<Console*>(data->UserData);
	const bool empty_history = instance->History.empty();
	const int  history_index = instance->HistoryIndex;
	std::string history;

	switch(data->EventKey) {
		case ImGuiKey_Tab:
			instance->Autocomplete.clear();
			for (auto& [cmd, entry] : instance->Commands)
				if (cmd.find(std::string(data->Buf)) != std::string::npos) instance->Autocomplete.push_back(cmd);
			instance->OpenAutocomplete = !instance->Autocomplete.empty();
			instance->CMDHint = NULLSTR;
			break;
		case ImGuiKey_UpArrow:
			if (empty_history) break;
			if(history_index < static_cast<int>(instance->History.size()) - 1) instance->HistoryIndex += 1;
			data->DeleteChars(0, data->BufTextLen);
			data->InsertChars(0, instance->History[instance->HistoryIndex].c_str());
			instance->CMDHint = NULLSTR;
			break;
		case ImGuiKey_DownArrow:
			if (empty_history) break;
			if (history_index > -1) instance->HistoryIndex -= 1;
			data->DeleteChars(0, data->BufTextLen);
			if(history_index >= 0)
				data->InsertChars(0, instance->History[history_index].c_str());
			instance->CMDHint = NULLSTR;
			break;
		case ImGuiKey_Escape:
			instance->HistoryIndex = -1;
			data->DeleteChars(0, data->BufTextLen);
			instance->OpenAutocomplete = false;
			instance->CMDHint = NULLSTR;
			break;
		default:
			instance->OpenAutocomplete = false;
			for (auto& [cmd, entry] : instance->Commands) {
				const std::vector<std::string> cmd_args = StringHelper::Split(std::string(data->Buf), " ");
				if (data->BufTextLen > 2 && !cmd_args.empty() && cmd.find(cmd_args[0]) != std::string::npos) {
					instance->CMDHint = cmd + " " + BuildUsage(entry);
					break;
				}
				instance->CMDHint = NULLSTR;
			}
	}
	return 0;
}

void Console::Append(const std::string& channel, Priority priority, const char* fmt, ...) {
	char buf[1024];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
	buf[IM_ARRAYSIZE(buf) - 1] = 0;
	va_end(args);
	this->Log[channel].push_back({ std::string(buf), priority });
}
