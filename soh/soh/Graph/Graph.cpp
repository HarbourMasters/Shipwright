#include "Graph.h"

#include <libultraship/luslog.h>

#include <algorithm>
#include <cmath>

typedef struct Rect {
    ImVec2 min;
    ImVec2 max;
} Rect;

Rect GetVisibleWorldRect(ImVec2 canvasSize, ImVec2 canvasPos, ImVec2 cameraOffset, float zoom) noexcept {
    Rect r;

    r.min = (-cameraOffset) / zoom;
    r.max = (canvasSize - cameraOffset) / zoom;

    return r;
}

static bool IsNodeVisible(const Node& node, const Rect& view, float zoom, GraphNodeOptions options,
                          bool scaleWithZoom) noexcept {
    float radius = options.baseSize * (scaleWithZoom ? zoom : 1);

    return !(node.position.x + radius < view.min.x || node.position.x - radius > view.max.x ||
             node.position.y + radius < view.min.y || node.position.y - radius > view.max.y);
}

static bool IsEdgeVisible(ImVec2 a, ImVec2 b, const Rect& view) noexcept {
    float minX = std::min(a.x, b.x);
    float maxX = std::max(a.x, b.x);

    float minY = std::min(a.y, b.y);
    float maxY = std::max(a.y, b.y);

    return !(maxX < view.min.x || minX > view.max.x || maxY < view.min.y || minY > view.max.y);
}

ImVec2 WorldSpaceToScreenSpace(ImVec2 vec, ImVec2 canvasPos, ImVec2 cameraOffset, float zoom) noexcept {
    return canvasPos + cameraOffset + vec * zoom;
}

Node Node::New(ImVec2 position, std::string label, ImU32 color, ImU32 labelColor) noexcept {
    return { position, { 0, 0 }, label, { -1, -1 }, color, labelColor };
}

void Node::Draw(ImDrawList* draw, ImVec2 canvasPos, ImVec2 cameraOffset, float zoom, GraphNodeOptions options,
                bool scaleWithZoom) noexcept {
    ImVec2 screenPos = WorldSpaceToScreenSpace(this->position, canvasPos, cameraOffset, zoom);

    draw->AddCircleFilled(screenPos, options.baseSize * (scaleWithZoom ? zoom : 1), this->color);
}

void Node::DrawLabel(ImDrawList* draw, ImVec2 canvasPos, ImVec2 cameraOffset, float zoom,
                     GraphNodeOptions options) noexcept {
    if (this->labelSize.x == -1) {
        this->labelSize = ImGui::CalcTextSize(label.c_str());
    }

    ImU32 labelBackgroundColor = options.label.backgroundColor;

    if (options.label.fadeout) {
        float alpha = (std::clamp(zoom, options.label.fadeoutCutoffLower, options.label.fadeoutCutoffUpper) -
                       options.label.fadeoutCutoffLower) /
                      (options.label.fadeoutCutoffUpper - options.label.fadeoutCutoffLower) * 255;
        ImU32 byteAlpha = static_cast<ImU32>(alpha);
        this->labelColor &= ~IM_COL32_A_MASK;
        this->labelColor |= byteAlpha << IM_COL32_A_SHIFT;

        float alphaBackground = (std::clamp(zoom, options.label.fadeoutCutoffLower, options.label.fadeoutCutoffUpper) -
                                 options.label.fadeoutCutoffLower) /
                                (options.label.fadeoutCutoffUpper - options.label.fadeoutCutoffLower) * 220;
        if (alpha == 0 && alphaBackground == 0) {
            return;
        }
        ImU32 byteAlphaBackground = static_cast<ImU32>(alpha);
        labelBackgroundColor &= ~IM_COL32_A_MASK;
        labelBackgroundColor |= byteAlphaBackground << IM_COL32_A_SHIFT;
    }

    ImVec2 screenPos = WorldSpaceToScreenSpace(this->position, canvasPos, cameraOffset, zoom);

    ImVec2 min = screenPos - this->labelSize * 0.5f - options.label.padding;

    ImVec2 max = screenPos + this->labelSize * 0.5f + options.label.padding;

    draw->AddRectFilled(min, max, labelBackgroundColor, options.label.rounding);

    draw->AddText(screenPos - this->labelSize * 0.5f, this->labelColor, this->label.c_str());
}

