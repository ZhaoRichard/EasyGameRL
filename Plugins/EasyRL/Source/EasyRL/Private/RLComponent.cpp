// Fill out your copyright notice in the Description page of Project Settings.


#include "RLComponent.h"
#include "Engine/GameEngine.h"
#include "Engine/World.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/DefaultValueHelper.h"
#include "HAL/FileManager.h"
#include "Containers/UnrealString.h"
#include "Containers/ContainerAllocationPolicies.h"
#include "Math/UnrealMathUtility.h"
#include "Math/RandomStream.h"
#include <Runtime\Core\Public\HAL\PlatformFilemanager.h>

#define Debug(x) if(GEngine && showDebug){GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, x);}
// Example using Debug for showing a variable
// int a = 1;
// Debug(FString::Printf(TEXT("Some int variable: %i"), a));
// float f = 1.0;
// Debug(FString::Printf(TEXT("Some float variable: %f"), f));


// Sets default values for this component's properties
URLComponent::URLComponent() {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void URLComponent::BeginPlay() {
	Super::BeginPlay();

	// defaults
	myExplorationRate = 0.2;
	myLearningRate = 0.5;
	myDiscountFactor = 0.1;
	myEligibilityDecay = 0;
	myLearningRateDecay = 0;
	myExplorationRateDecay = 0.001;
	linearFunctionApprox = false;
	NumOfFeatures = 0;
	NumOfActions = 1;
	NumOfStates = 0;
	curAction = 0;
	count = 0;
	weightLimit = 200;
	showDebug = true;
	logFilePath = "logfile.txt";
	logDataPath = "logdata.txt";

}


// Called every frame
void URLComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

TArray<FString, FDefaultAllocator> URLComponent::saveLoadAndRemoveSlashes() {
	//for saving / loading files.  
	FString CompleteFilePath = logFilePath;
	FString FileData = "";
	FFileHelper::LoadFileToString(FileData, *CompleteFilePath);

	//Remove Slashes
	TArray<FString, FDefaultAllocator> Array;
	FString MyString(FileData);
	MyString.ParseIntoArray(Array, TEXT("/"), true);

	return Array;
}



void URLComponent::takeScreenshot() {
	FString comd = "HighResShot 2";
	//GetWorld()->Exec(GetWorld(), *comd);

	APlayerController* PController = GetWorld()->GetFirstPlayerController();
	PController->ConsoleCommand(*comd);
}

void URLComponent::setDebugMessages(bool ShowDebugMessages) {
	showDebug = ShowDebugMessages;
}

void URLComponent::setLogFileLocation(FString logLocation) {
	logFilePath = logLocation;
}

void URLComponent::setLogDataLocation(FString logLocation) {
	logDataPath = logLocation;
}



//Creates log file if that doesn't exist 
void URLComponent::CreateLogFile() {

	if (FPaths::FileExists(*logFilePath))
	{
		return;
	}


	FString TextToSave = FString("\n");
	FString space = FString("/");
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();


	if (algorithm == AlgorithmType::QLearningTabular || algorithm == AlgorithmType::SARSATabular)
	{
		// fill and initilize Q values randomly
		for (int i = 0; i < NumOfStates; i++)
		{
			for (int j = 0; j < NumOfActions; j++)
			{
				float generatingFloat = (float)FMath::RandRange(0, 10) / 10.0;

				FString newString = FString::SanitizeFloat(generatingFloat);

				FFileHelper::SaveStringToFile(newString, *logFilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
				FFileHelper::SaveStringToFile(space, *logFilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);

			}
		}
	}
	else
	{
		// fill and initilize weights randomly
		for (int i = 0; i < NumOfFeatures; i++)
		{
			for (int j = 0; j < NumOfActions; j++)
			{
				float generatingFloat = (float)FMath::RandRange(0, 10) / 10.0;

				FString newString = FString::SanitizeFloat(generatingFloat);

				FFileHelper::SaveStringToFile(newString, *logFilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
				FFileHelper::SaveStringToFile(space, *logFilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);

			}
		}
	}
}

void URLComponent::writeToFiles(float reward) {


	FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*logFilePath);
	FString space = FString("/");

	//write updated table to text file
	if (algorithm == AlgorithmType::QLearningTabular || algorithm == AlgorithmType::SARSATabular)
	{
		for (int i = 0; i < NumOfStates; i++) {
			for (int j = 0; j < NumOfActions; j++) {

				float value = QTable[i].Action[j];

				FString newString = FString::SanitizeFloat(value);
				FFileHelper::SaveStringToFile(newString, *logFilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
				FFileHelper::SaveStringToFile(space, *logFilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);

			}
		}
	}
	else
	{
		for (int i = 0; i < NumOfFeatures; i++) {
			for (int j = 0; j < NumOfActions; j++) {

				float value = wTable[i].Action[j];

				FString newString = FString::SanitizeFloat(value);
				FFileHelper::SaveStringToFile(newString, *logFilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
				FFileHelper::SaveStringToFile(space, *logFilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);

			}
		}
	}


	count++;

	FString data1 = "reward: " + FString::SanitizeFloat(reward) + "\n";
	FFileHelper::SaveStringToFile(data1, *logDataPath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);

}

int URLComponent::RLSelectAction() {
	if (algorithm == AlgorithmType::QLearning || algorithm == AlgorithmType::QLambda)
	{
		return QLearningSelectAction();
	}
	else if (algorithm == AlgorithmType::SARSA || algorithm == AlgorithmType::SARSALambda ||
		algorithm == AlgorithmType::SARSATabular)
	{
		return SARSASelectAction();
	}
	else if (algorithm == AlgorithmType::QLearningTabular)
	{
		return QLearningSelectActionTabular();
	}
	return 0;
}


void URLComponent::RLUpdate(TArray<bool> nextState, float reward) {

	nextState.Add(true);  // add always1 feature
	if (nextState.Num() != NumOfFeatures)
	{
		Debug(FString::Printf(TEXT("Error number of features: %i"), nextState.Num()));
		return;
	}

	if (algorithm == AlgorithmType::QLearning || algorithm == AlgorithmType::QLambda)
	{
		QLearningUpdate(nextState, reward);
	}
	else if (algorithm == AlgorithmType::SARSA || algorithm == AlgorithmType::SARSALambda)
	{
		SARSAUpdate(nextState, reward);
	}


	if (myExplorationRate < 0) myExplorationRate = 0;
	if (myLearningRate < 0) myLearningRate = 0;
}

void URLComponent::RLTabularUpdate(int nextState, float reward) {

	if (nextState >= NumOfStates)
	{
		Debug(FString::Printf(TEXT("Error state: %i"), nextState));
		return;
	}

	if (algorithm == AlgorithmType::QLearningTabular)
	{
		QLearningUpdateTabular(nextState, reward);
	}
	else if (algorithm == AlgorithmType::SARSATabular)
	{
		SARSAUpdateTabular(nextState, reward);
	}

	if (myExplorationRate < 0) myExplorationRate = 0;
	if (myLearningRate < 0) myLearningRate = 0;
}



// Q Learning
int URLComponent::QLearningSelectAction() {
	int selectedAction = 0;

	FString temp = "";
	int counter = 0;

	TArray<float> QValsOfState;

	//for saving / loading files.  
	TArray<FString, FDefaultAllocator> ArrayVals = saveLoadAndRemoveSlashes();

	//Read values of Array
	for (int i = 0; i < NumOfFeatures; i++)
	{
		for (int j = 0; j < NumOfActions; j++)
		{
			temp = ArrayVals[counter];
			wTable[i].Action[j] = FCString::Atod(*temp);
			counter++;
		}
	}
	QValsOfState.SetNum(NumOfActions);

	for (int j = 0; j < NumOfActions; j++) //calculate Q(state, all actions)
	{
		float Qsum = 0; //resets Qsum to 0 each loop (for each different action)

		for (int i = 0; i < NumOfFeatures; i++) //looping through all the features
		{
			//if the feature is active, it's value is accumulated in the Q value calculation for this action
			if (curState[i] == true) {
				Qsum += wTable[i].Action[j]; 
			}

		}
		QValsOfState[j] = Qsum; //each action gets its calculated QValue put into an array
	}


	//generate random number for epsilon-greedy policy
	float randomNumber = float(FMath::RandRange(0, 100)) / 100.0;

	//Debug(FString::Printf(TEXT("random: %f %f"), randomNumber, myExplorationRate));
	curAction = 0;

	if (randomNumber > myExplorationRate)
	{
		//select the best action
		for (int j = 0; j < NumOfActions; j++) //loops through all actions
		{
			//For all the QVals, go through to find the best (highest) value
			if (QValsOfState[j] > QValsOfState[curAction])
			{
				curAction = j;
			}
		}

	}
	else
	{
		//do exploration. Select a random action
		curAction = FMath::RandRange(0, NumOfActions - 1);
	}

	return curAction; //  the action choice as determined above
}

void URLComponent::QLearningUpdate(TArray<bool> nextState, float reward) {

	Debug(FString::Printf(TEXT("Q reward: %f step: %i"), reward, count));

	FString temp = "";
	int counter = 0;
	float maxQnextSnextA = 0;
	float QSA = 0;

	//TArray<float> QValsOfNextState;

	//for saving / loading files.  
	TArray<FString, FDefaultAllocator> ArrayVals = saveLoadAndRemoveSlashes();

	//Read values of Array
	for (int i = 0; i < NumOfFeatures; i++)
	{
		for (int j = 0; j < NumOfActions; j++)
		{
			temp = ArrayVals[counter];
			wTable[i].Action[j] = FCString::Atod(*temp);
			counter++;
		}
	}

	//QValsOfNextState.SetNum(NumOfActions); // This is where the Q(s',a) for all actions in s' are stored

	//Calculate max Q(s',a')
	maxQnextSnextA = -FLT_MAX;

	for (int j = 0; j < NumOfActions; j++) //calculate Q(features, all actions)
	{
		float Qsum = 0; //resets Qsum to 0 each loop (for each different action)

		for (int i = 0; i < NumOfFeatures; i++) //looping through all the features
		{
			//if the feature is active, it's value is accumulated in the Q value calculation for this action
			if (nextState[i] == true) {
				Qsum += wTable[i].Action[j]; 
			}

		}

		//QValsOfNextState[j] = Qsum;

		if (Qsum > maxQnextSnextA)
			maxQnextSnextA = Qsum;
	}

	//for Q(s,a)
	float Qsum = 0; //resets Qsum to 0 

	for (int i = 0; i < NumOfFeatures; i++) //looping through all the features
	{
		//if the feature is active, it's value is accumulated in the Q value calculation for this action
		if (curState[i] == true) {
			Qsum += wTable[i].Action[curAction]; 
		}
	}

	QSA = Qsum;


	if (algorithm == AlgorithmType::QLambda)
	{
		// e(s,a)++
		for (int i = 0; i < NumOfFeatures; i++)
		{
			if (curState[i] == true)
			{
				eTable[i].Action[curAction] += 1;
			}
		}
	}

	// delta = reward + discountFactor (maxQ(s',a')) - Q(s,a)
	float delta = reward + myDiscountFactor * maxQnextSnextA - QSA;


	//update the weights
	if (algorithm == AlgorithmType::QLearning)
	{
		for (int i = 0; i < NumOfFeatures; i++)
		{
			if (curState[i] == true)
			{	// weight = weight + learningRate( reward + discountFactor (maxQ(s',a')) - Q(s,a) )
				wTable[i].Action[curAction] += myLearningRate * delta;

				if (wTable[i].Action[curAction] > weightLimit) wTable[i].Action[curAction] = weightLimit;
				if (wTable[i].Action[curAction] < -1 * weightLimit) wTable[i].Action[curAction] = -1 * weightLimit;
			}

		}
	}
	else if (algorithm == AlgorithmType::QLambda)
	{
		for (int j = 0; j < NumOfActions; j++)
		{
			for (int i = 0; i < NumOfFeatures; i++) //looping through all the features
			{
				// weight = weight + learningRate(delta)(e(s,a))
				wTable[i].Action[j] += myLearningRate * delta * eTable[i].Action[j];

				if (wTable[i].Action[j] > weightLimit) wTable[i].Action[j] = weightLimit;
				if (wTable[i].Action[j] < -1 * weightLimit) wTable[i].Action[j] = -1 * weightLimit;

				eTable[i].Action[j] = myDiscountFactor * myEligibilityDecay * eTable[i].Action[j];

			}
		}
	}

	//set current state to = next state
	curState = nextState;
	myExplorationRate -= myExplorationRateDecay;
	myLearningRate -= myLearningRateDecay;

	// log files
	writeToFiles(reward);
}

void URLComponent::InitializeQLearning(TArray<bool> State) {

	State.Add(true);  // add always1 feature
	if (State.Num() != NumOfFeatures)
	{
		Debug(FString::Printf(TEXT("Error number of features: %i"), State.Num()));
		return;
	}

	curState = State;

	if (linearFunctionApprox == true)
	{
		if (myEligibilityDecay > 0)
		{
			algorithm = AlgorithmType::QLambda;

			// e(s,a)
			for (int i = 0; i < NumOfFeatures; i++)
			{
				FNestedArray Array;
				for (int j = 0; j < NumOfActions; j++)
				{
					Array.Action.Add(0.0);
				}
				eTable.Add(Array);
			}
		}
		else
		{
			algorithm = AlgorithmType::QLearning;
		}



		for (int i = 0; i < NumOfFeatures; i++)
		{
			FNestedArray Array;
			for (int j = 0; j < NumOfActions; j++)
			{
				Array.Action.Add((float)FMath::RandRange(0, 10) / 10.0);
			}
			wTable.Add(Array);
		}
	}
	else
	{
		Debug(FString::Printf(TEXT("Error: No linear function approx specified.")));
		return;
	}

	CreateLogFile();
}


// Tabular Q Learning
int URLComponent::QLearningSelectActionTabular()
{
	int selectedAction = 0;

	FString temp = "";
	int counter = 0;

	//for saving / loading files.  
	TArray<FString, FDefaultAllocator> ArrayVals = saveLoadAndRemoveSlashes();

	//Read values of Array
	for (int i = 0; i < NumOfStates; i++)
	{
		for (int j = 0; j < NumOfActions; j++)
		{
			temp = ArrayVals[counter];
			QTable[i].Action[j] = FCString::Atod(*temp);
			counter++;
		}
	}


	//generate random number for epsilon-greedy policy
	float randomNumber = float(FMath::RandRange(0, 100)) / 100.0;

	//Debug(FString::Printf(TEXT("random: %f %f"), randomNumber, myExplorationRate));
	curAction = 0;

	if (randomNumber > myExplorationRate)
	{
		//select the best action
		for (int j = 0; j < NumOfActions; j++) //loops through all actions
		{
			//For all the QVals, go through to find the best (highest) value
			if (QTable[curStateTabular].Action[j] > QTable[curStateTabular].Action[curAction])
			{
				curAction = j;
			}
		}

	}
	else
	{
		//do exploration. Select a random action
		curAction = FMath::RandRange(0, NumOfActions - 1);
	}

	return curAction; //  the action choice as determined above
}

void URLComponent::QLearningUpdateTabular(int nextState, float reward) {

	Debug(FString::Printf(TEXT("QT reward: %f step: %i"), reward, count));

	FString temp = "";
	int counter = 0;
	float maxQnextSnextA = 0;
	float QSA = 0;

	//for saving / loading files.  
	TArray<FString, FDefaultAllocator> ArrayVals = saveLoadAndRemoveSlashes();

	//Read values of Array
	for (int i = 0; i < NumOfStates; i++)
	{
		for (int j = 0; j < NumOfActions; j++)
		{
			temp = ArrayVals[counter];
			QTable[i].Action[j] = FCString::Atod(*temp);
			counter++;
		}
	}


	//Calculate max Q(s',a')
	maxQnextSnextA = QTable[nextState].Action[0];

	for (int j = 0; j < NumOfActions; j++) 
	{
		if (QTable[nextState].Action[j] > maxQnextSnextA)
			maxQnextSnextA = QTable[nextState].Action[j];
	}

	//for Q(s,a)
	QSA = QTable[curStateTabular].Action[curAction];


	// delta = reward + discountFactor (maxQ(s',a')) - Q(s,a)
	float delta = reward + myDiscountFactor * maxQnextSnextA - QSA;


	//update the weights
	// Q = Q + learningRate( reward + discountFactor (maxQ(s',a')) - Q(s,a) )
	QTable[curStateTabular].Action[curAction] += myLearningRate * delta;

	if (QTable[curStateTabular].Action[curAction] > weightLimit) QTable[curStateTabular].Action[curAction] = weightLimit;
	if (QTable[curStateTabular].Action[curAction] < -1 * weightLimit) QTable[curStateTabular].Action[curAction] = -1 * weightLimit;
	

	//set current state to = next state
	curStateTabular = nextState;
	myExplorationRate -= myExplorationRateDecay;
	myLearningRate -= myLearningRateDecay;

	// log files
	writeToFiles(reward);

}

void URLComponent::InitializeTabularQLearning(int State) {


	curStateTabular = State;

	if (linearFunctionApprox == true)
	{
		Debug(FString::Printf(TEXT("Error: Linear function approx specified in tabular Q Learning.")));
		return;
	}
	else
	{
		algorithm = AlgorithmType::QLearningTabular;

		for (int i = 0; i < NumOfStates; i++)
		{
			FNestedArray Array;
			for (int j = 0; j < NumOfActions; j++)
			{
				Array.Action.Add((float)FMath::RandRange(0, 10) / 10.0);
			}
			QTable.Add(Array);
		}
	}

	CreateLogFile();
}


// SARSA
int URLComponent::SARSASelectAction() {

	return curAction;
}

void URLComponent::SARSAUpdate(TArray<bool> nextState, float reward) {

	Debug(FString::Printf(TEXT("S reward: %f step: %i"), reward, count));

	FString temp = "";
	int counter = 0;
	TArray<float> QValsOfNextState;

	//for saving / loading files.  
	TArray<FString, FDefaultAllocator> ArrayVals = saveLoadAndRemoveSlashes();

	//Read values of Array 
	for (int i = 0; i < NumOfFeatures; i++)
	{
		for (int j = 0; j < NumOfActions; j++)
		{
			temp = ArrayVals[counter];
			wTable[i].Action[j] = FCString::Atod(*temp);
			counter++;
		}
	}

	QValsOfNextState.SetNum(NumOfActions); // This is where the Q(s',a) for all actions in s' are stored


	for (int j = 0; j < NumOfActions; j++) //calculate Q(state, all actions)
	{
		float Qsum = 0; //resets Qsum to 0 each loop (for each different action)

		for (int i = 0; i < NumOfFeatures; i++) //looping through all the features
		{
			//if the feature is active, it's value is accumulated in the Q value calculation for this action
			if (nextState[i] == true) {
				Qsum += wTable[i].Action[j]; 

			}
		}
		QValsOfNextState[j] = Qsum; //each action gets its calculated QValue put into an array

	}

	//Debug(FString::Printf(TEXT("q values: %f %f %f %f %f"), QValsOfNextState[0], QValsOfNextState[1], QValsOfNextState[2], QValsOfNextState[3], QValsOfNextState[4]));

	//Select next action a', based on s' and Epsilon Greedy

	nextAction = 0;

	//Generate Random Number for epsilon-greedy policy
	float randomNumber = float(FMath::RandRange(0, 100)) / 100.0;

	if (randomNumber > myExplorationRate)
	{
		//select the best action
		for (int j = 0; j < NumOfActions; j++) //loops through all actions
		{
			//For all the QVals, go through to find the best (highest) value
			if (QValsOfNextState[j] > QValsOfNextState[nextAction])//
			{
				nextAction = j;
			}
		}

	}
	else
	{
		//do exploration. Select a random action.   
		nextAction = FMath::RandRange(0, NumOfActions - 1);
	}



	//Calculate the Q(s',a') and Q(s,a)
	float QSA = 0;
	float QNextSNextA = 0;


	//for Q(s,a)
	float Qsum = 0; //resets Qsum to 0

	for (int i = 0; i < NumOfFeatures; i++) //looping through all the features
	{
		//if the feature is active, it's value is accumulated in the Q value calculation for this action
		if (curState[i] == true) {
			Qsum += wTable[i].Action[curAction]; 
		}
	}

	QSA = Qsum;

	//for Q(s',a')
	QNextSNextA = QValsOfNextState[nextAction];

	if (algorithm == AlgorithmType::SARSALambda)
	{
		// e(s,a)++
		for (int i = 0; i < NumOfFeatures; i++)
		{
			if (curState[i] == true)
			{
				eTable[i].Action[curAction] += 1;
			}
		}
	}

	// delta = reward + discountFactor (Q(s',a')) - Q(s,a)
	float delta = reward + myDiscountFactor * QNextSNextA - QSA;


	//update the weights
	if (algorithm == AlgorithmType::SARSA)
	{
		for (int i = 0; i < NumOfFeatures; i++)
		{
			if (curState[i] == true)
			{	// weight = weight + learningRate( reward + discountFactor (Q(s',a')) - Q(s,a) )

				wTable[i].Action[curAction] += myLearningRate * delta;

				if (wTable[i].Action[curAction] > weightLimit) wTable[i].Action[curAction] = weightLimit;
				if (wTable[i].Action[curAction] < -1 * weightLimit) wTable[i].Action[curAction] = -1 * weightLimit;

			}

		}
	}
	else if (algorithm == AlgorithmType::SARSALambda)
	{

		for (int j = 0; j < NumOfActions; j++)
		{
			for (int i = 0; i < NumOfFeatures; i++) //looping through all the features
			{
				// weight = weight + learningRate(delta)(e(s,a))
				wTable[i].Action[j] += myLearningRate * delta * eTable[i].Action[j];

				if (wTable[i].Action[j] > weightLimit) wTable[i].Action[j] = weightLimit;
				if (wTable[i].Action[j] < -1 * weightLimit) wTable[i].Action[j] = -1 * weightLimit;

				eTable[i].Action[j] = myDiscountFactor * myEligibilityDecay * eTable[i].Action[j];

			}
		}
	}


	curState = nextState;
	curAction = nextAction;
	//decay
	myExplorationRate -= myExplorationRateDecay;
	myLearningRate -= myLearningRateDecay;

	// log files
	writeToFiles(reward);
}

void URLComponent::InitializeSARSA(TArray<bool> State) {

	State.Add(true);  // add always1 feature
	if (State.Num() != NumOfFeatures)
	{
		Debug(FString::Printf(TEXT("Error number of features: %i"), State.Num()));
		return;
	}

	curAction = FMath::RandRange(0, NumOfActions - 1);
	curState = State;

	if (linearFunctionApprox == true)
	{
		if (myEligibilityDecay > 0)
		{
			algorithm = AlgorithmType::SARSALambda;

			// e(s,a)
			for (int i = 0; i < NumOfFeatures; i++)
			{
				FNestedArray Array;
				for (int j = 0; j < NumOfActions; j++)
				{
					Array.Action.Add(0.0);
				}
				eTable.Add(Array);
			}
		}
		else
		{
			algorithm = AlgorithmType::SARSA;
		}


		for (int i = 0; i < NumOfFeatures; i++)
		{
			FNestedArray Array;
			for (int j = 0; j < NumOfActions; j++)
			{
				Array.Action.Add((float)FMath::RandRange(0, 10) / 10.0);
			}
			wTable.Add(Array);
		}

	}
	else
	{
		Debug(FString::Printf(TEXT("Error: No linear function approx specified.")));
		return;
	}

	CreateLogFile();
}


// Tabular SARSA
void URLComponent::SARSAUpdateTabular(int nextState, float reward) {

	Debug(FString::Printf(TEXT("ST reward: %f step: %i"), reward, count));

	FString temp = "";
	int counter = 0;
	TArray<float> QValsOfNextState;

	//for saving / loading files.  
	TArray<FString, FDefaultAllocator> ArrayVals = saveLoadAndRemoveSlashes();

	//Read values of Array 
	for (int i = 0; i < NumOfStates; i++)
	{
		for (int j = 0; j < NumOfActions; j++)
		{
			temp = ArrayVals[counter];
			QTable[i].Action[j] = FCString::Atod(*temp);
			counter++;
		}
	}

	//Select next action a', based on s' and Epsilon Greedy

	nextAction = 0;

	//Generate Random Number for epsilon-greedy policy
	float randomNumber = float(FMath::RandRange(0, 100)) / 100.0;

	if (randomNumber > myExplorationRate)
	{
		//select the best action
		for (int j = 0; j < NumOfActions; j++) //loops through all actions
		{
			//For all the QVals, go through to find the best (highest) value
			if (QTable[nextState].Action[j] > QTable[nextState].Action[nextAction])//
			{
				nextAction = j;
			}
		}

	}
	else
	{
		//do exploration. Select a random action.   
		nextAction = FMath::RandRange(0, NumOfActions - 1);
	}



	//Calculate the Q(s',a') and Q(s,a)
	float QSA = QTable[curStateTabular].Action[curAction];
	float QNextSNextA = QTable[nextState].Action[nextAction];


	// delta = reward + discountFactor (Q(s',a')) - Q(s,a)
	float delta = reward + myDiscountFactor * QNextSNextA - QSA;


	//update the weights
	// Q = Q + learningRate( reward + discountFactor (Q(s',a')) - Q(s,a) )
	QTable[curStateTabular].Action[curAction] += myLearningRate * delta;

	if (QTable[curStateTabular].Action[curAction] > weightLimit) QTable[curStateTabular].Action[curAction] = weightLimit;
	if (QTable[curStateTabular].Action[curAction] < -1 * weightLimit) QTable[curStateTabular].Action[curAction] = -1 * weightLimit;


	curStateTabular = nextState;
	curAction = nextAction;
	//decay
	myExplorationRate -= myExplorationRateDecay;
	myLearningRate -= myLearningRateDecay;

	// log files
	writeToFiles(reward);

	/*for (int s = QTable.Num() - 1; s >= 0; s--) {
		Debug(FString::Printf(TEXT("state %i: %f %f %f"), s, QTable[s].Action[0], QTable[s].Action[1], QTable[s].Action[2]));
	}*/

}

void URLComponent::InitializeTabularSARSA(int State) {


	curAction = FMath::RandRange(0, NumOfActions - 1);
	curStateTabular = State;

	if (linearFunctionApprox == true)
	{
		Debug(FString::Printf(TEXT("Error: Linear function approx specified in tabular SARSA.")));
		return;
	}
	else
	{
		algorithm = AlgorithmType::SARSATabular;

		for (int i = 0; i < NumOfStates; i++)
		{
			FNestedArray Array;
			for (int j = 0; j < NumOfActions; j++)
			{
				Array.Action.Add((float)FMath::RandRange(0, 10) / 10.0);
			}
			QTable.Add(Array);
		}
	}

	CreateLogFile();
}




void URLComponent::setLearningRate(float LearningRate) {
	this->myLearningRate = LearningRate;
}

void URLComponent::setExplorationRate(float EplorationRate) {
	this->myExplorationRate = EplorationRate;
}

void URLComponent::setDiscountFactor(float DiscountFactor) {
	this->myDiscountFactor = DiscountFactor;
}


void URLComponent::setEligibilityDecay(float Lambda) {
	this->myEligibilityDecay = Lambda;
}

void URLComponent::setLearningRateDecay(float Decay) {
	this->myLearningRateDecay = Decay;
}

void URLComponent::setExplorationRateDecay(float Decay) {
	this->myExplorationRateDecay = Decay;
}


void URLComponent::setNumberOfActions(int NumberOfActions) {
	this->NumOfActions = NumberOfActions;
}

void URLComponent::setLinearFunctionApprox(int NumberOfFeatures) {
	this->NumOfFeatures = NumberOfFeatures + 1;
	linearFunctionApprox = true;
}

void URLComponent::setNumberOfStates(int NumberOfStates) {
	this->NumOfStates = NumberOfStates;
	linearFunctionApprox = false;
}
