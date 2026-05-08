// Copyright (c) 2026 The Last Cartographer.

#include "World/ProceduralIsland.h"

#include "ProceduralMeshComponent.h"

AProceduralIsland::AProceduralIsland()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	Mesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	Mesh->bUseAsyncCooking = true;
}

static FORCEINLINE float Hash(int32 X, int32 Y, int32 Seed)
{
	uint32 H = (uint32)(X * 73856093) ^ (uint32)(Y * 19349663) ^ (uint32)Seed;
	H = (H ^ (H >> 13)) * 1274126177u;
	return (float)((H >> 16) & 0xFFFF) / 65535.f;
}

static FORCEINLINE float SmoothStep(float A, float B, float T)
{
	T = FMath::Clamp(T, 0.f, 1.f);
	T = T * T * (3.f - 2.f * T);
	return FMath::Lerp(A, B, T);
}

static float Noise2D(float X, float Y, int32 Seed)
{
	const int32 Xi = FMath::FloorToInt(X);
	const int32 Yi = FMath::FloorToInt(Y);
	const float Xf = X - Xi;
	const float Yf = Y - Yi;
	const float A = Hash(Xi,     Yi,     Seed);
	const float B = Hash(Xi + 1, Yi,     Seed);
	const float C = Hash(Xi,     Yi + 1, Seed);
	const float D = Hash(Xi + 1, Yi + 1, Seed);
	const float AB = SmoothStep(A, B, Xf);
	const float CD = SmoothStep(C, D, Xf);
	return SmoothStep(AB, CD, Yf);
}

static float Fbm(float X, float Y, int32 Seed, int32 Octaves)
{
	float Sum = 0.f;
	float Amp = 1.f;
	float Freq = 1.f;
	float Norm = 0.f;
	for (int32 i = 0; i < Octaves; ++i)
	{
		Sum += Noise2D(X * Freq, Y * Freq, Seed + i * 17) * Amp;
		Norm += Amp;
		Amp *= 0.5f;
		Freq *= 2.f;
	}
	return Sum / FMath::Max(Norm, 0.0001f);
}

float AProceduralIsland::HeightFunc(float Nx, float Ny) const
{
	// Nx, Ny in [-1, 1] — normalised position relative to island centre.
	const float WorldX = Nx * Size.X * 0.5f * NoiseScale * 1000.f;
	const float WorldY = Ny * Size.Y * 0.5f * NoiseScale * 1000.f;

	const float Base   = Fbm(WorldX, WorldY, Seed, 4);
	const float Ridge  = 1.f - FMath::Abs(Fbm(WorldX * 1.7f, WorldY * 1.7f, Seed + 7, 3) * 2.f - 1.f);
	const float Mtn    = Ridge * Ridge;

	const float R   = FMath::Sqrt(Nx * Nx + Ny * Ny);
	const float Coast = FMath::Pow(FMath::Clamp(1.f - R, 0.f, 1.f), CoastFalloffPower);

	const float H = Base * (1.f - RidgeStrength) + Mtn * RidgeStrength;
	const float Land = (H * Coast - 0.25f) * MaxAltitude;
	if (Land < SeaLevel - 50.f) { return SeaLevel - 50.f; }
	return Land;
}

float AProceduralIsland::SampleHeightAt(FVector2D WorldXY) const
{
	const FVector Center = GetActorLocation();
	const FVector2D Local = WorldXY - FVector2D(Center.X, Center.Y);
	const float Nx = FMath::Clamp(2.f * Local.X / Size.X, -1.f, 1.f);
	const float Ny = FMath::Clamp(2.f * Local.Y / Size.Y, -1.f, 1.f);
	return Center.Z + HeightFunc(Nx, Ny);
}

void AProceduralIsland::Rebuild()
{
	if (!Mesh) { return; }
	Mesh->ClearAllMeshSections();

	const int32 N = FMath::Max(2, Resolution);
	TArray<FVector> Verts;        Verts.Reserve(N * N);
	TArray<int32>   Tris;         Tris.Reserve((N - 1) * (N - 1) * 6);
	TArray<FVector> Normals;      Normals.SetNumZeroed(N * N);
	TArray<FVector2D> UVs;        UVs.Reserve(N * N);
	TArray<FProcMeshTangent> Tans; Tans.SetNum(N * N);
	TArray<FLinearColor> Colors;  Colors.SetNum(N * N);

	const float StepX = Size.X / (float)(N - 1);
	const float StepY = Size.Y / (float)(N - 1);

	for (int32 y = 0; y < N; ++y)
	{
		for (int32 x = 0; x < N; ++x)
		{
			const float Nx = (float)x / (float)(N - 1) * 2.f - 1.f;
			const float Ny = (float)y / (float)(N - 1) * 2.f - 1.f;
			const float H  = HeightFunc(Nx, Ny);
			const float Px = -Size.X * 0.5f + x * StepX;
			const float Py = -Size.Y * 0.5f + y * StepY;
			Verts.Add(FVector(Px, Py, H));
			UVs.Add(FVector2D((float)x / (float)(N - 1), (float)y / (float)(N - 1)));
			const float HeightTint = FMath::Clamp(H / FMath::Max(MaxAltitude, 1.f), 0.f, 1.f);
			Colors[y * N + x] = FLinearColor(HeightTint, 1.f - HeightTint, FMath::Clamp(0.5f - HeightTint, 0.f, 1.f), 1.f);
		}
	}

	for (int32 y = 0; y < N - 1; ++y)
	{
		for (int32 x = 0; x < N - 1; ++x)
		{
			const int32 I = y * N + x;
			Tris.Add(I);     Tris.Add(I + N); Tris.Add(I + 1);
			Tris.Add(I + 1); Tris.Add(I + N); Tris.Add(I + N + 1);
		}
	}

	// Smooth normals from triangle face normals.
	for (int32 i = 0; i + 2 < Tris.Num(); i += 3)
	{
		const FVector& A = Verts[Tris[i]];
		const FVector& B = Verts[Tris[i + 1]];
		const FVector& C = Verts[Tris[i + 2]];
		const FVector FN = FVector::CrossProduct(B - A, C - A).GetSafeNormal();
		Normals[Tris[i]]     += FN;
		Normals[Tris[i + 1]] += FN;
		Normals[Tris[i + 2]] += FN;
	}
	for (FVector& N1 : Normals) { N1 = N1.GetSafeNormal(); }
	for (int32 i = 0; i < Tans.Num(); ++i) { Tans[i] = FProcMeshTangent(1, 0, 0); }

	Mesh->CreateMeshSection_LinearColor(0, Verts, Tris, Normals, UVs, Colors, Tans, /*bCreateCollision*/ true);
	if (LandMaterial) { Mesh->SetMaterial(0, LandMaterial); }
}

void AProceduralIsland::OnConstruction(const FTransform& T)
{
	Super::OnConstruction(T);
	Rebuild();
}

void AProceduralIsland::BeginPlay()
{
	Super::BeginPlay();
	if (Mesh && Mesh->GetNumSections() == 0) { Rebuild(); }
}
