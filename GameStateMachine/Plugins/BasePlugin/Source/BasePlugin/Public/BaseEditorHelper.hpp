#pragma once

#if !WITH_EDITOR
#   define DHSetEditorFolderPath(A, B)
#else
#include "CoreMinimal.h"
#include "Engine/World.h"
static void DHSetEditorFolderPath(class UWorld* World, class AActor* Actor)
{
	FString Dir;
	switch (World->WorldType) {
	case EWorldType::PIE:
		Dir = TEXT("PIE");
		break;
	case EWorldType::Game:
		Dir = TEXT("Game");
		break;
	case EWorldType::GamePreview:
		Dir = TEXT("GamePreview");
		break;
	case EWorldType::GameRPC:
		Dir = TEXT("GameRPC");
		break;
	case EWorldType::Editor:
		Dir = TEXT("Editor");
		break;
	case EWorldType::EditorPreview:
		Dir = TEXT("EdEditorPreviewitor");
		break;
	case EWorldType::Inactive:
		Dir = TEXT("Inactive");
		break;
	default:
		Dir = TEXT("Others");
		break;
	}

	FString Path = TEXT("DarkHorse") / Dir;
	FName FolderPath(*Path);
	Actor->SetFolderPath(FolderPath);
};
#endif
