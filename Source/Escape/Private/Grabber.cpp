// Copyright Clayton Stamper 2020


#include "Grabber.h"

#include "DrawDebugHelpers.h"

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	SetupPhysicsHandle();
	SetupInput();

}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}

void UGrabber::SetupPhysicsHandle()
{
	//check for physics handle
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (!PhysicsHandle)
		UE_LOG(LogTemp, Error, TEXT("Physics handle component is missing on: %s"), *GetOwner()->GetName());
}

void UGrabber::SetupInput()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();

	if (!InputComponent)
		UE_LOG(LogTemp, Error, TEXT("Input component is missing on: %s"), *GetOwner()->GetName());

	InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::TryGrab);
	InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
}

void UGrabber::TryGrab()
{

	// try to reach actors with physics body and correct collision channel
	FVector GrabPoint;
	const FHitResult Hit = GetFirstGrabbableInReach(GrabPoint);
	AActor* GrabbedObject = Hit.GetActor();

	if (GrabbedObject)
	{
		StartGrab(GrabbedObject, &GrabPoint);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Grabber pressed, but there was nothing to grab!"));
	}


	// if we reach something, attach physics handles
	
}

void UGrabber::StartGrab(AActor* GrabbedObject, const FVector* const GrabLocation)
{
	UE_LOG(LogTemp, Warning, TEXT("Object: %s was grabbed at grab point: %s!"), *GrabbedObject->GetName(), *GrabLocation->ToString());
	// PhysicsHandle->GrabComponentAtLocation
	// 	(
	// 		
	// 	)
}

void UGrabber::Release()
{
	UE_LOG(LogTemp, Warning, TEXT("Grabber released!"));
}

FHitResult UGrabber::GetFirstGrabbableInReach(FVector& GrabPoint)
{

	// Get Player's view vector
	FVector ViewPos;
	FRotator ViewRot;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint
		(
        OUT ViewPos,
        OUT ViewRot
        );
	
	// UE_LOG(LogTemp, Warning, TEXT("Grabber:\n\tLocation: %s \n\tRotation: %s"),
	// 	*ViewPos.ToString(),
	// 	*ViewRot.ToString()
	// 	);

	// Draw line to show reach
	const FVector PlayerForward = ViewRot.Vector();
	GrabPoint = ViewPos + PlayerForward * Reach;

	if (ShouldDrawRay)
		DrawRay(ViewPos, GrabPoint);
	
	// Ray cast
	FHitResult Hit;
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());
	GetWorld()->LineTraceSingleByObjectType(
        OUT Hit,
        ViewPos,
        GrabPoint,
        FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
        TraceParams
        );

	AActor* ActorHit = Hit.GetActor();

	if (ActorHit && ShouldLogHit)
		UE_LOG(LogTemp, Warning, TEXT("Item: \"%s\" is in reach!"), *ActorHit->GetName());

	return Hit;
	
}

void UGrabber::DrawRay(const FVector &ViewPos, const FVector &RayEnd) const
{
	DrawDebugLine
	(
            GetWorld(),
            ViewPos,
            RayEnd,
            FColor(0, 255, 0),
            false,
            0.f,
            0,
            2.f
            );
}

