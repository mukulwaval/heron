#include "imgui_layout.h"
#include <unordered_map>
#include <memory>
#include "imgui.h"
#include "imgui_internal.h"

namespace ImLayout
{
    static Layout* FindLayout(ImGuiID id, LayoutType type)
    {
        IM_ASSERT(type == LayoutType::Horizontal || type == LayoutType::Vertical);

        ImGuiID window_id = ImGui::GetCurrentWindow()->ID;

        auto& wctx = LayoutContext::Get().PerWindow[window_id];
        auto& layouts = wctx.Layouts;

        auto it = layouts.find(id);
        if (it == layouts.end())
            return nullptr;

        Layout* layout = it->second.get();

        if (layout->Type != type)
        {
            layout->Type = type;
            layout->MinimumSize = ImVec2(0, 0);
            layout->Items.clear();
        }

        return layout;
    }

    static Layout* CreateNewLayout(ImGuiID id, LayoutType type, ImVec2 size)
    {
        IM_ASSERT(type == LayoutType::Horizontal || type == LayoutType::Vertical);

        ImGuiWindow* window = ImGui::GetCurrentWindow();
        auto& wctx = LayoutContext::Get().PerWindow[window->ID];

        auto layout = std::make_unique<Layout>(id, type);
        layout->Size = size;

        Layout* ptr = layout.get();
        wctx.Layouts[id] = std::move(layout);

        return ptr;
    }

    static void BeginLayout(ImGuiID id, LayoutType type, ImVec2 size, float align)
    {
        ImGui::PushID(id);

        Layout* layout = FindLayout(id, type);
        if (!layout)
            layout = CreateNewLayout(id, type, size);

        layout->Live = true;
        PushLayout(layout);

        layout->Size = size;

        if (align < 0.0f)
            layout->Align = -1.0f;
        else
            layout->Align = ImClamp(align, 0.0f, 1.0f);

        layout->CurrentItemIndex = 0;

        layout->CurrentSize.x = size.x > 0 ? size.x : layout->MinimumSize.x;
        layout->CurrentSize.y = size.y > 0 ? size.y : layout->MinimumSize.y;

        layout->StartPos = ImGui::GetCursorPos();
        layout->StartCursorMaxPos = ImGui::GetCursorScreenPos();

        if (type == LayoutType::Vertical)
        {
            // Force cursor sync without spacing side-effects
            ImGui::Dummy(ImVec2(0, 0));

            float new_x = layout->StartPos.x;
            float cur_x = ImGui::GetCursorPos().x;

            layout->Indent = new_x - cur_x;
            ImGui::SetCursorPosX(new_x);
        }

        BeginLayoutItem(*layout);
    }

    static void EndLayout(LayoutType type)
    {
        Layout* layout = GetCurrentLayout();
        IM_ASSERT(layout && layout->Type == type);

        EndLayoutItem(*layout);

        if (layout->CurrentItemIndex < layout->Items.size())
            layout->Items.resize(layout->CurrentItemIndex);

        if (layout->Type == LayoutType::Vertical)
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() - layout->Indent);

        PopLayout();

        const bool auto_w = layout->Size.x <= 0;
        const bool auto_h = layout->Size.y <= 0;

        ImVec2 new_size = layout->Size;

        if (auto_w) new_size.x = layout->CurrentSize.x;
        if (auto_h) new_size.y = layout->CurrentSize.y;

        ImVec2 min_size = CalculateLayoutSize(*layout, true);
        layout->MinimumSize = min_size;

        if (auto_w) new_size.x = min_size.x;
        if (auto_h) new_size.y = min_size.y;

        layout->CurrentSize = new_size;

        ImVec2 measured = new_size;

        if ((auto_w || auto_h) && layout->Parent)
        {
            if (layout->Type == LayoutType::Horizontal && auto_w)
                layout->CurrentSize.x = layout->Parent->CurrentSize.x;
            else if (layout->Type == LayoutType::Vertical && auto_h)
                layout->CurrentSize.y = layout->Parent->CurrentSize.y;

            BalanceLayoutSprings(*layout);
            measured = layout->CurrentSize;
        }

