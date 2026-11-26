#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Proyectil.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class UProjectileMovementComponent;

UCLASS()
class BULLET_API AProyectil : public AActor
{
	GENERATED_BODY()

public:
	AProyectil();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, Category = "Componentes")
	USphereComponent* Colision;

	UPROPERTY(VisibleAnywhere, Category = "Componentes")
	UStaticMeshComponent* CuerpoBala;

	UPROPERTY(VisibleAnywhere, Category = "Componentes")
	UStaticMeshComponent* PuntaBala;

	UPROPERTY(VisibleAnywhere, Category = "Camara")
	USpringArmComponent* BrazoCamara;

	UPROPERTY(VisibleAnywhere, Category = "Camara")
	UCameraComponent* CamaraBala;

	UPROPERTY(VisibleAnywhere, Category = "Movimiento")
	UProjectileMovementComponent* MovimientoProyectil;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ajustes")
	float VelocidadBala = 700.0f;

	UFUNCTION()
	void AlImpactar(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};