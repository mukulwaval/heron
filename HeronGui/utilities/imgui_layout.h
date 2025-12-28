#pragma once

#include <unordered_map>
#include <memory>
#include <vector>

#include "imgui.h"
#include "imgui_internal.h"

namespace ImLayout
{
    struct Layout;
    struct LayoutItem;

    struct WindowLayoutContext
    {
        std::unordered_map<ImGuiID, std::unique_ptr<Layout>> Layouts;
        Layout* CurrentLayout = nullptr;
        LayoutItem* CurrentLayoutItem = nullptr;
        std::vector<Layout*> LayoutStack;
    };

    struct LayoutContext
    {
        std::unordered_map<ImGuiID, WindowLayoutContext> PerWindow;

        static LayoutContext& Get()
        {
            static LayoutContext ctx;
            return ctx;
        }
    };

    enum class LayoutItemType
    {
        Item,
        Spring
    };

    struct LayoutItem
    {
        LayoutItemType Type;

        ImRect MeasuredBounds;

        int VertexIndexBegin = 0;
        int VertexIndexEnd = 0;

        // Spring data
        float SpringWeight = 0.0f;
        float SpringSize = 0.0f;
        float SpringSpacing = 0.0f;

        // Alignment
        float CurrentAlign = 0.0f;
        float CurrentAlignOffset = 0.0f;

        LayoutItem(LayoutItemType type = LayoutItemType::Item)
            : Type(type) {
        }
    };

    enum class LayoutType
    {
        Horizontal,
        Vertical
    };

    struct Layout
    {
        // Identity
        ImGuiID Id = 0;
        LayoutType Type = LayoutType::Horizontal;

        // Hierarchy
        Layout* Parent = nullptr;
        Layout* FirstChild = nullptr;
        Layout* NextSibling = nullptr;
        int ParentItemIndex = 0;

        // State
        bool Live = false;
        float Align = -1.0f;

        // Sizing
        ImVec2 Size = ImVec2(0, 0);
        ImVec2 MinimumSize = ImVec2(0, 0);
        ImVec2 CurrentSize = ImVec2(0, 0);

        // Cursor bookkeeping
        ImVec2 StartPos = ImVec2(0, 0);
        ImVec2 StartCursorMaxPos = ImVec2(0, 0);

        // Vertical indent
        float Indent = 0.0f;

        // THIS IS THE BIG ONE
        ImVector<LayoutItem> Items;
        int CurrentItemIndex = 0;

        Layout(ImGuiID id, LayoutType type)
            : Id(id), Type(type)
        {
        }
    };

    static Layout* FindLayout(ImGuiID id, LayoutType type);
    static Layout* CreateNewLayout(ImGuiID id, LayoutType type, ImVec2 size);
    static void BeginLayout(ImGuiID id, LayoutType type, ImVec2 size, float align);
    static void EndLayout(LayoutType type);
    static ImVec2 CalculateLayoutSize(Layout& layout, bool collapse_springs);
    static void PushLayout(Layout* layout);
    static void PopLayout();
    static void BalanceLayoutSprings(Layout& layout);
    static ImVec2 BalanceLayoutItemAlignment(Layout& layout, LayoutItem& item);
    static void BalanceLayoutItemsAlignment(Layout& layout);
    static bool HasAnyNonZeroSpring(const Layout& layout);
    static void BalanceChildLayouts(Layout& layout);
    static LayoutItem* GenerateLayoutItem(Layout& layout, LayoutItemType type);
    static float CalculateLayoutItemAlignmentOffset(Layout& layout, LayoutItem& item);
    static void TranslateLayoutItem(LayoutItem& item, const ImVec2& offset);
    static void SignedIndent(float indent);
    static void BeginLayoutItem(Layout& layout);
    static void EndLayoutItem(Layout& layout);
    static void AddLayoutSpring(Layout& layout, float weight, float spacing);
    void BeginHorizontal(const char* str_id, const ImVec2& size = ImVec2(0, 0), float align = -1.0f);
    void BeginHorizontal(const void* ptr_id, const ImVec2& size = ImVec2(0, 0), float align = -1.0f);
    void BeginHorizontal(int id, const ImVec2& size = ImVec2(0, 0), float align = -1.0f);
    void EndHorizontal();
    void BeginVertical(const char* str_id, const ImVec2& size = ImVec2(0, 0), float align = -1.0f);
    void BeginVertical(const void* ptr_id, const ImVec2& size = ImVec2(0, 0), float align = -1.0f);
    void BeginVertical(int id, const ImVec2& size = ImVec2(0, 0), float align = -1.0f);
    void EndVertical();
    void Spring(float weight = 1.0f, float spacing = -1.0f);
    void SuspendLayout();
    void ResumeLayout();
    static Layout* GetCurrentLayout();
}