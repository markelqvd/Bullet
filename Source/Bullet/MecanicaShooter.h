#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"
#include "MecanicaShooter.generated.h"

UCLASS()
class BULLET_API AMecanicaShooter : public AActor
{
	GENERATED_BODY()

public:
	AMecanicaShooter();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, Category = "Camara")
	UCameraComponent* CamaraPrimeraPersona;

	void IniciarApuntado();
	void DetenerApuntado();
	void Disparar();
};