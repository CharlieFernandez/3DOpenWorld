// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Item.h"

#include "NiagaraComponent.h"
#include "Characters/OpenWorldCharacter.h"
#include "Components/SphereComponent.h"

// Sets default values
AItem::AItem(): Amplitude(0.25f), TimeConstant(5.f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	RootComponent = ItemMesh;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Comp"));
	SphereComponent -> SetupAttachment(GetRootComponent());

	PickUpParticles = CreateDefaultSubobject<UNiagaraComponent>("Pick-Up Particles");
	PickUpParticles->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereBeginOverlap);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);
}

void AItem::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(AOpenWorldCharacter* OpenWorldCharacter = Cast<AOpenWorldCharacter>(OtherActor))
	{
		OpenWorldCharacter->SetOverlappingItem(this);
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(AOpenWorldCharacter* OpenWorldCharacter = Cast<AOpenWorldCharacter>(OtherActor))
	{
		OpenWorldCharacter -> SetOverlappingItem(nullptr);
	}
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RunningTime += DeltaTime;

	if(ItemState == EItemState::EIS_Hovering)
	{
		AddActorWorldOffset(FVector(0, 0, TransformSine()));
		AddActorLocalRotation(FRotator(0, SpinSpeed * DeltaTime, 0));
	}
}


