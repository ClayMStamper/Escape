// Copyright Clayton Stamper 2020


#include "OpenDoor.h"

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	
	Super::BeginPlay();
	AActor* owner = GetOwner();
	StartRot = owner->GetActorRotation();
	CurrentRot = StartRot;

	CheckSerializedRefsForNull();
	ActorToOpen = GetWorld()->GetFirstPlayerController()->GetPawn();

}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	PollForTrigger();
	RotateDoor(DeltaTime);
	
	// ...
}

void UOpenDoor::RotateDoor(float dTime)
{
	CurrentRot = FMath::Lerp(CurrentRot, StartRot + RotOffset, dTime * DoorSpeed);
	GetOwner()->SetActorRotation(CurrentRot);
}

void UOpenDoor::PollForTrigger()
{
	const bool doorShouldOpen = PressurePlate && PressurePlate->IsOverlappingActor(ActorToOpen);

	if (doorShouldOpen)
	{
		SetDoorDirection(Open);
	} else
	{
		SetDoorDirection(Close);
	}

}

void UOpenDoor::SetDoorDirection(EDoorDirection direction)
{

	const float worldTime = GetWorld()->GetTimeSeconds();

	switch (direction)
	{
		case Open:
			Opened_TimeStamp = worldTime;
			DoorSpeed = OpenDoorSpeed;
			RotOffset = OpenOffset;
			break;
		case Close:
			const bool delayIsFinished = DoesClose && worldTime - Opened_TimeStamp > CloseDoorDelay;
			if (delayIsFinished)
			{
				DoorSpeed = CloseDoorSpeed;
				RotOffset = ClosedOffset; 
			}
			break;
	}
}

void UOpenDoor::CheckSerializedRefsForNull()
{
	if (!PressurePlate)
	{
		UE_LOG(LogTemp, Error, TEXT("%s has the OpenDoor component, but no pressure plate ref!"),
			*GetOwner()->GetName());
	}
}




