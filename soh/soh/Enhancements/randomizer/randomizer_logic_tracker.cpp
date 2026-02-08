#include "randomizer_logic_tracker.h"

#include "location_access.h"
#include "entrance.h"
#include "logic_expression.h"
#include "../../OTRGlobals.h"

struct ExpressionTable {
    struct ExpressionRow {
        std::shared_ptr<LogicExpression> Expression;
        std::vector<ExpressionRow> Children;
        std::optional<LogicExpression::ValueVariant> ChildDay;
        std::optional<LogicExpression::ValueVariant> ChildNight;
        std::optional<LogicExpression::ValueVariant> AdultDay;
        std::optional<LogicExpression::ValueVariant> AdultNight;
        bool Expanded = false;
        std::string ErrorMessage;
    };

    ExpressionRow Root;
    bool CombineAll = false;
    bool CombineChild = false;
    bool CombineAdult = false;
};

struct LogicTrackerNode {
    struct Connection {
        std::string ParentName;
        RandomizerRegion ParentRandomizerRegion = RR_NONE;
        ExpressionTable ExpressionTable;

        bool ChildDayAccess = false;
        bool ChildNightAccess = false;
        bool AdultDayAccess = false;
        bool AdultNightAccess = false;
        bool IsDiscovered = true;
    };

    std::string NodeName;
    std::vector<Connection> Connections;
    int NodeId = -1;
    RandomizerRegion RandomizerRegion = RR_NONE;
};

struct RandomizerEventInfo {
    std::string ConditionStr;
    RandomizerRegion RandomizerRegion = RR_NONE;
};

std::unordered_map<std::string, std::vector<RandomizerEventInfo>> randomizerEventMap;

bool expandingNode = false;
int expandNodeId = -1;
int clearNodesAfterNodeID = -1;
std::vector<LogicTrackerNode> nodes;

static ExpressionTable::ExpressionRow CreateExpressionRows(const std::shared_ptr<LogicExpression>& expression) {
    ExpressionTable::ExpressionRow row;
    row.Expression = expression;
    row.Expanded = false;

    const auto& children = expression->GetChildren();
    row.Children.reserve(children.size());
    for (const auto& child : children) {
        row.Children.emplace_back(CreateExpressionRows(child));
    }

    return row;
}

enum class AgeTime { ChildDay, ChildNight, AdultDay, AdultNight };

static void PopulateExpressionValues(ExpressionTable::ExpressionRow& row, const ExpressionEvaluation& eval,
                                     AgeTime ageTime) {
    if (ageTime == AgeTime::ChildDay) {
        row.ChildDay = eval.Result;
    } else if (ageTime == AgeTime::ChildNight) {
        row.ChildNight = eval.Result;
    } else if (ageTime == AgeTime::AdultDay) {
        row.AdultDay = eval.Result;
    } else if (ageTime == AgeTime::AdultNight) {
        row.AdultNight = eval.Result;
    }

    for (auto& rowChild : row.Children) {
        for (const auto& evalChild : eval.Children) {
            if (rowChild.Expression == evalChild.Expression) {
                PopulateExpressionValues(rowChild, evalChild, ageTime);
                break;
            }
        }
    }
}

static std::tuple<bool, bool, bool> CalculateCombines(const ExpressionTable::ExpressionRow& row) {
    bool combineChild = row.ChildDay == row.ChildNight;
    bool combineAdult = row.AdultDay == row.AdultNight;
    bool combineAll = combineChild && combineAdult && row.ChildDay == row.AdultDay;
    for (const auto& child : row.Children) {
        auto [childCombineAll, childCombineChild, childCombineAdult] = CalculateCombines(child);
        combineAll &= childCombineAll;
        combineChild &= childCombineChild;
        combineAdult &= childCombineAdult;
    }
    return { combineAll, combineChild, combineAdult };
}

