#include "../core/GameCore.h"
#include "../utils/Timer.h"

int main()
{
	g_pGameApp->StartGame();

	const int framesPerSecond =
		g_pGameApp->GetGameSettings()->GetIntValue("maxFrameRate");
	const double timePerFrame = 1000.0 / framesPerSecond;
	double renderStart = 0;
	unsigned int sleepFor = 0;
	double timeEllapsed = 0;
	double lastRender = 0;

	while (g_pGameApp->CheckForNotifications())
	{
		renderStart = Application_Layer::Timer::GetCurrentTimestamp();

		g_pGameApp->RenderCurrentScene((renderStart - lastRender) * 1000);
		timeEllapsed =
			Application_Layer::Timer::GetTimeEllapsed(renderStart);
		sleepFor = (timePerFrame - timeEllapsed);
		lastRender =
			Application_Layer::Timer::GetCurrentTimestamp();

		if (sleepFor <= 0)
		{
			continue;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(sleepFor));
	}

	Application_Layer::GameCore::DestroyInstance();

	return 0;
}
