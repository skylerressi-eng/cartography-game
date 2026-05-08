// Copyright (c) 2026 The Last Cartographer.

using UnrealBuildTool;
using System.Collections.Generic;

public class CartographyGameEditorTarget : TargetRules
{
	public CartographyGameEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
		ExtraModuleNames.Add("CartographyGame");
	}
}