static void PopulateConnectionExpression(LogicTrackerNode::Connection& connection, const std::string& expressionStr) {
    std::shared_ptr<LogicExpression> expression;

    try {
        expression = LogicExpression::Parse(expressionStr);
    } catch (const std::exception& e) {
        connection.ExpressionTable.Root.ErrorMessage = std::string("Parse Error: ") + e.what();
        connection.ExpressionTable.Root.Children.clear();
        connection.ExpressionTable.CombineAll = true;
        return;
    }

    try {
        logic->IsChild = false;
        logic->IsAdult = false;
        logic->AtDay = false;
        logic->AtNight = false;

        connection.ExpressionTable.Root = CreateExpressionRows(expression);

        if (connection.ChildDayAccess) {
            logic->IsChild = true;
            logic->AtDay = true;

            const auto& eval = EvaluateExpression(connection.ExpressionTable.Root.Expression);
            PopulateExpressionValues(connection.ExpressionTable.Root, eval, AgeTime::ChildDay);

            logic->IsChild = false;
            logic->AtDay = false;
        }
        if (connection.ChildNightAccess) {
            logic->IsChild = true;
            logic->AtNight = true;

            const auto& eval = EvaluateExpression(connection.ExpressionTable.Root.Expression);
            PopulateExpressionValues(connection.ExpressionTable.Root, eval, AgeTime::ChildNight);

            logic->IsChild = false;
            logic->AtNight = false;
        }
        if (connection.AdultDayAccess) {
            logic->IsAdult = true;
            logic->AtDay = true;

            const auto& eval = EvaluateExpression(connection.ExpressionTable.Root.Expression);
            PopulateExpressionValues(connection.ExpressionTable.Root, eval, AgeTime::AdultDay);

            logic->IsAdult = false;
            logic->AtDay = false;
        }
        if (connection.AdultNightAccess) {
            logic->IsAdult = true;
            logic->AtNight = true;

            const auto& eval = EvaluateExpression(connection.ExpressionTable.Root.Expression);
            PopulateExpressionValues(connection.ExpressionTable.Root, eval, AgeTime::AdultNight);

            logic->IsAdult = false;
            logic->AtNight = false;
        }
    } catch (const std::exception& e) {
        connection.ExpressionTable.Root.ErrorMessage = std::string("Eval Error: ") + e.what();
        connection.ExpressionTable.Root.Children.clear();
        connection.ExpressionTable.CombineAll = true;
        return;
    }

    auto [combineAll, combineChild, combineAdult] = CalculateCombines(connection.ExpressionTable.Root);
    connection.ExpressionTable.CombineAll = combineAll;
    connection.ExpressionTable.CombineChild = combineChild;
    connection.ExpressionTable.CombineAdult = combineAdult;
}

RandomizerCheck showRandomizerCheck = RC_UNKNOWN_CHECK;

void LogicTrackerWindow::ShowRandomizerCheck(RandomizerCheck randomizerCheck) {
    showRandomizerCheck = randomizerCheck;

    auto window = Ship::Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow("Logic Tracker");
    window->Show();
    ImGui::SetWindowFocus(window->GetName().c_str());
}

static void CalculateShowRandomizerCheck() {
    if (showRandomizerCheck == RC_UNKNOWN_CHECK) {
        return;
    }
    logic->CurrentCheckKey = showRandomizerCheck;

    const auto& location = Rando::StaticData::GetLocation(showRandomizerCheck);
    nodes.clear();

    LogicTrackerNode node;
    node.NodeName = "Check: " + location->GetName();
    node.NodeId = nodes.size();

    for (int randomizerRegion = RR_NONE; randomizerRegion < RR_MAX; ++randomizerRegion) {
        const auto& region = areaTable[randomizerRegion];
        for (const auto& locationAccess : region.locations) {
            if (locationAccess.GetLocation() == showRandomizerCheck) {
                logic->CurrentRegionKey = RandomizerRegion(randomizerRegion);

                LogicTrackerNode::Connection connection;
                connection.ParentName = "Region: " + region.regionName;
                connection.ParentRandomizerRegion = RandomizerRegion(randomizerRegion);
                connection.ChildDayAccess = region.childDay;
                connection.ChildNightAccess = region.childNight;
                connection.AdultDayAccess = region.adultDay;
                connection.AdultNightAccess = region.adultNight;

                PopulateConnectionExpression(connection, locationAccess.GetConditionStr());

                node.Connections.emplace_back(std::move(connection));
                logic->CurrentRegionKey = RR_NONE;
            }
        }
    }

    nodes.emplace_back(std::move(node));

    expandNodeId = node.NodeId;

    logic->CurrentCheckKey = RC_UNKNOWN_CHECK;
    showRandomizerCheck = RC_UNKNOWN_CHECK;
}

RandomizerRegion showToRandomizerRegion = RR_NONE;
RandomizerRegion showFromRandomizerRegion = RR_NONE;

void LogicTrackerWindow::ShowRandomizerRegion(RandomizerRegion toRandomizerRegion,
                                              RandomizerRegion fromRandomizerRegion) {
    showToRandomizerRegion = toRandomizerRegion;
    showFromRandomizerRegion = fromRandomizerRegion;
}

