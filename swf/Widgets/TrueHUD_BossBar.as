import Widgets.TrueHUD_InfoBarBase;

class Widgets.TrueHUD_BossBar extends Widgets.TrueHUD_InfoBarBase
{
	var uBossResourcesMode: Number;
	var uBossSpecialMode: Number;
	var uResourceAlignment: Number;
	var uNameAlignment: Number;

	var resourcesXOffset: Number = 15;

	public function TrueHUD_BossBar()
	{
		super();

		bHealthBarHasShadow = true;
		bHealthBarHasFlash = true;
		healthXOffset = 6;
		killDelay = 4;
		nameY = -35;
		damageCounterY = 4;
		resourcesYNone = 5;
		resourcesYStamina = 11;
		resourcesYBoth = 14;
		specialYHidden = -6;
		specialYShown = -12;

		Bars.Resources._y = resourcesYNone;
		Bars.Special._y = specialYHidden;

		TargetName._alpha = 100;
	}

	public function init() // override
	{
		super.init();

		DamageCounter._visible = bDisplayDamageCounter;

		toggleResources(uBossResourcesMode);
		toggleSpecial(uBossSpecialMode);

		if ((uNameAlignment == 1 && uResourceAlignment == 2) || (uNameAlignment == 2 && uResourceAlignment == 1))
		{
			// if opposing sides, don't offset the name up
			specialNameOffset = 0;
		}

		var format = new TextFormat();
		switch (uNameAlignment) 
		{
		case 0: // center
			format.align = "center";
			break;
		case 1: // left
			format.align = "left";
			break;
		case 2: // right
			format.align = "right";
			break;
		}

		toggleIndicator(bDisplayIndicator);

		TargetName.setTextFormat(format);
		TargetName.setNewTextFormat(format);

		setWidth(fWidth, fResourceWidth);
	}

	public function loadConfig(a_uBossBarDirection: Number,
		a_bDisplayBossPhantomBars: Boolean,
		a_bDisplayBossSpecialPhantomBar: Boolean,
		a_bDisplayBossDamageCounter: Boolean,
		a_bDisplayBossIndicator: Boolean,
		a_uBossDamageCounterAlignment: Number,
		a_uBossResourcesMode: Number,
		a_uBossSpecialMode: Number,
		a_bBossSpecialDefaultMode: Boolean,
		a_uBossResourceAlignment: Number,
		a_uBossNameAlignment: Number,
		a_uBossIndicatorMode: Number,
		a_fBossPhantomDuration: Number,
		a_fBossDamageCounterDuration: Number,
		a_fBossBarOpacity: Number,
		a_fBossBarWidth: Number,
		a_fBossBarResourceWidth: Number,
		a_uHealthColor: Number,
		a_uHealthPhantomColor: Number,
		a_uHealthBackgroundColor: Number,
		a_uHealthPenaltyColor: Number,
		a_uHealthFlashColor: Number,
		a_uMagickaColor: Number,
		a_uMagickaPhantomColor: Number,
		a_uMagickaBackgroundColor: Number,
		a_uMagickaPenaltyColor: Number,
		a_uMagickaFlashColor: Number,
		a_uStaminaColor: Number,
		a_uStaminaPhantomColor: Number,
		a_uStaminaBackgroundColor: Number,
		a_uStaminaPenaltyColor: Number,
		a_uStaminaFlashColor: Number,
		a_uSpecialColor: Number,
		a_uSpecialPhantomColor: Number,
		a_uSpecialBackgroundColor: Number,
		a_uSpecialPenaltyColor: Number,
		a_uSpecialFlashColor: Number)
	{
		uBarDirection = a_uBossBarDirection;
		bDisplayPhantomBars = a_bDisplayBossPhantomBars;
		bDisplaySpecialPhantomBar = a_bDisplayBossSpecialPhantomBar;
		bDisplayDamageCounter = a_bDisplayBossDamageCounter;
		bDisplayIndicator = a_bDisplayBossIndicator;
		uDamageCounterAlignment = a_uBossDamageCounterAlignment;
		uBossResourcesMode = a_uBossResourcesMode;
		uBossSpecialMode = a_uBossSpecialMode;
		bSpecialDefaultMode = a_bBossSpecialDefaultMode;
		uResourceAlignment = a_uBossResourceAlignment;
		uNameAlignment = a_uBossNameAlignment;
		uIndicatorMode = a_uBossIndicatorMode;
		fPhantomDuration = a_fBossPhantomDuration;
		fDamageCounterDuration = a_fBossDamageCounterDuration;
		fOpacity = a_fBossBarOpacity;
		fWidth = a_fBossBarWidth;
		fResourceWidth = a_fBossBarResourceWidth;
		uHealthColor = a_uHealthColor;
		uHealthPhantomColor = a_uHealthPhantomColor;
		uHealthBackgroundColor = a_uHealthBackgroundColor;
		uHealthPenaltyColor = a_uHealthPenaltyColor;
		uHealthFlashColor = a_uHealthFlashColor;
		uMagickaColor = a_uMagickaColor;
		uMagickaPhantomColor = a_uMagickaPhantomColor;
		uMagickaBackgroundColor = a_uMagickaBackgroundColor;
		uMagickaPenaltyColor = a_uMagickaPenaltyColor;
		uMagickaFlashColor = a_uMagickaFlashColor;
		uStaminaColor = a_uStaminaColor;
		uStaminaPhantomColor = a_uStaminaPhantomColor;
		uStaminaBackgroundColor = a_uStaminaBackgroundColor;
		uStaminaPenaltyColor = a_uStaminaPenaltyColor;
		uStaminaFlashColor = a_uStaminaFlashColor;
		uSpecialColor = a_uSpecialColor;
		uSpecialPhantomColor = a_uSpecialPhantomColor;
		uSpecialBackgroundColor = a_uSpecialBackgroundColor;
		uSpecialPenaltyColor = a_uSpecialPenaltyColor;
		uSpecialFlashColor = a_uSpecialFlashColor;

		// delay init till everything's ready
		this.onEnterFrame = function(): Void {
			init();
			delete this.onEnterFrame;
		}
	}

