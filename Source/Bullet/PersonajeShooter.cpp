#include "PersonajeShooter.h"
#include "Proyectil.h"
#include "MiHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Components/InputComponent.h"

APersonajeShooter::APersonajeShooter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Crear cámara de 1ª persona fijada a la cabeza/ojos del personaje
	CamaraPrimeraPersona = CreateDefaultSubobject<UCameraComponent>(TEXT("CamaraPrimeraPersona"));
	CamaraPrimeraPersona->SetupAttachment(GetMesh(), TEXT("head"));
	CamaraPrimeraPersona->SetRelativeLocation(FVector(0.0f, 15.0f, 0.0f));
	CamaraPrimeraPersona->bUsePawnControlRotation = true;
	CamaraPrimeraPersona->SetActive(false); // Empieza apagada
}

void APersonajeShooter::BeginPlay()
{
	Super::BeginPlay();
}

void APersonajeShooter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindKey(EKeys::RightMouseButton, IE_Pressed, this, &APersonajeShooter::IniciarApuntado);
	PlayerInputComponent->BindKey(EKeys::RightMouseButton, IE_Released, this, &APersonajeShooter::DetenerApuntado);
	PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &APersonajeShooter::Disparar);
}

void APersonajeShooter::IniciarApuntado()
{
	bApuntando = true;

	// 1. Apagar la cámara de 3ª persona (FollowCamera)
	TArray<UCameraComponent*> Camaras;
	GetComponents<UCameraComponent>(Camaras);
	for (UCameraComponent* Cam : Camaras)
	{
		if (Cam != CamaraPrimeraPersona)
		{
			Cam->SetActive(false);
		}
	}

	// 2. Encender la cámara de 1ª persona
	if (CamaraPrimeraPersona)
	{
		CamaraPrimeraPersona->SetActive(true);
	}

	// 3. Mostrar el HUD
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC && PC->GetHUD())
	{
		AMiHUD* HUD = Cast<AMiHUD>(PC->GetHUD());
		if (HUD) HUD->bEstaApuntando = true;
	}
}

void APersonajeShooter::DetenerApuntado()
{
	bApuntando = false;

	// 1. Apagar la cámara de 1ª persona
	if (CamaraPrimeraPersona)
	{
		CamaraPrimeraPersona->SetActive(false);
	}

	// 2. Volver a encender la cámara de 3ª persona
	TArray<UCameraComponent*> Camaras;
	GetComponents<UCameraComponent>(Camaras);
	for (UCameraComponent* Cam : Camaras)
	{
		if (Cam != CamaraPrimeraPersona)
		{
			Cam->SetActive(true);
		}
	}

	// 3. Ocultar el HUD
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC && PC->GetHUD())
	{
		AMiHUD* HUD = Cast<AMiHUD>(PC->GetHUD());
		if (HUD) HUD->bEstaApuntando = false;
	}
}

void APersonajeShooter::Disparar()
{
	APlayerCameraManager* CamaraMgr = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if (!CamaraMgr) return;

	FVector InicioRayo = CamaraMgr->GetCameraLocation();
	FVector DireccionRayo = CamaraMgr->GetActorForwardVector();
	FVector FinRayo = InicioRayo + (DireccionRayo * 10000.0f);

	FHitResult Impacto;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(Impacto, InicioRayo, FinRayo, ECC_Visibility, Params);
	FVector PuntoObjetivo = bHit ? Impacto.ImpactPoint : FinRayo;

	FVector OrigenBala = CamaraPrimeraPersona->GetComponentLocation();
	FVector DireccionBala = PuntoObjetivo - OrigenBala;
	FRotator RotacionBala = DireccionBala.Rotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;

	GetWorld()->SpawnActor<AProyectil>(AProyectil::StaticClass(), OrigenBala, RotacionBala, SpawnParams);
}