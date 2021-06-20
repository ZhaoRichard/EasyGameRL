// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RLComponent.generated.h"

USTRUCT()
struct FNestedArray {

	GENERATED_BODY()
	TArray<float> Action;
};

enum class AlgorithmType : uint8 {
	QLearning = 0, 
	SARSA = 1,
	QLambda = 2,
	SARSALambda = 3,
	QLearningTabular = 4,
	SARSATabular = 5,
	DQN = 20
};




UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class EASYRL_API URLComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URLComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

	// Variables
	AlgorithmType algorithm;

	float myLearningRate;     //Alpha
	float myExplorationRate;  //Epsilon
	float myDiscountFactor;   //Gamma
	float myEligibilityDecay;      //Lambda
	float myLearningRateDecay;
	float myExplorationRateDecay;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int NumOfActions;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int NumOfFeatures;
	bool linearFunctionApprox;
	int NumOfStates;

	int curAction;
	int nextAction;
	TArray<bool> curState;
	int curStateTabular;

	int weightLimit;

	TArray<FNestedArray> wTable;
	//Access like so
	//wTable[0].Action[0];

	TArray<FNestedArray> QTable;
	
	TArray<FNestedArray> eTable;

	FString logFilePath;
	FString logDataPath;

	bool showDebug;
	int count;

	// Functions ========
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	TArray<FString, FDefaultAllocator> saveLoadAndRemoveSlashes();

	UFUNCTION(BlueprintCallable)
		void setLogFileLocation(FString logLocation);

	UFUNCTION(BlueprintCallable)
		void setLogDataLocation(FString logLocation);

	UFUNCTION(BlueprintCallable)
		void CreateLogFile();


	UFUNCTION(BlueprintCallable)
		void RLUpdate(TArray<bool> nextState, float reward);

	UFUNCTION(BlueprintCallable)
		void RLTabularUpdate(int nextState, float reward);


	UFUNCTION(BlueprintCallable)
		int RLSelectAction();


	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "State"))
		void InitializeQLearning(TArray<bool> State);

	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "State"))
		void InitializeSARSA(TArray<bool> State);

	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "State"))
		void InitializeTabularQLearning(int State);

	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "State"))
		void InitializeTabularSARSA(int State);



	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "NumberOfActions"))
		void setNumberOfActions(int NumberOfActions);

	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "NumberOfFeatures"))
		void setLinearFunctionApprox(int NumberOfFeatures);

	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "NumberOfStates"))
		void setNumberOfStates(int NumberOfStates);

	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "LearningRate", DefaultToSelf, HideSelfPin))
		void setLearningRate(float LearningRate);

	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "ExplorationRate"))
		void setExplorationRate(float ExplorationRate);

	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "DiscountFactor"))
		void setDiscountFactor(float DiscountFactor);

	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "Lambda"))
		void setEligibilityDecay(float Lambda);

	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "Decay"))
		void setLearningRateDecay(float Decay);

	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "Decay"))
		void setExplorationRateDecay(float Decay);




	UFUNCTION(BlueprintCallable)
		void takeScreenshot();

	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "ShowDebugMessages"))
		void setDebugMessages(bool ShowDebugMessages);


	// Helper Functions ======
	int QLearningSelectAction();
	void QLearningUpdate(TArray<bool> nextState, float reward);

	int QLearningSelectActionTabular();
	void QLearningUpdateTabular(int nextState, float reward);

	int SARSASelectAction();
	void SARSAUpdate(TArray<bool> nextState, float reward);
	
	void SARSAUpdateTabular(int nextState, float reward);
	
	void writeToFiles(float reward);


};
