#pragma once

#include "CoreMinimal.h"
#include "Buildables/FGBuildable.h"
#include "FGPowerConnectionComponent.h"
#include "FGPowerInfoComponent.h"
#include "Network/FINNetworkConnector.h"
#include "FINNetworkPowerSwitch.generated.h"

UCLASS()
class AFINNetworkPowerSwitch : public AFGBuildable {
	GENERATED_BODY()

public:
	bool bConnectedHasChanged;

	UPROPERTY(BlueprintReadOnly, SaveGame, Category="NetworkPowerSwitch")
		bool bConnected;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"), Category="NetworkPowerSwitch")
		UFGPowerConnectionComponent* PowerConnection1 = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "NetworkPowerSwitch")
		UFGPowerConnectionComponent* PowerConnection2 = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "NetworkPowerSwitch")
		UFGPowerInfoComponent* PowerInfo1 = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "NetworkPowerSwitch")
		UFGPowerInfoComponent* PowerInfo2 = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "NetworkPowerSwitch")
		UFINNetworkConnector* NetworkConnector = nullptr;

	AFINNetworkPowerSwitch();

	// Begin AActor
	virtual void BeginPlay() override;
	virtual void Tick(float dt) override;
	// End AFGBuildable

	// Begin IFGSaveInterface
	virtual bool ShouldSave_Implementation() const override;
	// End IFGSaveInterface

	/**
	 * Changes the connection state of the power switch.
	 * true if it should transfer energy, false if not.
	 * Might queue a trigger of OnConnectedChanged
	 */
	UFUNCTION(BlueprintCallable, Category="Network|Components")
		void SetConnected(bool bNewConnected);

	/**
	 * Notifies when the connection state has changed.
	 * Gets only triggerd in actor tick.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Network|Components")
		void OnConnectedChanged();
};