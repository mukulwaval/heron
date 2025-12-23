#pragma once

#include "Core.h"

namespace Heron {
	class HERON_API Application {
	public:
		Application();
		virtual ~Application();

		virtual void Run();
	};
} // namespace Heron
