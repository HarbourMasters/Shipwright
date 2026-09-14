#pragma once

#include <string>
#include <vector>

#include <imgui.h>

typedef struct GraphNodeOptions final {
    float baseSize;
    struct {
        ImVec2 padding;
        float rounding;
        ImU32 backgroundColor;
        bool fadeout;
        float fadeoutCutoffLower;
        float fadeoutCutoffUpper;
    } label;
} GraphNodeOptions;

typedef struct GraphEdgeOptions final {
    float thickness;
    struct {
        float separation;
        ImVec2 padding;
        float rounding;
        ImU32 backgroundColor;
        bool fadeout;
        float fadeoutCutoffLower;
        float fadeoutCutoffUpper;
    } label;
} GraphEdgeOptions;

typedef struct Node final {
    ImVec2 position;
    ImVec2 displacement;
    std::string label;
    // must be the result of ImGui::CalcTextSize(this->label.c_str());
    ImVec2 labelSize;
    ImU32 color;
    ImU32 labelColor;

    static Node New(ImVec2 position, std::string label, ImU32 color, ImU32 labelColor) noexcept;
    void Draw(ImDrawList* draw, ImVec2 canvasPos, ImVec2 cameraOffset, float zoom, GraphNodeOptions options,
              bool scaleWithZoom) noexcept;
    void DrawLabel(ImDrawList* draw, ImVec2 canvasPos, ImVec2 cameraOffset, float zoom,
                   GraphNodeOptions options) noexcept;
} Node;

typedef struct Edge final {
    // index of the source node in the nodes vector
    int src;
    // index of the destination node in the nodes vector
    int dst;
    std::string label;
    // must be the result of ImGui::CalcTextSize(this->label.c_str());
    ImVec2 labelSize;
    ImU32 color;
    ImU32 labelColor;

    static Edge New(int src, int dst, std::string label, ImU32 color, ImU32 labelColor) noexcept;
    void Draw(ImDrawList* draw, ImVec2 canvasPos, ImVec2 cameraOffset, float zoom, GraphEdgeOptions options,
              bool scaleWithZoom, const std::vector<Node>& nodes) noexcept;
    void DrawLabel(ImDrawList* draw, ImVec2 canvasPos, ImVec2 cameraOffset, float zoom, GraphEdgeOptions options,
                   ImVec2 a, ImVec2 b) noexcept;
} Edge;

typedef struct GraphOptions final {
    struct {
        float min;
        float max;
        bool nodesScaleWithZoom;
        bool edgesScaleWithZoom;
    } zoom;
    struct {
        float repulsion;
        float attraction;
    } forceMultipliers;
    struct {
        float starting;
        float decreasePerIteration;
        float ending;
    } temperature;
    GraphNodeOptions nodes;
    GraphEdgeOptions edges;
} GraphOptions;

class Graph final {
  public:
    void Stabilize(float width, float height) noexcept;
    float StabilizeStep(float width, float height, float temperature) noexcept;
    void Draw(ImVec2 canvasSize, ImVec2 canvasPos) noexcept;
    void Focus(ImVec2 pos);
    void FocusOnNode(size_t index);
    void FocusOnEdge(size_t index);
    [[nodiscard("There's no point in calling the function without using the options returned")]]
    // comment so clang format doesn't mess this up too much
    GraphOptions&
    GetOptions() noexcept;
    void ResetView() noexcept;
    void ReplaceEdges(std::vector<Edge> edges) noexcept;
    [[nodiscard("There's no point in calling the function without using the graph returned")]]
#ifndef _MSC_VER // msvc complains about an unknown attribute
    [[gnu::pure]]
#endif
    static Graph
    New(std::vector<Node> nodes, std::vector<Edge> edges, GraphOptions options) noexcept;

  private:
    Graph() = delete;
    Graph(std::vector<Node> _nodes, std::vector<Edge> _edges, GraphOptions _options);
    void HandleMouse(ImVec2 canvasPos) noexcept;
    std::vector<Node> nodes;
    std::vector<Edge> edges;
    ImVec2 cameraOffset = { 0, 0 };
    float zoom = 1.0f;
    GraphOptions options;
};