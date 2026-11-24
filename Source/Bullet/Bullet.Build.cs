// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class Bullet : ModuleRules
{
	public Bullet(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",     // Acciones de input (IA_Apuntar, IA_Disparar)
			"AIModule",          // AAIController para EnemigoPatrulla
			"NavigationSystem",  // MoveToActor / PathFollowing
			"GameplayTasks"      // Requerido por AIModule
		});

		// Menu principal hecho con Slate
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
	}
}
