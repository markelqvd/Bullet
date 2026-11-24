#include "PersonajeShooter.h"
#include "Proyectil.h"
#include "MiHUD.h"
#include "Camera/CameraComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/InputComponent.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"

APersonajeShooter::APersonajeShooter()
{
	PrimaryActorTick.bCanEverTick = false;

	CamaraPrimeraPersona = CreateDefaultSubobject<UCameraComponent>(TEXT("CamaraPrimeraPersona"));
	CamaraPrimeraPersona->SetupAttachment(GetMesh(), TEXT("head"));
	CamaraPrimeraPersona->SetRelativeLocation(FVector(0.0f, 15.0f, 0.0f));
	CamaraPrimeraPersona->bUsePawnControlRotation = true;
	CamaraPrimeraPersona->bAutoActivate = false; // Empieza apagada

	ClaseProyectil = AProyectil::StaticClass();
}

void APersonajeShooter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* InputMejorado = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (InputMejorado && AccionApuntar)
	{
		InputMejorado->BindAction(AccionApuntar, ETriggerEvent::Started, this, &APersonajeShooter::IniciarApuntado);
		InputMejorado->BindAction(AccionApuntar, ETriggerEvent::Completed, this, &APersonajeShooter::DetenerApuntado);
		InputMejorado->BindAction(AccionApuntar, ETriggerEvent::Canceled, this, &APersonajeShooter::DetenerApuntado);
	}
	else
	{
		PlayerInputComponent->BindKey(EKeys::RightMouseButton, IE_Pressed, this, &APersonajeShooter::IniciarApuntado);
		PlayerInputComponent->BindKey(EKeys::RightMouseButton, IE_Released, this, &APersonajeShooter::DetenerApuntado);
	}

	if (InputMejorado && AccionDisparar)
	{
		InputMejorado->BindAction(AccionDisparar, ETriggerEvent::Started, this, &APersonajeShooter::Disparar);
	}
	else
	{
		PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &APersonajeShooter::Disparar);
	}
}

void APersonajeShooter::IniciarApuntado()
{
	if (bApuntando)
	{
		return;
	}
	bApuntando = true;

	ActivarCamaraPrimeraPersona(true);
	ActualizarMirilla(true);
}

void APersonajeShooter::DetenerApuntado()
{
	if (!bApuntando)
	{
		return;
	}
	bApuntando = false;

	ActivarCamaraPrimeraPersona(false);
	ActualizarMirilla(false);
}

void APersonajeShooter::ActivarCamaraPrimeraPersona(bool bActivar)
{
	TArray<UCameraComponent*> Camaras;
	GetComponents<UCameraComponent>(Camaras);
	for (UCameraComponent* Cam : Camaras)
	{
		const bool bEsPrimeraPersona = (Cam == CamaraPrimeraPersona);
		Cam->SetActive(bEsPrimeraPersona ? bActivar : !bActivar);
	}

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC)
	{
		return;
	}

	if (bActivar)
	{
		AActor* VistaActual = PC->GetViewTarget();
		if (VistaActual != this)
		{
			VistaAnterior = VistaActual;
			PC->SetViewTarget(this);
		}
	}
	else if (VistaAnterior.IsValid())
	{
		PC->SetViewTarget(VistaAnterior.Get());
		VistaAnterior.Reset();
	}
}

void APersonajeShooter::ActualizarMirilla(bool bVisible) const
{
	if (const APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (AMiHUD* HUD = Cast<AMiHUD>(PC->GetHUD()))
		{
			HUD->bEstaApuntando = bVisible;
		}
	}
}

void APersonajeShooter::Disparar()
{
	UWorld* Mundo = GetWorld();
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!Mundo || !PC || !PC->PlayerCameraManager || !ClaseProyectil)
	{
		return;
	}

	const float Ahora = Mundo->GetTimeSeconds();
	if (Ahora - UltimoDisparo < CadenciaDisparo)
	{
		return;
	}
	UltimoDisparo = Ahora;

	const FVector InicioRayo = PC->PlayerCameraManager->GetCameraLocation();
	const FVector DireccionRayo = PC->PlayerCameraManager->GetCameraRotation().Vector();
	const FVector FinRayo = InicioRayo + DireccionRayo * AlcanceDisparo;

	FHitResult Impacto;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(DisparoPersonaje), false, this);
	const bool bHit = Mundo->LineTraceSingleByChannel(Impacto, InicioRayo, FinRayo, ECC_Visibility, Params);
	const FVector PuntoObjetivo = bHit ? Impacto.ImpactPoint : FinRayo;

	const FVector OrigenBala = bApuntando
		? CamaraPrimeraPersona->GetComponentLocation()
		: GetActorLocation() + GetActorForwardVector() * 60.0f + FVector(0.0f, 0.0f, 50.0f);
	const FRotator RotacionBala = (PuntoObjetivo - OrigenBala).Rotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	Mundo->SpawnActor<AProyectil>(ClaseProyectil, OrigenBala, RotacionBala, SpawnParams);
}
