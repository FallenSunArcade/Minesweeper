// Fill out your copyright notice in the Description page of Project Settings.
#include "MinesweeperEditorSubsystem.h"
#include "HttpModule.h"
#include "ImageUtils.h"
#include "Interfaces/IHttpRequest.h"
#include "Styling/SlateStyleRegistry.h"


const FString APIKeyChunk1 =
		TEXT("sk-proj-5DCsATJjZJIEJ7LtwYf9sRsUr7keBUxWPxv0BAkq3xXp56FScBbp_i1hl");

const FString APIKeyChunk2 =
		TEXT("alDPt89VV0Ru1QZ2fT3BlbkFJhfZH8GY7-zJRrvKdWwzzm49oCGPXKlfXZSs9_RB53ozZlBf-waFkE7OkhWdA7nAmrtBmZACsoA");

const FString Primer =
	TEXT("The user will specify the number of rows, columns, and mines. If any of these parameters are missing, send an error message formatted as JSON with key-value pairs. "
		 "If all parameters are provided, respond to the following user message with a JSON object containing the keys 'Rows', 'Cols', and 'NumMines'. "
		 "The values should be the specified number of rows, columns, and mines. If any of the parameters are missing, the JSON response should look like this: "
		 "{\"error\": \"Missing parameters\"}. Otherwise, respond with a JSON object like this: "
		 "{\"Rows\": <value>, \"Cols\": <value>, \"NumMines\": <value>}");

void UMinesweeperEditorSubsystem::SendMessageToLLM(const FString& InputMessage)
{
	FString APIKey = APIKeyChunk1 + APIKeyChunk2;
	const FString OpenAIEndpoint = TEXT("https://api.openai.com/v1/chat/completions");
	
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UMinesweeperEditorSubsystem::OnLLMResponseReceived);
	HttpRequest->SetURL(OpenAIEndpoint);
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *APIKey));
	
	TSharedPtr<FJsonObject> RequestBody = MakeShareable(new FJsonObject);
	RequestBody->SetStringField(TEXT("model"), TEXT("gpt-3.5-turbo"));
	
	TArray<TSharedPtr<FJsonValue>> Messages;
	TSharedPtr<FJsonObject> Message = MakeShareable(new FJsonObject);
	Message->SetStringField(TEXT("role"), TEXT("user"));
	FString NewMessage = Primer + InputMessage;
	Message->SetStringField(TEXT("content"), NewMessage);
	Messages.Add(MakeShareable(new FJsonValueObject(Message)));
	
	RequestBody->SetArrayField(TEXT("messages"), Messages);
	
	FString RequestBodyString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBodyString);
	FJsonSerializer::Serialize(RequestBody.ToSharedRef(), Writer);

	HttpRequest->SetContentAsString(RequestBodyString);
	
	HttpRequest->ProcessRequest();
}

void UMinesweeperEditorSubsystem::OnLLMResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!Response.IsValid()) return;
	
	FString JsonResponse = Response->GetContentAsString();
	UE_LOG(LogTemp, Log, TEXT("Response = %s"), *JsonResponse);
	
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(JsonResponse);
    
	if (!FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to parse top-level JSON."));
		return;
	}
	
	if (JsonObject->HasField(TEXT("choices")))
	{
		const TSharedPtr<FJsonObject> MessageObject = JsonObject->GetArrayField(TEXT("choices"))[0]->AsObject()->GetObjectField(TEXT("message"));
		FString ContentString = MessageObject->GetStringField(TEXT("content"));
		
		TSharedPtr<FJsonObject> NestedJsonObject;
		TSharedRef<TJsonReader<TCHAR>> NestedReader = TJsonReaderFactory<TCHAR>::Create(ContentString);
        
		if (FJsonSerializer::Deserialize(NestedReader, NestedJsonObject))
		{
			if(!NestedJsonObject->HasField(TEXT("error")))
			{
				const int32 Rows = NestedJsonObject->GetIntegerField(TEXT("Rows"));
				const int32 Cols = NestedJsonObject->GetIntegerField(TEXT("Cols"));
				const int32 NumMines = NestedJsonObject->GetIntegerField(TEXT("NumMines"));
				
				if(OnGenerateBoard.ExecuteIfBound(Rows, Cols, NumMines))
				{
					UE_LOG(LogTemp, Log, TEXT("Rows: %d, Cols: %d, NumMines: %d"), Rows, Cols, NumMines);
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to parse nested JSON in content."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No choices field in JSON response."));
	}
}

void UMinesweeperEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}
