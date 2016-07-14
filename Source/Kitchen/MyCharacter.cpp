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

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(MyCharacterCamera);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	//Set the value of the applied force
	AppliedForce = FVector(1000, 1000, 1000);

	//Initialize TraceParams parameter
	TraceParams = FCollisionQueryParams(FName(TEXT("Trace")), true, this);
	TraceParams.bTraceComplex = true;
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = false;

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

	// Default Camera view bindings
	InputComponent->BindAxis("LookUp", this, &AMyCharacter::AddControllerPitchInput);
	InputComponent->BindAxis("Turn", this, &AMyCharacter::AddControllerYawInput);

	// Respond every frame to the values of our two movement axes, "MoveX" and "MoveY".
	InputComponent->BindAxis("MoveForward", this, &AMyCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AMyCharacter::MoveRight);

	//Set up the input from the mouse
	InputComponent->BindAction("Click", IE_Pressed,this, &AMyCharacter::Click);

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

void AMyCharacter::Click()
{
	//UE_LOG(LogTemp, Warning, TEXT("Button Pressed : Left mouse button!"));

	FVector Start = MyCharacterCamera->GetComponentLocation();
	FVector End = Start + MyCharacterCamera->GetForwardVector()*1000.0f;

	FHitResult HitObject = FHitResult(ForceInit);

	GetWorld()->LineTraceSingleByChannel(HitObject, Start, End, ECC_Pawn, TraceParams);

	if (HitObject.bBlockingHit)
	{
		//Selects the interractive asset that has been clicked on
		if (HitObject.GetActor()->GetName().Contains("Handle"))
		{
			SelectedObject = HitObject.GetActor()->GetAttachParentActor();
		}
		else
		{
			SelectedObject = HitObject.GetActor();
		}

		//Add force to open/close the selected asset and update it's state in the TMap
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

			FString TestString = AMyCharacter::GetEnumValueToString<EAssetState>("EAssetState", AssetStateMap.FindRef(SelectedObject));
			UE_LOG(LogTemp, Warning, TEXT("Asset : %s  Is now : %s"), *SelectedObject->GetName(), *TestString);

		}
	}
}



