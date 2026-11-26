#include "PersonajeShooter.h"
#include "Proyectil.h"
#include "MiHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/PlayerController.h"

APersonajeShooter::APersonajeShooter()
{
	PrimaryActorTick.bCanEverTick = false;

	CamaraPrimeraPersona = CreateDefaultSubobject<UCameraComponent>(TEXT("CamaraPrimeraPersona"));
	CamaraPrimeraPersona->SetupAttachment(GetMesh(), TEXT("head"));
	CamaraPrimeraPersona->SetRelativeLocation(FVector(0.0f, 15.0f, 0.0f));
	CamaraPrimeraPersona->bUsePawnControlRotation = true;
	CamaraPrimeraPersona->SetActive(false);
}

void APersonajeShooter::BeginPlay()
{
	Super::BeginPlay();

	// Forzar que se vea en tercera persona al iniciar la partida para corregir bug de camara en primera persona al iniciar la partida
	DetenerApuntado();
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
	// Si ya gasto su unica bala, no vuelve a entrar en modo francotirador
	if (bBalaDisparada) return;

	bApuntando = true;

	// Apagar camara de 3a persona de los companeros y encender 1a persona
	TArray<UCameraComponent*> Camaras;
	GetComponents<UCameraComponent>(Camaras);
	for (UCameraComponent* Cam : Camaras)
	{
		if (Cam != CamaraPrimeraPersona)
		{
			Cam->SetActive(false);
		}
	}

	if (CamaraPrimeraPersona)
	{
		CamaraPrimeraPersona->SetActive(true);
	}

	if (GetMesh())
	{
		GetMesh()->SetOwnerNoSee(true);
	}

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		AMiHUD* HUD = Cast<AMiHUD>(PC->GetHUD());
		if (HUD) HUD->bEstaApuntando = true;
	}
}

void APersonajeShooter::DetenerApuntado()
{
	// Si ya disparo la bala, mantener la camara siguiendo la bala
	if (bBalaDisparada) return;

	bApuntando = false;

	if (CamaraPrimeraPersona)
	{
		CamaraPrimeraPersona->SetActive(false);
	}

	TArray<UCameraComponent*> Camaras;
	GetComponents<UCameraComponent>(Camaras);
	for (UCameraComponent* Cam : Camaras)
	{
		if (Cam != CamaraPrimeraPersona)
		{
			Cam->SetActive(true);
		}
	}

	if (GetMesh())
	{
		GetMesh()->SetOwnerNoSee(false);
	}

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		AMiHUD* HUD = Cast<AMiHUD>(PC->GetHUD());
		if (HUD) HUD->bEstaApuntando = false;
	}
}

void APersonajeShooter::Disparar()
{
	// Solo dispara si esta apuntando en modo francotirador y aun tiene su unica bala
	if (!bApuntando || bBalaDisparada) return;

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	bBalaDisparada = true;
	bApuntando = false;

	// Quitar la mirilla de francotirador y volver a hacer visible el personaje
	AMiHUD* HUD = Cast<AMiHUD>(PC->GetHUD());
	if (HUD) HUD->bEstaApuntando = false;

	if (GetMesh())
	{
		GetMesh()->SetOwnerNoSee(false);
	}

	APlayerCameraManager* CamaraMgr = PC->PlayerCameraManager;
	if (!CamaraMgr) return;

	FVector InicioRayo = CamaraMgr->GetCameraLocation();
	FVector DireccionRayo = CamaraMgr->GetActorForwardVector();
	FVector FinRayo = InicioRayo + (DireccionRayo * 10000.0f);

	FHitResult Impacto;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(Impacto, InicioRayo, FinRayo, ECC_Visibility, Params);
	FVector PuntoObjetivo = bHit ? Impacto.ImpactPoint : FinRayo;

	FVector OrigenBala = InicioRayo + (DireccionRayo * 60.0f);
	FRotator RotacionBala = (PuntoObjetivo - OrigenBala).Rotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;

	AProyectil* Bala = GetWorld()->SpawnActor<AProyectil>(AProyectil::StaticClass(), OrigenBala, RotacionBala, SpawnParams);
	if (Bala)
	{
		// La camara pasa a seguir inmediatamente a la bala
		PC->SetViewTarget(Bala);
	}
}