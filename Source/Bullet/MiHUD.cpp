#include "MiHUD.h"
#include "Engine/Canvas.h"

void AMiHUD::DrawHUD()
{
	Super::DrawHUD();

	if (bEstaApuntando && Canvas)
	{
		FVector2D Centro(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);
		float RadioInterno = 170.0f; // Tamaño del agujero central de la mirilla
		float RadioExterno = FMath::Max(Canvas->ClipX, Canvas->ClipY); // Cubre toda la pantalla por fuera

		int32 Segmentos = 180; // Cantidad de trazos para hacer la circunferencia perfecta
		float PasoAngulo = 6.2831853f / Segmentos; // 2 * Pi

		// 1. Dibujar el marco negro exterior envolviendo la mirilla circular
		for (int32 i = 0; i < Segmentos; ++i)
		{
			float Angulo = i * PasoAngulo;
			float CosA = FMath::Cos(Angulo);
			float SinA = FMath::Sin(Angulo);

			FVector2D Inicio(Centro.X + CosA * RadioInterno, Centro.Y + SinA * RadioInterno);
			FVector2D Fin(Centro.X + CosA * RadioExterno, Centro.Y + SinA * RadioExterno);

			DrawLine(Inicio.X, Inicio.Y, Fin.X, Fin.Y, FLinearColor::Black, 14.0f);
		}

		// 2. Dibujar el perfil del círculo interior
		for (int32 i = 0; i < Segmentos; ++i)
		{
			float Angulo1 = i * PasoAngulo;
			float Angulo2 = (i + 1) * PasoAngulo;

			FVector2D P1(Centro.X + FMath::Cos(Angulo1) * RadioInterno, Centro.Y + FMath::Sin(Angulo1) * RadioInterno);
			FVector2D P2(Centro.X + FMath::Cos(Angulo2) * RadioInterno, Centro.Y + FMath::Sin(Angulo2) * RadioInterno);

			DrawLine(P1.X, P1.Y, P2.X, P2.Y, FLinearColor::Black, 4.0f);
		}

		// 3. Cruceta roja central
		DrawLine(Centro.X - 20.0f, Centro.Y, Centro.X + 20.0f, Centro.Y, FLinearColor::Red, 2.0f);
		DrawLine(Centro.X, Centro.Y - 20.0f, Centro.X, Centro.Y + 20.0f, FLinearColor::Red, 2.0f);
	}
}