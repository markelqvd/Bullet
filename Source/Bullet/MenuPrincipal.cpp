
#include "MenuPrincipal.h"

#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Styling/CoreStyle.h"
#include "Input/Reply.h"

AMenuPrincipal::AMenuPrincipal()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AMenuPrincipal::BeginPlay()
{
    Super::BeginPlay();

    if (!GEngine || !GEngine->GameViewport)
    {
        return;
    }

    MenuWidget =
        SNew(SBorder)
        .BorderBackgroundColor(
            FLinearColor(0.03f, 0.05f, 0.08f, 1.0f)
        )
        .HAlign(HAlign_Center)
        .VAlign(VAlign_Center)
        [
            SAssignNew(Pantallas, SWidgetSwitcher)

                // PANTALLA 0: MENU PRINCIPAL
                + SWidgetSwitcher::Slot()
                [
                    SNew(SVerticalBox)

                        // TITULO
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .HAlign(HAlign_Center)
                        .Padding(0, 0, 0, 35)
                        [
                            SNew(STextBlock)
                                .Text(FText::FromString(TEXT("BULLET")))
                                .Font(
                                    FCoreStyle::GetDefaultFontStyle("Bold", 50)
                                )
                        ]

                    // JUGAR
                    + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(0, 0, 0, 12)
                        [
                            SNew(SBox)
                                .WidthOverride(260)
                                .HeightOverride(55)
                                [
                                    SNew(SButton)
                                        .HAlign(HAlign_Center)
                                        .VAlign(VAlign_Center)
                                        .OnClicked_Lambda([this]()
                                            {
                                                if (APlayerController* PC =
                                                    GetWorld()->GetFirstPlayerController())
                                                {
                                                    PC->SetInputMode(FInputModeGameOnly());
                                                    PC->bShowMouseCursor = false;
                                                }

                                                UGameplayStatics::OpenLevel(
                                                    this, FName(TEXT("Level1"))
                                                );

                                                return FReply::Handled();
                                            })
                                        [
                                            SNew(STextBlock)
                                                .Text(FText::FromString(TEXT("JUGAR")))
                                        ]
                                ]
                        ]

                    // AJUSTES
                    + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(0, 0, 0, 12)
                        [
                            SNew(SBox)
                                .WidthOverride(260)
                                .HeightOverride(55)
                                [
                                    SNew(SButton)
                                        .HAlign(HAlign_Center)
                                        .VAlign(VAlign_Center)
                                        .OnClicked_Lambda([this]()
                                            {
                                                if (Pantallas.IsValid())
                                                {
                                                    Pantallas->SetActiveWidgetIndex(1);
                                                }

                                                return FReply::Handled();
                                            })
                                        [
                                            SNew(STextBlock)
                                                .Text(FText::FromString(TEXT("AJUSTES")))
                                        ]
                                ]
                        ]

                    // SALIR
                    + SVerticalBox::Slot()
                        .AutoHeight()
                        [
                            SNew(SBox)
                                .WidthOverride(260)
                                .HeightOverride(55)
                                [
                                    SNew(SButton)
                                        .HAlign(HAlign_Center)
                                        .VAlign(VAlign_Center)
                                        .OnClicked_Lambda([this]()
                                            {
                                                UKismetSystemLibrary::QuitGame(
                                                    this,
                                                    GetWorld()->GetFirstPlayerController(),
                                                    EQuitPreference::Quit,
                                                    false
                                                );

                                                return FReply::Handled();
                                            })
                                        [
                                            SNew(STextBlock)
                                                .Text(FText::FromString(TEXT("SALIR")))
                                        ]
                                ]
                        ]
                ]

            // PANTALLA 1: AJUSTES
            + SWidgetSwitcher::Slot()
                [
                    SNew(SVerticalBox)

                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .HAlign(HAlign_Center)
                        .Padding(0, 0, 0, 35)
                        [
                            SNew(STextBlock)
                                .Text(FText::FromString(TEXT("AJUSTES")))
                                .Font(
                                    FCoreStyle::GetDefaultFontStyle("Bold", 40)
                                )
                        ]

                    + SVerticalBox::Slot()
                        .AutoHeight()
                        .HAlign(HAlign_Center)
                        .Padding(0, 0, 0, 25)
                        [
                            SNew(STextBlock)
                                .Text(FText::FromString(
                                    TEXT("Opciones por configurar")
                                ))
                        ]

                        // VOLVER
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        [
                            SNew(SBox)
                                .WidthOverride(260)
                                .HeightOverride(55)
                                [
                                    SNew(SButton)
                                        .HAlign(HAlign_Center)
                                        .VAlign(VAlign_Center)
                                        .OnClicked_Lambda([this]()
                                            {
                                                if (Pantallas.IsValid())
                                                {
                                                    Pantallas->SetActiveWidgetIndex(0);
                                                }

                                                return FReply::Handled();
                                            })
                                        [
                                            SNew(STextBlock)
                                                .Text(FText::FromString(TEXT("VOLVER")))
                                        ]
                                ]
                        ]
                ]
        ];

    GEngine->GameViewport->AddViewportWidgetContent(
        MenuWidget.ToSharedRef(), 100
    );

    if (APlayerController* PC =
        GetWorld()->GetFirstPlayerController())
    {
        PC->SetInputMode(FInputModeUIOnly());
        PC->bShowMouseCursor = true;
    }
}

void AMenuPrincipal::EndPlay(
    const EEndPlayReason::Type EndPlayReason)
{
    if (GEngine && GEngine->GameViewport &&
        MenuWidget.IsValid())
    {
        GEngine->GameViewport->RemoveViewportWidgetContent(
            MenuWidget.ToSharedRef()
        );
    }

    MenuWidget.Reset();
    Pantallas.Reset();

    Super::EndPlay(EndPlayReason);
}
