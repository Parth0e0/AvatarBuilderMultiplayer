// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Http.h"

#include "LoginMenu.generated.h"

/**
 * 
 */
UCLASS()
class AVATARBUILDER_API ULoginMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	ULoginMenu(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	void OnLoginClicked();

	UPROPERTY(EditAnywhere)
	FString APIGatewayEndpoint;
	UPROPERTY(EditAnywhere)
	FString LoginURI;
	UPROPERTY(EditAnywhere)
	FString StartSessionURI;

	UPROPERTY(BlueprintReadWrite)
	FString UserName;
	UPROPERTY(BlueprintReadWrite)
	FString Password;

private:
	FHttpModule* HttpModule;

	void LoginRequest(FString Usr, FString pwd);
	void StartSessionRequest(FString IDToken);

	void OnLogInResponse(FHttpRequestPtr Request, FHttpResponsePtr Respond, bool bWasSuccessfull);
	void OnStartSessionResponse(FHttpRequestPtr Request, FHttpResponsePtr Respond, bool bWasSuccessfull);
};
