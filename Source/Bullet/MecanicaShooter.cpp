#include "MecanicaShooter.h"
#include "Proyectil.h"
#include "MiHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Components/InputComponent.h"
#include "GameFramework/PlayerController.h"

AMecanicaShooter::AMecanicaShooter()
{
	PrimaryActorTick.bCanEverTick = false;

	CamaraPrimeraPersona = CreateDefaultSubobject<UCameraComponent>(TEXT("CamaraPrimeraPersona"));
}

void AMecanicaShooter::BeginPlay()
{
	Super::BeginPlay();

	// Conectar las teclas de ratón directamente en C++ al arrancar el nivel
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

void AMecanicaShooter::IniciarApuntado()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	APawn* Jugador = PC->GetPawn();
	if (!Jugador) return;

	if (CamaraPrimeraPersona)
	{
		// Enganchar la cámara de 1ª persona a la cabeza del monigote que esté en escena
		if (!CamaraPrimeraPersona->GetAttachParent())
		{
			CamaraPrimeraPersona->AttachToComponent(Jugador->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			CamaraPrimeraPersona->SetRelativeLocation(FVector(0.0f, 0.0f, 60.0f));
		}

		// Desactivar la cámara 3ª persona previa
		TArray<UCameraComponent*> Camaras;
		Jugador->GetComponents<UCameraComponent>(Camaras);
		for (UCameraComponent* Cam : Camaras)
		{
			if (Cam != CamaraPrimeraPersona)
			{
				Cam->SetActive(false);
			}
		}

		CamaraPrimeraPersona->SetActive(true);
	}

	// Activar la mirilla en el HUD
	AMiHUD* HUD = Cast<AMiHUD>(PC->GetHUD());
	if (HUD)
	{
		HUD->bEstaApuntando = true;
	}
}

void AMecanicaShooter::DetenerApuntado()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	APawn* Jugador = PC->GetPawn();
	if (!Jugador) return;

	if (CamaraPrimeraPersona)
	{
		CamaraPrimeraPersona->SetActive(false);

		// Reactivar cámara de 3ª persona
		TArray<UCameraComponent*> Camaras;
		Jugador->GetComponents<UCameraComponent>(Camaras);
		for (UCameraComponent* Cam : Camaras)
		{
			if (Cam != CamaraPrimeraPersona)
			{
				Cam->SetActive(true);
			}
		}
	}

	AMiHUD* HUD = Cast<AMiHUD>(PC->GetHUD());
	if (HUD)
	{
		HUD->bEstaApuntando = false;
	}
}

void AMecanicaShooter::Disparar()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	APlayerCameraManager* CamaraMgr = PC->PlayerCameraManager;
	if (!CamaraMgr) return;

	FVector InicioRayo = CamaraMgr->GetCameraLocation();
	FVector DireccionRayo = CamaraMgr->GetActorForwardVector();
	FVector FinRayo = InicioRayo + (DireccionRayo * 10000.0f);

	FHitResult Impacto;
	FCollisionQueryParams Params;
	if (PC->GetPawn())
	{
		Params.AddIgnoredActor(PC->GetPawn());
	}

	bool bHit = GetWorld()->LineTraceSingleByChannel(Impacto, InicioRayo, FinRayo, ECC_Visibility, Params);
	FVector PuntoObjetivo = bHit ? Impacto.ImpactPoint : FinRayo;

	FVector OrigenBala = CamaraMgr->GetCameraLocation();
	FVector DireccionBala = PuntoObjetivo - OrigenBala;
	FRotator RotacionBala = DireccionBala.Rotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = PC->GetPawn();

	GetWorld()->SpawnActor<AProyectil>(AProyectil::StaticClass(), OrigenBala, RotacionBala, SpawnParams);
}