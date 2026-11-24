#include "MecanicaShooter.h"
#include "PersonajeShooter.h"
#include "Proyectil.h"
#include "MiHUD.h"
#include "Camera/CameraComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/InputComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

AMecanicaShooter::AMecanicaShooter()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false; // Solo hace Tick mientras se apunta

	CamaraPrimeraPersona = CreateDefaultSubobject<UCameraComponent>(TEXT("CamaraPrimeraPersona"));
	RootComponent = CamaraPrimeraPersona;
}

void AMecanicaShooter::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		EnableInput(PC);
		if (InputComponent)
		{
			InputComponent->BindKey(EKeys::RightMouseButton, IE_Pressed, this, &AMecanicaShooter::IniciarApuntado);
			InputComponent->BindKey(EKeys::RightMouseButton, IE_Released, this, &AMecanicaShooter::DetenerApuntado);
			InputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &AMecanicaShooter::Disparar);
		}
	}
}

APawn* AMecanicaShooter::ObtenerJugadorSinShooter() const
{
	const APlayerController* PC = GetWorld()->GetFirstPlayerController();
	APawn* Jugador = PC ? PC->GetPawn() : nullptr;

	// Si el personaje ya tiene la mecanica propia, este actor no interviene
	if (!Jugador || Jugador->IsA<APersonajeShooter>())
	{
		return nullptr;
	}
	return Jugador;
}

void AMecanicaShooter::IniciarApuntado()
{
	APawn* Jugador = ObtenerJugadorSinShooter();
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!Jugador || !PC || bApuntando)
	{
		return;
	}
	bApuntando = true;

	// Colocar esta cámara a la altura de los ojos del jugador y usarla como vista
	SetActorLocationAndRotation(Jugador->GetPawnViewLocation(), PC->GetControlRotation());
	VistaAnterior = PC->GetViewTarget();
	PC->SetViewTarget(this);
	SetActorTickEnabled(true);

	if (AMiHUD* HUD = Cast<AMiHUD>(PC->GetHUD()))
	{
		HUD->bEstaApuntando = true;
	}
}

void AMecanicaShooter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Seguir la cabeza y el raton del jugador mientras se apunta
	const APlayerController* PC = GetWorld()->GetFirstPlayerController();
	const APawn* Jugador = PC ? PC->GetPawn() : nullptr;
	if (bApuntando && Jugador)
	{
		SetActorLocationAndRotation(Jugador->GetPawnViewLocation(), PC->GetControlRotation());
	}
}

void AMecanicaShooter::DetenerApuntado()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC || !bApuntando)
	{
		return;
	}
	bApuntando = false;
	SetActorTickEnabled(false);

	AActor* Vista = VistaAnterior.IsValid() ? VistaAnterior.Get() : PC->GetPawn();
	PC->SetViewTarget(Vista);
	VistaAnterior.Reset();

	if (AMiHUD* HUD = Cast<AMiHUD>(PC->GetHUD()))
	{
		HUD->bEstaApuntando = false;
	}
}

void AMecanicaShooter::Disparar()
{
	APawn* Jugador = ObtenerJugadorSinShooter();
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!Jugador || !PC || !PC->PlayerCameraManager)
	{
		return;
	}

	const FVector InicioRayo = PC->PlayerCameraManager->GetCameraLocation();
	const FVector DireccionRayo = PC->PlayerCameraManager->GetCameraRotation().Vector();
	const FVector FinRayo = InicioRayo + DireccionRayo * 10000.0f;

	FHitResult Impacto;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(DisparoMecanica), false, Jugador);
	Params.AddIgnoredActor(this);

	const bool bHit = GetWorld()->LineTraceSingleByChannel(Impacto, InicioRayo, FinRayo, ECC_Visibility, Params);
	const FVector PuntoObjetivo = bHit ? Impacto.ImpactPoint : FinRayo;

	const FVector OrigenBala = bApuntando
		? Jugador->GetPawnViewLocation()
		: Jugador->GetActorLocation() + Jugador->GetActorForwardVector() * 60.0f + FVector(0.0f, 0.0f, 50.0f);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Jugador;
	SpawnParams.Instigator = Jugador;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	GetWorld()->SpawnActor<AProyectil>(AProyectil::StaticClass(), OrigenBala, (PuntoObjetivo - OrigenBala).Rotation(), SpawnParams);
}
