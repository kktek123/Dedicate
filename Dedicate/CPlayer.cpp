#include "CPlayer.h"
#include "Global.h"
#include "CPlayerState.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/DamageType.h"
#include "Net/UnrealNetwork.h"
#include "Materials/MaterialInstanceDynamic.h"

DEFINE_LOG_CATEGORY_STATIC(LogFps, Warning, All);

ACPlayer::ACPlayer()
{
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;


	GetMesh()->SetOwnerNoSee(true);

	FP_Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	FP_Mesh->SetOnlyOwnerSee(true);
	FP_Mesh->SetupAttachment(FirstPersonCameraComponent);
	FP_Mesh->bCastDynamicShadow = false;
	FP_Mesh->CastShadow = false;
	FP_Mesh->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	FP_Mesh->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(L"FP_Gun");
	FP_Gun->SetOnlyOwnerSee(true);
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	FP_Gun->SetupAttachment(FP_Mesh, L"GripPoint");
	//FP_Gun->SetupAttachment(RootComponent);

	TP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(L"TP_Gun");
	TP_Gun->SetOwnerNoSee(true);
	TP_Gun->SetupAttachment(GetMesh(), L"hand_rSocket");


	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	GunOffset = FVector(100.0f, 0.0f, 10.0f);


	FP_GunShotParticle = CreateDefaultSubobject<UParticleSystemComponent>(L"FP_GunShotParticle");
	FP_GunShotParticle->bAutoActivate = false;
	FP_GunShotParticle->SetupAttachment(FP_Gun);
	FP_GunShotParticle->SetOnlyOwnerSee(true);

	TP_GunShotParticle = CreateDefaultSubobject<UParticleSystemComponent>(L"TP_GunShotParticle");
	TP_GunShotParticle->bAutoActivate = false;
	TP_GunShotParticle->SetupAttachment(TP_Gun);
	TP_GunShotParticle->SetOwnerNoSee(true);

	FP_BulletParticle = CreateDefaultSubobject<UParticleSystemComponent>(L"FP_BulletParticle");
	FP_BulletParticle->bAutoActivate = false;
	FP_BulletParticle->SetupAttachment(FirstPersonCameraComponent);
}

void ACPlayer::BeginPlay()
{
	Super::BeginPlay();

	//FP_Gun->AttachToComponent(FP_Mesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), L"GripPoint");
	FP_Gun->SetupAttachment(FP_Mesh, L"GripPoint");
	FP_Mesh->SetHiddenInGame(false, true);

	if (Role != ROLE_Authority)
		SetTeam(CurrentTeam);
}

void ACPlayer::PossessedBy(AController * NewController)
{
	Super::PossessedBy(NewController);

	SelfPlayerState = Cast<ACPlayerState>(GetPlayerState());
	if (Role == ROLE_Authority && SelfPlayerState != NULL)
		SelfPlayerState->Health = 100.0f;
}

//////////////////////////////////////////////////////////////////////////
// Input

void ACPlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ACPlayer::OnFire);

	PlayerInputComponent->BindAxis("MoveForward", this, &ACPlayer::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACPlayer::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ACPlayer::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ACPlayer::LookUpAtRate);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
}

float ACPlayer::TakeDamage(float Damage, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	if (Role == ROLE_Authority && DamageCauser != this && SelfPlayerState->Health > 0)
	{
		SelfPlayerState->Health -= Damage;
		PlayDamage();

		if (SelfPlayerState->Health <= 0) //»ç¸Á
		{
			SelfPlayerState->Death++;

			MultiCastDeath();
			ACPlayer* other = Cast<ACPlayer>(DamageCauser);
			if (other != NULL)
				other->SelfPlayerState->Score += 1.0f;

			FTimerHandle handle;
			GetWorldTimerManager().SetTimer<ACPlayer>(handle, this, &ACPlayer::Respawn, 3.0f, false);
		}
	}

	return Damage;
}

ACPlayerState * ACPlayer::GetSelfPlayerState()
{
	if (SelfPlayerState == NULL)
		SelfPlayerState = Cast<ACPlayerState>(GetPlayerState());

	return SelfPlayerState;
}

void ACPlayer::SetSelfPlayerState(ACPlayerState* NewState)
{
	if (Role == ROLE_Authority && NewState != NULL)
	{
		SelfPlayerState = NewState;
		SetPlayerState(NewState);
	}
}

