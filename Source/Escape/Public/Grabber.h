// Copyright Clayton Stamper 2020

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"

#include "Grabber.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ESCAPE_API UGrabber : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrabber();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
private:

	void CalcGrabPoint();
	void TryGrab();
	void StartGrab(UPrimitiveComponent* GrabbedObject);
	void Release();
	void SetupPhysicsHandle();
	void SetupInput();
	void DrawRay() const;

	FHitResult GetFirstGrabbableInReach();
	UPROPERTY()
	UPhysicsHandleComponent* PhysicsHandle = nullptr;
	UPROPERTY()
	UInputComponent* InputComponent = nullptr;
	FVector GrabRayEnd, ViewPos;

	UPROPERTY(EditAnywhere)
	bool ShouldDrawRay = false;
	UPROPERTY(EditAnywhere)
	bool ShouldLogHit = false;
	UPROPERTY(EditAnywhere)
	float Reach = 200.f;
		
};
