// Copyright Clayton Stamper 2020

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/TriggerVolume.h"

#include "OpenDoor.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ESCAPE_API UOpenDoor : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UOpenDoor();
	enum EDoorDirection {Open, Close};

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	FString EnumToString(EDoorDirection Direction);

// private fields
private:

	UPROPERTY(EditAnywhere)
	bool DoesClose = true;
	UPROPERTY(EditAnywhere)
	float WeightRequirement = 50.f;
	UPROPERTY(EditAnywhere)
	float CloseDoorDelay = 0.5f;
	UPROPERTY(EditAnywhere)
	float CloseDoorSpeed = 3.f;
	UPROPERTY(EditAnywhere)
	float OpenDoorSpeed = 1.f;
	UPROPERTY(EditAnywhere)
	FRotator ClosedOffset = {0, 0, 0};
	UPROPERTY(EditAnywhere)
	FRotator OpenOffset = {0, 120, 0};
	
	UPROPERTY(EditAnywhere)
	ATriggerVolume* PlateTriggerVolume;
	UPROPERTY(EditAnywhere)
	AActor* TriggerPlateActor;
	UPROPERTY()
	UAudioComponent* AudioComponent = nullptr;
	
	FRotator StartRot, CurrentRot, RotOffset;
	EDoorDirection CurrentDirection = Close;
	float Opened_TimeStamp = 0.f;
	float DoorSpeed = 1.f;
	float ClickRotationThreshold = 10.f; //degrees 

// private methods
private:
	void FindAudioComponent();
	void RotateDoor(float dTime);
	void PollForTrigger(const float& DeltaTime);
	void SetDoorDirection(EDoorDirection NewDirection);
	void CheckSerializedRefsForNull();
	float CalcTotalMassReceived() const;
	void UpdatePressurePlateVisuals(const float& WeightReceived, const float& DeltaTime);
		
};