static void CalculateShowRandomizerRegion() {
    if (showToRandomizerRegion == RR_NONE) {
        return;
    }

    const auto& region = RegionTable(showToRandomizerRegion);

    LogicTrackerNode node;
    node.NodeName = "Region: " + region->regionName;
    node.NodeId = nodes.size();
    node.RandomizerRegion = showToRandomizerRegion;

    for (const auto& entrance : region->entrances) {
        if (entrance->GetParentRegionKey() == showFromRandomizerRegion) {
            continue;
        }
        logic->CurrentRegionKey = entrance->GetParentRegionKey();

        const auto& parentRegion = entrance->GetParentRegion();

        LogicTrackerNode::Connection connection;
        connection.ParentName = "Region: " + parentRegion->regionName;
        if (entrance->IsShuffled()) {
            const auto& originalConnectedRegion = RegionTable(entrance->GetOriginalConnectedRegionKey());
            connection.ParentName += ",    Originally to: " + originalConnectedRegion->regionName;
            connection.IsDiscovered =
                Entrance_GetIsEntranceDiscovered(entrance->GetIndex()) || logic->ACProcessUndiscoveredExits;
        }
        connection.ParentRandomizerRegion = entrance->GetParentRegionKey();
        connection.ChildDayAccess = parentRegion->childDay;
        connection.ChildNightAccess = parentRegion->childNight;
        connection.AdultDayAccess = parentRegion->adultDay;
        connection.AdultNightAccess = parentRegion->adultNight;

        PopulateConnectionExpression(connection, entrance->GetConditionStr());

        node.Connections.emplace_back(std::move(connection));
        logic->CurrentRegionKey = RR_NONE;
    }

    nodes.emplace_back(std::move(node));

    expandNodeId = node.NodeId;

    showToRandomizerRegion = RR_NONE;
    showFromRandomizerRegion = RR_NONE;
}

std::string showEvent = "";

void LogicTrackerWindow::ShowRandomizerEvent(const std::string& event) {
    showEvent = event;
}

static void CalculateShowRandomizerEvent() {
    if (showEvent.empty()) {
        return;
    }

    const auto& events = randomizerEventMap.find(showEvent);
    if (events == randomizerEventMap.end()) {
        return;
    }

    LogicTrackerNode node;
    node.NodeName = "Event: " + showEvent;
    node.NodeId = nodes.size();

    for (const auto& eventInfo : events->second) {
        logic->CurrentRegionKey = eventInfo.RandomizerRegion;
        const auto& region = areaTable[eventInfo.RandomizerRegion];

        LogicTrackerNode::Connection connection;
        connection.ParentName = "Region: " + region.regionName;
        connection.ParentRandomizerRegion = eventInfo.RandomizerRegion;
        connection.ChildDayAccess = region.childDay;
        connection.ChildNightAccess = region.childNight;
        connection.AdultDayAccess = region.adultDay;
        connection.AdultNightAccess = region.adultNight;

        PopulateConnectionExpression(connection, eventInfo.ConditionStr);

        node.Connections.emplace_back(std::move(connection));
        logic->CurrentRegionKey = RR_NONE;
    }

    nodes.emplace_back(std::move(node));

    expandNodeId = node.NodeId;

    showEvent.clear();
}

static std::string ToString(const std::optional<LogicExpression::ValueVariant>& value) {
    if (!value.has_value()) {
        return "";
    }
    return ToString(value.value());
}

static void DrawColoredWrappedText(const std::vector<std::pair<ImVec4, std::string>>& segments) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    float wrapWidth = ImGui::GetContentRegionAvail().x;
    float x = pos.x;
    float y = pos.y;
    float startX = x;
    ImFont* font = ImGui::GetFont();
    float fontSize = ImGui::GetFontSize();

    for (const auto& [color, text] : segments) {
        std::istringstream iss(text + " ");
        std::string word;
        bool firstWordInSegment = true;
        while (std::getline(iss, word, ' ')) {
            std::string toDraw = word;
            if (!firstWordInSegment) {
                toDraw = " " + word;
            }
            ImVec2 textSize = ImGui::CalcTextSize(toDraw.c_str());
            if (x + textSize.x > startX + wrapWidth && x > startX) {
                x = startX;
                y += fontSize;
                toDraw = word;
                textSize = ImGui::CalcTextSize(toDraw.c_str());
            }
            draw_list->AddText(font, fontSize, ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(color), toDraw.c_str());
            x += textSize.x;
            firstWordInSegment = false;
        }
    }
    ImGui::SetCursorScreenPos(ImVec2(pos.x, y + fontSize));
}

