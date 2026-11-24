
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MenuPrincipal.generated.h"

class SWidget;
class SWidgetSwitcher;
class SVerticalBox;

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
    TSharedPtr<SWidgetSwitcher> Pantallas;
    TSharedPtr<SVerticalBox> ListaNiveles;

protected:
    UPROPERTY(EditAnywhere, Category = "Menu")
    FString CarpetaNiveles = TEXT("/Game/Scenes");

    UPROPERTY(EditAnywhere, Category = "Menu", meta = (ClampMin = "1"))
    int32 MaxNiveles = 10;

private:
    // Ajustes
    float Volumen = 1.0f;
    bool bPantallaCompleta = false;
    int32 IndiceResolucion = 2;

    TArray<FIntPoint> Resoluciones = {
        FIntPoint(1280, 720),
        FIntPoint(1600, 900),
        FIntPoint(1920, 1080),
        FIntPoint(2560, 1440)
    };

    void AplicarAjustes();

    // Crear botones del selector
    TSharedRef<SWidget> CrearBotonNivel(int32 Nivel);
};
