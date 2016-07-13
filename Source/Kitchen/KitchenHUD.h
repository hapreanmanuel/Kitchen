// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "KitchenHUD.generated.h"

/**
 * 
 */
UCLASS()
class KITCHEN_API AKitchenHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	AKitchenHUD();

	//Draw call for the HUD
	virtual void DrawHUD() override;

private:

	//Crosshair asset pointer
	class UTexture2D* CrosshairTex;
	
	
};
