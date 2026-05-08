// Copyright (c) 2026 The Last Cartographer.

using UnrealBuildTool;
using System.Collections.Generic;

public class CartographyGameTarget : TargetRules
{
	public CartographyGameTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
		ExtraModuleNames.Add("CartographyGame");
	}
}
