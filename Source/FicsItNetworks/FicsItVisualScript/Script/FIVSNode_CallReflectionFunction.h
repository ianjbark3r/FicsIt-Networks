#pragma once

#include "CoreMinimal.h"
#include "FIVSScriptNode.h"
#include "FIVSNode_CallReflectionFunction.generated.h"


UCLASS()
class UFIVSNode_CallReflectionFunction : public UFIVSScriptNode {
	GENERATED_BODY()
private:
	UPROPERTY()
	UFIVSPin* ExecIn = nullptr;
	UPROPERTY()
	UFIVSPin* ExecOut = nullptr;
	UPROPERTY()
	UFIVSPin* Self = nullptr;
	UPROPERTY()
	TArray<UFIVSPin*> InputPins;
	UPROPERTY()
	TArray<UFIVSPin*> OutputPins;

	UPROPERTY()
	UFINFunction* Function = nullptr;
	
public:
	// Begin UFIVSNode
	virtual TArray<FFIVSNodeAction> GetNodeActions() const override;
	// End UFIVSNodes
	
	// Begin UFIVSScriptNode
	virtual void InitPins() override;

	virtual FString GetNodeName() const override { return Function->GetDisplayName().ToString(); }

	virtual TArray<UFIVSPin*> PreExecPin(UFIVSPin* ExecPin, FFIVSRuntimeContext& Context) override;

	virtual UFIVSPin* ExecPin(UFIVSPin* ExecPin, FFIVSRuntimeContext& Context) override;
	// End UFIVSScriptNode

	void SetFunction(UFINFunction* InFunction) {
		check(ExecIn == nullptr);
		Function = InFunction;
	}
};