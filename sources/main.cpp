#include <exception>

#include <cage-core/core.h>
#include <cage-core/log.h>
#include <cage-core/math.h>
#include <cage-core/config.h>
#include <cage-core/assets.h>
#include <cage-core/ini.h>
#include <cage-core/hashString.h>

#include <cage-client/core.h>
#include <cage-client/window.h>
#include <cage-client/engine.h>
#include <cage-client/engineProfiling.h>
#include <cage-client/highPerformanceGpuHint.h>

using namespace cage;

namespace
{
	bool windowClose()
	{
		engineStop();
		return true;
	}
}

int main(int argc, const char *args[])
{
	try
	{
		holder<loggerClass> log1 = newLogger();
		log1->filter.bind<logFilterPolicyPass>();
		log1->format.bind<logFormatPolicyConsole>();
		log1->output.bind<logOutputPolicyStdOut>();

		controlThread().timePerTick = 1000000 / 30;
		engineInitialize(engineCreateConfig());
		assets()->add(hashString("flittermouse/flittermouse.pack"));

		eventListener<bool()> windowCloseListener;
		windowCloseListener.bind<&windowClose>();
		window()->events.windowClose.attach(windowCloseListener);

		window()->title("flittermouse");
		window()->setMaximized();

		{
			holder<engineProfilingClass> engineProfiling = newEngineProfiling();
			engineProfiling->profilingScope = engineProfilingScopeEnum::None;

			engineStart();
		}

		assets()->remove(hashString("flittermouse/flittermouse.pack"));
		engineFinalize();

		try
		{
			configSaveIni("flittermouse.ini", "flittermouse");
		}
		catch (...)
		{
			CAGE_LOG(severityEnum::Warning, "flittermouse", "failed to save game configuration");
		}
		return 0;
	}
	catch (const cage::exception &e)
	{
		CAGE_LOG(severityEnum::Note, "exception", e.message);
		CAGE_LOG(severityEnum::Error, "exception", "caught cage exception in main");
	}
	catch (const std::exception &e)
	{
		CAGE_LOG(severityEnum::Note, "exception", e.what());
		CAGE_LOG(severityEnum::Error, "exception", "caught std exception in main");
	}
	catch (...)
	{
		CAGE_LOG(severityEnum::Error, "exception", "caught unknown exception in main");
	}
	return 1;
}
