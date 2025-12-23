#include "Heron/Application.h"
#include <iostream>

namespace Heron {

	Application::Application() {}

	Application::~Application() {}

	void Application::Run() {
		std::cout << "Application is running!" << std::endl;
	}

} // namespace Heron
