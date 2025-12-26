#pragma once

#include <vector>

namespace HeronGui {
	struct AppState;
}

namespace HeronGui::Sections::ModelConfiguration
{
	struct ModelConfigurationState
	{
		std::vector<int> userNodeIndices;
	};

	static const char* ActNames[] = {
		"ReLU",
		"Sigmoid",
		"Tanh",
		"Softmax",
		"None"
	};

	void RenderModelConfigSection(AppState& state);
}