// Fill out your copyright notice in the Description page of Project Settings.

#include "Kitchen.h"
#include "MyCharacter.h"
#include "GameFramework/InputSettings.h"


// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set this pawn to be controlled by the lowest-numbered player
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(5.f, 80.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	MyCharacterCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("MyCharacterCamera"));
	MyCharacterCamera->SetupAttachment(GetCapsuleComponent());
	MyCharacterCamera->RelativeLocation = FVector(0.f, 0.f, 64.f); // Position the camera
	MyCharacterCamera->bUsePawnControlRotation = true;

	//Set the value of the applied force
	AppliedForce = FVector(1000, 1000, 1000);

	//Initialize TraceParams parameter
	TraceParams = FCollisionQueryParams(FName(TEXT("Trace")), true, this);
	TraceParams.bTraceComplex = true;
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = false;

	//Set the maximum grasping length
	MaxGraspLength = 300;

	//Set the pointers to the items held in hands to null at the begining of the game
	LeftHandSlot = nullptr;
	RightHandSlot = nullptr;

	//By default our character will perfom actions with the right hand first
	bRightHandSelected = true;

}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	//Gets all actors in the world, used for identifying our drawers and setting their initial state to closed
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);

	//Loop that checks if current object matches our drawer
	for (const auto ActorIt : AllActors)
	{
		//Command used for determining the exact name of an actor. Only useful when designing.
		//UE_LOG(LogTemp, Warning, TEXT("Actor name: %s"), *ActorIt->GetName());

		//Finds the actors for the Handles, used to set the initial state of our drawers to closed 
		if (ActorIt->GetName().Contains("Handle"))
		{
			GetStaticMesh(ActorIt->GetComponents());

			if (SelectedObjectMesh != nullptr)
			{
				if (!ActorIt->GetName().Contains("Door"))
				{
					SelectedObjectMesh->AddImpulse(-6 * AppliedForce * ActorIt->GetActorForwardVector());
				}
				AssetStateMap.Add(ActorIt->GetAttachParentActor(), EAssetState::Closed);
			}
		}
		//Remember to tag items when adding them into the world
		//Or CREATE a function which does that automatically
		else if (ActorIt->ActorHasTag(FName(TEXT("Item"))))
		{
			ItemMap.Add(ActorIt, EItemType::GeneralItem);
		}
	}
}


//Function which returns static mesh, used for physics manipulation
void AMyCharacter::GetStaticMesh(TSet<UActorComponent*> Components)
{
	for (auto Component : Components)
	{
		if (Component->GetName().Contains("StaticMeshComponent"))
		{
			SelectedObjectMesh = Cast<UStaticMeshComponent>(Component);
			break;
		}
	}
}

// Called every frame
void AMyCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

/*
	Called to bind functionality to input
*/
void AMyCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	//// Default Camera view bindings
	InputComponent->BindAxis("LookUp", this, &AMyCharacter::AddControllerPitchInput);
	InputComponent->BindAxis("Turn", this, &AMyCharacter::AddControllerYawInput);

	//// Respond every frame to the values of our two movement axes, "MoveX" and "MoveY".
	InputComponent->BindAxis("MoveForward", this, &AMyCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AMyCharacter::MoveRight);

	////Set up the input from the mouse
	InputComponent->BindAction("Click", IE_Pressed,this, &AMyCharacter::Click);

	////Input from the tab button which switches selected hand
	InputComponent->BindAction("SwitchSelectedHand", IE_Pressed, this, &AMyCharacter::SwitchSelectedHand);
}

void AMyCharacter::MoveForward(const float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// Find out which way is forward
		FRotator Rotation = Controller->GetControlRotation();
		// Limit pitch when walking or falling
		if (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling())
		{
			Rotation.Pitch = 0.0f;
		}
		// add movement in that direction
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
		AddMovementInput(Direction, Value *0.5);
	}
}

void AMyCharacter::MoveRight(const float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value *0.5);
	}
}

void AMyCharacter::SwitchSelectedHand()
{
	bRightHandSelected = !bRightHandSelected;
	if (bRightHandSelected)
	{
		SelectedObject = RightHandSlot;
		UE_LOG(LogTemp, Warning, TEXT("Our character will perform the next action with his RIGHT hand"));
	}
	else
	{
		SelectedObject = LeftHandSlot;
		UE_LOG(LogTemp, Warning, TEXT("Our character will perform the next action with his LEFT hand"));
	}
}

