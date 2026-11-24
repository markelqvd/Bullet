#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AITypes.h"
#include "Navigation/PathFollowingComponent.h"
#include "EnemigoPatrulla.generated.h"

UCLASS()
class BULLET_API AEnemigoPatrulla : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemigoPatrulla();

	// daño pryectiles
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void BeginPlay() override;

	// lista puntos
	UPROPERTY(EditInstanceOnly, Category = "Patrulla")
	TArray<TObjectPtr<AActor>> PuntosDePatrulla;

	// espera en cada punt
	UPROPERTY(EditAnywhere, Category = "Patrulla")
	float TiempoDeEspera = 1.0f;

	// vida enemgo
	UPROPERTY(EditAnywhere, Category = "Combate")
	float VidaMaxima = 100.0f;

	UPROPERTY(VisibleInstanceOnly, Category = "Combate")
	float Vida = 100.0f;

	
	UPROPERTY(EditAnywhere, Category = "Patrulla")
	float RadioLlegada = 50.0f;

	// pos list
	int32 IndicePuntoActual = 0;

	FTimerHandle TemporizadorEspera;

	void IniciarPatrulla();
	void IrAlSiguientePunto();
	void AvisoPatrulla(const FString& Mensaje) const;

	// Se ejecuta cuando el enemigo termina un movimiento
	UFUNCTION()
	void AlTerminarMovimiento(FAIRequestID RequestID, EPathFollowingResult::Type Resultado);

	void Morir();
};
