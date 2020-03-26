#include "DETNetBattle.h"
#include "DETNetStruct.h"
#include "DETNetMsgEvent.h"
#include "DETNetCode.h"
#include "GenericPlatformMisc.h"
#include "CommandLine.h"
#include "Paths.h"
#include "GenericPlatformOutputDevices.h"
#include "OutputDeviceFile.h"

#if UE_SERVER
#include "Engine.h"
#include "EngineGlobals.h"
#include "GameLiftServerSDK.h"
#endif

DETNetBattle DETNetBattle::Ins;
DETNetBattle& DETNetBattle::Instance()
{
	return Ins;
}

DETNetBattle::DETNetBattle()
{
}
DETNetBattle::~DETNetBattle()
{

}
int DETNetBattle::Init(const FString& matchAddr, int SrvId, int MapId, const FString& WelcomeAddr, int Port, bool bGameLift)
{
	MatchServerAddr = matchAddr;
	_port = Port;
	_bGameLift = bGameLift;
	HelloCmd.Id = SrvId;
	HelloCmd.Type = MSG_Battle;
	HelloCmd.Params.Add(MapId);
	HelloCmd.Params2 = WelcomeAddr;
	DETNetTcp::Init();
	if (!bGameLift)
	{
		return Connect();
	}
	else
	{
#if UE_SERVER
		InitGameLiftSDK(_port);
#endif
	}
	return 0;
}

void DETNetBattle::OnConnected()
{
	UE_LOG(LogDETNet, Log, TEXT("OnConnected %s"), *MatchServerAddr);
	Send(HelloCmd);
	if (_bGameLift)
	{
#if UE_SERVER
		FGameLiftServerSDKModule* gameLiftSdkModule = &FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>(FName("GameLiftServerSDK"));
		gameLiftSdkModule->ActivateGameSession();
#endif
	}
}
void DETNetBattle::OnConnectionError(const FString& Error)
{
	UE_LOG(LogDETNet, Log, TEXT("OnConnectionError,%s,%s"), *MatchServerAddr, *Error);
	Exit();

}
void DETNetBattle::OnClosed(bool Initiative)
{
	UE_LOG(LogDETNet, Log, TEXT("OnClosed %s,%d"), *MatchServerAddr, int(Initiative));
	Exit();
}
bool WaitForProcWithTimeout(FProcHandle Proc, const double TimeoutInSec, const double SleepIntervalInSec)
{
	double StartSeconds = FPlatformTime::Seconds();
	for (;;)
	{
		if (!FPlatformProcess::IsProcRunning(Proc))
		{
			break;
		}

		if (FPlatformTime::Seconds() - StartSeconds > TimeoutInSec)
		{
			return false;
		}

		FPlatformProcess::Sleep(SleepIntervalInSec);
	};

	return true;
}
void DETNetBattle::Exit()
{
	FGenericPlatformMisc::RequestExit(false);
	if (_bGameLift)
	{
#if UE_SERVER
		ProcessEnding();
#endif
	}
	FString Url;
	if (FParse::Value(FCommandLine::Get(), TEXT("LogHttpUrl="), Url))
	{
		const TCHAR* RelativePathToCrashReporter = TEXT("../../../Engine/Binaries/Linux/CrashReportClient");
		auto File = static_cast<FOutputDeviceFile*>(FGenericPlatformOutputDevices::GetLog());
		if (File)
		{
			FString LogSrcAbsolute = FPaths::ConvertRelativePathToFull(File->GetFilename());

			FString ClientArguments = TEXT("-log=") + LogSrcAbsolute + TEXT(" -httpUrl=") + Url;
			FProcHandle RunningProc = FPlatformProcess::CreateProc(RelativePathToCrashReporter, *ClientArguments, true, false, false, NULL, 0, NULL, NULL);
			const double kCrashTimeOut = 30.0;
			const double kCrashSleepInterval = 1.0;
			if (!WaitForProcWithTimeout(RunningProc, kCrashTimeOut, kCrashSleepInterval))
			{
				FPlatformProcess::TerminateProc(RunningProc);
			}

			FPlatformProcess::CloseProc(RunningProc);
		}
	}
}

