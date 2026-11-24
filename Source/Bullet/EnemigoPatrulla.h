#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AITypes.h"
#include "Navigation/PathFollowingComponent.h"
#include "EnemigoPatrulla.generated.h"

UENUM(BlueprintType)
enum class EEstadoEnemigo : uint8
{
	Buscando  UMETA(DisplayName = "Buscando"),
	Sospecha  UMETA(DisplayName = "Sospecha"),
	Detectado UMETA(DisplayName = "Detectado")
};

UCLASS()
class BULLET_API AEnemigoPatrulla : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemigoPatrulla();

	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditInstanceOnly, Category = "Patrulla")
	TArray<TObjectPtr<AActor>> PuntosDePatrulla;

	UPROPERTY(EditAnywhere, Category = "Patrulla")
	float TiempoDeEspera = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Patrulla")
	float RadioLlegada = 50.0f;

	UPROPERTY(EditAnywhere, Category = "Vision", meta = (ClampMin = "0"))
	float DistanciaVision = 1500.0f;

	UPROPERTY(EditAnywhere, Category = "Vision", meta = (ClampMin = "1", ClampMax = "89"))
	float AnguloVision = 35.0f;

	UPROPERTY(EditAnywhere, Category = "Vision")
	bool bMostrarDebugVision = true;

	UPROPERTY(VisibleInstanceOnly, Category = "Vision")
	bool bVeJugador = false;

	UPROPERTY(EditAnywhere, Category = "Sospecha", meta = (ClampMin = "0"))
	float VelocidadSubida = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Sospecha", meta = (ClampMin = "0"))
	float VelocidadBajada = 0.2f;

	UPROPERTY(EditAnywhere, Category = "Sospecha", meta = (ClampMin = "0"))
	float DistanciaDeteccionInmediata = 300.0f;

	UPROPERTY(EditAnywhere, Category = "Sospecha", meta = (ClampMin = "0"))
	float TiempoParaPerderlo = 3.0f;

	UPROPERTY(EditAnywhere, Category = "Sospecha", meta = (ClampMin = "0", ClampMax = "1"))
	float SospechaTrasPerderlo = 0.6f;

	UPROPERTY(VisibleInstanceOnly, Category = "Sospecha")
	float Sospecha = 0.0f;

	float TiempoSinVerlo = 0.0f;
	FVector UltimaPosicionConocida = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "Combate")
	float VidaMaxima = 100.0f;

	UPROPERTY(VisibleInstanceOnly, Category = "Combate")
	float Vida = 100.0f;

	UPROPERTY(VisibleInstanceOnly, Category = "Deteccion")
	EEstadoEnemigo EstadoActual = EEstadoEnemigo::Buscando;

	UPROPERTY(Transient)
	TObjectPtr<APawn> Jugador;

	int32 IndicePuntoActual = 0;
	FTimerHandle TemporizadorEspera;

	void IniciarPatrulla();
	void IrAlSiguientePunto();

	UFUNCTION()
	void AlTerminarMovimiento(FAIRequestID RequestID, EPathFollowingResult::Type Resultado);

	void CambiarEstado(EEstadoEnemigo Nuevo);
	void ActualizarSospecha(float DeltaTime);
	void ActualizarEstado(float DeltaTime);

	void ObtenerVision(FVector& Origen, FVector& Direccion) const;

	bool PuedeVerAlJugador() const;

	void DibujarDebugVision() const;

	void AvisoPatrulla(const FString& Mensaje) const;
	void Morir();
};