#include "WRKartData.h"
#include "WastelandRacers/Vehicles/WRKart.h"
#include "Components/SkeletalMeshComponent.h"

float UWRKartData::GetOverallRating() const
{
	// Calculate overall rating based on stats (0-100 scale)
	float SpeedRating = (KartStats.MaxSpeed / 1500.0f) * 25.0f;
	float AccelRating = (KartStats.Acceleration / 1000.0f) * 25.0f;
	float HandlingRating = KartStats.Handling * 25.0f;
	float WeightRating = (2000.0f - KartStats.Weight) / 2000.0f * 25.0f;

	return FMath::Clamp(SpeedRating + AccelRating + HandlingRating + WeightRating, 0.0f, 100.0f);
}

void UWRKartData::ApplyToKart(AWRKart* Kart) const
{
	if (!Kart)
		return;

	// Apply kart mesh
	if (KartMesh && Kart->GetMesh())
	{
		Kart->GetMesh()->SetSkeletalMesh(KartMesh);
	}

	// Apply kart stats
	// This would need to be implemented in AWRKart
	// Kart->SetKartStats(KartStats);

	// Apply engine sound
	if (EngineSound)
	{
		// Set engine sound on kart
	}

	UE_LOG(LogTemp, Warning, TEXT("Applied kart data: %s"), *KartName);
}
