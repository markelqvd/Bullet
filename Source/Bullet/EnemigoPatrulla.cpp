#include "EnemigoPatrulla.h"
#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "TimerManager.h"
#include "Engine/Engine.h"

AEnemigoPatrulla::AEnemigoPatrulla()
{
	
	PrimaryActorTick.bCanEverTick = false;

	
	AIControllerClass = AAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void AEnemigoPatrulla::BeginPlay()
{
	Super::BeginPlay();

	Vida = VidaMaxima;

	
	GetWorldTimerManager().SetTimerForNextTick(this, &AEnemigoPatrulla::IniciarPatrulla);
}

void AEnemigoPatrulla::AvisoPatrulla(const FString& Mensaje) const
{
	const FString Texto = FString::Printf(TEXT("[Patrulla] %s: %s"), *GetActorNameOrLabel(), *Mensaje);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Texto);

#if !UE_BUILD_SHIPPING
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor::Yellow, Texto);
	}
#endif
}

void AEnemigoPatrulla::IniciarPatrulla()
{
	
	if (!Cast<AAIController>(GetController()))
	{
		SpawnDefaultController();
	}

	AAIController* ControladorIA = Cast<AAIController>(GetController());
	if (!ControladorIA)
	{
		AvisoPatrulla(TEXT("no tiene AIController. Revisa 'AI Controller Class' y 'Auto Possess AI' en el Blueprint."));
		return;
	}

	if (PuntosDePatrulla.IsEmpty())
	{
		AvisoPatrulla(TEXT("la lista 'Puntos De Patrulla' esta vacia. Seleccionalo en el nivel y asigna puntos."));
		return;
	}

	// 
	ControladorIA->ReceiveMoveCompleted.AddUniqueDynamic(this, &AEnemigoPatrulla::AlTerminarMovimiento);
	IrAlSiguientePunto();
}

void AEnemigoPatrulla::IrAlSiguientePunto()
{
	AAIController* ControladorIA = Cast<AAIController>(GetController());
	if (!ControladorIA || PuntosDePatrulla.IsEmpty() || Vida <= 0.0f)
	{
		return;
	}

	AActor* Punto = PuntosDePatrulla[IndicePuntoActual];
	IndicePuntoActual = (IndicePuntoActual + 1) % PuntosDePatrulla.Num();

	
	if (!IsValid(Punto))
	{
		AvisoPatrulla(TEXT("hay un hueco vacio en 'Puntos De Patrulla'."));
		GetWorldTimerManager().SetTimer(TemporizadorEspera, this, &AEnemigoPatrulla::IrAlSiguientePunto, 0.5f, false);
		return;
	}

	const EPathFollowingRequestResult::Type Resultado =
		ControladorIA->MoveToActor(Punto, RadioLlegada, /*bStopOnOverlap*/ true, /*bUsePathfinding*/ true,
			/*bCanStrafe*/ false, /*FilterClass*/ nullptr, /*bAllowPartialPath*/ true);

	switch (Resultado)
	{
	case EPathFollowingRequestResult::Failed:
		
		AvisoPatrulla(FString::Printf(
			TEXT("no encuentra camino hasta '%s'. Pon un NavMeshBoundsVolume que cubra la zona (pulsa P para verlo en verde)."),
			*Punto->GetActorNameOrLabel()));
		GetWorldTimerManager().SetTimer(TemporizadorEspera, this, &AEnemigoPatrulla::IrAlSiguientePunto, 1.0f, false);
		break;

	case EPathFollowingRequestResult::AlreadyAtGoal:
		
		GetWorldTimerManager().SetTimer(TemporizadorEspera, this, &AEnemigoPatrulla::IrAlSiguientePunto,
			FMath::Max(TiempoDeEspera, 0.01f), false);
		break;

	default:
		
		break;
	}
}

void AEnemigoPatrulla::AlTerminarMovimiento(FAIRequestID RequestID, EPathFollowingResult::Type Resultado)
{
	
	GetWorldTimerManager().SetTimer(TemporizadorEspera, this, &AEnemigoPatrulla::IrAlSiguientePunto,
		FMath::Max(TiempoDeEspera, 0.01f), false);
}

float AEnemigoPatrulla::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	const float DanoAplicado = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (Vida <= 0.0f)
	{
		return 0.0f;
	}

	Vida -= DanoAplicado;
	if (Vida <= 0.0f)
	{
		Morir();
	}
	return DanoAplicado;
}

void AEnemigoPatrulla::Morir()
{
	GetWorldTimerManager().ClearTimer(TemporizadorEspera);

	if (AAIController* ControladorIA = Cast<AAIController>(GetController()))
	{
		ControladorIA->StopMovement();
		ControladorIA->ReceiveMoveCompleted.RemoveDynamic(this, &AEnemigoPatrulla::AlTerminarMovimiento);
	}

	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);
	SetLifeSpan(5.0f);
}
