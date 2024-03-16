// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RSType.h"
#include "UObject/Object.h"
#include "RSObjectPool.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTRS_API URSInstancedObjectPool : public UObject
{
	GENERATED_BODY()

public:
	template <typename T>
	void Initialize(int32 InInitSize, int32 InMaxSize, int32 InGrowSize)
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
	
	void Deinitialize()
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
				UE_LOG(LogRS, Display, TEXT("%s Pool Grow: %d"), *T::StaticClass()->GetName(), CurGrowSize);
			}
		}
		
		if (PoolArray.Num() > 0)
		{
			++LiveObjectCount;
			UE_LOG(LogRS, Verbose, TEXT("%s Pool Live Object Count: %d"), *T::StaticClass()->GetName(), LiveObjectCount);
			
			if (T* Object = Cast<T>(PoolArray.Pop()))
			{
				ensure(Object->GetClass() == T::StaticClass());
				return Object;
			}
			else
			{
				ensure(false);
			}
		}

		UE_LOG(LogRS, Warning, TEXT("%s Pool Exceed: Max(%d)"), *T::StaticClass()->GetName(), MaxSize);
		return NewObject<T>();
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

		if (PoolArray.Num() < MaxSize)
		{
			--LiveObjectCount;
			UE_LOG(LogRS, Verbose, TEXT("%s Pool Live Object Count: %d"), *T::StaticClass()->GetName(), LiveObjectCount);
			PoolArray.Push(Object);
		}
	}
	
private:
	template <typename T>
	void Alloc()
	{
		T* SkillEffect = NewObject<T>();
		PoolArray.Push(SkillEffect);
		++AllocSize;
	}

	UPROPERTY(Transient)
	TArray<TObjectPtr<UObject>> PoolArray;

	int32 InitSize = 0;
	int32 MaxSize = 0;
	int32 GrowSize = 0;
	int32 AllocSize = 0;
	int32 LiveObjectCount = 0;
};

UCLASS()
class PROJECTRS_API URSObjectPool : public UObject
{
	GENERATED_BODY()

public:
	void SetDefaultSize(int32 InitSize, int32 MaxSize, int32 GrowSize)
	{
		DefaultInitSize = InitSize;
		DefaultMaxSize = MaxSize;
		DefaultGrowSize = GrowSize;
	}
	
	template <typename T>
	void Initialize(int32 InInitSize, int32 InMaxSize, int32 InGrowSize)
	{
		UClass* ObjectClass = T::StaticClass();
		if (!ObjectClass)
		{
			ensure(false);
			return;
		}
		
		if (!InstancedObjectPoolMap.Contains(ObjectClass))
		{
			InstancedObjectPoolMap.Add(ObjectClass, NewObject<URSInstancedObjectPool>());
		}
		
		InstancedObjectPoolMap[ObjectClass]->Initialize<T>(InInitSize, InMaxSize, InGrowSize);
	}
	
	template <typename T>
	void Deinitialize()
	{
		const UClass* ObjectClass = T::StaticClass();
		if (!ObjectClass)
		{
			ensure(false);
			return;
		}
		
		if (!InstancedObjectPoolMap.Contains(ObjectClass))
		{
			ensure(false);
			return;
		}
		
		InstancedObjectPoolMap[ObjectClass]->Deinitialize();
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
		
		if (!InstancedObjectPoolMap.Contains(ObjectClass))
		{
			Initialize<T>(DefaultInitSize, DefaultMaxSize, DefaultGrowSize);
		}
		
		return InstancedObjectPoolMap[ObjectClass]->Pop<T>();
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
		
		if (!InstancedObjectPoolMap.Contains(ObjectClass))
		{
			ensure(false);
			return;
		}
		
		InstancedObjectPoolMap[ObjectClass]->Push<T>(Object);
	}
	
private:
	UPROPERTY(Transient)
	TMap<UClass*, TObjectPtr<URSInstancedObjectPool>> InstancedObjectPoolMap;

	int32 DefaultInitSize = 8;
	int32 DefaultMaxSize = 32;
	int32 DefaultGrowSize = 4;
};