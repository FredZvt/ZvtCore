#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "ZvtDebugManager.generated.h"

class UCanvas;
class APlayerController;

/**
 * A single on-screen debug message managed by UZvtDebugManager.
 *
 * Keyed messages (Key != INDEX_NONE) are deduplicated: re-posting with the same key
 * overwrites this record in place. Un-keyed messages (Key == INDEX_NONE) are always
 * appended as independent entries.
 */
struct FZvtDebugMessage
{
    /** Deduplication key. INDEX_NONE means un-keyed (always appended as a new line). */
    int32 Key;

    /** The string to display on screen. */
    FString Text;

    /** Render color for this message. */
    FColor Color;

    /** Absolute expiration time in seconds obtained from FApp::GetCurrentTime(). */
    float ExpirationTime;

    /** Uniform scale applied to the font when this message is drawn. */
    float Scale;
};

/**
 * Engine subsystem that renders text messages on the game viewport using a monospace font (RobotoMono).
 *
 * It is an alternative to GEngine->AddOnScreenDebugMessage with support for keyed deduplication
 * and explicit expiration times. The subsystem is created and managed automatically by the engine;
 * do not instantiate it directly. Post messages through the static AddZvtDebugMessage method.
 */
UCLASS()
class ZVTCORE_API UZvtDebugManager : public UEngineSubsystem
{
    GENERATED_BODY()

public:

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    /**
     * Posts a text message to the on-screen debug overlay.
     *
     * This is a static convenience method that resolves the subsystem from GEngine internally.
     * It is safe to call from any context where GEngine is available.
     *
     * @param Key       Deduplication key. Pass INDEX_NONE to always append a new line regardless
     *                  of other messages. Any other integer causes the existing entry with the same
     *                  key to be updated in place rather than creating a duplicate.
     * @param Duration  Seconds until the message is removed. Pass 0.0f to show the message for
     *                  exactly one frame, which is the typical choice for per-tick overlays that
     *                  are re-posted every frame.
     * @param Text      The string to display.
     * @param Color     Render color. Defaults to FColor::Green.
     * @param Scale     Uniform font scale. Defaults to 1.0f.
     */
    static void AddOnScreenMessage(
        const int32 Key,
        const float Duration,
        const FString& Text,
        const FColor Color = FColor::Green,
        const float Scale = 1.0f);

    /**
     * Configures the optional semi-transparent background rectangle drawn behind all
     * debug messages. The background is sized to fit the entire message block and is
     * drawn before text so that text renders on top.
     *
     * @param bEnabled        Pass true to enable the background, false to disable it.
     * @param Color           Background fill color. The alpha channel controls opacity
     *                        (0 = fully transparent, 255 = fully opaque).
     *                        Defaults to half-opacity black (0, 0, 0, 128).
     */
    static void SetOnScreenMessagesBackground(
        bool bEnabled,
        FColor Color = FColor(0, 0, 0, 128));

private:
    
    void OnDrawCanvas(UCanvas* Canvas, APlayerController* PC);

    UPROPERTY()
    TObjectPtr<UFont> Font;

    TArray<FZvtDebugMessage> Messages;
    FCriticalSection MessagesMutex;
    FDelegateHandle CanvasDrawHandle;

    bool bBackgroundEnabled = true;
    FColor BackgroundColor = FColor(0, 0, 0, 128);

    // Background size cached from the previous frame using DrawnSize reported by FCanvasTextItem.
    // Canvas::TextSize() uses legacy UFont metrics while FCanvasTextItem::Draw uses Slate's
    // CharacterList::GetMaxHeight(), which can differ for Runtime (TTF) fonts. Caching the
    // DrawnSize from one frame and using it the next avoids that mismatch at the cost of a
    // single-frame lag, which is imperceptible at normal frame rates.
    float CachedBackgroundWidth = 0.f;
    float CachedBackgroundHeight = 0.f;
    bool bBackgroundCacheValid = false;
};