        layout->CurrentSize = new_size;

        ImGui::PopID();

        ImGui::SetCursorPos(layout->StartPos);
        ImGui::ItemSize(new_size);
        ImGui::ItemAdd(ImRect(
            ImGui::GetCursorScreenPos(),
            ImVec2(ImGui::GetCursorScreenPos().x + measured.x, ImGui::GetCursorScreenPos().y + measured.y)
        ), 0);

        if (!layout->Parent)
            BalanceChildLayouts(*layout);
    }

    static ImVec2 CalculateLayoutSize(Layout& layout, bool collapse_springs)
    {
        ImVec2 bounds(0.0f, 0.0f);

        if (layout.Type == LayoutType::Vertical)
        {
            for (auto& item : layout.Items)
            {
                ImVec2 size = item.MeasuredBounds.GetSize();

                if (item.Type == LayoutItemType::Item)
                {
                    bounds.x = ImMax(bounds.x, size.x);
                    bounds.y += size.y;
                }
                else
                {
                    bounds.y += ImFloor(item.SpringSpacing);

                    if (!collapse_springs)
                        bounds.y += item.SpringSize;
                }
            }
        }
        else
        {
            for (auto& item : layout.Items)
            {
                ImVec2 size = item.MeasuredBounds.GetSize();

                if (item.Type == LayoutItemType::Item)
                {
                    bounds.x += size.x;
                    bounds.y = ImMax(bounds.y, size.y);
                }
                else
                {
                    bounds.x += ImFloor(item.SpringSpacing);

                    if (!collapse_springs)
                        bounds.x += item.SpringSize;
                }
            }
        }

        return bounds;
    }

    static void PushLayout(Layout* layout)
    {
        ImGuiID window_id = ImGui::GetCurrentWindow()->ID;
        auto& wctx = LayoutContext::Get().PerWindow[window_id];

        if (layout)
        {
            layout->Parent = wctx.CurrentLayout;

            if (layout->Parent)
            {
                layout->ParentItemIndex = layout->Parent->CurrentItemIndex;
                layout->NextSibling = layout->Parent->FirstChild;
                layout->Parent->FirstChild = layout;
            }
            else
            {
                layout->NextSibling = nullptr;
            }

            layout->FirstChild = nullptr;
        }

        wctx.LayoutStack.push_back(layout);
        wctx.CurrentLayout = layout;
        wctx.CurrentLayoutItem = nullptr;
    }

    static void PopLayout()
    {
        ImGuiID window_id = ImGui::GetCurrentWindow()->ID;
        auto& wctx = LayoutContext::Get().PerWindow[window_id];

        IM_ASSERT(!wctx.LayoutStack.empty());

        wctx.LayoutStack.pop_back();

        if (!wctx.LayoutStack.empty())
        {
            wctx.CurrentLayout = wctx.LayoutStack.back();
            Layout& parent = *wctx.CurrentLayout;

            if (parent.CurrentItemIndex < parent.Items.Size)
                wctx.CurrentLayoutItem = &parent.Items[parent.CurrentItemIndex];
            else
                wctx.CurrentLayoutItem = nullptr;
        }
        else
        {
            wctx.CurrentLayout = nullptr;
            wctx.CurrentLayoutItem = nullptr;
        }
    }

    static void BalanceLayoutSprings(Layout& layout)
    {
        float total_weight = 0.0f;
        int last_spring = -1;

        for (int i = 0; i < (int)layout.Items.size(); i++)
        {
            if (layout.Items[i].Type == LayoutItemType::Spring)
            {
                total_weight += layout.Items[i].SpringWeight;
                last_spring = i;
            }
        }

        const bool horizontal = (layout.Type == LayoutType::Horizontal);

        const bool auto_sized =
            ((horizontal ? layout.Size.x : layout.Size.y) <= 0.0f) &&
            (layout.Parent == nullptr);

        const float occupied =
            horizontal ? layout.MinimumSize.x : layout.MinimumSize.y;

        const float available =
            auto_sized
            ? occupied
            : (horizontal ? layout.CurrentSize.x : layout.CurrentSize.y);

        const float free_space = ImMax(available - occupied, 0.0f);

        float span_start = 0.0f;
        float current_weight = 0.0f;

        for (int i = 0; i < (int)layout.Items.size(); i++)
        {
            LayoutItem& item = layout.Items[i];
            if (item.Type != LayoutItemType::Spring)
                continue;

            float old_size = item.SpringSize;

            if (free_space > 0.0f && total_weight > 0.0f)
            {
                float next_weight = current_weight + item.SpringWeight;
                float span_end = ImFloor(
                    (i == last_spring)
                    ? free_space
                    : (free_space * next_weight / total_weight)
                );

                item.SpringSize = span_end - span_start;
                span_start = span_end;
                current_weight = next_weight;
            }
            else
            {
                item.SpringSize = 0.0f;
            }

            if (old_size != item.SpringSize)
            {
                float delta = item.SpringSize - old_size;
                ImVec2 offset = horizontal ? ImVec2(delta, 0) : ImVec2(0, delta);

                item.MeasuredBounds.Max.x += offset.x;
                item.MeasuredBounds.Max.y += offset.y;

                for (int j = i + 1; j < (int)layout.Items.size(); j++)
                {
                    LayoutItem& it = layout.Items[j];
                    it.MeasuredBounds.Min.x += offset.x;
                    it.MeasuredBounds.Min.y += offset.y;
                    it.MeasuredBounds.Max.x += offset.x;
                    it.MeasuredBounds.Max.y += offset.y;
                }
            }
        }
    }

    static ImVec2 BalanceLayoutItemAlignment(Layout& layout, LayoutItem& item)
    {
        ImVec2 correction(0, 0);

        if (item.CurrentAlign > 0.0f)
        {
            float target_offset = CalculateLayoutItemAlignmentOffset(layout, item);

            if (item.CurrentAlignOffset != target_offset)
            {
                float delta = target_offset - item.CurrentAlignOffset;

                if (layout.Type == LayoutType::Horizontal)
                    correction.y = delta;
                else
                    correction.x = delta;

                TranslateLayoutItem(item, correction);
                item.CurrentAlignOffset = target_offset;
            }
        }

        return correction;
    }

    static void BalanceLayoutItemsAlignment(Layout& layout)
    {
        for (LayoutItem& item : layout.Items)
            BalanceLayoutItemAlignment(layout, item);
    }

    static bool HasAnyNonZeroSpring(const Layout& layout)
    {
        for (const LayoutItem& item : layout.Items)
        {
            if (item.Type == LayoutItemType::Spring && item.SpringWeight > 0.0f)
                return true;
        }
        return false;
    }

    static void BalanceChildLayouts(Layout& layout)
    {
        for (Layout* child = layout.FirstChild; child; child = child->NextSibling)
        {
            // Propagate size downward if child is auto-sized
            if (child->Type == LayoutType::Horizontal && child->Size.x <= 0.0f)
                child->CurrentSize.x = layout.CurrentSize.x;
            else if (child->Type == LayoutType::Vertical && child->Size.y <= 0.0f)
                child->CurrentSize.y = layout.CurrentSize.y;

            BalanceChildLayouts(*child);

            // If child has springs, expand parent item bounds
            if (HasAnyNonZeroSpring(*child))
            {
                LayoutItem& parent_item = layout.Items[child->ParentItemIndex];

                if (child->Type == LayoutType::Horizontal && child->Size.x <= 0.0f)
                {
                    parent_item.MeasuredBounds.Max.x =
                        ImMax(parent_item.MeasuredBounds.Max.x,
                            parent_item.MeasuredBounds.Min.x + layout.CurrentSize.x);
                }
                else if (child->Type == LayoutType::Vertical && child->Size.y <= 0.0f)
                {
                    parent_item.MeasuredBounds.Max.y =
                        ImMax(parent_item.MeasuredBounds.Max.y,
                            parent_item.MeasuredBounds.Min.y + layout.CurrentSize.y);
                }
            }
        }

        BalanceLayoutSprings(layout);
        BalanceLayoutItemsAlignment(layout);
    }

    static LayoutItem* GenerateLayoutItem(Layout& layout, LayoutItemType type)
    {
        IM_ASSERT(layout.CurrentItemIndex <= (int)layout.Items.size());

        if (layout.CurrentItemIndex < (int)layout.Items.size())
        {
            LayoutItem& item = layout.Items[layout.CurrentItemIndex];
            if (item.Type != type)
                item = LayoutItem(type);
        }
        else
        {
            layout.Items.push_back(type);
        }

        return &layout.Items[layout.CurrentItemIndex];
    }

    static float CalculateLayoutItemAlignmentOffset(Layout& layout, LayoutItem& item)
    {
        if (item.CurrentAlign <= 0.0f)
            return 0.0f;

        ImVec2 item_size = item.MeasuredBounds.GetSize();

        float layout_extent =
            (layout.Type == LayoutType::Horizontal)
            ? layout.CurrentSize.y
            : layout.CurrentSize.x;

        float item_extent =
            (layout.Type == LayoutType::Horizontal)
            ? item_size.y
            : item_size.x;

        if (item_extent <= 0.0f)
            return 0.0f;

        return ImFloor(item.CurrentAlign * (layout_extent - item_extent));
    }

    static void TranslateLayoutItem(LayoutItem& item, const ImVec2& offset)
    {
        if (offset.x == 0.0f && offset.y == 0.0f)
            return;

        item.MeasuredBounds.Min.x += offset.x;
        item.MeasuredBounds.Min.y += offset.y;
        item.MeasuredBounds.Max.x += offset.x;
        item.MeasuredBounds.Max.y += offset.y;
    }

    static void SignedIndent(float indent)
    {
        if (indent > 0.0f)
            ImGui::Indent(indent);
        else if (indent < 0.0f)
            ImGui::Unindent(-indent);
    }

    static void BeginLayoutItem(Layout& layout)
    {
        LayoutItem& item = *GenerateLayoutItem(layout, LayoutItemType::Item);

        // Resolve alignment
        item.CurrentAlign = layout.Align;
        if (item.CurrentAlign < 0.0f)
            item.CurrentAlign = ImClamp(/*ImGui::GetStyle().LayoutAlign*/0.0f, 0.0f, 1.0f);

        item.CurrentAlignOffset = CalculateLayoutItemAlignmentOffset(layout, item);

        // Apply initial alignment offset
        if (item.CurrentAlign > 0.0f)
        {
            if (layout.Type == LayoutType::Horizontal)
            {
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + item.CurrentAlignOffset);
            }
            else
            {
                SignedIndent(item.CurrentAlignOffset);
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + item.CurrentAlignOffset);
            }
        }

        ImVec2 pos = ImGui::GetCursorPos();
        item.MeasuredBounds.Min = pos;
        item.MeasuredBounds.Max = pos;
    }

    static void EndLayoutItem(Layout& layout)
    {
        IM_ASSERT(layout.CurrentItemIndex < (int)layout.Items.size());
        LayoutItem& item = layout.Items[layout.CurrentItemIndex];

        ImVec2 end_pos = ImGui::GetCursorPos();
        item.MeasuredBounds.Max = end_pos;

        // Undo indent for vertical alignment
        if (item.CurrentAlign > 0.0f && layout.Type == LayoutType::Vertical)
            SignedIndent(-item.CurrentAlignOffset);

        // Correct alignment if size changed this frame
        ImVec2 correction = BalanceLayoutItemAlignment(layout, item);

        item.MeasuredBounds.Min.x += correction.x;
        item.MeasuredBounds.Min.y += correction.y;
        item.MeasuredBounds.Max.x += correction.x;
        item.MeasuredBounds.Max.y += correction.y;

        // Reset cursor for next item
        if (layout.Type == LayoutType::Horizontal)
            ImGui::SetCursorPosY(layout.StartPos.y);
        else
            ImGui::SetCursorPosX(layout.StartPos.x);

        layout.CurrentItemIndex++;
    }

    static void AddLayoutSpring(Layout& layout, float weight, float spacing)
    {
        // Finish previous item cleanly
        if (layout.CurrentItemIndex < (int)layout.Items.size())
            EndLayoutItem(layout);

        LayoutItem& spring = *GenerateLayoutItem(layout, LayoutItemType::Spring);

        if (weight < 0.0f)
            weight = 0.0f;

        spring.SpringWeight = weight;

        if (spacing < 0.0f)
        {
            ImVec2 style_spacing = ImGui::GetStyle().ItemSpacing;
            spacing = (layout.Type == LayoutType::Horizontal)
                ? style_spacing.x
                : style_spacing.y;
        }

        spring.SpringSpacing = spacing;

        // Measure spring as zero-size placeholder
        ImVec2 cursor = ImGui::GetCursorPos();
        spring.MeasuredBounds.Min = cursor;
        spring.MeasuredBounds.Max = cursor;

        // Optional visual spacing (Dummy only if needed)
        if (spacing > 0.0f)
        {
            ImVec2 dummy_size =
                (layout.Type == LayoutType::Horizontal)
                ? ImVec2(spacing, 0.0f)
                : ImVec2(0.0f, spacing);

            ImGui::Dummy(dummy_size);
            spring.MeasuredBounds.Max = ImGui::GetCursorPos();
        }

        layout.CurrentItemIndex++;

        // Prepare next item
        BeginLayoutItem(layout);
    }

    void BeginHorizontal(const char* str_id, const ImVec2& size /*= ImVec2(0, 0)*/, float align /*= -1.0f*/)
    {
        BeginLayout(ImGui::GetID(str_id), LayoutType::Horizontal, size, align);
    }

    void BeginHorizontal(const void* ptr_id, const ImVec2& size /*= ImVec2(0, 0)*/, float align /*= -1.0f*/)
    {
        BeginLayout(ImGui::GetID(ptr_id), LayoutType::Horizontal, size, align);
    }

    void BeginHorizontal(int id, const ImVec2& size /*= ImVec2(0, 0)*/, float align /*= -1.0f*/)
    {
        BeginLayout(ImGui::GetID((void*)(intptr_t)id), LayoutType::Horizontal, size, align);
    }

    void EndHorizontal()
    {
        EndLayout(LayoutType::Horizontal);
    }

    void BeginVertical(const char* str_id, const ImVec2& size /*= ImVec2(0, 0)*/, float align /*= -1.0f*/)
    {
        BeginLayout(ImGui::GetID(str_id), LayoutType::Vertical, size, align);
    }

    void BeginVertical(const void* ptr_id, const ImVec2& size /*= ImVec2(0, 0)*/, float align /*= -1.0f*/)
    {
        BeginLayout(ImGui::GetID(ptr_id), LayoutType::Vertical, size, align);
    }

    void BeginVertical(int id, const ImVec2& size /*= ImVec2(0, 0)*/, float align /*= -1.0f*/)
    {
        BeginLayout(ImGui::GetID((void*)(intptr_t)id), LayoutType::Vertical, size, align);
    }

    void EndVertical()
    {
        EndLayout(LayoutType::Vertical);
    }

    // Inserts spring separator in layout
    //      weight <= 0     : spring will always have zero size
    //      weight > 0      : power of current spring
    //      spacing < 0     : use default spacing
    //      spacing >= 0    : enforce spacing amount
    void Spring(float weight /*= 1.0f*/, float spacing /*= -1.0f*/)
    {
        Layout* layout = GetCurrentLayout();
        IM_ASSERT(layout && "Spring() called outside of a layout");

        AddLayoutSpring(*layout, weight, spacing);
    }

    void SuspendLayout()
    {
        PushLayout(nullptr);
    }

    void ResumeLayout()
    {
        IM_ASSERT(!GetCurrentLayout() && "ResumeLayout() called while a layout is active");
        PopLayout();
    }

    static Layout* GetCurrentLayout()
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        return ImLayout::LayoutContext::Get()
            .PerWindow[ImGui::GetCurrentWindow()->ID]
            .CurrentLayout;
    }
}