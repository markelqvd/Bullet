#include "Proyectil.h"
#include "UObject/ConstructorHelpers.h"

AProyectil::AProyectil()
{
	PrimaryActorTick.bCanEverTick = false;

	// Crear la malla de la bala (Esfera placeholder)
	MeshProyectil = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshProyectil"));
	RootComponent = MeshProyectil;
	MeshProyectil->SetWorldScale3D(FVector(0.2f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereAsset(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereAsset.Succeeded())
	{
		MeshProyectil->SetStaticMesh(SphereAsset.Object);
	}

	// Movimiento recto sin gravedad
	MovimientoProyectil = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovimientoProyectil"));
	MovimientoProyectil->InitialSpeed = 5000.0f;
	MovimientoProyectil->MaxSpeed = 5000.0f;
	MovimientoProyectil->ProjectileGravityScale = 0.0f;
}