// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemigoPatrulla.h"
#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

// Sets default values
AEnemigoPatrulla::AEnemigoPatrulla()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//Ia controla enemigo
	AIControllerClass = AAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// Hace que mire hacia donde camina
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

}

// Called when the game starts or when spawned
void AEnemigoPatrulla::BeginPlay()
{
	Super::BeginPlay();

	if (AAIController* ControladorIA = Cast<AAIController>(GetController()))
	{
		// Avisa al enemigo cuando termine cada movimiento
		ControladorIA->ReceiveMoveCompleted.AddDynamic(this, &AEnemigoPatrulla::AlTerminarMovimiento);
		IrAlSiguientePunto();
	}
	
}

// Called every frame
void AEnemigoPatrulla::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemigoPatrulla::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
void AEnemigoPatrulla::IrAlSiguientePunto()
{
	AAIController* ControladorIA = Cast<AAIController>(GetController());
	if (!ControladorIA || PuntosDePatrulla.IsEmpty()) { return; }

	ControladorIA->MoveToActor(PuntosDePatrulla[IndicePuntoActual], 50.0f);
	IndicePuntoActual = (IndicePuntoActual + 1) % PuntosDePatrulla.Num();
}

void AEnemigoPatrulla::AlTerminarMovimiento(FAIRequestID RequestID, EPathFollowingResult::Type Resultado)
{
	// Espera y luego va al siguiente punto
	GetWorldTimerManager().SetTimer(TemporizadorEspera, this, &AEnemigoPatrulla::IrAlSiguientePunto, FMath::Max(TiempoDeEspera, 0.01f), false);
}

