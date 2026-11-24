#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Proyectil.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;

UCLASS()
class BULLET_API AProyectil : public AActor
{
	GENERATED_BODY()

public:
	AProyectil();

	UPROPERTY(VisibleAnywhere, Category = "Proyectil")
	TObjectPtr<USphereComponent> ColisionProyectil;

	UPROPERTY(VisibleAnywhere, Category = "Proyectil")
	TObjectPtr<UStaticMeshComponent> MeshProyectil;

	UPROPERTY(VisibleAnywhere, Category = "Proyectil")
	TObjectPtr<UProjectileMovementComponent> MovimientoProyectil;

	UPROPERTY(EditAnywhere, Category = "Proyectil")
	float Dano = 25.0f;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void AlImpactar(UPrimitiveComponent* ComponenteGolpeado, AActor* OtroActor,
		UPrimitiveComponent* OtroComponente, FVector ImpulsoNormal, const FHitResult& Impacto);
};
