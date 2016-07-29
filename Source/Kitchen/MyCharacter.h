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

	//Camera component for our character
	UPROPERTY(EditAnywhere)
	class UCameraComponent* MyCharacterCamera;

	//Array to store all actors in the world; used to find which object is selected
	TArray<AActor*> AllActors;

	//TMap which keeps the open/closed state for our island drawers
	TMap<AActor*, EAssetState> AssetStateMap;

	//TMap which keeps the interractive items from the kitchen
	TMap<AActor*, EItemType> ItemMap;

	//Actor pointer for the item currently selected
	AActor* SelectedObject;

	//Actor currently focused
	AActor* HighlightedActor;

	//Pointer to the item held in the right hand
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	AActor* RightHandSlot;

	//Pointer to the item held in the left hand
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	AActor* LeftHandSlot;

	//Parameters for the ray trace
	FCollisionQueryParams TraceParams;

	//Vectors used in ray tracing
	FVector Start;
	FVector End;

	//Line trace Hit Result 
	FHitResult HitObject;

	//Variable for maximum grasping length
	float MaxGraspLength;

	//Variable storing which hand should perform the next action
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	bool bRightHandSelected;

	// Input variable for the force applied on the selected object
	FVector AppliedForce;

	//Vectors used for getting the size of an item
	FVector Min;
	FVector Max;

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
	UFUNCTION(BlueprintNativeEvent, Category = "Inputs")
	void Click();

	//Switches between which hand will perform the next action
	UFUNCTION(BlueprintNativeEvent, Category = "Inputs")
	void SwitchSelectedHand();

	//Function which returns the static mesh component of the selected object; NOT efficient --> Look for alternatives
	UStaticMeshComponent* GetStaticMesh(AActor* Actor);

	//Function to pick an item in one of our hands
	void PickToInventory(AActor* CurrentObject);

	//Function to release the currently held item
	void DropFromInventory(AActor* CurrentObject, FHitResult HitSurface);

	//Function to open / close drawers and doors
	void OpenCloseAction(AActor* OpenableActor);
	
public:
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetMyCharacterCamera() const { return MyCharacterCamera; }
	
};
