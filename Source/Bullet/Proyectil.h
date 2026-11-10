#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Proyectil.generated.h"

UCLASS()
class BULLET_API AProyectil : public AActor
{
	GENERATED_BODY()

public:
	AProyectil();

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshProyectil;

	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* MovimientoProyectil;
};