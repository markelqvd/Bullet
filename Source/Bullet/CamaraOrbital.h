
#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "CamaraOrbital.generated.h"

UCLASS()
class BULLET_API ACamaraOrbital : public ACameraActor
{
    GENERATED_BODY()

public:
    ACamaraOrbital();

    virtual void Tick(float DeltaSeconds) override;

protected:
    virtual void BeginPlay() override;

    // Distancia de la camara al punto central
    UPROPERTY(EditAnywhere, Category = "Camara")
    float Radio = 900.0f;

    // Altura de la camara
    UPROPERTY(EditAnywhere, Category = "Camara")
    float Altura = 350.0f;

    // Angulo maximo de oscilacion
    UPROPERTY(EditAnywhere, Category = "Camara")
    float Amplitud = 45.0f;

    // Velocidad del movimiento
    UPROPERTY(EditAnywhere, Category = "Camara")
    float Velocidad = 0.6f;

private:
    FVector Centro;
    float Tiempo = 0.0f;
};
