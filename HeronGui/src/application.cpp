#include "Application.h"
#include "imgui.h"
#include "Configuration/Style.h"
#include "Sections/ModelConfiguration.h"
#include "Components/Dockspace.h"
#include <Heron.h>

#include <functional>

namespace HeronGui
{
	void RenderUI(AppState& state)
	{
		Component::Dockspace([&]() {
			HeronGui::Sections::ModelConfiguration::RenderModelConfigSection(state);
			});

		ImGui::ShowDemoWindow();
	}
}
