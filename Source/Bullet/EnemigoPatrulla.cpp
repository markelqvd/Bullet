#include "EnemigoPatrulla.h"
#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "TimerManager.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

AEnemigoPatrulla::AEnemigoPatrulla()
{
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void AEnemigoPatrulla::BeginPlay()
{
	Super::BeginPlay();

	Vida = VidaMaxima;

	Jugador = UGameplayStatics::GetPlayerPawn(this, 0);

	GetWorldTimerManager().SetTimerForNextTick(this, &AEnemigoPatrulla::IniciarPatrulla);
}

void AEnemigoPatrulla::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Vida <= 0.0f)
	{
		return;
	}

	if (!Jugador)
	{
		Jugador = UGameplayStatics::GetPlayerPawn(this, 0);
	}

	bVeJugador = PuedeVerAlJugador();
	if (bVeJugador)
	{
		UltimaPosicionConocida = Jugador->GetActorLocation();
	}

	ActualizarSospecha(DeltaTime);
	ActualizarEstado(DeltaTime);

	if (bMostrarDebugVision)
	{
		DibujarDebugVision();
	}
}

void AEnemigoPatrulla::ObtenerVision(FVector& Origen, FVector& Direccion) const
{
	Origen = GetPawnViewLocation();
	Direccion = GetActorForwardVector();
}

bool AEnemigoPatrulla::PuedeVerAlJugador() const
{
	if (!Jugador)
	{
		return false;
	}

	FVector Origen, Direccion;
	ObtenerVision(Origen, Direccion);

	const FVector Objetivo = Jugador->GetActorLocation();
	const FVector HaciaJugador = Objetivo - Origen;
	const float Distancia = HaciaJugador.Size();

	if (Distancia > DistanciaVision)
	{
		return false;
	}

	const FVector DireccionAlJugador = HaciaJugador.GetSafeNormal();
	const float Alineacion = FVector::DotProduct(Direccion, DireccionAlJugador);
	const float LimiteCono = FMath::Cos(FMath::DegreesToRadians(AnguloVision));

	if (Alineacion < LimiteCono)
	{
		return false;
	}

	FHitResult Impacto;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(VisionEnemigo), true, this);

	const bool bChoca = GetWorld()->LineTraceSingleByChannel(Impacto, Origen, Objetivo, ECC_Visibility, Params);

	return !bChoca || Impacto.GetActor() == Jugador;
}

void AEnemigoPatrulla::DibujarDebugVision() const
{
#if ENABLE_DRAW_DEBUG
	FVector Origen, Direccion;
	ObtenerVision(Origen, Direccion);

	const float AnguloRad = FMath::DegreesToRadians(AnguloVision);

	const FColor ColorCono = bVeJugador ? FColor::Red : FColor::Yellow;
	DrawDebugCone(GetWorld(), Origen, Direccion, DistanciaVision, AnguloRad, AnguloRad,
		16, ColorCono, false, -1.0f, 0, 1.5f);

	if (Jugador && FVector::Dist(Origen, Jugador->GetActorLocation()) <= DistanciaVision)
	{
		const FColor ColorLinea = bVeJugador ? FColor::Red : FColor(128, 128, 128);
		DrawDebugLine(GetWorld(), Origen, Jugador->GetActorLocation(), ColorLinea, false, -1.0f, 0, 2.0f);
	}
	FColor ColorEstado = FColor::Green;
	if (EstadoActual == EEstadoEnemigo::Sospecha) { ColorEstado = FColor::Yellow; }
	if (EstadoActual == EEstadoEnemigo::Detectado) { ColorEstado = FColor::Red; }

	const FString TextoEstado = FString::Printf(TEXT("%s  %.0f%%"),
		*UEnum::GetDisplayValueAsText(EstadoActual).ToString(), Sospecha * 100.0f);

	DrawDebugString(GetWorld(), GetActorLocation() + FVector(0.0f, 0.0f, 120.0f),
		TextoEstado, nullptr, ColorEstado, 0.0f, true);
#endif
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

	// Avisa al enemigo cuando termine cada movimiento
	ControladorIA->ReceiveMoveCompleted.AddUniqueDynamic(this, &AEnemigoPatrulla::AlTerminarMovimiento);

	if (PuntosDePatrulla.IsEmpty())
	{
		AvisoPatrulla(TEXT("la lista 'Puntos De Patrulla' esta vacia. Seleccionalo en el nivel y asigna puntos."));
		return;
	}

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
		ControladorIA->MoveToActor(Punto, RadioLlegada, true, true,
			false, nullptr, true);

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
	// Espera y luego va al siguiente punto
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

void AEnemigoPatrulla::CambiarEstado(EEstadoEnemigo Nuevo)
{
	if (Nuevo == EstadoActual)
	{
		return;
	}

	EstadoActual = Nuevo;
	if (Nuevo == EEstadoEnemigo::Detectado)
	{
		TiempoSinVerlo = 0.0f;
	}

	AvisoPatrulla(FString::Printf(TEXT("Nuevo estado: %s"), *UEnum::GetValueAsString(EstadoActual)));
}

void AEnemigoPatrulla::ActualizarSospecha(float DeltaTime)
{
	if (EstadoActual == EEstadoEnemigo::Detectado)
	{
		return;
	}

	if (bVeJugador)
	{
		const float Distancia = FVector::Dist(GetActorLocation(), Jugador->GetActorLocation());

		if (Distancia <= DistanciaDeteccionInmediata)
		{
			Sospecha = 1.0f;
		}
		else
		{
			const float Cercania = 1.0f - (Distancia / DistanciaVision);
			Sospecha += VelocidadSubida * (1.0f + Cercania * 2.0f) * DeltaTime;
		}
	}
	else
	{
		Sospecha -= VelocidadBajada * DeltaTime;
	}

	Sospecha = FMath::Clamp(Sospecha, 0.0f, 1.0f);
}

void AEnemigoPatrulla::ActualizarEstado(float DeltaTime)
{
	switch (EstadoActual)
	{
	case EEstadoEnemigo::Buscando:
		if (Sospecha > 0.0f)
		{
			CambiarEstado(EEstadoEnemigo::Sospecha);
		}
		break;

	case EEstadoEnemigo::Sospecha:
		if (Sospecha >= 1.0f)
		{
			CambiarEstado(EEstadoEnemigo::Detectado);
		}
		else if (Sospecha <= 0.0f)
		{
			CambiarEstado(EEstadoEnemigo::Buscando);
		}
		break;

	case EEstadoEnemigo::Detectado:
		if (bVeJugador)
		{
			TiempoSinVerlo = 0.0f;
		}
		else
		{
			TiempoSinVerlo += DeltaTime;

			if (TiempoSinVerlo >= TiempoParaPerderlo)
			{
				Sospecha = SospechaTrasPerderlo;
				CambiarEstado(EEstadoEnemigo::Sospecha);
			}
		}
		break;
	}
}