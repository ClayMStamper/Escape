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

	CalcGrabPoint();
	
	if (PhysicsHandle->GrabbedComponent)
	{
		PhysicsHandle->SetTargetLocation(GrabRayEnd);
		UE_LOG(LogTemp, Warning, TEXT("Setting grabbed component's position to = %s"), *GrabRayEnd.ToString());
	}
	
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

void UGrabber::CalcGrabPoint()
{
	// Get Player's view vector
	FRotator ViewRot;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint
        (
        OUT ViewPos,
        OUT ViewRot
        );
	
	const FVector PlayerForward = ViewRot.Vector();
	GrabRayEnd = ViewPos + PlayerForward * Reach;
}

void UGrabber::TryGrab()
{

	// try to reach actors with physics body and correct collision channel
	const FHitResult Hit = GetFirstGrabbableInReach();
	UPrimitiveComponent* GrabbedObject = Hit.GetComponent();

	// if we reach something, attach physics handles
	if (GrabbedObject)
	{
		StartGrab(GrabbedObject);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Grabber pressed, but there was nothing to grab!"));
	}
	
}

void UGrabber::StartGrab(UPrimitiveComponent* GrabbedObject)
{
	UE_LOG(LogTemp, Warning, TEXT("Object: %s was grabbed at grab point: %s!"),
		*GrabbedObject->GetName(), *GrabRayEnd.ToString());

	PhysicsHandle->GrabComponentAtLocation
		(
			GrabbedObject,
			NAME_None,
			GrabRayEnd
		);
}

void UGrabber::Release()
{
	UE_LOG(LogTemp, Warning, TEXT("Grabber released!"));
	PhysicsHandle->ReleaseComponent();
}

FHitResult UGrabber::GetFirstGrabbableInReach()
{

	if (ShouldDrawRay)
		DrawRay();
	
	// Ray cast
	FHitResult Hit;
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());
	GetWorld()->LineTraceSingleByObjectType(
        OUT Hit,
        ViewPos,
        GrabRayEnd,
        FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
        TraceParams
        );

	AActor* ActorHit = Hit.GetActor();

	if (ActorHit && ShouldLogHit)
		UE_LOG(LogTemp, Warning, TEXT("Item: \"%s\" is in reach!"), *ActorHit->GetName());

	return Hit;
	
}

void UGrabber::DrawRay() const
{
	DrawDebugLine
	(
            GetWorld(),
            ViewPos,
            GrabRayEnd,
            FColor(0, 255, 0),
            false,
            0.f,
            0,
            2.f
            );
}

