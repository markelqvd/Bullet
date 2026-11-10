
#include "CamaraOrbital.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"

ACamaraOrbital::ACamaraOrbital()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ACamaraOrbital::BeginPlay()
{
    Super::BeginPlay();

    // Guardar el punto central fijo
    Centro = GetActorLocation();

    // Activar nuestra camara
    if (APlayerController* Controlador =
        UGameplayStatics::GetPlayerController(this, 0))
    {
        Controlador->SetViewTarget(this);
    }
}

void ACamaraOrbital::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    // Avanzar la oscilacion solo cuando el jugador se mueve
    if (ACharacter* Jugador = UGameplayStatics::GetPlayerCharacter(this, 0))
    {
        if (Jugador->GetVelocity().Size2D() > 5.0f)
        {
            Tiempo += DeltaSeconds;
        }
    }

    // Calcular la oscilacion de izquierda a derecha
    float Angulo = FMath::Sin(Tiempo * Velocidad) * Amplitud;
    Angulo = FMath::DegreesToRadians(Angulo);

    FVector Objetivo = Centro;
    float RadioActual = Radio;

    // Buscar al personaje principal
    ACharacter* Jugador =
        UGameplayStatics::GetPlayerCharacter(this, 0);

    if (Jugador)
    {
        FVector PosJugador =
            Jugador->GetActorLocation() + FVector(0, 0, 80);

        // Apuntar entre el centro y el jugador
        Objetivo = (Centro + PosJugador) * 0.5f;

        // Alejar la camara si el jugador se aleja
        float Separacion = FVector::Dist(Centro, PosJugador);
        RadioActual = FMath::Max(Radio, Separacion * 1.6f);
    }

    // Calcular la posicion orbital
    FVector NuevaPosicion = Centro + FVector(
        -FMath::Cos(Angulo) * RadioActual,
        -FMath::Sin(Angulo) * RadioActual,
        Altura
    );

    // Girar la camara hacia el objetivo
    FRotator NuevaRotacion =
        (Objetivo - NuevaPosicion).Rotation();

    // Aplicar posicion y rotacion
    SetActorLocationAndRotation(
        NuevaPosicion,
        NuevaRotacion
    );
}
