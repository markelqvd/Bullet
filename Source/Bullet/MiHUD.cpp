#include "MiHUD.h"
#include "Engine/Canvas.h"

void AMiHUD::DrawHUD()
{
	Super::DrawHUD();

	if (bEstaApuntando && Canvas)
	{
		float W = Canvas->ClipX;
		float H = Canvas->ClipY;
		FVector2D Centro(W * 0.5f, H * 0.5f);
		float Radio = 170.0f;

		DrawRect(FLinearColor::Black, 0.0f, 0.0f, W, Centro.Y - Radio);
		DrawRect(FLinearColor::Black, 0.0f, Centro.Y + Radio, W, H - (Centro.Y + Radio));
		DrawRect(FLinearColor::Black, 0.0f, Centro.Y - Radio, Centro.X - Radio, Radio * 2.0f);
		DrawRect(FLinearColor::Black, Centro.X + Radio, Centro.Y - Radio, W - (Centro.X + Radio), Radio * 2.0f);

		int32 Segmentos = 360;
		float PasoAngulo = 6.2831853f / Segmentos;
		float RadioEsquina = Radio * 1.45f;

		for (int32 i = 0; i < Segmentos; ++i)
		{
			float Angulo = i * PasoAngulo;
			float CosA = FMath::Cos(Angulo);
			float SinA = FMath::Sin(Angulo);

			FVector2D Inicio(Centro.X + CosA * Radio, Centro.Y + SinA * Radio);
			FVector2D Fin(Centro.X + CosA * RadioEsquina, Centro.Y + SinA * RadioEsquina);

			DrawLine(Inicio.X, Inicio.Y, Fin.X, Fin.Y, FLinearColor::Black, 10.0f);
		}

		for (int32 i = 0; i < Segmentos; ++i)
		{
			float Angulo1 = i * PasoAngulo;
			float Angulo2 = (i + 1) * PasoAngulo;

			FVector2D P1(Centro.X + FMath::Cos(Angulo1) * Radio, Centro.Y + FMath::Sin(Angulo1) * Radio);
			FVector2D P2(Centro.X + FMath::Cos(Angulo2) * Radio, Centro.Y + FMath::Sin(Angulo2) * Radio);

			DrawLine(P1.X, P1.Y, P2.X, P2.Y, FLinearColor::Black, 4.0f);
		}

		DrawLine(Centro.X - 20.0f, Centro.Y, Centro.X + 20.0f, Centro.Y, FLinearColor::Red, 2.0f);
		DrawLine(Centro.X, Centro.Y - 20.0f, Centro.X, Centro.Y + 20.0f, FLinearColor::Red, 2.0f);
	}
}