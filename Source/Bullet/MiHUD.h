#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MiHUD.generated.h"

UCLASS()
class BULLET_API AMiHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

	bool bEstaApuntando = false;
};