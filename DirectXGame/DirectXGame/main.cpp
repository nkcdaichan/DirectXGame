#include "PostProcessingDemo.h"
#include "InputSystem.h"


int main(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	try
	{
		GraphicsEngine::create();
		InputSystem::create();
	}
	catch (...) { return -1; }

	{
		try
		{
			PostProcessingDemo app;
			while (app.isRun());
		}
		catch (...) {
			InputSystem::release();
			GraphicsEngine::release();
			return -1;
		}
	}

	InputSystem::release();
	GraphicsEngine::release();

	return 0;
}