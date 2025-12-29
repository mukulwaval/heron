#pragma once
#include <ImNodeFlow.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <vector>

namespace Nodes {
	class SimpleSum : public ImFlow::BaseNode {
	public:
		SimpleSum() {
			setTitle("Simple sum");
			setStyle(ImFlow::NodeStyle::green());
			ImFlow::BaseNode::addIN<int>("In", 0, ImFlow::ConnectionFilter::SameType());
			ImFlow::BaseNode::addOUT<int>("Out", nullptr)->behaviour([this]() {
				return getInVal<int>("In") + m_valB;
				});
		}

		void draw() override {
			ImGui::SetNextItemWidth(100.f);
			ImGui::InputInt("##ValB", &m_valB);
		}

	private:
		int m_valB = 0;
	};

	class CollapsingNode : public ImFlow::BaseNode {
	public:
		CollapsingNode() {
			setTitle("Collapsing node");
			setStyle(ImFlow::NodeStyle::red());
			ImFlow::BaseNode::addIN<int>("A", 0, ImFlow::ConnectionFilter::SameType());
			ImFlow::BaseNode::addIN<int>("B", 0, ImFlow::ConnectionFilter::SameType());
			ImFlow::BaseNode::addOUT<int>("Out", nullptr)->behaviour([this]() {
				return getInVal<int>("A") + getInVal<int>("B");
				});
		}

		void draw() override {
			if (ImFlow::BaseNode::isSelected()) {
				ImGui::SetNextItemWidth(100.f);
				ImGui::Text("You can only see me when the node is selected!");
			}
		}
	};

	class ResultNode : public ImFlow::BaseNode {
	public:
		ResultNode() {
			setTitle("Result node");
			setStyle(ImFlow::NodeStyle::brown());
			ImFlow::BaseNode::addIN<int>("A", 0, ImFlow::ConnectionFilter::SameType());
			ImFlow::BaseNode::addIN<int>("B", 0, ImFlow::ConnectionFilter::SameType());
		}

		void draw() override {
			ImGui::Text("Result: %d", getInVal<int>("A") + getInVal<int>("B"));
		}
	};

    struct NodeEditor : ImFlow::BaseNode
    {
        ImFlow::ImNodeFlow mINF;
        ImGuiWindow* mWindow; // store window reference
        ImFlow::BaseNode* contextNode = nullptr;
        ImVec2 openPopupMousePos{};
        ImVec2 mSize{};

        NodeEditor(float size, bool seedGraph, ImGuiWindow* editorWindow)
            : ImFlow::BaseNode(), mWindow(editorWindow)
        {
            mSize = { size, size };
            mINF.setSize({ size, size });

            // Setup the "link dropped" popup
            mINF.droppedLinkPopUpContent(
                [this](ImFlow::Pin* fromPin)
                {
                    linkDropped(fromPin);
                }
            );

            if (seedGraph)
            {
                mINF.addNode<SimpleSum>({ 40, 40 });
                mINF.addNode<SimpleSum>({ 40, 150 });
                mINF.addNode<ResultNode>({ 250, 80 });
                mINF.addNode<CollapsingNode>({ 300, 300 });
            }
        }

        void set_size(ImVec2 size)
        {
            mINF.setSize(size);
            mSize = size;
        }

        void draw() override
        {
            mINF.update();
            handleContextMenus();
        }

    private:
        void handleContextMenus()
        {
            // Check if mouse is over the last ImGui item (the node editor canvas)
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) &&
                ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            {
                openPopupMousePos = ImGui::GetMousePos();
                ImGui::OpenPopup("Background Context Menu");
            }

            drawBackgroundContextMenu();
        }

        void drawBackgroundContextMenu()
        {
            if (ImGui::BeginPopup("Background Context Menu", ImGuiWindowFlags_NoMove))
            {
                ImVec2 spawnPos = openPopupMousePos;

                if (ImGui::MenuItem("Add Simple Sum"))
                    mINF.addNode<SimpleSum>(mINF.screen2grid(spawnPos));

                if (ImGui::MenuItem("Add Result Node"))
                    mINF.addNode<ResultNode>(mINF.screen2grid(spawnPos));

                if (ImGui::MenuItem("Add Collapsing Node"))
                    mINF.addNode<CollapsingNode>(mINF.screen2grid(spawnPos));

                ImGui::Separator();

                if (ImGui::MenuItem("Clear All"))
                    clear();

                ImGui::EndPopup();
            }
        }

        void linkDropped(ImFlow::Pin* droppedFrom)
        {
            if (!droppedFrom)
                return;

            if (!ImGui::IsPopupOpen("Link To Pin"))
                ImGui::OpenPopup("Link To Pin");

            if (ImGui::BeginPopup("Link To Pin", ImGuiWindowFlags_NoMove))
            {
                ImVec2 spawnPos = ImGui::GetMousePos();

                if (ImGui::MenuItem("Add Simple Sum"))
                    mINF.addNode<SimpleSum>(mINF.screen2grid(spawnPos));

                if (ImGui::MenuItem("Add Result Node"))
                    mINF.addNode<ResultNode>(mINF.screen2grid(spawnPos));

                if (ImGui::MenuItem("Add Collapsing Node"))
                    mINF.addNode<CollapsingNode>(mINF.screen2grid(spawnPos));

                ImGui::Separator();

                if (ImGui::MenuItem("Clear All"))
                    clear();

                ImGui::EndPopup();
            }
        }

        void clear()
        {
            // Clear all nodes and links
            mINF = ImFlow::ImNodeFlow();
            mINF.setSize(mSize);

            // Setup the "link dropped" popup
            mINF.droppedLinkPopUpContent(
                [this](ImFlow::Pin* fromPin)
                {
                    linkDropped(fromPin);
                }
            );
        }
    };
}