#if UE_SERVER
void DETNetBattle::InitGameLiftSDK(unsigned short Port)
{
	UE_LOG(LogDETNet, Log, TEXT("InitGameLiftSDK,%d"), Port);
	FGameLiftServerSDKModule* gameLiftSdkModule = &FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>(FName("GameLiftServerSDK"));

	//InitSDK will establish a local connection with GameLift's agent to enable further communication.
	auto ret = gameLiftSdkModule->InitSDK();

	if (!ret.IsSuccess())
	{
		gameLiftSdkModule->ProcessEnding();
		FGenericPlatformMisc::RequestExit(false);
		return;
	}
	//When a game session is created, GameLift sends an activation request to the game server and passes along the game session object containing game properties and other settings.
	//Here is where a game server should take action based on the game session object.
	//Once the game server is ready to receive incoming player connections, it should invoke GameLiftServerAPI.ActivateGameSession()
	auto onGameSession = [=](Aws::GameLift::Server::Model::GameSession gameSession)
	{
		int BtlId = 0;
		FString SessionData = gameSession.GetGameSessionData();
		FParse::Value(*SessionData, TEXT("battle_id="), BtlId);
		FString Addr;
		FParse::Value(*SessionData, TEXT("match_addr="), Addr);
		UE_LOG(LogDETNet, Log, TEXT("OnStartGameSession,%s,%d,%s"), gameSession.GetGameSessionId(), BtlId, *Addr);
		HelloCmd.Id = BtlId;
		if (Connect(Addr) != 0)
		{
			Close(true);
		}
	};

	FProcessParameters* params = new FProcessParameters();
	params->OnStartGameSession.BindLambda(onGameSession);

	//OnProcessTerminate callback. GameLift will invoke this callback before shutting down an instance hosting this game server.
	//It gives this game server a chance to save its state, communicate with services, etc., before being shut down.
	//In this case, we simply tell GameLift we are indeed going to shutdown.
	params->OnTerminate.BindLambda([=]() { gameLiftSdkModule->ProcessEnding(); FGenericPlatformMisc::RequestExit(false); });

	//This is the HealthCheck callback.
	//GameLift will invoke this callback every 60 seconds or so.
	//Here, a game server might want to check the health of dependencies and such.
	//Simply return true if healthy, false otherwise.
	//The game server has 60 seconds to respond with its health status. GameLift will default to 'false' if the game server doesn't respond in time.
	//In this case, we're always healthy!
	params->OnHealthCheck.BindLambda([]() { return true; });

	//This game server tells GameLift that it will listen on port 7777 for incoming player connections.
	params->port = Port;

	//Here, the game server tells GameLift what set of files to upload when the game session ends.
	//GameLift will upload everything specified here for the developers to fetch later.
	TArray<FString> logfiles;
	logfiles.Add(TEXT("PWGame/Saved/Logs/PWGame.log"));
	logfiles.Add(TEXT("PWGame/Saved/Logs/"));
	params->logParameters = logfiles;

	//Calling ProcessReady tells GameLift this game server is ready to receive incoming game sessions!
	ret = gameLiftSdkModule->ProcessReady(*params);

	if (!ret.IsSuccess())
	{
		gameLiftSdkModule->ProcessEnding();
		FGenericPlatformMisc::RequestExit(false);
		return;
	}
}

void DETNetBattle::ProcessEnding()
{
	UE_LOG(LogDETNet, Log, TEXT("ProcessEnding"));
	FGameLiftServerSDKModule* gameLiftSdkModule = &FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>(FName("GameLiftServerSDK"));

	gameLiftSdkModule->ProcessEnding();
}
#endif