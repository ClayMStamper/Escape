// Copyright Clayton Stamper 2020


#include "OpenDoor.h"

#include "Components/AudioComponent.h"

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
	FindAudioComponent();

}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	PollForTrigger(DeltaTime);
	RotateDoor(DeltaTime);
	
	// ...
}


void UOpenDoor::FindAudioComponent() 
{
	AudioComponent = GetOwner()->FindComponentByClass<UAudioComponent>();
	if (!AudioComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("%s is missing audio component!"), *GetOwner()->GetName());
	} else
	{
		UE_LOG(LogTemp, Warning, TEXT("Found audio component!"));
	}
}

void UOpenDoor::RotateDoor(float dTime)
{
	CurrentRot = FMath::Lerp(CurrentRot, StartRot + RotOffset, dTime * DoorSpeed);
	GetOwner()->SetActorRotation(CurrentRot);
}

void UOpenDoor::PollForTrigger(const float& DeltaTime)
{
	// const bool doorShouldOpen = PressurePlate && PressurePlate->IsOverlappingActor(ActorToOpen);

	if (!PlateTriggerVolume)
	{
		UE_LOG(LogTemp, Error, TEXT("Pressure Plate's Trigger Volume isn't set!!!"));
		return;
	}

	const float TotalMassReceived = CalcTotalMassReceived();
	UpdatePressurePlateVisuals(TotalMassReceived, DeltaTime);
	const bool DoorShouldOpen = TotalMassReceived > WeightRequirement;
	
	if (DoorShouldOpen)
	{
		if (CurrentDirection == Close)
			SetDoorDirection(Open);
	} else
	{
		if (CurrentDirection == Open)
			SetDoorDirection(Close);
	}

}

void UOpenDoor::SetDoorDirection(EDoorDirection NewDirection)
{

	const float worldTime = GetWorld()->GetTimeSeconds();
	CurrentDirection = NewDirection;
	AudioComponent->Play();

	// UE_LOG(LogTemp, Warning, TEXT("Setting door direction to"), *EnumToString(NewDirection));
	
	switch (NewDirection)
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
	if (!PlateTriggerVolume)
	{
		UE_LOG(LogTemp, Error, TEXT("%s has the OpenDoor component, but no pressure plate ref!"),
			*GetOwner()->GetName());
	}
}

float UOpenDoor::CalcTotalMassReceived() const
{

	float TotalMass = 0.f;

	//find all overlapping actors
	TArray<AActor*> OverlappingActors;
	if (PlateTriggerVolume)
		PlateTriggerVolume->GetOverlappingActors(OUT OverlappingActors);
	else
		UE_LOG(LogTemp, Error, TEXT("Pressure Plate's Trigger Volume isn't set!!!"));

	//add up masses
	for (AActor* Actor : OverlappingActors)
	{
		TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
	}
	
	return  TotalMass;
}

void UOpenDoor::UpdatePressurePlateVisuals(const float& WeightReceived, const float& DeltaTime)
{
	UPrimitiveComponent* PlatePrim = TriggerPlateActor->FindComponentByClass<UPrimitiveComponent>();

	if (!PlatePrim)
	{
		UE_LOG(LogTemp, Error, TEXT("Couldn't find pressure plate's primitive component!"));
		return;
	}

	float WeightLerpAlpha = WeightReceived / WeightRequirement;
	WeightLerpAlpha = FMath::Clamp(WeightLerpAlpha, 0.f, 1.f);
	const FVector CurrentScale = PlatePrim->GetRelativeScale3D();
	const float NewZ = FMath::Lerp(2.f, 0.1f, WeightLerpAlpha);
	FVector TargetScale = CurrentScale;
	TargetScale.Z = NewZ;
	const FVector NewScale = FMath::Lerp(CurrentScale, TargetScale, DeltaTime);

	PlatePrim->SetRelativeScale3D(NewScale);
	
}

FString UOpenDoor::EnumToString(EDoorDirection Direction)
{
	switch (Direction)
	{
	case Open:
		return "Open";
	case Close:
		return "Close";
	}

	return "***Enum not recognized ERROR***";
	
}




