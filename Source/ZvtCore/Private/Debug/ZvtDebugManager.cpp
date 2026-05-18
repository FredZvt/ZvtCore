#include "ZvtDebugManager.h"
#include "Module.h"
#include "ZvtCoreLogCategories.h"
#include "CanvasItem.h"
#include "Debug/DebugDrawService.h"
#include "Engine/Canvas.h"
#include "Engine/Font.h"
#include "Misc/App.h"
#include "GlobalRenderResources.h"

static constexpr float StartX = 20.f;
static constexpr float StartY = 20.f;
static constexpr float LineSpacing = 2.f;
static constexpr float BackgroundPadding = 6.f;

void UZvtDebugManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    const FZvtCoreModule& Module = FModuleManager::GetModuleChecked<FZvtCoreModule>("ZvtCore");
    Font = Module.GetMonoFont();

    CanvasDrawHandle = UDebugDrawService::Register(
        TEXT("Game"), FDebugDrawDelegate::CreateUObject(this, &UZvtDebugManager::OnDrawCanvas));
}

void UZvtDebugManager::Deinitialize()
{
    UDebugDrawService::Unregister(CanvasDrawHandle);
    Super::Deinitialize();
}

void UZvtDebugManager::AddOnScreenMessage(
    const int32 Key,
    const float Duration,
    const FString& Text,
    const FColor Color,
    const float Scale)
{
    if (!GEngine)
    {
        UE_LOG(LogZvtCore, Error, TEXT("UZvtDebugManager::AddZvtDebugMessage: GEngine is null."));
        return;
    }

    UZvtDebugManager* Manager = GEngine->GetEngineSubsystem<UZvtDebugManager>();
    if (!Manager)
    {
        UE_LOG(LogZvtCore, Error, TEXT("UZvtDebugManager::AddZvtDebugMessage: subsystem not found."));
        return;
    }

    const float ExpirationTime = static_cast<float>(FApp::GetCurrentTime()) + Duration;

    FScopeLock Lock(&Manager->MessagesMutex);

    if (Key != INDEX_NONE)
    {
        for (FZvtDebugMessage& Msg : Manager->Messages)
        {
            if (Msg.Key == Key)
            {
                Msg.Text = Text;
                Msg.Color = Color;
                Msg.ExpirationTime = ExpirationTime;
                Msg.Scale = Scale;
                return;
            }
        }
    }

    FZvtDebugMessage NewMsg;
    NewMsg.Key = Key;
    NewMsg.Text = Text;
    NewMsg.Color = Color;
    NewMsg.ExpirationTime = ExpirationTime;
    NewMsg.Scale = Scale;
    Manager->Messages.Add(NewMsg);
}

void UZvtDebugManager::SetOnScreenMessagesBackground(const bool bEnabled, const FColor Color)
{
    if (!GEngine)
    {
        UE_LOG(LogZvtCore, Error, TEXT("UZvtDebugManager::SetZvtDebugBackground: GEngine is null."));
        return;
    }

    UZvtDebugManager* Manager = GEngine->GetEngineSubsystem<UZvtDebugManager>();
    if (!Manager)
    {
        UE_LOG(LogZvtCore, Error, TEXT("UZvtDebugManager::SetZvtDebugBackground: subsystem not found."));
        return;
    }

    FScopeLock Lock(&Manager->MessagesMutex);
    Manager->bBackgroundEnabled = bEnabled;
    Manager->BackgroundColor = Color;
}

void UZvtDebugManager::OnDrawCanvas(UCanvas* Canvas, APlayerController* PC)
{
    if (!Canvas || !Font)
    {
        return;
    }

    const double CurrentTime = FApp::GetCurrentTime();

    FScopeLock Lock(&MessagesMutex);

    // Draw background using dimensions cached from the previous frame.
    // FCanvasTextItem::Draw reports the Slate-correct line height via DrawnSize after the call,
    // but the background must be drawn before text. Using last frame's DrawnSize values avoids
    // the mismatch between Canvas::TextSize (legacy UFont metrics) and Slate's CharacterList
    // GetMaxHeight (used at draw time). The one-frame lag is imperceptible at normal frame rates.
    if (bBackgroundEnabled && !Messages.IsEmpty() && bBackgroundCacheValid)
    {
        const FVector2D BgPosition(StartX - BackgroundPadding, StartY - BackgroundPadding);
        const FVector2D BgSize(CachedBackgroundWidth + BackgroundPadding * 2.f, CachedBackgroundHeight + BackgroundPadding * 2.f);
        FCanvasTileItem BgItem(BgPosition, GWhiteTexture, BgSize, FLinearColor(BackgroundColor));
        BgItem.BlendMode = SE_BLEND_Translucent;
        Canvas->DrawItem(BgItem);
    }

    // Draw text. After each DrawItem call, Item.DrawnSize holds the Slate-correct dimensions.
    float CurrentY = StartY;
    float NewMaxWidth = 0.f;
    float NewTotalHeight = 0.f;

    for (const FZvtDebugMessage& Msg : Messages)
    {
        FCanvasTextItem Item(FVector2D(StartX, CurrentY), FText::FromString(Msg.Text), Font, FLinearColor(Msg.Color));
        Item.Scale = FVector2D(Msg.Scale, Msg.Scale);
        Canvas->DrawItem(Item);

        const float LineHeight = Item.DrawnSize.Y > 0.f ? Item.DrawnSize.Y : Font->GetMaxCharHeight() * Msg.Scale;
        NewMaxWidth = FMath::Max(NewMaxWidth, Item.DrawnSize.X);
        NewTotalHeight += LineHeight + LineSpacing;
        CurrentY += LineHeight + LineSpacing;
    }

    if (!Messages.IsEmpty())
    {
        NewTotalHeight -= LineSpacing;
    }

    // Cache background dimensions for next frame.
    CachedBackgroundWidth = NewMaxWidth;
    CachedBackgroundHeight = NewTotalHeight;
    bBackgroundCacheValid = true;

    // Expire old messages.
    for (int32 i = Messages.Num() - 1; i >= 0; --i)
    {
        if (static_cast<double>(Messages[i].ExpirationTime) <= CurrentTime)
        {
            Messages.RemoveAt(i);
        }
    }
}