Edge Edge::New(int src, int dst, std::string label, ImU32 color, ImU32 labelColor) noexcept {
    return { src, dst, label, { -1, -1 }, color, labelColor };
}

void Edge::Draw(ImDrawList* draw, ImVec2 canvasPos, ImVec2 cameraOffset, float zoom, GraphEdgeOptions options,
                bool scaleWithZoom, const std::vector<Node>& nodes) noexcept {
    // LUSLOG_INFO("[Edge::Draw] Drawing edge: src = %d | dst = %d | label = \"%s\"", this->src, this->dst,
    // this->label.c_str());

    if (this->src >= nodes.size() || this->dst >= nodes.size()) {
        LUSLOG_ERROR("[Edge::Draw] Invalid src (%d) or dst (%d) for node list of size (%d)", this->src, this->dst,
                     nodes.size());
        assert(false);
        return;
    }

    ImVec2 a = nodes[this->src].position;
    ImVec2 b = nodes[this->dst].position;

    draw->AddLine(WorldSpaceToScreenSpace(a, canvasPos, cameraOffset, zoom),
                  WorldSpaceToScreenSpace(b, canvasPos, cameraOffset, zoom), this->color,
                  options.thickness * (scaleWithZoom ? zoom : 1));
}

void Edge::DrawLabel(ImDrawList* draw, ImVec2 canvasPos, ImVec2 cameraOffset, float zoom, GraphEdgeOptions options,
                     ImVec2 a, ImVec2 b) noexcept {
    ImVec2 delta = b - a;

    float len = std::sqrt(delta.x * delta.x + delta.y * delta.y);

    if (len < 1e-5f) {
        return;
    }

    ImVec2 normal = { -delta.y / len, delta.x / len };

    ImVec2 labelPosWorld = a * 0.6f + b * 0.4f + normal * options.label.separation;

    ImVec2 labelPosScreen = WorldSpaceToScreenSpace(labelPosWorld, canvasPos, cameraOffset, zoom);

    if (this->labelSize.x == -1) {
        this->labelSize = ImGui::CalcTextSize(label.c_str());
    }

    ImU32 labelBackgroundColor = options.label.backgroundColor;

    if (options.label.fadeout) {
        float alpha = (std::clamp(zoom, options.label.fadeoutCutoffLower, options.label.fadeoutCutoffUpper) -
                       options.label.fadeoutCutoffLower) /
                      (options.label.fadeoutCutoffUpper - options.label.fadeoutCutoffLower) * 255;
        ImU32 byteAlpha = static_cast<ImU32>(alpha);
        this->labelColor &= ~IM_COL32_A_MASK;
        this->labelColor |= byteAlpha << IM_COL32_A_SHIFT;

        float alphaBackground = (std::clamp(zoom, options.label.fadeoutCutoffLower, options.label.fadeoutCutoffUpper) -
                                 options.label.fadeoutCutoffLower) /
                                (options.label.fadeoutCutoffUpper - options.label.fadeoutCutoffLower) * 220;
        if (alpha == 0 && alphaBackground == 0) {
            return;
        }
        ImU32 byteAlphaBackground = static_cast<ImU32>(alpha);
        labelBackgroundColor &= ~IM_COL32_A_MASK;
        labelBackgroundColor |= byteAlphaBackground << IM_COL32_A_SHIFT;
    }

    ImVec2 min = labelPosScreen - this->labelSize * 0.5f - options.label.padding;

    ImVec2 max = labelPosScreen + this->labelSize * 0.5f + options.label.padding;

    draw->AddRectFilled(min, max, labelBackgroundColor, options.label.rounding);

    draw->AddText(labelPosScreen - this->labelSize * 0.5f, this->labelColor, this->label.c_str());
}

