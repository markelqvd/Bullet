#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PersonajeShooter.generated.h"

class UCameraComponent;
class UInputAction;
class AProyectil;

UCLASS()
class BULLET_API APersonajeShooter : public ACharacter
{
	GENERATED_BODY()

public:
	APersonajeShooter();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Funciones de apuntado y disparo
	UFUNCTION(BlueprintCallable, Category = "Shooter")
	void IniciarApuntado();

	UFUNCTION(BlueprintCallable, Category = "Shooter")
	void DetenerApuntado();

	UFUNCTION(BlueprintCallable, Category = "Shooter")
	void Disparar();

	UFUNCTION(BlueprintPure, Category = "Shooter")
	bool EstaApuntando() const { return bApuntando; }

protected:
	// Cámara de 1ª persona para la mirilla
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camara")
	TObjectPtr<UCameraComponent> CamaraPrimeraPersona;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> AccionApuntar;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> AccionDisparar;

	UPROPERTY(EditDefaultsOnly, Category = "Shooter")
	TSubclassOf<AProyectil> ClaseProyectil;

	UPROPERTY(EditDefaultsOnly, Category = "Shooter", meta = (ClampMin = "0.0"))
	float CadenciaDisparo = 0.15f;

	UPROPERTY(EditDefaultsOnly, Category = "Shooter")
	float AlcanceDisparo = 10000.0f;

private:
	bool bApuntando = false;
	float UltimoDisparo = -1000.0f;

	TWeakObjectPtr<AActor> VistaAnterior;

	void ActivarCamaraPrimeraPersona(bool bActivar);
	void ActualizarMirilla(bool bVisible) const;
};
