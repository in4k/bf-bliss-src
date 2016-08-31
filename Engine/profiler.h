#ifdef _DEBUG

class Profiler
{
public:
	static Profiler GlobalProfiler;
	bool EnableProfiling;

	void Initialize(ID3D11Device* device, ID3D11DeviceContext* immContext)
	{
		this->device = device;
		this->context = immContext;
		this->currentFrame = 0;
		this->EnableProfiling = false;
	}

	void StartFrame()
	{
		currentFrame++;
		profileCount = 0;

		if (!EnableProfiling)
			return;

		startFrame = currentFrame;
	}

	int StartProfile(char* name)
	{
		if (!EnableProfiling)
			return -1;

		int index = profileCount++;
		ProfileData& profileData = profiles[index];
		profileData.Name = name;

		// Create the queries
		D3D11_QUERY_DESC desc;
		desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
		desc.MiscFlags = 0;
		device->CreateQuery(&desc, &profileData.DisjointQuery);

		desc.Query = D3D11_QUERY_TIMESTAMP;
		device->CreateQuery(&desc, &profileData.TimestampStartQuery);
		device->CreateQuery(&desc, &profileData.TimestampEndQuery);

		// Start a disjoint query first
		context->Begin(profileData.DisjointQuery);

		// Insert the start timestamp    
		context->End(profileData.TimestampStartQuery);

		profileData.QueryStarted = TRUE;
		profileData.QueryFinished = FALSE;
		return index;
	}

	void EndProfile(int index)
	{
		if (!EnableProfiling)
			return;

		ProfileData& profileData = profiles[index];

		if (profileData.QueryFinished)
			throw;

		// Insert the end timestamp    
		context->End(profileData.TimestampEndQuery);

		// End the disjoint query
		context->End(profileData.DisjointQuery);

		profileData.QueryStarted = FALSE;
		profileData.QueryFinished = TRUE;
	}

	void EndFrame()
	{
		if (currentFrame < startFrame + queryLatency)
			return;

		if (!EnableProfiling)
			return;

		deviceContext->Flush();
		EnableProfiling = false;
		float queryTime = 0.0f;

		// Iterate over all of the profiles
		for (int i = 0; i < profileCount; ++i)
		{
			ProfileData& profile = profiles[i];
			if (profile.QueryFinished == FALSE)
				continue;

			profile.QueryFinished = FALSE;

			// Get the query data
			UINT64 startTime = 0;
			while (context->GetData(profile.TimestampStartQuery, &startTime, sizeof(startTime), 0) != S_OK);

			UINT64 endTime = 0;
			while (context->GetData(profile.TimestampEndQuery, &endTime, sizeof(endTime), 0) != S_OK);

			D3D11_QUERY_DATA_TIMESTAMP_DISJOINT disjointData;
			while (context->GetData(profile.DisjointQuery, &disjointData, sizeof(disjointData), 0) != S_OK);

			float time = 9.0f;
			if (disjointData.Disjoint == FALSE)
			{
				UINT64 delta = endTime - startTime;
				float frequency = static_cast<float>(disjointData.Frequency);
				time = (delta * 1000.0f) / frequency;
			}

			char message[256] = { 0 };
			sprintf_s(message, 256, "%s: %.8f ms\n", profile.Name, time);
			printf(message);
			OutputDebugStringA(message);
		}
	}

protected:
	struct ProfileData
	{
		ID3D11Query* DisjointQuery;
		ID3D11Query* TimestampStartQuery;
		ID3D11Query* TimestampEndQuery;
		BOOL QueryStarted;
		BOOL QueryFinished;
		char* Name;

		ProfileData() : QueryStarted(FALSE), QueryFinished(FALSE) {}
	};

	typedef ProfileData ProfileMap[64];

	ProfileMap profiles;
	int profileCount;
	int startFrame;
	int currentFrame;
	const int queryLatency = 5;

	ID3D11Device* device;
	ID3D11DeviceContext* context;
};

struct ProfileBlock
{
public:

	ProfileBlock(char* name)
	{
		this->index = Profiler::GlobalProfiler.StartProfile(name);
	}

	~ProfileBlock()
	{
		Profiler::GlobalProfiler.EndProfile(this->index);
	}

protected:
	int index;
};

Profiler Profiler::GlobalProfiler;
#endif