Graph Graph::New(std::vector<Node> nodes, std::vector<Edge> edges, GraphOptions options) noexcept {
    return Graph(nodes, edges, options);
}

Graph::Graph(std::vector<Node> _nodes, std::vector<Edge> _edges, GraphOptions _options)
    : nodes(_nodes), edges(_edges), options(_options) {
}

#ifdef _MSC_VER
#pragma optimize("t", on)
#else // msvc complains about unknown attributes
[[gnu::hot, gnu::optimize(3)]]
#endif
float Graph::StabilizeStep(float width, float height, float temperature) noexcept {
    const float area = width * height;
    const float k = std::sqrt(area / nodes.size());

    const float repulsionK = options.forceMultipliers.repulsion * k * k;
    const float attractionK = options.forceMultipliers.attraction / k;

    // reset displacement
    for (auto& n : this->nodes) {
        n.displacement = { 0, 0 };
    }

    // repulsive forces O(n^2)
    for (size_t i = 0; i < this->nodes.size(); i += 1) {
        for (size_t j = i + 1; j < this->nodes.size(); j += 1) {
            ImVec2 delta = this->nodes[i].position - this->nodes[j].position;

            float distSquared = std::max(delta.x * delta.x + delta.y * delta.y, 0.01f);

            ImVec2 displacement = delta * repulsionK / distSquared;

            this->nodes[i].displacement += displacement;
            this->nodes[j].displacement -= displacement;
        }
    }

    // attractive forces O(n)
    for (const auto& e : this->edges) {
        auto& u = this->nodes[e.src];
        auto& v = this->nodes[e.dst];

        ImVec2 delta = u.position - v.position;

        float dist = std::max(std::sqrt(delta.x * delta.x + delta.y * delta.y), 0.01f);

        ImVec2 displacement = delta * attractionK * dist;

        u.displacement -= displacement;
        v.displacement += displacement;
    }

    float totalDisplacement = 0;
    float maxDisplacement = 0;

    // apply displacements
    for (auto& v : this->nodes) {
        float len = std::sqrt(v.displacement.x * v.displacement.x + v.displacement.y * v.displacement.y);

        if (len > 0.0f) {
            v.position += (v.displacement / len) * std::min(len, temperature);
        }

        if (len > maxDisplacement) {
            maxDisplacement = len;
        }
        totalDisplacement += len;
    }

    // return totalDisplacement;
    return maxDisplacement;
}
#ifdef _MSC_VER
#pragma optimize("", on)
#endif

void Graph::Stabilize(float width, float height) noexcept {
    for (float temperature = this->options.temperature.starting; temperature > this->options.temperature.ending;
         temperature -= this->options.temperature.decreasePerIteration) {
        this->StabilizeStep(width, height, temperature);
    }
}

void Graph::HandleMouse(ImVec2 canvasPos) noexcept {
    if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        this->cameraOffset += ImGui::GetIO().MouseDelta;
    } else if (ImGui::IsItemHovered()) {
        float wheel = ImGui::GetIO().MouseWheel;

        if (wheel != 0.0f) {
            ImVec2 mouse = ImGui::GetIO().MousePos;

            // Mouse position in world space before zoom
            ImVec2 mouseWorldBefore = (mouse - canvasPos - this->cameraOffset) / this->zoom;

            // Apply zoom
            this->zoom *= (wheel > 0.0f) ? 1.1f : 0.9f;
            this->zoom = std::clamp(this->zoom, this->options.zoom.min, this->options.zoom.max);

            // Adjust camera so mouseWorldBefore stays under cursor
            this->cameraOffset = mouse - canvasPos - mouseWorldBefore * this->zoom;
        }
    }
}

