#pragma once
#include "CoreMinimal.h"
#include "NboSerializer.h"
#include "WebSocketsModule.h"
#include "IWebSocket.h"
#include "SocketSubsystem.h"
#include "IPAddress.h"
#include "Sockets.h"
#include "HttpModule.h"
#include "PlatformHttp.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"
#include "Interfaces/IHttpResponse.h"
#include "Kismet/GameplayStatics.h"

#include <functional>

#ifdef WIN32
#define DETNET_EXPORT	__declspec(dllexport)
#define DETNET_IMPORT	__declspec(dllimport)
#else
#define DETNET_EXPORT	
#define PWNET_IMPORT	
#endif 

DECLARE_LOG_CATEGORY_EXTERN(LogDETNet, Log, All);
