// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

//Enum used in the TMap which keeps the state of the drawer
UENUM(BlueprintType)
enum class EDrawerState : uint8
{
	Closed UMETA(DisplayName = "Closed"),
	Open UMETA(DisplayName = "Open"),
	Unkown UMETA(DisplayName= "Unkown")
};

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

	//TMap which keeps the open/closed state for our drawers
	TMap<AActor*, AMyCharacter::EDrawerState> DrawerStateMap;


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

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* Mesh1P;

	//Function which returns the static mesh component of the selected object; NOT efficient --> Look for alternatives
	void GetStaticMesh(TSet<UActorComponent*> Components);
	

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetMyCharacterCamera() const { return MyCharacterCamera; }
	
};
