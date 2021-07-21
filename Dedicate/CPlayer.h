// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CGameMode.h"
#include "CPlayer.generated.h"

class UInputComponent;

UCLASS(config = Game)
class DEDICATE_API ACPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = Camera)
		float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, Category = Camera)
		float BaseLookUpRate;

	UPROPERTY(EditAnywhere, Category = Gameplay)
		float Damage = 20.0f;

	UPROPERTY(EditAnywhere, Category = Gameplay)
		FVector GunOffset;

	UPROPERTY(EditAnywhere, Category = Gameplay)
		class USoundBase* FireSound;

	UPROPERTY(EditAnywhere, Category = Gameplay)
		class USoundBase* PainSound;

	UPROPERTY(EditAnywhere, Category = Gameplay)
		class UAnimMontage* FP_FireAnimation;

	UPROPERTY(EditAnywhere, Category = Gameplay)
		class UAnimMontage* TP_FireAnimation;

	UPROPERTY(EditAnywhere, Category = Gameplay)
		class UAnimMontage* TP_HitAnimation;

	UPROPERTY(VisibleAnywhere, Category = Gameplay)
		class UParticleSystemComponent* FP_GunShotParticle;

	UPROPERTY(VisibleAnywhere, Category = Gameplay)
		class UParticleSystemComponent* TP_GunShotParticle;

	UPROPERTY(VisibleAnywhere, Category = Gameplay)
		class UParticleSystemComponent* FP_BulletParticle;

	UPROPERTY(EditAnywhere, Category = Gameplay)
		class UForceFeedbackEffect* HitSuccessFeedback;

private:
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USkeletalMeshComponent* FP_Mesh;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USkeletalMeshComponent* FP_Gun;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USceneComponent* FP_MuzzleLocation;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USkeletalMeshComponent* TP_Gun;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FirstPersonCameraComponent;

public:
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Team")
		ETeam CurrentTeam;

private:
	UFUNCTION(Server, Reliable, WithValidation)
		void OnServerFire(const FVector pos, const FVector dir);

	bool OnServerFire_Validate(const FVector pos, const FVector dir);
	void OnServerFire_Implementation(const FVector pos, const FVector dir);

	UFUNCTION(NetMulticast, Unreliable)
		void MultiCastShootEffects();

	void MultiCastShootEffects_Implementation();


	UFUNCTION(NetMulticast, Unreliable)
		void MultiCastDeath();

	void MultiCastDeath_Implementation();

	UFUNCTION(Client, Reliable)
		void PlayDamage();

	void PlayDamage_Implementation();

public:
	UFUNCTION(NetMulticast, Reliable)
		void SetTeam(ETeam NewTeam);

private:
	void SetTeam_Implementation(ETeam NewTeam);

public:
	ACPlayer();
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

protected:
	virtual void BeginPlay();
	virtual void PossessedBy(AController* NewController) override;

protected:
	void OnFire();

	void MoveForward(float Val);
	void MoveRight(float Val);

	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	void Fire(const FVector pos, const FVector dir);

	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

public:
	class USkeletalMeshComponent* GetFP_Mesh() const { return FP_Mesh; }
	class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	class ACPlayerState* GetSelfPlayerState();
	void SetSelfPlayerState(class ACPlayerState* NewState);
	void Respawn();

private:
	class UMaterialInstanceDynamic* DynamicMaterial;
	class ACPlayerState* SelfPlayerState;

};
