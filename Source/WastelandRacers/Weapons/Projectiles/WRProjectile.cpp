#include "WRProjectile.h"
#include "WastelandRacers/Vehicles/WRKart.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

AWRProjectile::AWRProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create collision component
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->SetSphereRadius(5.0f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	RootComponent = CollisionComponent;

	// Create mesh component
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Create projectile movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = Speed;
	ProjectileMovement->MaxSpeed = Speed;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.0f;

	// Create trail effect
	TrailEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailEffect"));
	TrailEffect->SetupAttachment(RootComponent);

	// Bind hit event
	CollisionComponent->OnComponentHit.AddDynamic(this, &AWRProjectile::OnHit);

	// Set lifespan
	InitialLifeSpan = LifeTime;
}

void AWRProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	if (ProjectileMovement)
	{
		ProjectileMovement->InitialSpeed = Speed;
		ProjectileMovement->MaxSpeed = Speed;
	}
}

void AWRProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWRProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (bHasExploded)
		return;

	// Don't hit the owner
	if (OtherActor == GetOwner())
		return;

	OnImpact(Hit);
}

void AWRProjectile::OnImpact(const FHitResult& HitResult)
{
	if (bHasExploded)
		return;

	bHasExploded = true;

	// Apply damage to hit kart
	if (AWRKart* HitKart = Cast<AWRKart>(HitResult.GetActor()))
	{
		// Apply damage/effects to the kart
		UE_LOG(LogTemp, Warning, TEXT("Projectile hit kart: %s"), *HitKart->GetName());
	}

	Explode();
}

void AWRProjectile::Explode()
{
	// Spawn impact effect
	if (ImpactEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(), ImpactEffect, GetActorLocation(), GetActorRotation());
	}

	// Play impact sound
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, GetActorLocation());
	}

	// Destroy projectile
	Destroy();
}
