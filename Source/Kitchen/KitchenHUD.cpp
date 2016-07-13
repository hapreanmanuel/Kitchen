// Fill out your copyright notice in the Description page of Project Settings.

#include "Kitchen.h"
#include "KitchenHUD.h"
#include "TextureResource.h"
#include "CanvasItem.h"

AKitchenHUD::AKitchenHUD()
{
	//Set the crosshair textrure
	static ConstructorHelpers::FObjectFinder<UTexture2D>CrosshairTexObj(TEXT("/Game/FirstPerson/Textures/FirstPersonCrosshair"));
	CrosshairTex = CrosshairTexObj.Object;
}

void AKitchenHUD::DrawHUD()
{
	Super::DrawHUD();

	//Draw a simple crosshair

	//Find the center of the Canvas
	const FVector2D Center(Canvas->ClipX*0.5f, Canvas->ClipY*0.5f);

	//Offset by half the texture's dimensions so that the center of the texture aligns with ther center of the Canvas
	const FVector2D CrosshairDrawPosition((Center.X), (Center.Y));

	//Draw the crosshair
	FCanvasTileItem TileItem(CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(TileItem);
}

