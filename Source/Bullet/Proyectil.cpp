#include "Proyectil.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AProyectil::AProyectil()
{
	PrimaryActorTick.bCanEverTick = false;

	InitialLifeSpan = 3.0f;

	ColisionProyectil = CreateDefaultSubobject<USphereComponent>(TEXT("ColisionProyectil"));
	ColisionProyectil->InitSphereRadius(5.0f);
	ColisionProyectil->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	ColisionProyectil->SetNotifyRigidBodyCollision(true);
	ColisionProyectil->CanCharacterStepUpOn = ECB_No;
	RootComponent = ColisionProyectil;

	MeshProyectil = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshProyectil"));
	MeshProyectil->SetupAttachment(RootComponent);
	MeshProyectil->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshProyectil->SetRelativeScale3D(FVector(0.1f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereAsset(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereAsset.Succeeded())
	{
		MeshProyectil->SetStaticMesh(SphereAsset.Object);
	}

	// Movimiento recto sin gravedad
	MovimientoProyectil = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovimientoProyectil"));
	MovimientoProyectil->UpdatedComponent = ColisionProyectil;
	MovimientoProyectil->InitialSpeed = 5000.0f;
	MovimientoProyectil->MaxSpeed = 5000.0f;
	MovimientoProyectil->bRotationFollowsVelocity = true;
	MovimientoProyectil->ProjectileGravityScale = 0.0f;
}

void AProyectil::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Tirador = GetOwner())
	{
		ColisionProyectil->IgnoreActorWhenMoving(Tirador, true);
	}

	ColisionProyectil->OnComponentHit.AddDynamic(this, &AProyectil::AlImpactar);
}

void AProyectil::AlImpactar(UPrimitiveComponent* ComponenteGolpeado, AActor* OtroActor,
	UPrimitiveComponent* OtroComponente, FVector ImpulsoNormal, const FHitResult& Impacto)
{
	if (OtroActor && OtroActor != this && OtroActor != GetOwner())
	{
		UGameplayStatics::ApplyPointDamage(OtroActor, Dano, GetVelocity().GetSafeNormal(), Impacto,
			GetInstigatorController(), this, nullptr);

		if (OtroComponente && OtroComponente->IsSimulatingPhysics())
		{
			OtroComponente->AddImpulseAtLocation(GetVelocity() * 0.5f, Impacto.ImpactPoint);
		}
	}

	Destroy();
}
