#pragma once

#include "CoreMinimal.h"
#include "FGBuildable.h"
#include "Components/SplineMeshComponent.h"
#include "FINNetworkCable.generated.h"

class UFINNetworkConnector;

UCLASS()
class FICSITNETWORKS_API AFINNetworkCable : public AFGBuildable {
	GENERATED_BODY()

public:
	UPROPERTY(SaveGame)
	UFINNetworkConnector* Connector1 = nullptr;

	UPROPERTY(SaveGame)
	UFINNetworkConnector* Connector2 = nullptr;

	UPROPERTY(EditDefaultsOnly)
	USplineMeshComponent* CableSpline = nullptr;

	AFINNetworkCable();
	~AFINNetworkCable();

	// Begin AActor
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type reason) override;
	// End AActor

	// Begin IFGSaveInterface
	virtual bool ShouldSave_Implementation() const override;
	// End IFGSaveInterface

	// Begin IFGDismantleInterface
	virtual int32 GetDismantleRefundReturnsMultiplier() const;
	// End IFGDismantleInterface
};