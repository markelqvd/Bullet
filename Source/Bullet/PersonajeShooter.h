#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PersonajeShooter.generated.h"

class UCameraComponent;
class UInputAction;
class AProyectil;

/**
 * Personaje con apuntado en 1ª persona y disparo.
 * BP_ThirdPersonCharacter debe heredar de esta clase
 * (Class Settings > Parent Class > PersonajeShooter).
 */
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

	// Acciones de Enhanced Input (opcionales).
	// Si se dejan vacias se usan el boton derecho/izquierdo del raton directamente.
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> AccionApuntar;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> AccionDisparar;

	// Clase de bala que se dispara (se puede cambiar por un Blueprint hijo)
	UPROPERTY(EditDefaultsOnly, Category = "Shooter")
	TSubclassOf<AProyectil> ClaseProyectil;

	// Tiempo minimo entre disparos
	UPROPERTY(EditDefaultsOnly, Category = "Shooter", meta = (ClampMin = "0.0"))
	float CadenciaDisparo = 0.15f;

	// Alcance del rayo de apuntado
	UPROPERTY(EditDefaultsOnly, Category = "Shooter")
	float AlcanceDisparo = 10000.0f;

private:
	bool bApuntando = false;
	float UltimoDisparo = -1000.0f;

	// Vista que habia antes de apuntar (p. ej. CamaraOrbital) para restaurarla
	TWeakObjectPtr<AActor> VistaAnterior;

	void ActivarCamaraPrimeraPersona(bool bActivar);
	void ActualizarMirilla(bool bVisible) const;
};
