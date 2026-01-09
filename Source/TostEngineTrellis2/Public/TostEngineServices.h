#pragma once

#include "CoreMinimal.h"
#include "TostEngineServices.generated.h"

UENUM(BlueprintType)
enum class EAIParameterType : uint8
{
	String,
	Number,
	Boolean,
	File,
	Select,
	Slider
};

UENUM(BlueprintType)
enum class EAIServiceCategory : uint8
{
	Image,
	Video,
	ThreeD,
	Audio,
	Conversion,
	LLM
};

USTRUCT(BlueprintType)
struct FTostEngineParameter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TostEngine")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TostEngine")
	EAIParameterType Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TostEngine")
	FString Label;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TostEngine")
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TostEngine")
	bool bRequired = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TostEngine")
	FString DefaultValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TostEngine")
	TArray<FString> Options;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TostEngine")
	float Min = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TostEngine")
	float Max = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TostEngine")
	float Step = 0.1f;
};

USTRUCT(BlueprintType)
struct FTostEngineService
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TostEngine")
	FString Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TostEngine")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TostEngine")
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TostEngine")
	EAIServiceCategory Category;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TostEngine")
	FString Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TostEngine")
	TArray<FString> InputTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TostEngine")
	TArray<FTostEngineParameter> Parameters;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TostEngine")
	FString SampleRequest;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TostEngine")
	FString SampleResponse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TostEngine")
	FString Overview;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TostEngine")
	FString Documentation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TostEngine")
	FString CodeRepository;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TostEngine")
	FString ProjectPage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TostEngine")
	FString ResearchPaper;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TostEngine")
	FString WorkerId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TostEngine")
	TArray<FString> Examples;
};

UCLASS()
class TOSTENGINETRELLIS2_API UTostEngineServices : public UObject
{
	GENERATED_BODY()

public:
	static TArray<FTostEngineService> GetAIServices();
	static FString GetCategoryIcon(EAIServiceCategory Category);
	static FString GetCategoryColor(EAIServiceCategory Category);
	static FString GenerateJobId();
};
