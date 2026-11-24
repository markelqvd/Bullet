#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MecanicaShooter.generated.h"

class UCameraComponent;

UCLASS()
class BULLET_API AMecanicaShooter : public AActor
{
	GENERATED_BODY()

public:
	AMecanicaShooter();

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, Category = "Camara")
	TObjectPtr<UCameraComponent> CamaraPrimeraPersona;

	void IniciarApuntado();
	void DetenerApuntado();
	void Disparar();

protected:
	virtual void BeginPlay() override;

private:
	bool bApuntando = false;
	TWeakObjectPtr<AActor> VistaAnterior;

	APawn* ObtenerJugadorSinShooter() const;
};
