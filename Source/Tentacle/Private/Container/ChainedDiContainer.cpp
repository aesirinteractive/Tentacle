// Fill out your copyright notice in the Description page of Project Settings.


#include "Container/ChainedDiContainer.h"

void Tentacle::FChainedDiContainer::SetParentContainer(TWeakPtr<FChainedDiContainer> DiContainer)
{
	ParentContainer = DiContainer;
}


