#include "TostEngineServices.h"

TArray<FTostEngineService> UTostEngineServices::GetAIServices()
{
	TArray<FTostEngineService> Services;

	// TRELLIS 2
	{
		FTostEngineService Service;
		Service.Id = "trellis-2";
		Service.Name = "TRELLIS v2";
		Service.Description = "TRELLIS 2: Native and Compact Structured Latents for 3D Generation";
		Service.Category = EAIServiceCategory::ThreeD;
		Service.Icon = "Box";
		Service.InputTypes = { "image" };
		Service.WorkerId = "xm1w5e194g5nlw";
		Service.Overview = "TRELLIS 2: Native and Compact Structured Latents for 3D Generation";
		Service.Documentation = "TRELLIS 2";
		Service.CodeRepository = "https://github.com/microsoft/TRELLIS.2";
		Service.ProjectPage = "https://microsoft.github.io/TRELLIS.2/";
		Service.ResearchPaper = "https://arxiv.org/abs/2512.14692";
		Service.Examples = { "https://examples.tost.ai/f43f435tgrfsd.jpg", "https://examples.tost.ai/ewr43543rwe3453.glb" };

		FTostEngineParameter Param1;
		Param1.Name = "input_image";
		Param1.Type = EAIParameterType::File;
		Param1.Label = "Input Image";
		Param1.bRequired = true;
		Service.Parameters.Add(Param1);

		FTostEngineParameter Param2;
		Param2.Name = "seed";
		Param2.Type = EAIParameterType::Number;
		Param2.Label = "Seed";
		Param2.DefaultValue = "0";
		Service.Parameters.Add(Param2);

		FTostEngineParameter Param3;
		Param3.Name = "resolution";
		Param3.Type = EAIParameterType::Select;
		Param3.Label = "Resolution";
		Param3.DefaultValue = "1024";
		Param3.Options = { "512", "1024", "1536" };
		Service.Parameters.Add(Param3);

		FTostEngineParameter Param4;
		Param4.Name = "remove_bg";
		Param4.Type = EAIParameterType::Boolean;
		Param4.Label = "Remove Background";
		Param4.DefaultValue = "true";
		Service.Parameters.Add(Param4);

		FTostEngineParameter Param5;
		Param5.Name = "ss_guidance_strength";
		Param5.Type = EAIParameterType::Slider;
		Param5.Label = "Guidance Strength";
		Param5.DefaultValue = "7.5";
		Param5.Min = 1.0f;
		Param5.Max = 10.0f;
		Param5.Step = 0.1f;
		Service.Parameters.Add(Param5);

		FTostEngineParameter Param6;
		Param6.Name = "ss_guidance_rescale";
		Param6.Type = EAIParameterType::Slider;
		Param6.Label = "Guidance Rescale";
		Param6.DefaultValue = "0.7";
		Param6.Min = 0.0f;
		Param6.Max = 1.0f;
		Param6.Step = 0.01f;
		Service.Parameters.Add(Param6);

		FTostEngineParameter Param7;
		Param7.Name = "ss_sampling_steps";
		Param7.Type = EAIParameterType::Slider;
		Param7.Label = "Sampling Steps";
		Param7.DefaultValue = "12";
		Param7.Min = 1.0f;
		Param7.Max = 50.0f;
		Param7.Step = 1.0f;
		Service.Parameters.Add(Param7);

		FTostEngineParameter Param8;
		Param8.Name = "ss_rescale_t";
		Param8.Type = EAIParameterType::Slider;
		Param8.Label = "Rescale";
		Param8.DefaultValue = "5";
		Param8.Min = 1.0f;
		Param8.Max = 6.0f;
		Param8.Step = 0.1f;
		Service.Parameters.Add(Param8);

		FTostEngineParameter Param9;
		Param9.Name = "shape_slat_guidance_strength";
		Param9.Type = EAIParameterType::Slider;
		Param9.Label = "Shape Slat Guidance Strength";
		Param9.DefaultValue = "7.5";
		Param9.Min = 1.0f;
		Param9.Max = 10.0f;
		Param9.Step = 0.1f;
		Service.Parameters.Add(Param9);

		FTostEngineParameter Param10;
		Param10.Name = "shape_slat_guidance_rescale";
		Param10.Type = EAIParameterType::Slider;
		Param10.Label = "Shape Slat Guidance Rescale";
		Param10.DefaultValue = "0.5";
		Param10.Min = 0.0f;
		Param10.Max = 1.0f;
		Param10.Step = 0.01f;
		Service.Parameters.Add(Param10);

		FTostEngineParameter Param11;
		Param11.Name = "shape_slat_sampling_steps";
		Param11.Type = EAIParameterType::Slider;
		Param11.Label = "Shape Slat Sampling Steps";
		Param11.DefaultValue = "12";
		Param11.Min = 1.0f;
		Param11.Max = 50.0f;
		Param11.Step = 1.0f;
		Service.Parameters.Add(Param11);

		FTostEngineParameter Param12;
		Param12.Name = "shape_slat_rescale_t";
		Param12.Type = EAIParameterType::Slider;
		Param12.Label = "Shape Slat Rescale";
		Param12.DefaultValue = "3";
		Param12.Min = 1.0f;
		Param12.Max = 6.0f;
		Param12.Step = 0.1f;
		Service.Parameters.Add(Param12);

		FTostEngineParameter Param13;
		Param13.Name = "tex_slat_guidance_strength";
		Param13.Type = EAIParameterType::Slider;
		Param13.Label = "Tex Slat Guidance Strength";
		Param13.DefaultValue = "1";
		Param13.Min = 1.0f;
		Param13.Max = 10.0f;
		Param13.Step = 0.1f;
		Service.Parameters.Add(Param13);

		FTostEngineParameter Param14;
		Param14.Name = "tex_slat_guidance_rescale";
		Param14.Type = EAIParameterType::Slider;
		Param14.Label = "Tex Slat Guidance Rescale";
		Param14.DefaultValue = "0";
		Param14.Min = 0.0f;
		Param14.Max = 1.0f;
		Param14.Step = 0.01f;
		Service.Parameters.Add(Param14);

		FTostEngineParameter Param15;
		Param15.Name = "tex_slat_sampling_steps";
		Param15.Type = EAIParameterType::Slider;
		Param15.Label = "Tex Slat Sampling Steps";
		Param15.DefaultValue = "12";
		Param15.Min = 1.0f;
		Param15.Max = 50.0f;
		Param15.Step = 1.0f;
		Service.Parameters.Add(Param15);

		FTostEngineParameter Param16;
		Param16.Name = "tex_slat_rescale_t";
		Param16.Type = EAIParameterType::Slider;
		Param16.Label = "Tex Slat Rescale";
		Param16.DefaultValue = "3";
		Param16.Min = 1.0f;
		Param16.Max = 6.0f;
		Param16.Step = 0.1f;
		Service.Parameters.Add(Param16);

		FTostEngineParameter Param17;
		Param17.Name = "job_id";
		Param17.Type = EAIParameterType::String;
		Param17.Label = "Job ID";
		Param17.bRequired = false;
		Param17.DefaultValue = "";
		Service.Parameters.Add(Param17);

		Services.Add(Service);
	}

	return Services;
}

