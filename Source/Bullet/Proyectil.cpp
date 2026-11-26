#include "Proyectil.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "UObject/ConstructorHelpers.h"

AProyectil::AProyectil()
{
	PrimaryActorTick.bCanEverTick = false;

	Colision = CreateDefaultSubobject<USphereComponent>(TEXT("Colision"));
	Colision->InitSphereRadius(8.0f);
	Colision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	Colision->OnComponentHit.AddDynamic(this, &AProyectil::AlImpactar);
	RootComponent = Colision;

	CuerpoBala = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CuerpoBala"));
	CuerpoBala->SetupAttachment(RootComponent);
	CuerpoBala->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CuerpoBala->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	CuerpoBala->SetRelativeScale3D(FVector(0.10f, 0.10f, 0.42f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CilindroAsset(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (CilindroAsset.Succeeded())
	{
		CuerpoBala->SetStaticMesh(CilindroAsset.Object);
	}

	PuntaBala = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PuntaBala"));
	PuntaBala->SetupAttachment(RootComponent);
	PuntaBala->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PuntaBala->SetRelativeLocation(FVector(33.0f, 0.0f, 0.0f));
	PuntaBala->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	PuntaBala->SetRelativeScale3D(FVector(0.10f, 0.10f, 0.24f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> ConoAsset(TEXT("/Engine/BasicShapes/Cone.Cone"));
	if (ConoAsset.Succeeded())
	{
		PuntaBala->SetStaticMesh(ConoAsset.Object);
	}

	BrazoCamara = CreateDefaultSubobject<USpringArmComponent>(TEXT("BrazoCamara"));
	BrazoCamara->SetupAttachment(RootComponent);
	BrazoCamara->TargetArmLength = 130.0f;
	BrazoCamara->SocketOffset = FVector(0.0f, 0.0f, 22.0f);
	BrazoCamara->bDoCollisionTest = false;

	CamaraBala = CreateDefaultSubobject<UCameraComponent>(TEXT("CamaraBala"));
	CamaraBala->SetupAttachment(BrazoCamara);

	MovimientoProyectil = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovimientoProyectil"));
	MovimientoProyectil->UpdatedComponent = Colision;
	MovimientoProyectil->InitialSpeed = VelocidadBala;
	MovimientoProyectil->MaxSpeed = VelocidadBala;
	MovimientoProyectil->bRotationFollowsVelocity = true;
	MovimientoProyectil->ProjectileGravityScale = 0.0f;
}

void AProyectil::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner())
	{
		Colision->IgnoreActorWhenMoving(GetOwner(), true);
	}

	MovimientoProyectil->InitialSpeed = VelocidadBala;
	MovimientoProyectil->MaxSpeed = VelocidadBala;
	MovimientoProyectil->Velocity = GetActorForwardVector() * VelocidadBala;
}

void AProyectil::AlImpactar(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this && OtherActor != GetOwner())
	{
		MovimientoProyectil->StopMovementImmediately();
	}
}