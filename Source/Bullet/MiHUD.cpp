#include "MiHUD.h"
#include "Engine/Canvas.h"
#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "Engine/Texture2D.h"

void AMiHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!bEstaApuntando || !Canvas)
	{
		return;
	}

	const float W = Canvas->ClipX;
	const float H = Canvas->ClipY;
	const FVector2D Centro(W * 0.5f, H * 0.5f);
	const float Radio = 170.0f;

	DrawRect(FLinearColor::Black, 0.0f, 0.0f, W, Centro.Y - Radio);
	DrawRect(FLinearColor::Black, 0.0f, Centro.Y + Radio, W, H - (Centro.Y + Radio));
	DrawRect(FLinearColor::Black, 0.0f, Centro.Y - Radio, Centro.X - Radio, Radio * 2.0f);
	DrawRect(FLinearColor::Black, Centro.X + Radio, Centro.Y - Radio, W - (Centro.X + Radio), Radio * 2.0f);

	constexpr int32 Segmentos = 64;
	const float RadioExterior = Radio * 1.45f;
	const float Paso = 2.0f * PI / Segmentos;

	TArray<FCanvasUVTri> Triangulos;
	Triangulos.Reserve(Segmentos * 2);

	for (int32 i = 0; i < Segmentos; ++i)
	{
		const FVector2D Dir1(FMath::Cos(i * Paso), FMath::Sin(i * Paso));
		const FVector2D Dir2(FMath::Cos((i + 1) * Paso), FMath::Sin((i + 1) * Paso));

		const FVector2D Int1 = Centro + Dir1 * Radio;
		const FVector2D Int2 = Centro + Dir2 * Radio;
		const FVector2D Ext1 = Centro + Dir1 * RadioExterior;
		const FVector2D Ext2 = Centro + Dir2 * RadioExterior;

		FCanvasUVTri T1;
		T1.V0_Pos = Int1; T1.V1_Pos = Ext1; T1.V2_Pos = Ext2;
		T1.V0_Color = T1.V1_Color = T1.V2_Color = FLinearColor::Black;
		Triangulos.Add(T1);

		FCanvasUVTri T2;
		T2.V0_Pos = Int1; T2.V1_Pos = Ext2; T2.V2_Pos = Int2;
		T2.V0_Color = T2.V1_Color = T2.V2_Color = FLinearColor::Black;
		Triangulos.Add(T2);
	}

	if (Canvas->DefaultTexture && Canvas->DefaultTexture->GetResource())
	{
		FCanvasTriangleItem Anillo(Triangulos, Canvas->DefaultTexture->GetResource());
		Canvas->DrawItem(Anillo);
	}

	for (int32 i = 0; i < Segmentos; ++i)
	{
		const FVector2D P1 = Centro + FVector2D(FMath::Cos(i * Paso), FMath::Sin(i * Paso)) * Radio;
		const FVector2D P2 = Centro + FVector2D(FMath::Cos((i + 1) * Paso), FMath::Sin((i + 1) * Paso)) * Radio;
		DrawLine(P1.X, P1.Y, P2.X, P2.Y, FLinearColor::Black, 4.0f);
	}

	DrawLine(Centro.X - 20.0f, Centro.Y, Centro.X + 20.0f, Centro.Y, FLinearColor::Red, 2.0f);
	DrawLine(Centro.X, Centro.Y - 20.0f, Centro.X, Centro.Y + 20.0f, FLinearColor::Red, 2.0f);
}