void AMyCharacter::Click()
{
	FVector Start = MyCharacterCamera->GetComponentLocation();
	FVector End = Start + MyCharacterCamera->GetForwardVector()*1000.0f;

	FHitResult HitObject = FHitResult(ForceInit);

	GetWorld()->LineTraceSingleByChannel(HitObject, Start, End, ECC_Pawn, TraceParams);

	//Check if player clicked on a valid zone, if not exit the function call
	if (!HitObject.bBlockingHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not a valid action!"));
		return;
	}

	//Check if the object to interact with is not too far away
	if (HitObject.Distance > MaxGraspLength)
	{
		UE_LOG(LogTemp, Warning, TEXT("Target is too far away!"));
		return;
	}

	//Behaviour when we want to drop the item currently held in hand
	if (SelectedObject)
	{
		//Drops our currently selected item on the surface clicked on
		DropFromInventory(SelectedObject, HitObject);

		//Remove the reference to the selected object
		SelectedObject = nullptr;
	}

	//Behaviour when wanting to grab an item or opening/closing actions
	else
	{
		//Check if the player has clicked on a handle
		if (HitObject.GetActor()->GetName().Contains("Handle"))
		{
			SelectedObject = HitObject.GetActor()->GetAttachParentActor();
		}
		//Select the object that has been clicked on, or the parent in case it's a handle
		else
		{
			SelectedObject = HitObject.GetActor();
		}

		//Section for assets which can be opened or closed
		if (AssetStateMap.Contains(SelectedObject))
		{
			GetStaticMesh(SelectedObject->GetComponents());

			if (AssetStateMap.FindRef(SelectedObject) == EAssetState::Closed)
			{
				SelectedObjectMesh->AddImpulse(AppliedForce * SelectedObject->GetActorForwardVector());
				AssetStateMap.Add(SelectedObject, EAssetState::Open);
			}
			else if (AssetStateMap.FindRef(SelectedObject) == EAssetState::Open)
			{
				SelectedObjectMesh->AddImpulse(-AppliedForce * SelectedObject->GetActorForwardVector());
				AssetStateMap.Add(SelectedObject, EAssetState::Closed);
			}
			SelectedObject = nullptr;
			return;
		}

		//Section for items that can be picked up and moved around
		else if (ItemMap.Contains(SelectedObject))
		{
			//Picks up the item selected
			PickToInventory(SelectedObject);
		}

		//If the item can not pe opened nor moved, remove the reference from it
		else
		{
			SelectedObject = nullptr;
		}
	}
}

//Function to pick an item in one of our hands
void AMyCharacter::PickToInventory(AActor* CurrentObject)
{
	//Add a reference and an icon of the object in the correct item slot (left or right hand) 
	if (bRightHandSelected)
	{
		RightHandSlot = CurrentObject;

		/*	TODO
		Add icon of the object in the inventory slot
		*/
	}
	else
	{
		LeftHandSlot = CurrentObject;
		/*	TODO
		Add icon of the object in the inventory slot
		*/
	}

	//Hide the object from the world
	CurrentObject->SetActorHiddenInGame(true);
}

//Function to release the currently held item
void AMyCharacter::DropFromInventory(AActor* CurrentObject, FHitResult HitSurface)
{
	//Set the item visible in the game again
	CurrentObject->SetActorHiddenInGame(false);

	//Selects the mesh of our object so that we can change it's position in the world
	GetStaticMesh(CurrentObject->GetComponents());

	//Find the bounding limits of the currently selected object 
	SelectedObjectMesh->GetLocalBounds(Min, Max);

	//Method to move the object to our newly selected position
	SelectedObjectMesh->SetWorldLocation(HitSurface.Location - HitSurface.Normal*(Min *SelectedObjectMesh->GetComponentScale()));

	//Remove the reference to the object because we are not holding it any more
	if (bRightHandSelected)
	{
		RightHandSlot = nullptr;
		/*	TODO
		Remove icon of the object in the inventory slot
		*/
	}
	else
	{
		LeftHandSlot = nullptr;
		/*	TODO
		Remove icon of the object in the inventory slot
		*/
	}
}

/*
TODO
Create an inventory!!!!
*/

