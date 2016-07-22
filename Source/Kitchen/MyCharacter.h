// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


//Enum used in the TMap which keeps the state of the drawer
UENUM(BlueprintType)
enum class EAssetState : uint8
{
	Closed UMETA(DisplayName = "Closed"),
	Open UMETA(DisplayName = "Open"),
	Unkown UMETA(DisplayName = "Unkown")
};

//Enum used when mapping the items
UENUM(BlueprintType)
enum class EItemType : uint8
{
	GeneralItem UMETA(DisplayName = "GeneralItem"),
	Cup UMETA(DisplayName = "Cup"),
	Plate UMETA(DisplayName = "Plate"),
	Mug	UMETA(DisplayName = "Plate"),
	Pan UMETA(DisplayName = "Plate"),
	Spatula UMETA(DisplayName = "Plate"),
	Spoon UMETA(DisplayName = "Plate")
};

#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"



UCLASS()
class KITCHEN_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCharacter();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	//Camera component for our character
	UPROPERTY(EditAnywhere)
	class UCameraComponent* MyCharacterCamera;

	//Array to store all actors in the world; used to find which object is selected
	TArray<AActor*> AllActors;

	//Actor to select which object is to be manipulated 
	UPROPERTY(EditAnywhere)
	AActor* SelectedObject;

	//Component of the selected object with physics behaviour
	UStaticMeshComponent* SelectedObjectMesh;

	// Input variable for the force applied on the selected object
	FVector AppliedForce;

	//Parameters for the ray trace
	FCollisionQueryParams TraceParams;

	//Variable for maximum grasping length
	float MaxGraspLength;

	//Variable storing which hand should perform the next action
	bool bRightHandSelected;

	//Pointer to the item held in the right hand
	AActor* RightHandSlot;

	//Pointer to the item held in the left hand
	AActor* LeftHandSlot;

	//Vectors used for getting the offset needed when placing an item on a surface
	FVector Min;
	FVector Max;

	//Function to pick an item in one of our hands
	void PickToInventory(AActor* CurrentObject);

	//Function to release the currently held item
	void DropFromInventory(AActor* CurrentObject, FHitResult HitSurface);

	//TMap which keeps the open/closed state for our island drawers
	TMap<AActor*, EAssetState> AssetStateMap;

	//TMap which keeps the interractive items from the kitchen
	TMap<AActor*, EItemType> ItemMap;

	//Function to return a string out of the enum type
	template<typename TEnum>
	static FORCEINLINE FString GetEnumValueToString(const FString& Name, TEnum Value)
	{
		const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *Name, true);
		if (!enumPtr)
		{
			return FString("Invalid");
		}
		return enumPtr->GetEnumName((int32)Value);
	}

protected:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	/** Handles movement of our character forward/backward */
	void MoveForward(const float Value);

	/** Handles movement of our character, left and right */
	void MoveRight(const float Value);

	//Handles the input from the mouse
	void Click();

	//Switches between which hand will perform the next action
	void SwitchSelectedHand();

	//Function which returns the static mesh component of the selected object; NOT efficient --> Look for alternatives
	void GetStaticMesh(TSet<UActorComponent*> Components);
	

public:
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetMyCharacterCamera() const { return MyCharacterCamera; }
	
};
