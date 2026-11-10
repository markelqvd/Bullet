
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MenuPrincipal.generated.h"

class SWidget;

UCLASS()
class BULLET_API AMenuPrincipal : public AActor
{
    GENERATED_BODY()

public:
    AMenuPrincipal();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(
        const EEndPlayReason::Type EndPlayReason
    ) override;

private:
    TSharedPtr<SWidget> MenuWidget;
};