static void DrawCondition(const LogicExpression& expression) {
    static ImVec4 fontColors[] = {
        ImVec4(0.5f, 0.5f, 1.0f, 1.0f), // Blue
        ImVec4(0.5f, 1.0f, 0.5f, 1.0f), // Green
        ImVec4(1.0f, 0.5f, 0.5f, 1.0f), // Red
        ImVec4(1.0f, 1.0f, 0.5f, 1.0f), // Yellow
        ImVec4(1.0f, 0.5f, 1.0f, 1.0f), // Magenta
        ImVec4(0.5f, 1.0f, 1.0f, 1.0f), // Cyan
    };
    static size_t fontColorsLength = std::size(fontColors);
    const ImVec4 defaultColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);

    LogicExpression::Type type = expression.GetType();
    const auto& children = expression.GetChildren();

    if (type == LogicExpression::Type::And || type == LogicExpression::Type::Or ||
        type == LogicExpression::Type::Comparison || type == LogicExpression::Type::Add ||
        type == LogicExpression::Type::Subtract || type == LogicExpression::Type::Multiply ||
        type == LogicExpression::Type::Divide) {
        std::vector<std::pair<ImVec4, std::string>> segments;
        for (size_t i = 0; i < children.size(); ++i) {
            segments.emplace_back(fontColors[i % fontColorsLength], children[i]->ToString());
            if (i < children.size() - 1) {
                segments.emplace_back(defaultColor, " " + expression.GetOperation() + " ");
            }
        }
        DrawColoredWrappedText(segments);
    } else if (type == LogicExpression::Type::Not) {
        DrawColoredWrappedText({ { defaultColor, "!" }, { fontColors[0], children[0]->ToString() } });
    } else if (type == LogicExpression::Type::FunctionCall) {
        std::vector<std::pair<ImVec4, std::string>> segments;
        segments.emplace_back(fontColors[0], expression.GetFunctionName());
        segments.emplace_back(defaultColor, "(");
        for (size_t i = 0; i < children.size(); ++i) {
            segments.emplace_back(fontColors[(i + 1) % fontColorsLength], children[i]->ToString());
            if (i < children.size() - 1) {
                segments.emplace_back(defaultColor, ", ");
            }
        }
        segments.emplace_back(defaultColor, ")");
        DrawColoredWrappedText(segments);
    } else if (type == LogicExpression::Type::Ternary) {
        DrawColoredWrappedText({ { fontColors[0], children[0]->ToString() },
                                 { defaultColor, " ? " },
                                 { fontColors[1], children[1]->ToString() },
                                 { defaultColor, " : " },
                                 { fontColors[2], children[2]->ToString() } });
    } else if (type == LogicExpression::Type::Value) {
        DrawColoredWrappedText({ { fontColors[0], expression.ToString() } });
    } else {
        DrawColoredWrappedText({ { defaultColor, expression.ToString() } });
    }
}

static void DrawExpressionRow(const LogicTrackerNode& node, const ExpressionTable& table,
                              ExpressionTable::ExpressionRow& row, int level) {
    const auto& expression = *row.Expression;

    ImGui::TableNextRow();
    ImGui::PushID(&row);
    ImGui::PushFont(OTRGlobals::Instance->fontMono);
    if (level > 0) {
        ImGui::Indent(20.0f);
    }

    ImGui::TableNextColumn();
    if (!row.Children.empty()) {
        if (ImGui::Button(std::to_string(level).c_str())) {
            row.Expanded = !row.Expanded;
        }
    } else {
        ImGui::BeginDisabled();
        ImGui::Button(std::to_string(level).c_str());
        ImGui::EndDisabled();

        if (expression.GetType() == LogicExpression::Type::Value &&
            expression.GetValueType() == LogicExpression::ValueType::Enum) {
            std::string identifier = expression.ToString();
            auto it = randomizerEventMap.find(identifier);
            if (it != randomizerEventMap.end()) {
                ImGui::SameLine();
                if (ImGui::Button(ICON_FA_COGS)) {
                    LogicTrackerWindow::ShowRandomizerEvent(identifier);
                    clearNodesAfterNodeID = node.NodeId;
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Show Event Logic");
                }
            }
        }
    }

    ImGui::TableNextColumn();
    DrawCondition(expression);
    if (!row.ErrorMessage.empty()) {
        ImGui::TextWrapped("%s", row.ErrorMessage.c_str());
    }

    ImGui::TableNextColumn();
    ImGui::TextUnformatted(ToString(row.ChildDay).c_str());

    if (!table.CombineAll) {
        if (!table.CombineChild) {
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(ToString(row.ChildNight).c_str());
        }

        ImGui::TableNextColumn();
        ImGui::TextUnformatted(ToString(row.AdultDay).c_str());

        if (!table.CombineAdult) {
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(ToString(row.AdultNight).c_str());
        }
    }

    if (row.Expanded) {
        for (auto& child : row.Children) {
            DrawExpressionRow(node, table, child, level + 1);
        }
    }

    if (level > 0) {
        ImGui::Unindent(20.0f);
    }
    ImGui::PopFont();
    ImGui::PopID();
}

