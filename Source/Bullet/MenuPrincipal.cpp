
#include "MenuPrincipal.h"

#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Misc/App.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/PackageName.h"

#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Text/STextBlock.h"
#include "Styling/CoreStyle.h"
#include "Input/Reply.h"

// ======================================
// CONSTRUCTOR
// ======================================

AMenuPrincipal::AMenuPrincipal()
{
    PrimaryActorTick.bCanEverTick = false;
}

// ======================================
// INICIAR MENU
// ======================================

void AMenuPrincipal::BeginPlay()
{
    Super::BeginPlay();

    if (!GEngine || !GEngine->GameViewport)
    {
        return;
    }

    // Recuperar volumen guardado
    GConfig->GetFloat(
        TEXT("Bullet.Ajustes"),
        TEXT("Volumen"),
        Volumen,
        GGameUserSettingsIni
    );

    Volumen = FMath::Clamp(Volumen, 0.0f, 1.0f);
    FApp::SetVolumeMultiplier(Volumen);

    // Recuperar ajustes de pantalla
    if (UGameUserSettings* Config =
        GEngine->GetGameUserSettings())
    {
        bPantallaCompleta =
            Config->GetFullscreenMode() != EWindowMode::Windowed;

        FIntPoint Actual = Config->GetScreenResolution();

        int32 Encontrado = Resoluciones.IndexOfByKey(Actual);

        if (Encontrado == INDEX_NONE &&
            Actual.X > 0 && Actual.Y > 0)
        {
            Encontrado = Resoluciones.Add(Actual);
        }

        if (Encontrado != INDEX_NONE)
        {
            IndiceResolucion = Encontrado;
        }
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

                +SWidgetSwitcher::Slot()
                [
                    SNew(SVerticalBox)

                        // TITULO BULLET
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
                                                // Abrir selector de niveles
                                                if (Pantallas.IsValid())
                                                {
                                                    Pantallas->SetActiveWidgetIndex(2);
                                                }

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
            

            +SWidgetSwitcher::Slot()
                [
                    SNew(SVerticalBox)

                        // TITULO
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

                    // VOLUMEN
                    + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(0, 0, 0, 10)
                        [
                            SNew(STextBlock)
                                .Text_Lambda([this]()
                                    {
                                        int32 Porcentaje =
                                            FMath::RoundToInt(Volumen * 100.0f);

                                        return FText::FromString(
                                            FString::Printf(
                                                TEXT("VOLUMEN GENERAL: %d%%"),
                                                Porcentaje
                                            )
                                        );
                                    })
                        ]

                    // BARRA DE VOLUMEN
                    + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(0, 0, 0, 25)
                        [
                            SNew(SBox)
                                .WidthOverride(300)
                                [
                                    SNew(SSlider)
                                        .Value_Lambda([this]()
                                            {
                                                return Volumen;
                                            })
                                        .OnValueChanged_Lambda([this](float Nuevo)
                                            {
                                                Volumen = FMath::Clamp(
                                                    Nuevo, 0.0f, 1.0f
                                                );

                                                FApp::SetVolumeMultiplier(Volumen);

                                                GConfig->SetFloat(
                                                    TEXT("Bullet.Ajustes"),
                                                    TEXT("Volumen"),
                                                    Volumen,
                                                    GGameUserSettingsIni
                                                );

                                                GConfig->Flush(
                                                    false,
                                                    GGameUserSettingsIni
                                                );
                                            })
                                ]
                        ]

                    // RESOLUCION
                    + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(0, 0, 0, 15)
                        [
                            SNew(SBox)
                                .WidthOverride(300)
                                .HeightOverride(50)
                                [
                                    SNew(SButton)
                                        .HAlign(HAlign_Center)
                                        .VAlign(VAlign_Center)
                                        .OnClicked_Lambda([this]()
                                            {
                                                IndiceResolucion =
                                                    (IndiceResolucion + 1) %
                                                    Resoluciones.Num();

                                                return FReply::Handled();
                                            })
                                        [
                                            SNew(STextBlock)
                                                .Text_Lambda([this]()
                                                    {
                                                        FIntPoint R =
                                                            Resoluciones[IndiceResolucion];

                                                        return FText::FromString(
                                                            FString::Printf(
                                                                TEXT("RESOLUCION: %d x %d"),
                                                                R.X, R.Y
                                                            )
                                                        );
                                                    })
                                        ]
                                ]
                        ]

                    // PANTALLA COMPLETA
                    + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(0, 0, 0, 25)
                        [
                            SNew(SBox)
                                .WidthOverride(300)
                                .HeightOverride(50)
                                [
                                    SNew(SButton)
                                        .HAlign(HAlign_Center)
                                        .VAlign(VAlign_Center)
                                        .OnClicked_Lambda([this]()
                                            {
                                                bPantallaCompleta = !bPantallaCompleta;
                                                return FReply::Handled();
                                            })
                                        [
                                            SNew(STextBlock)
                                                .Text_Lambda([this]()
                                                    {
                                                        return FText::FromString(
                                                            bPantallaCompleta
                                                            ? TEXT("PANTALLA COMPLETA: SI")
                                                            : TEXT("PANTALLA COMPLETA: NO")
                                                        );
                                                    })
                                        ]
                                ]
                        ]

                    // APLICAR
                    + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(0, 0, 0, 12)
                        [
                            SNew(SBox)
                                .WidthOverride(300)
                                .HeightOverride(50)
                                [
                                    SNew(SButton)
                                        .HAlign(HAlign_Center)
                                        .VAlign(VAlign_Center)
                                        .OnClicked_Lambda([this]()
                                            {
                                                AplicarAjustes();
                                                return FReply::Handled();
                                            })
                                        [
                                            SNew(STextBlock)
                                                .Text(FText::FromString(TEXT("APLICAR")))
                                        ]
                                ]
                        ]

                    // VOLVER AL MENU
                    + SVerticalBox::Slot()
                        .AutoHeight()
                        [
                            SNew(SBox)
                                .WidthOverride(300)
                                .HeightOverride(50)
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

            
            // PANTALLA 2: SELECTOR
            

            +SWidgetSwitcher::Slot()
                [
                    SNew(SVerticalBox)

                        // TITULO
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .HAlign(HAlign_Center)
                        .Padding(0, 0, 0, 25)
                        [
                            SNew(STextBlock)
                                .Text(FText::FromString(
                                    TEXT("SELECCIONAR NIVEL")
                                ))
                                .Font(
                                    FCoreStyle::GetDefaultFontStyle("Bold", 35)
                                )
                        ]

                    // LISTA DE NIVELES (se rellena con los mapas que existen)
                    + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(0, 0, 0, 15)
                        [
                            SAssignNew(ListaNiveles, SVerticalBox)
                        ]

                        // VOLVER AL MENU PRINCIPAL
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        [
                            SNew(SBox)
                                .WidthOverride(300)
                                .HeightOverride(50)
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

    // Crear un boton por cada LevelN que exista en Content/Scenes
    if (ListaNiveles.IsValid())
    {
        for (int32 Nivel = 1; Nivel <= MaxNiveles; ++Nivel)
        {
            const FString RutaNivel =
                FString::Printf(TEXT("%s/Level%d"), *CarpetaNiveles, Nivel);

            if (!FPackageName::DoesPackageExist(RutaNivel))
            {
                continue;
            }

            ListaNiveles->AddSlot()
                .AutoHeight()
                .Padding(0, 0, 0, 10)
                [
                    CrearBotonNivel(Nivel)
                ];
        }
    }

    // Mostrar menu
    GEngine->GameViewport->AddViewportWidgetContent(
        MenuWidget.ToSharedRef(),
        100
    );

    // Activar cursor
    if (APlayerController* PC =
        GetWorld()->GetFirstPlayerController())
    {
        PC->SetInputMode(FInputModeUIOnly());
        PC->bShowMouseCursor = true;
    }
}


// CREAR BOTONES DE LOS NIVELES


TSharedRef<SWidget> AMenuPrincipal::CrearBotonNivel(int32 Nivel)
{
    return SNew(SBox)
        .WidthOverride(300)
        .HeightOverride(50)
        [
            SNew(SButton)
                .HAlign(HAlign_Center)
                .VAlign(VAlign_Center)
                .OnClicked_Lambda([this, Nivel]()
                    {
                        // Devolver control al juego
                        if (APlayerController* PC =
                            GetWorld()->GetFirstPlayerController())
                        {
                            PC->SetInputMode(FInputModeGameOnly());
                            PC->bShowMouseCursor = false;
                        }

                        // Cargar Level1, Level2, etc.
                        FString NombreNivel = FString::Printf(
                            TEXT("%s/Level%d"), *CarpetaNiveles, Nivel);

                        UGameplayStatics::OpenLevel(
                            this,
                            FName(*NombreNivel)
                        );

                        return FReply::Handled();
                    })
                [
                    SNew(STextBlock)
                        .Text(FText::FromString(
                            FString::Printf(
                                TEXT("NIVEL %d"),
                                Nivel
                            )
                        ))
                ]
        ];
}


// APLICAR AJUSTES


void AMenuPrincipal::AplicarAjustes()
{
    if (!GEngine)
    {
        return;
    }

    UGameUserSettings* Config =
        GEngine->GetGameUserSettings();

    if (!Config)
    {
        return;
    }

    Config->SetScreenResolution(
        Resoluciones[IndiceResolucion]
    );

    Config->SetFullscreenMode(
        bPantallaCompleta
        ? EWindowMode::Fullscreen
        : EWindowMode::Windowed
    );

    Config->ApplySettings(false);
    Config->ConfirmVideoMode();
    Config->SaveSettings();
}


// CERRAR MENU


void AMenuPrincipal::EndPlay(
    const EEndPlayReason::Type EndPlayReason)
{
    if (GEngine &&
        GEngine->GameViewport &&
        MenuWidget.IsValid())
    {
        GEngine->GameViewport->RemoveViewportWidgetContent(
            MenuWidget.ToSharedRef()
        );
    }

    MenuWidget.Reset();
    Pantallas.Reset();
    ListaNiveles.Reset();

    Super::EndPlay(EndPlayReason);
}
