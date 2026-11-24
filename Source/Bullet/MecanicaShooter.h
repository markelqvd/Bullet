#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MecanicaShooter.generated.h"

class UCameraComponent;

/**
 * OBSOLETO: mecanica de disparo como actor suelto en el nivel.
 * Se mantiene solo para que los niveles que ya lo tienen colocado sigan funcionando
 * mientras el personaje no herede de APersonajeShooter.
 * Si el jugador es un APersonajeShooter, este actor no hace nada (evita doble disparo).
 * Cuando BP_ThirdPersonCharacter herede de PersonajeShooter, borra este actor de los niveles.
 */
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

	// Devuelve el pawn del jugador solo si NO es un APersonajeShooter
	APawn* ObtenerJugadorSinShooter() const;
};