void Graph::Draw(ImVec2 canvasSize, ImVec2 canvasPos) noexcept {
    if (canvasSize.x <= 0 || canvasSize.y <= 0) {
        LUSLOG_ERROR("[Graph::Draw] Invalid canvasSize = (%f, %f)", canvasSize.x, canvasSize.y);
        assert(false);
        return;
    }

    /** /
    LUSLOG_INFO(
        "[Graph::Draw] Begin: canvasSize = (%f, %f) | canvasPos = (%f, %f) | cameraOffset = (%f, %f) | zoom = %f",
        canvasSize.x, canvasSize.y,
        canvasPos.x, canvasPos.y,
        this->cameraOffset.x, this->cameraOffset.y,
        this->zoom
    );
    /**/
    ImGui::BeginGroup();

    ImDrawList* draw = ImGui::GetWindowDrawList();

    ImGui::SetCursorScreenPos(canvasPos);
    ImGui::InvisibleButton("graph_canvas", canvasSize, ImGuiButtonFlags_MouseButtonLeft);

    this->HandleMouse(canvasPos);

    Rect visibleRect = GetVisibleWorldRect(canvasSize, canvasPos, this->cameraOffset, this->zoom);

    for (auto& e : this->edges) {
        const ImVec2& a = this->nodes[e.src].position;
        const ImVec2& b = this->nodes[e.dst].position;

        if (!IsEdgeVisible(a, b, visibleRect)) {
            continue;
        }

        e.Draw(draw, canvasPos, this->cameraOffset, this->zoom, this->options.edges,
               this->options.zoom.edgesScaleWithZoom, this->nodes);
    }

    for (auto& n : this->nodes) {
        if (!IsNodeVisible(n, visibleRect, this->zoom, this->options.nodes, this->options.zoom.nodesScaleWithZoom)) {
            continue;
        }

        n.Draw(draw, canvasPos, this->cameraOffset, this->zoom, this->options.nodes,
               this->options.zoom.nodesScaleWithZoom);
    }

    for (auto& e : this->edges) {
        const ImVec2& a = this->nodes[e.src].position;
        const ImVec2& b = this->nodes[e.dst].position;

        if (!IsEdgeVisible(a, b, visibleRect)) {
            continue;
        }

        if (e.label != "") {
            e.DrawLabel(draw, canvasPos, this->cameraOffset, this->zoom, this->options.edges, a, b);
        }
    }

    for (auto& n : this->nodes) {
        if (!IsNodeVisible(n, visibleRect, this->zoom, this->options.nodes, this->options.zoom.nodesScaleWithZoom)) {
            continue;
        }

        if (n.label != "") {
            n.DrawLabel(draw, canvasPos, cameraOffset, this->zoom, this->options.nodes);
        }
    }

    ImGui::EndGroup();

    // LUSLOG_INFO("[Graph::Draw] End\n");
}

void Graph::Focus(ImVec2 pos) {
    this->cameraOffset = pos;
    if (this->zoom < 1.0f) {
        this->zoom = 1.0f;
    }
}

void Graph::FocusOnNode(size_t index) {
    if (index >= this->nodes.size()) {
        LUSLOG_ERROR("[Graph::FocusOnNode] Invalid index = %d", index);
        assert(false);
        return;
    }

    this->Focus(this->nodes[index].position);
}

void Graph::FocusOnEdge(size_t index) {
    if (index >= this->edges.size()) {
        LUSLOG_ERROR("[Graph::FocusOnEdge] Invalid index = %d", index);
        assert(false);
        return;
    }

    Edge edge = this->edges[index];
    this->Focus((this->nodes[edge.src].position + this->nodes[edge.dst].position) / 2);
}

GraphOptions& Graph::GetOptions() noexcept {
    return this->options;
}

void Graph::ResetView() noexcept {
    this->cameraOffset = { 0, 0 };
    this->zoom = 1.0f;
}

void Graph::ReplaceEdges(std::vector<Edge> edges) noexcept {
    this->edges = edges;
}