static void DrawExpressionTable(const LogicTrackerNode& node, ExpressionTable& table) {
    int columnCount = 3;
    if (!table.CombineAll) {
        if (!table.CombineChild) {
            columnCount += 1;
        }
        columnCount += 1;
        if (!table.CombineAdult) {
            columnCount += 1;
        }
    }

    ImGui::PushID(&table);
    if (ImGui::BeginTable("", columnCount,
                          ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit)) {
        ImGui::TableSetupColumn("Level", ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Expression", ImGuiTableColumnFlags_WidthStretch);
        if (table.CombineAll) {
            ImGui::TableSetupColumn("All", ImGuiTableColumnFlags_WidthFixed);
        } else {
            if (table.CombineChild) {
                ImGui::TableSetupColumn("Child", ImGuiTableColumnFlags_WidthFixed);
            } else {
                ImGui::TableSetupColumn("Child Day", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Child Night", ImGuiTableColumnFlags_WidthFixed);
            }
            if (table.CombineAdult) {
                ImGui::TableSetupColumn("Adult", ImGuiTableColumnFlags_WidthFixed);
            } else {
                ImGui::TableSetupColumn("Adult Day", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Adult Night", ImGuiTableColumnFlags_WidthFixed);
            }
        }
        ImGui::TableHeadersRow();

        DrawExpressionRow(node, table, table.Root, 0);

        ImGui::EndTable();
    }
    ImGui::PopID();
}

static void DrawNodeConnection(const LogicTrackerNode& node, LogicTrackerNode::Connection& connection) {
    DrawExpressionTable(node, connection.ExpressionTable);
}

static std::string GetAccessString(const LogicTrackerNode::Connection& connection) {
    std::string access = "";
    if (connection.ChildDayAccess && connection.ChildNightAccess && connection.AdultDayAccess &&
        connection.AdultNightAccess) {
        access += "All";
    } else if (connection.ChildDayAccess || connection.ChildNightAccess || connection.AdultDayAccess ||
               connection.AdultNightAccess) {
        std::vector<std::string> accessParts;
        if (connection.ChildDayAccess && connection.ChildNightAccess) {
            accessParts.push_back("Child");
        } else {
            if (connection.ChildDayAccess) {
                accessParts.push_back("Child Day");
            }
            if (connection.ChildNightAccess) {
                accessParts.push_back("Child Night");
            }
        }
        if (connection.AdultDayAccess && connection.AdultNightAccess) {
            accessParts.push_back("Adult");
        } else {
            if (connection.AdultDayAccess) {
                accessParts.push_back("Adult Day");
            }
            if (connection.AdultNightAccess) {
                accessParts.push_back("Adult Night");
            }
        }
        if (!accessParts.empty()) {
            for (size_t i = 0; i < accessParts.size(); ++i) {
                if (i > 0) {
                    access += ", ";
                }
                access += accessParts[i];
            }
        }
    } else {
        access = "None";
    }
    return access;
}

static bool ToBool(const std::optional<LogicExpression::ValueVariant>& value) {
    if (!value.has_value()) {
        return false;
    }
    return LogicExpression::GetValue<bool>(value.value());
}

static std::string GetAvailableString(const LogicTrackerNode::Connection& connection) {
    const auto& expressionRow = connection.ExpressionTable.Root;

    bool childDay = ToBool(expressionRow.ChildDay);
    bool childNight = ToBool(expressionRow.ChildNight);
    bool adultDay = ToBool(expressionRow.AdultDay);
    bool adultNight = ToBool(expressionRow.AdultNight);

    std::string available = "";
    if (childDay && childNight && adultDay && adultNight) {
        available += "All";
    } else if (childDay || childNight || adultDay || adultNight) {
        std::vector<std::string> availableParts;
        if (childDay && childNight) {
            availableParts.push_back("Child");
        } else {
            if (childDay) {
                availableParts.push_back("Child Day");
            }
            if (childNight) {
                availableParts.push_back("Child Night");
            }
        }
        if (adultDay && adultNight) {
            availableParts.push_back("Adult");
        } else {
            if (adultDay) {
                availableParts.push_back("Adult Day");
            }
            if (adultNight) {
                availableParts.push_back("Adult Night");
            }
        }
        if (!availableParts.empty()) {
            for (size_t i = 0; i < availableParts.size(); ++i) {
                if (i > 0) {
                    available += ", ";
                }
                available += availableParts[i];
            }
        }
    } else {
        available = "None";
    }
    return available;
}

static void DrawNode(LogicTrackerNode& node) {
    ImGui::PushID(node.NodeId);

    if (expandingNode) {
        ImGui::SetNextItemOpen(expandNodeId == node.NodeId, ImGuiCond_Always);
    }

    bool nodeOpen = ImGui::CollapsingHeader(("To " + node.NodeName).c_str(),
                                            ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_SpanFullWidth);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Show Connections");
    }

    if (nodeOpen) {
        ImGui::Indent(25.0f);

        for (int i = 0; i < node.Connections.size(); i++) {
            auto& connection = node.Connections[i];

            if (!connection.IsDiscovered) {
                ImGui::BeginDisabled();
                ImGui::CollapsingHeader("Undiscovered Entrance",
                                        ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_SpanAvailWidth);
                ImGui::EndDisabled();
                continue;
            }

            ImGui::PushID(connection.ParentName.c_str());

            if (expandingNode && node.Connections.size() == 1) {
                ImGui::SetNextItemOpen(expandNodeId == node.NodeId, ImGuiCond_Always);
            }

            bool connectionOpen =
                ImGui::CollapsingHeader(("From " + connection.ParentName).c_str(),
                                        ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_SpanAvailWidth);
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Show Connection Logic");
            }

            std::string accessAvailable =
                GetAccessString(connection) + " " + ICON_FA_ARROW_RIGHT + " " + GetAvailableString(connection);

            ImGui::SameLine(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(accessAvailable.c_str()).x);
            ImGui::TextUnformatted(accessAvailable.c_str());
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Connection Access " ICON_FA_ARROW_RIGHT " Available");
            }

            ImGui::SameLine();
            if (ImGui::Button(ICON_FA_COGS)) {
                LogicTrackerWindow::ShowRandomizerRegion(connection.ParentRandomizerRegion, node.RandomizerRegion);
                clearNodesAfterNodeID = node.NodeId;
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Show Region Logic");
            }

            if (connectionOpen) {
                DrawNodeConnection(node, connection);
            }

            ImGui::PopID();
        }

        ImGui::Unindent(25.0f);
    }

    ImGui::PopID();
}

