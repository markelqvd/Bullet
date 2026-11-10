#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "PersonajeShooter.generated.h"

UCLASS()
class BULLET_API APersonajeShooter : public ACharacter
{
	GENERATED_BODY()

public:
	APersonajeShooter();

protected:
	virtual void BeginPlay() override;

	// Cámara de 1ª persona para la mirilla
	UPROPERTY(VisibleAnywhere, Category = "Camara")
	UCameraComponent* CamaraPrimeraPersona;

public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Funciones de apuntado y disparo
	void IniciarApuntado();
	void DetenerApuntado();
	void Disparar();

private:
	bool bApuntando = false;
};