	public function toggleIndicator(a_enable: Boolean) // override
	{
		super.toggleIndicator(a_enable);

		Indicator._visible = a_enable;
		Bars.HealthBar.Outline.Left._visible = !a_enable;
		Bars.HealthBar.Frame.Left._visible = !a_enable;
		if (a_enable)
		{
			Bars.setMask(IndicatorMask);
		}
		else
		{
			Bars.setMask(null);
		}
	}

	public function calculateDamageCounterY() : Number // override
	{
		if ((uDamageCounterAlignment == 1 && uResourceAlignment == 1) || (uDamageCounterAlignment == 2 && uResourceAlignment == 2))
		{
			return super.calculateDamageCounterY();
		}

		var resourcesOffset = 0;
		switch (uDisplayedResources)
		{
		case 0:
			break;
		case 1:
			resourcesOffset = resourcesXOffsetOne;
			break;
		case 2:
			resourcesOffset = resourcesXOffsetTwo;
			break;
		}
		
		var damageCounterWidth = fCurrentWidth + healthXOffset;

		if (uResourceAlignment == 0) // center
		{
			if (((damageCounterWidth - (fCurrentResourceWidth + resourcesOffset)) / 2) > DamageCounter.textWidth)
			{
				return damageCounterY;
			}
			else
			{
				return super.calculateDamageCounterY();
			}
		}

		// if we're here, they're on the opposite sides
		if (damageCounterWidth - (fCurrentResourceWidth + resourcesOffset) > DamageCounter.textWidth)
		{
			return damageCounterY;
		}
		else
		{
			return super.calculateDamageCounterY();
		}
	}

	public function calculateNameY() : Number // override
	{
		if ((uNameAlignment == 1 && uResourceAlignment == 1) || (uNameAlignment == 2 && uResourceAlignment == 2))
		{
			// same sides, always offset
			return super.calculateNameY();
		}

		var resourcesOffset = 0;
		if (bDisplayedSpecial)
		{
			resourcesOffset = resourcesXOffsetOne;
		}
		
		if (uResourceAlignment == 0) // center
		{
			if (((fCurrentWidth - (fCurrentResourceWidth + resourcesOffset)) / 2) > TargetName.textWidth)
			{
				return nameY;
			}
			else
			{
				return super.calculateNameY();
			}
		}

		// if we're here, they're on the opposite sides
		if (fCurrentWidth - (fCurrentResourceWidth + resourcesOffset) > TargetName.textWidth)
		{
			return nameY;
		}
		else
		{
			return super.calculateNameY();
		}
	}

	public function setWidth(a_width: Number, a_resourceWidth: Number) // override
	{
		super.setWidth(a_width, a_resourceWidth);

		// Name
		TargetName._width = a_width;
		TargetName._x = -a_width / 2;

		// Resources
		Bars.ResourceMask._width = a_width;
		switch (uResourceAlignment)
		{
		case 0: // center
			Bars.Resources._x = 0;
			Bars.Special._x = 0;
			break;
		case 1: // left
			Bars.Resources._x = (-a_width / 2) + (a_resourceWidth / 2) + resourcesXOffset;
			Bars.Special._x = (-a_width / 2) + (a_resourceWidth / 2) + resourcesXOffset;
			break;
		case 2: // right
			Bars.Resources._x = (a_width / 2) - (a_resourceWidth / 2) - resourcesXOffset;
			Bars.Special._x = (a_width / 2) - (a_resourceWidth / 2) - resourcesXOffset;
			break;
		}

		// Indicator
		var widthOffset = -((a_width - 100) / 2);
		var indicatorOffset = -50 + widthOffset;
		var indicatorMaskWidth = a_width + 100;
		var indicatorMaskOffset = widthOffset -104 + 0.32 * (indicatorMaskWidth);
		Indicator._x = indicatorOffset;
		IndicatorMask._width = indicatorMaskWidth;
		IndicatorMask._x = indicatorMaskOffset;
	}
}
