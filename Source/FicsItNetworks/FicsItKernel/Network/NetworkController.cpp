#include "NetworkController.h"

#include "FicsItNetworksCustomVersion.h"
#include "Computer/FINComputerSubsystem.h"
#include "Network/FINNetworkComponent.h"
#include "Network/FINNetworkCircuit.h"
#include "Network/FINNetworkCircuitNode.h"
#include "Network/FINNetworkUtils.h"

void UFINKernelNetworkController::Serialize(FArchive& Ar) {
	Super::Serialize(Ar);
	if (!Ar.IsSaveGame()) return;
	
	if (AFINComputerSubsystem::GetComputerSubsystem(this)->Version < FINKernelRefactor) return;
	
	// serialize signals
	int32 SignalCount = SignalQueue.Num();
	Ar << SignalCount;
	if (Ar.IsLoading()) {
		SignalQueue.Empty();
	}
	for (int i = 0; i < SignalCount; ++i) {
		FFINSignalData Signal;
		FFINNetworkTrace Trace;
		if (Ar.IsSaving()) {
			const TTuple<FFINSignalData, FFINNetworkTrace>& SignalData = SignalQueue[i];
			Signal = SignalData.Key;
			Trace = SignalData.Value;
		}
		bool bIsValid = Trace.IsValid();
		Ar << bIsValid;
		if (!bIsValid) continue;
		
		// save/save signal
		Signal.Serialize(Ar);
		Ar << Trace;
		
		if (Ar.IsLoading()) {
			SignalQueue.Add(TPair<FFINSignalData, FFINNetworkTrace>{Signal, Trace});
		}
	}
}

void UFINKernelNetworkController::PreSaveGame_Implementation(int32 saveVersion, int32 gameVersion) {
	bLockSignalReceiving = true;
}

void UFINKernelNetworkController::PostSaveGame_Implementation(int32 saveVersion, int32 gameVersion) {
	bLockSignalReceiving = false;
}

void UFINKernelNetworkController::PreLoadGame_Implementation(int32 saveVersion, int32 gameVersion) {
	bLockSignalReceiving = true;
}

void UFINKernelNetworkController::PostLoadGame_Implementation(int32 saveVersion, int32 gameVersion) {
	bLockSignalReceiving = false;
}

void UFINKernelNetworkController::SetComponent(TScriptInterface<IFINNetworkComponent> InNetworkComponent) {
	Component = InNetworkComponent;
}

TScriptInterface<IFINNetworkComponent> UFINKernelNetworkController::GetComponent() const {
	return Component;
}

void UFINKernelNetworkController::HandleSignal(const FFINSignalData& signal, const FFINNetworkTrace& sender) {
	PushSignal(signal, sender);
}

FFINSignalData UFINKernelNetworkController::PopSignal(FFINNetworkTrace& OutSender) {
	if (GetSignalCount() < 1) return FFINSignalData();
	FScopeLock Lock(&MutexSignals);
	const TTuple<FFINSignalData, FFINNetworkTrace> Signal = SignalQueue.Pop();
	OutSender = Signal.Value;
	return Signal.Key;
}

void UFINKernelNetworkController::PushSignal(const FFINSignalData& signal, const FFINNetworkTrace& sender) {
	FScopeLock Lock(&MutexSignals);
	if (GetSignalCount() >= MaxSignalCount || bLockSignalReceiving) return;
	SignalQueue.Add(TPair<FFINSignalData, FFINNetworkTrace>{signal, sender});
}

void UFINKernelNetworkController::ClearSignals() {
	FScopeLock Lock(&MutexSignals);
	SignalQueue.Empty();
}

size_t UFINKernelNetworkController::GetSignalCount() {
	FScopeLock Lock(&MutexSignals);
	return SignalQueue.Num();
}

FFINNetworkTrace UFINKernelNetworkController::GetComponentByID(const FGuid& InID) const {
	if (!Component.GetObject()->Implements<UFINNetworkCircuitNode>()) return FFINNetworkTrace();
	return FFINNetworkTrace(Component.GetObject()) / IFINNetworkCircuitNode::Execute_GetCircuit(Component.GetObject())->FindComponent(InID, Component).GetObject();
}

TSet<FFINNetworkTrace> UFINKernelNetworkController::GetComponentByNick(const FString& InNick) const {
	if (!Component.GetObject()->Implements<UFINNetworkCircuitNode>()) return TSet<FFINNetworkTrace>();
	TSet<FFINNetworkTrace> OutComponents;
	TSet<UObject*> Components = IFINNetworkCircuitNode::Execute_GetCircuit(Component.GetObject())->FindComponentsByNick(InNick, Component);
	for (UObject* Comp : Components) {
		OutComponents.Add(FFINNetworkTrace(Component.GetObject()) / Comp);
	}
	return OutComponents;
}

TSet<FFINNetworkTrace> UFINKernelNetworkController::GetComponentByClass(UClass* InClass, bool bRedirect) const {
	if (!Component.GetObject()->Implements<UFINNetworkCircuitNode>()) return TSet<FFINNetworkTrace>();
	TSet<FFINNetworkTrace> outComps;
	TSet<UObject*> Comps = IFINNetworkCircuitNode::Execute_GetCircuit(Component.GetObject())->GetComponents();
	for (UObject* Comp : Comps) {
		if (bRedirect) {
			UObject* RedirectedComp = UFINNetworkUtils::RedirectIfPossible(FFINNetworkTrace(Comp)).Get();
			if (!RedirectedComp->IsA(InClass)) continue;
		} else if (!Comp->IsA(InClass)) continue;
		if (!IFINNetworkComponent::Execute_AccessPermitted(Comp, IFINNetworkComponent::Execute_GetID(Component.GetObject()))) continue;
		outComps.Add(FFINNetworkTrace(Component.GetObject()) / Comp);
	}
	return outComps;
}