void ACPlayer::Respawn()
{
	if (Role == ROLE_Authority)
	{
		SelfPlayerState->Health = 100.0f;
		//Cast<ACGameMode>(GetWorld()->GetAuthGameMode())->respaw

		Destroy(true, true);
	}
}

void ACPlayer::OnFire()
{
	if (FP_FireAnimation != NULL)
	{
		UAnimInstance* AnimInstance = FP_Mesh->GetAnimInstance();
		if (AnimInstance != NULL)
			AnimInstance->Montage_Play(FP_FireAnimation, 1.f);
	}

	if (FP_GunShotParticle != NULL)
		FP_GunShotParticle->Activate(true);


	FVector mousePosition;
	FVector mouseDirection;

	APlayerController* controller = Cast<APlayerController>(GetController());

	int x, y;
	controller->GetViewportSize(x, y);

	controller->DeprojectScreenPositionToWorld(x * 0.5f, y * 0.5f, mousePosition, mouseDirection);
	mouseDirection *= 1e+6f;

	OnServerFire(mousePosition, mouseDirection);
}

void ACPlayer::MoveForward(float Value)
{
	if (Value != 0.0f)
		AddMovementInput(GetActorForwardVector(), Value);
}

void ACPlayer::MoveRight(float Value)
{
	if (Value != 0.0f)
		AddMovementInput(GetActorRightVector(), Value);
}

void ACPlayer::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ACPlayer::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ACPlayer::Fire(const FVector pos, const FVector dir)
{
	FCollisionObjectQueryParams params;
	params.AddObjectTypesToQuery(ECC_GameTraceChannel1);

	FCollisionQueryParams params2;
	params2.AddIgnoredActor(this);


	FHitResult result;
	GetWorld()->LineTraceSingleByObjectType(result, pos, dir, params, params2);
	DrawDebugLine(GetWorld(), pos, dir, FColor::Red, false, 3, 0, 0.5f);


	if (result.bBlockingHit)
	{
		ACPlayer* other = Cast<ACPlayer>(result.GetActor());
		if (other != NULL && other->GetSelfPlayerState()->Team != GetSelfPlayerState()->Team)
		{
			FDamageEvent thisEvent(UDamageType::StaticClass());
			other->TakeDamage(Damage, thisEvent, this->GetController(), this);

			APlayerController* controller = Cast<APlayerController>(GetController());

			FForceFeedbackParameters params;
			params.bLooping = false;
			controller->ClientPlayForceFeedback(HitSuccessFeedback);
		}
	}
}

void ACPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACPlayer, CurrentTeam);
}

bool ACPlayer::OnServerFire_Validate(const FVector pos, const FVector dir)
{
	if (pos != FVector(ForceInit) && dir != FVector(ForceInit))
		return true;

	return false;
}

void ACPlayer::OnServerFire_Implementation(const FVector pos, const FVector dir)
{
	Fire(pos, dir);
	MultiCastShootEffects();
}

void ACPlayer::MultiCastShootEffects_Implementation()
{
	if (TP_FireAnimation != NULL)
	{
		UAnimInstance* anim = GetMesh()->GetAnimInstance();
		if (anim != NULL)
			anim->Montage_Play(TP_FireAnimation, 1.1f);
	}

	if (FireSound != NULL)
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());

	if (TP_GunShotParticle != NULL)
		TP_GunShotParticle->Activate(true);

	if (FP_BulletParticle != NULL)
	{
		UGameplayStatics::SpawnEmitterAtLocation
		(
			GetWorld(), FP_BulletParticle->Template, FP_BulletParticle->GetComponentLocation(),
			FP_BulletParticle->GetComponentRotation()
		);
	}
}

void ACPlayer::MultiCastDeath_Implementation()
{
	GetMesh()->SetPhysicsBlendWeight(1.0f);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName("Ragdoll");
}

void ACPlayer::PlayDamage_Implementation()
{
	if (Role == ROLE_AutonomousProxy)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PainSound, GetActorLocation());
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(TP_HitAnimation, 1.f);
	}
}

void ACPlayer::SetTeam_Implementation(ETeam NewTeam)
{
	FLinearColor color;

	if (NewTeam == ETeam::Blue)
		color = FLinearColor(0, 0, 0.5f);
	else
		color = FLinearColor(0.5f, 0, 0.0f);

	if (DynamicMaterial == NULL)
	{
		DynamicMaterial = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(0), this);
		DynamicMaterial->SetVectorParameterValue("BodyColor", color);
		GetMesh()->SetMaterial(0, DynamicMaterial);
		FP_Mesh->SetMaterial(0, DynamicMaterial);
	}
}