static void PopulateRandomizerEventMap() {
    if (!randomizerEventMap.empty()) {
        return;
    }

    for (int i = RR_NONE; i < RR_MAX; i++) {
        for (const auto& event : areaTable[i].events) {
            const auto& eventStr = event.GetEventStr();
            auto it = randomizerEventMap.find(eventStr);
            auto eventInfo = RandomizerEventInfo{ event.GetConditionStr(), RandomizerRegion(i) };
            if (it != randomizerEventMap.end()) {
                it->second.emplace_back(std::move(eventInfo));
            } else {
                randomizerEventMap.emplace(eventStr, std::vector<RandomizerEventInfo>{ std::move(eventInfo) });
            }
        }
    }
}

void LogicTrackerWindow::DrawElement() {
    PopulateRandomizerEventMap();
    CalculateShowRandomizerCheck();
    CalculateShowRandomizerRegion();
    CalculateShowRandomizerEvent();

    if (expandNodeId != -1) {
        expandingNode = true;
    }

    for (LogicTrackerNode& node : nodes) {
        DrawNode(node);
    }

    if (expandingNode) {
        expandNodeId = -1;
        expandingNode = false;
    }

    if (clearNodesAfterNodeID != -1) {
        while (nodes.back().NodeId > clearNodesAfterNodeID) {
            nodes.pop_back();
        }
        clearNodesAfterNodeID = -1;
    }
}

void LogicTrackerWindow::InitElement() {
    return;
}

void LogicTrackerWindow::UpdateElement() {
    return;
}