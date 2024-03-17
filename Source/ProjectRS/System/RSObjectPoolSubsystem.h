// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "RSType.h"
#include "RSObjectPoolSubsystem.generated.h"

UCLASS()
class PROJECTRS_API URSAllocatedObjectPool : public UObject
{
	GENERATED_BODY()

public:
	template <typename T>
	void Init(int32 InInitSize, int32 InMaxSize, int32 InGrowSize)
	{
		ensure(InInitSize <= InMaxSize);
		ensure(InInitSize > 0);
		ensure(InMaxSize > 0);
		ensure(InGrowSize > 0);
		
		InitSize = FMath::Max(1, InInitSize);
		MaxSize = FMath::Max(1, InMaxSize);
		GrowSize = FMath::Max(1, InGrowSize);
		AllocSize = 0;
		LiveObjectCount = 0;
		
		PoolArray.Reset();
		for (int32 Index = 0; Index < InitSize; ++Index)
		{
			Alloc<T>();
		}
		
		UE_LOG(LogRS, Display, TEXT("%s Pool is Initialized(InitSize: %d, MaxSize: %d, GrowSize: %d)"),
			*T::StaticClass()->GetName(), InitSize, MaxSize, GrowSize);
	}
	
	void Reset()
	{
		InitSize = 0;
		MaxSize = 0;
		GrowSize = 0;
		AllocSize = 0;
		LiveObjectCount = 0;
		
		PoolArray.Reset();
	}

	template <typename T>
	T* Pop()
	{
		if (PoolArray.Num() == 0)
		{
			if (AllocSize < MaxSize)
			{
				const int32 RemainSize = MaxSize - AllocSize;
				const int32 CurGrowSize = FMath::Min(GrowSize, RemainSize);
				for (int32 Index = 0; Index < CurGrowSize; ++Index)
				{
					Alloc<T>();
				}
				UE_LOG(LogRS, Display, TEXT("%s Pool Grow: %d, AllocSize: %d"), *T::StaticClass()->GetName(), CurGrowSize, AllocSize);
			}
		}
		
		T* Object;
		if (PoolArray.Num() > 0)
		{
			Object = Cast<T>(PoolArray.Pop());
		}
		else
		{
			UE_LOG(LogRS, Warning, TEXT("%s Pool Exceed: Max(%d)"), *T::StaticClass()->GetName(), MaxSize);
			Object = NewObject<T>();
		}

		if (!Object)
		{
			ensure(false);
			return nullptr;
		}

		
		ensure(Object->GetClass() == T::StaticClass());
		
		++LiveObjectCount;
		UE_LOG(LogRS, Verbose, TEXT("%s Pool Live Object Count: %d"), *T::StaticClass()->GetName(), LiveObjectCount);
		
		return Object;
	}

	template <typename T>
	void Push(UObject* Object)
	{
		if (!Object)
		{
			ensure(false);
			return;
		}
		
		ensure(Object->GetClass() == T::StaticClass());

		--LiveObjectCount;
		UE_LOG(LogRS, Verbose, TEXT("%s Pool Live Object Count: %d"), *T::StaticClass()->GetName(), LiveObjectCount);
			
		if (LiveObjectCount < MaxSize)
		{
			PoolArray.Push(Object);
		}
		else
		{
			Object->MarkAsGarbage();
		}
	}
	
private:
	template <typename T>
	void Alloc()
	{
		if (UObject* Object = NewObject<T>())
		{
			PoolArray.Push(Object);
			++AllocSize;
		}
		else
		{
			ensure(false);
		}
	}

	UPROPERTY(Transient)
	TArray<TObjectPtr<UObject>> PoolArray;

	int32 InitSize = 0;
	int32 MaxSize = 0;
	int32 GrowSize = 0;
	int32 AllocSize = 0;
	int32 LiveObjectCount = 0;
};

/**
 * 
 */
UCLASS()
class PROJECTRS_API URSObjectPoolSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	static URSObjectPoolSubsystem& Get();
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	bool IsInitialized() const { return bInitialized; }

	void SetDefaultSize(int32 InitSize, int32 MaxSize, int32 GrowSize);

	template <typename T>
	void InitPool(int32 InInitSize, int32 InMaxSize, int32 InGrowSize)
	{
		UClass* ObjectClass = T::StaticClass();
		if (!ObjectClass)
		{
			ensure(false);
			return;
		}
		
		if (!AllocatedObjectPoolMap.Contains(ObjectClass))
		{
			AllocatedObjectPoolMap.Add(ObjectClass, NewObject<URSAllocatedObjectPool>());
		}
		
		AllocatedObjectPoolMap[ObjectClass]->Init<T>(InInitSize, InMaxSize, InGrowSize);
	}
	
	template <typename T>
	void ResetPool()
	{
		const UClass* ObjectClass = T::StaticClass();
		if (!ObjectClass)
		{
			ensure(false);
			return;
		}
		
		if (!AllocatedObjectPoolMap.Contains(ObjectClass))
		{
			ensure(false);
			return;
		}
		
		AllocatedObjectPoolMap[ObjectClass]->Reset();
	}

	template <typename T>
	T* Pop()
	{
		const UClass* ObjectClass = T::StaticClass();
		if (!ObjectClass)
		{
			ensure(false);
			return nullptr;
		}
		
		if (!AllocatedObjectPoolMap.Contains(ObjectClass))
		{
			InitPool<T>(DefaultInitSize, DefaultMaxSize, DefaultGrowSize);
		}
		
		return AllocatedObjectPoolMap[ObjectClass]->Pop<T>();
	}

	template <typename T>
	void Push(UObject* Object)
	{
		const UClass* ObjectClass = T::StaticClass();
		if (!ObjectClass)
		{
			ensure(false);
			return;
		}
		
		if (!AllocatedObjectPoolMap.Contains(ObjectClass))
		{
			ensure(false);
			return;
		}
		
		AllocatedObjectPoolMap[ObjectClass]->Push<T>(Object);
	}
	
private:
	UPROPERTY(Transient)
	TMap<UClass*, TObjectPtr<URSAllocatedObjectPool>> AllocatedObjectPoolMap;

	int32 DefaultInitSize = 8;
	int32 DefaultMaxSize = 32;
	int32 DefaultGrowSize = 4;
	bool bInitialized = false;
};
