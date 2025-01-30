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

const FString Primer = TEXT(
	"You are a Minesweeper game generator. The user specifies the number of rows, columns, and mines. "
	"If any of these parameters are missing, respond with a JSON object: {\"error\": \"Missing parameters\"}. "
	"If all parameters are provided, generate a Minesweeper-style grid with EXACTLY the specified number of mines ('X'). "
	"Do not add extra mines or remove any. The grid should be a 2D array where each cell contains either 'X' for mines or "
	"the correct number of adjacent mines. The response should be **pure JSON**, without Markdown, without code block formatting, "
	"and without any explanations or extra text. The JSON format should be as follows: "
	"{\"grid\": [[\"1\", \"X\", \"1\"], [\"2\", \"2\", \"1\"], [\"X\", \"1\", \"0\"]], \"mines\": 2}. "
	"Ensure the \"mines\" field **exactly matches** the number of mines in the generated grid."
);

void UMinesweeperEditorSubsystem::SendMessageToLLM(const FString& InputMessage)
{
	CurrentInputMessage = InputMessage;
	FString APIKey = APIKeyChunk1 + APIKeyChunk2;
	const FString OpenAIEndpoint = TEXT("https://api.openai.com/v1/chat/completions");
	
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UMinesweeperEditorSubsystem::OnLLMResponseReceived);
	HttpRequest->SetURL(OpenAIEndpoint);
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *APIKey));
	
	TSharedPtr<FJsonObject> RequestBody = MakeShareable(new FJsonObject);
	RequestBody->SetStringField(TEXT("model"), TEXT("gpt-4o"));
	
	TArray<TSharedPtr<FJsonValue>> Messages;
	TSharedPtr<FJsonObject> SystemMessage = MakeShareable(new FJsonObject());
	SystemMessage->SetStringField(TEXT("role"), TEXT("system"));
	SystemMessage->SetStringField(TEXT("content"), Primer);
	Messages.Add(MakeShareable(new FJsonValueObject(SystemMessage)));
	
	TSharedPtr<FJsonObject> Message = MakeShareable(new FJsonObject);
	Message->SetStringField(TEXT("role"), TEXT("user"));
	FString NewMessage = InputMessage;
	Message->SetStringField(TEXT("content"), NewMessage);
	Messages.Add(MakeShareable(new FJsonValueObject(Message)));
	
	RequestBody->SetArrayField(TEXT("messages"), Messages);

	RequestBody->SetNumberField(TEXT("temperature"), 1);
	
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
			if (NestedJsonObject->HasField(TEXT("error")))
			{
				UE_LOG(LogTemp, Warning, TEXT("Error in response: %s"), *NestedJsonObject->GetStringField(TEXT("error")));
				return;
			}

			TArray<TArray<FString>> Grid;
			const TArray<TSharedPtr<FJsonValue>> GridArray = NestedJsonObject->GetArrayField(TEXT("grid"));

			for (const TSharedPtr<FJsonValue>& RowValue : GridArray)
			{
				TArray<FString> Row;
				const TArray<TSharedPtr<FJsonValue>> RowArray = RowValue->AsArray();
				
				for (const TSharedPtr<FJsonValue>& CellValue : RowArray)
				{
					Row.Add(CellValue->AsString());
				}

				Grid.Add(Row);
			}

			for (int32 i = 0; i < Grid.Num(); ++i)
			{
				FString RowString = FString::Join(Grid[i], TEXT(" "));
				UE_LOG(LogTemp, Log, TEXT("%s"), *RowString);
			}

			int32 NumMines = NestedJsonObject->GetNumberField(TEXT("mines"));

			if(HasExpectedMines(Grid, NumMines))
			{
				if (OnBoardGenerated.IsBound())
				{
					UE_LOG(LogTemp, Warning, TEXT("Attempts needed %d"), CurrentTryCount);
					CurrentTryCount = 0;
					OnBoardGenerated.Execute(Grid);
				}
			}
			else
			{
				if(CurrentTryCount < 20)
				{
					++CurrentTryCount;
					SendMessageToLLM(CurrentInputMessage);
				}
				else
				{
					CurrentTryCount = 0;
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

bool UMinesweeperEditorSubsystem::HasExpectedMines(const TArray<TArray<FString>>& Grid, int32 ExpectedMines)
{
	int32 MineCount = 0;
	for (const TArray<FString>& Row : Grid)
	{
		for (const FString& Cell : Row)
		{
			if (Cell == "X") 
			{
				MineCount++;
			}
		}
	}
	return MineCount == ExpectedMines; 
}

void UMinesweeperEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}
