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

public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camara")
	UCameraComponent* CamaraPrimeraPersona;

	UPROPERTY(BlueprintReadOnly, Category = "Estado")
	bool bApuntando = false;

	UPROPERTY(BlueprintReadOnly, Category = "Estado")
	bool bBalaDisparada = false;

	void IniciarApuntado();
	void DetenerApuntado();
	void Disparar();
};