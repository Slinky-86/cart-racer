#include "WastelandRacers.h"
#include "Modules/ModuleManager.h"

DEFINE_LOG_CATEGORY(LogWastelandRacers);

class FWastelandRacersModule : public FDefaultGameModuleImpl
{
public:
	virtual void StartupModule() override
	{
		UE_LOG(LogWastelandRacers, Warning, TEXT("WastelandRacers module started"));
	}
	
	virtual void ShutdownModule() override
	{
		UE_LOG(LogWastelandRacers, Warning, TEXT("WastelandRacers module shut down"));
	}
};

IMPLEMENT_PRIMARY_GAME_MODULE(FWastelandRacersModule, WastelandRacers, "WastelandRacers");
