// Fill out your copyright notice in the Description page of Project Settings.


#include "TankGameModeBase.h"
#include "ToonTanks/Pawns/PawnTank.h"
#include "ToonTanks/Pawns/PawnTurret.h"
#include "ToonTanks/PlayerControllers/PlayerControllerBase.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

void ATankGameModeBase::BeginPlay() 
{
    Super::BeginPlay();

    HandleGameStart();

}

void ATankGameModeBase::ActorDied(AActor* DeadActor) 
{
    if(DeadActor == PlayerTank)
    {
        PlayerTank->HandleDestruction();
        HandleGameOver(false);

        if(PlayerControllerRef)
        {
            PlayerControllerRef->SetPlayerEnabledState(false);
        }
    }
    else if (APawnTurret* DestroyedTurret = Cast<APawnTurret>(DeadActor))
    {
        DestroyedTurret->HandleDestruction();
        if(--TargetTurrets == 0)
        {
            HandleGameOver(true);
        }
    }
    
}

void ATankGameModeBase::HandleGameStart() 
{
    TargetTurrets = GetTargetTurretCount();
    PlayerTank = Cast<APawnTank>(UGameplayStatics::GetPlayerPawn(this, 0)); //This is a single player game so '0' have been passed
    PlayerControllerRef = Cast<APlayerControllerBase>(UGameplayStatics::GetPlayerController(this, 0));
    GameStart();

    if(PlayerControllerRef)
        {
            PlayerControllerRef->SetPlayerEnabledState(false);

            FTimerHandle PlayerEnableHandle;
            FTimerDelegate PlayerEnableDelegate = FTimerDelegate::CreateUObject(PlayerControllerRef,
             &APlayerControllerBase::SetPlayerEnabledState, true);

            GetWorld()->GetTimerManager().SetTimer(PlayerEnableHandle, PlayerEnableDelegate, StartDelay, false);
        }
}

void ATankGameModeBase::HandleGameOver(bool bPlayerWon) 
{
    GameOver(bPlayerWon);
}

int32 ATankGameModeBase::GetTargetTurretCount() 
{
    TArray<AActor*> TurretActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APawnTurret::StaticClass(), TurretActors);
    return TurretActors.Num();
}