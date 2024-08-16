// Fill out your copyright notice in the Description page of Project Settings.


#include "ChainedDiContainer.h"

void Tentacle::FChainedDiContainer::SetParentContainer(TWeakPtr<FChainedDiContainer> DiContainer)
{
	ParentContainer = DiContainer;
}


