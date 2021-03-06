// Copyright © 2021 Aneudy Mota

#include "GrabbableObject.h"
#include <Actor.h>
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "ProfilingDebugging/CookStats.h"

#define OUT

// Sets default values for this component's properties
UGrabbableObject::UGrabbableObject()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UGrabbableObject::BeginPlay()
{
	Super::BeginPlay();

	FindPhysicsHandle();
	SetupInputComponent();
}

void UGrabbableObject::SetupInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent)
	{
		InputComponent->BindAction( "Grab", IE_Pressed, this, &UGrabbableObject::Grab);
		InputComponent->BindAction( "Grab", IE_Released, this, &UGrabbableObject::Release);
	}
}

// Check for the physics handle component
void UGrabbableObject::FindPhysicsHandle()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle == nullptr)
	{
		UE_LOG( LogTemp, Error, TEXT( "No physics handle component found on %s." ), *GetOwner()->GetName() );
	}
}

void UGrabbableObject::Grab()
{

	const FHitResult HitResult = GetFirstPhysicsBodyInReach();
	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();

	// if we hit something then attach the physics handle.
	if (HitResult.GetActor())
	{
		// attach physics handle.
		PhysicsHandle->GrabComponentAtLocation(
			ComponentToGrab,
			NAME_None,
			GetPlayerReach()
		);
	}
}



void UGrabbableObject::Release()
{
	PhysicsHandle->ReleaseComponent();
}

// Called every frame
void UGrabbableObject::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// If the physics handle is attached...
	if (PhysicsHandle->GrabbedComponent)
	{
		// move the object that we are holding.
		PhysicsHandle->SetTargetLocation(GetPlayerReach());
	}
}

FHitResult UGrabbableObject::GetFirstPhysicsBodyInReach() const
{
	// visual line for debugging
	//DrawDebugLine
	//(
	//	GetWorld(),
	//	PlayerViewPointLocation,
	//	LineTraceEnd,
	//	FColor( 0, 255, 0 ),
	//	false,
	//	0.f,
	//	0,
	//	5.f
	//);

	FHitResult Hit;
	// Ray-cast out to a certain distance (Reach)
	FCollisionQueryParams TraceParams( FName( TEXT( "" ) ), false, GetOwner() );

	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		GetPlayerWorldPos(),
		GetPlayerReach(),
		FCollisionObjectQueryParams( ECollisionChannel::ECC_PhysicsBody ),
		TraceParams
	);

	return Hit;
}

FVector UGrabbableObject::GetPlayerWorldPos() const
{
	// Get the player's viewpoint
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);

	return PlayerViewPointLocation;
}

FVector UGrabbableObject::GetPlayerReach() const
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);

	return PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;
}