FString UTostEngineServices::GetCategoryIcon(EAIServiceCategory Category)
{
	switch (Category)
	{
	case EAIServiceCategory::Image:
		return "Image";
	case EAIServiceCategory::Video:
		return "Video";
	case EAIServiceCategory::ThreeD:
		return "Cube";
	case EAIServiceCategory::Audio:
		return "AudioWaveform";
	case EAIServiceCategory::Conversion:
		return "RefreshCw";
	case EAIServiceCategory::LLM:
		return "MessageSquare";
	default:
		return "Zap";
	}
}

FString UTostEngineServices::GetCategoryColor(EAIServiceCategory Category)
{
	switch (Category)
	{
	case EAIServiceCategory::Image:
		return "bg-chart-1/10 text-chart-1 border-chart-1/20";
	case EAIServiceCategory::Video:
		return "bg-chart-2/10 text-chart-2 border-chart-2/20";
	case EAIServiceCategory::ThreeD:
		return "bg-chart-3/10 text-chart-3 border-chart-3/20";
	case EAIServiceCategory::Audio:
		return "bg-chart-4/10 text-chart-4 border-chart-4/20";
	case EAIServiceCategory::Conversion:
		return "bg-chart-5/10 text-chart-5 border-chart-5/20";
	case EAIServiceCategory::LLM:
		return "bg-purple-500/10 text-purple-500 border-purple-500/20";
	default:
		return "bg-muted/10 text-muted-foreground border-muted/20";
	}
}

FString UTostEngineServices::GenerateJobId()
{
	FString JobId;
	for (int32 i = 0; i < 32; ++i)
	{
		int32 RandomDigit = FMath::RandRange(0, 15);
		JobId.AppendChar('0' + RandomDigit);
		if (RandomDigit > 9)
		{
			JobId[JobId.Len() - 1] = 'a' + (RandomDigit - 10);
		}
	}
	return JobId;
}
