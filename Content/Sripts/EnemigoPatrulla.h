// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AITypes.h" // Añade esta línea
#include "Navigation/PathFollowingComponent.h" // Añade esta línea
#include "EnemigoPatrulla.generated.h"

UCLASS()
class BULLET_API AEnemigoPatrulla : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemigoPatrulla();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected: 
	//Lista Puntos
	UPROPERTY(EditInstanceOnly, Category="Patrulla")
	TArray<TObjectPtr<AActor>> PuntosDePatrulla;
	//segundos en cada punto
	UPROPERTY(EditAnywhere, Category="Patrulla")
	float TiempoDeEspera = 1.0f;
	//posicion en la lista
	int32 IndicePuntoActual = 0;
	//Temporizador
	FTimerHandle TemporizadorEspera;

	//siguiente punto
	void IrAlSiguientePunto();

	// Se ejecuta cuando el enemigo termina un movimiento
	UFUNCTION()
	void AlTerminarMovimiento(FAIRequestID RequestID, EPathFollowingResult::Type Resultado);

};
