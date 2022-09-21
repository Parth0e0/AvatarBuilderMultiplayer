// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginMenu.h"

#include "Json.h"
#include "JsonUtilities.h"
#include "Kismet/GameplayStatics.h"

ULoginMenu::ULoginMenu(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	HttpModule = &FHttpModule::Get();


	APIGatewayEndpoint = FString("https://0gkcjacqii.execute-api.ap-south-1.amazonaws.com/TestAPIGateWay");

	LoginURI = FString("/login");
	StartSessionURI = FString("/startsession");
}

void ULoginMenu::OnLoginClicked()
{
	LoginRequest(UserName, Password);
}

void ULoginMenu::LoginRequest(FString Usr, FString pwd)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	JsonObject->SetStringField(TEXT("username"), FString::Printf(TEXT("%s"), *Usr));
	JsonObject->SetStringField(TEXT("password"), FString::Printf(TEXT("%s"), *pwd));

	FString JsonBody;
	TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&JsonBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> LoginHttpRequest = HttpModule->CreateRequest();

	LoginHttpRequest->SetVerb("POST");
	LoginHttpRequest->SetURL(APIGatewayEndpoint + LoginURI);
	LoginHttpRequest->SetHeader("Content-Type", "application/json");
	LoginHttpRequest->SetContentAsString(JsonBody);
	LoginHttpRequest->OnProcessRequestComplete().BindUObject(this, &ULoginMenu::OnLogInResponse);
	LoginHttpRequest->ProcessRequest();
}

void ULoginMenu::StartSessionRequest(FString IDToken)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> StartSessionHttpRequest = HttpModule->CreateRequest();

	StartSessionHttpRequest->SetVerb("GET");
	StartSessionHttpRequest->SetURL(APIGatewayEndpoint + StartSessionURI);
	StartSessionHttpRequest->SetHeader("Content-Type", "application/json");
	StartSessionHttpRequest->SetHeader("Authorization", IDToken);
	StartSessionHttpRequest->OnProcessRequestComplete().BindUObject(this, &ULoginMenu::OnStartSessionResponse);
	StartSessionHttpRequest->ProcessRequest();
}

void ULoginMenu::OnLogInResponse(FHttpRequestPtr Request, FHttpResponsePtr Respond, bool bWasSuccessfull)
{
	/*if (bWasSuccessfull)
	{
		UE_LOG(LogTemp, Warning, TEXT("Login Successfull"));
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Respond->GetContentAsString());

		if (FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			FString IDTokenL = JsonObject->GetObjectField("tokens")->GetStringField("IdToken");
			StartSessionRequest(IDTokenL);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Login Failed"));
	}*/

	if (bWasSuccessfull)
	{

		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Respond->GetContentAsString());

		if (FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			FString Status = JsonObject->GetStringField("status");
			UE_LOG(LogTemp, Warning, TEXT("Login Status -> %s"), *Status);

			if (Status == FString("fail"))
			{
				FString Message = JsonObject->GetStringField("msg");
				UE_LOG(LogTemp, Warning, TEXT("Message -> %s"), *Message);
			}
			else
			{
				/*FString IdToken = JsonObject->GetObjectField("tokens")->GetStringField("IdToken");
				StartSessionRequest(IdToken);*/
				UGameplayStatics::OpenLevel(this, FName("MainMenu"));
			}
		}
	}
}

void ULoginMenu::OnStartSessionResponse(FHttpRequestPtr Request, FHttpResponsePtr Respond, bool bWasSuccessfull)
{
	if (bWasSuccessfull)
	{
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Respond->GetContentAsString());

		if (FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			FString IPAddress = JsonObject->GetObjectField("PlayerSession")->GetStringField("IpAddress");
			FString Port = JsonObject->GetObjectField("PlayerSession")->GetStringField("Port");

			FString LevelName = IPAddress + ":" + Port;

			UGameplayStatics::OpenLevel(GetWorld(), FName(*LevelName), false);
		}
	}
}