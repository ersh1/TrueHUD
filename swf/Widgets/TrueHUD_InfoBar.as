import com.greensock.TimelineLite;
import com.greensock.easing.*;

import TrueHUD_InfoBarBase;

class Widgets.TrueHUD_InfoBar extends Widgets.TrueHUD_InfoBarBase
{	
	var nameFadeTimeline: TimelineLite;
	var showIndicatorTimeline: TimelineLite;
	var hideIndicatorTimeline: TimelineLite;
	var widthTimeline: TimelineLite;

	var uDisplayName: Number;
	var uDisplayDamageCounter: Number;
	var uDisplayIndicator: Number;
	var uResourcesModeTarget: Number;
	var uResourcesModeHostiles: Number;
	var uResourcesModeTeammates: Number;
	var uResourcesModeOthers: Number;
	var uSpecialModeTarget: Number;
	var uSpecialModeHostiles: Number;
	var uSpecialModeTeammates: Number;
	var uSpecialModeOthers: Number;
	var fTargetWidth: Number;
	var fTargetResourceWidth: Number;

	var widthTimelineDuration = 0.25;

	public function TrueHUD_InfoBar()
	{
		super();

		nameFadeTimeline = new TimelineLite({paused:true});
		showIndicatorTimeline = new TimelineLite({paused:true});
		hideIndicatorTimeline = new TimelineLite({paused:true});
		widthTimeline = new TimelineLite({paused:true});

		bHealthBarHasFlash = true;
		bHealthBarHasShapeMask = true;
		healthXOffset = 15;
		killDelay = 2;
		nameY = -30;
		damageCounterY = 2;
		resourcesYNone = 2.5;
		resourcesYStamina = 8.5;
		resourcesYBoth = 11.5;
		specialYHidden = -3.5;
		specialYShown = -9.5;
	
		Bars.Resources._y = 2.5;
		Bars.Special._y = -3.5;

		Indicator._alpha = 0;
		TargetName._alpha = 0;

		nameFadeTimeline.set(TargetName, {_alpha:0});
		nameFadeTimeline.to(TargetName, 0.25, {_alpha:100}, 0);
	}

	public function init() // override
	{
		super.init();

		if (targetType == 0) {
			setWidth(fTargetWidth, fTargetResourceWidth);
		} else {
			setWidth(fWidth, fResourceWidth);
		}

		showIndicatorTimeline.clear();
		showIndicatorTimeline.set(Indicator, {_visible:true});
		showIndicatorTimeline.to(Indicator, 0.25, {_alpha:100, ease:Sine.easeIn}, 0);
		showIndicatorTimeline.call(toggleIndicatorMask, [true], this);

		hideIndicatorTimeline.clear();
		hideIndicatorTimeline.call(toggleIndicatorMask, [false], this);
		hideIndicatorTimeline.to(Indicator, 0.25, {_alpha:0, ease:Sine.easeIn}, 0);
		hideIndicatorTimeline.set(Indicator, {_visible:false});
	}

	public function cleanUp() // override
	{
		super.cleanUp();

		nameFadeTimeline.clear();
		nameFadeTimeline.kill();
		nameFadeTimeline = null;

		showIndicatorTimeline.clear();
		showIndicatorTimeline.kill();
		showIndicatorTimeline = null;

		hideIndicatorTimeline.clear();
		hideIndicatorTimeline.kill();
		hideIndicatorTimeline = null;

		widthTimeline.clear();
		widthTimeline.kill();
		widthTimeline = null;
	}

	public function toggleIndicator(a_enable: Boolean) // override
	{
		super.toggleIndicator(a_enable);

		if (a_enable == bDisplayIndicator)
		{
			return;
		}

		bDisplayIndicator = a_enable;

		if (bDisplayIndicator)
		{
			hideIndicatorTimeline.pause();
			showIndicatorTimeline.restart();
		}
		else 
		{
			showIndicatorTimeline.pause();
			hideIndicatorTimeline.restart();
		}
	}

	public function calculateDamageCounterY() : Number // override
	{
		// Don't offset the damage counter if it doesn't clip with the resources
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

		if (((damageCounterWidth - (fCurrentResourceWidth + resourcesOffset)) / 2) > DamageCounter.textWidth)
		{
			return damageCounterY;
		}
		else
		{
			return super.calculateDamageCounterY();
		}
	}

	public function updateTargetType(a_newTargetType: Number) // override
	{
		var prevTargetType = targetType;

		// Indicator
		if (uDisplayIndicator > a_newTargetType)
		{
			toggleIndicator(true);
		}
		else 
		{
			toggleIndicator(false);
		}

		super.updateTargetType(a_newTargetType);

		// Name
		if (uDisplayName > a_newTargetType)
		{
			nameFadeTimeline.play();
		}
		else
		{
			nameFadeTimeline.reverse();
		}

		// Damage counter
		if (uDisplayDamageCounter > a_newTargetType)
		{
			bDisplayDamageCounter = true;
			DamageCounter._visible = true;
		}
		else 
		{
			bDisplayDamageCounter = false;
			DamageCounter._visible = false;
		}

		// Width
		if (a_newTargetType == 0) // if new type should toggle target (expanded) mode
		{
			setWidthTimeline(fTargetWidth, fTargetResourceWidth);
		}
		else if (prevTargetType == 0) // if was target mode
		{
			setWidthTimeline(fWidth, fResourceWidth);
		}

		// Resources and special bar
		switch (a_newTargetType)
		{
		case 0: // Target
			toggleResources(uResourcesModeTarget);
			toggleSpecial(uSpecialModeTarget);
			break;
		case 1: // Hostile
			toggleResources(uResourcesModeHostiles);
			toggleSpecial(uSpecialModeHostiles);
			break;
		case 2: // Teammate
			toggleResources(uResourcesModeTeammates);
			toggleSpecial(uSpecialModeTeammates);
			break;
		case 3: // Other
			toggleResources(uResourcesModeOthers);
			toggleSpecial(uSpecialModeOthers);
			break;
		}
	}

	public function setWidth(a_width: Number, a_resourceWidth: Number) // override
	{
		super.setWidth(a_width, a_resourceWidth);

		// Indicator and mask
		var indicatorOffset = -60.5;
		if (a_width != 100)
		{
			var indicatorOffset = -60.5 - ((a_width - 100) / 2);
		}
		Indicator._x = indicatorOffset;
		IndicatorMask._x = indicatorOffset;
	}

	public function toggleIndicatorMask(a_enable: Boolean)
	{
		if (a_enable)
		{
			Bars.setMask(IndicatorMask);
		}
		else
		{
			Bars.setMask(null);
		}
	}

	public function loadConfig(a_uInfoBarDirection: Number,
		a_uInfoBarDisplayName: Number,
		a_bInfoBarDisplayPhantomBars: Boolean,
		a_bInfoBarDisplaySpecialPhantomBar: Boolean,
		a_uInfoBarDisplayDamageCounter: Number,
		a_uInfoBarDisplayIndicator: Number,
		a_uInfoBarDamageCounterAlignment: Number,
		a_uInfoBarResourcesModeTarget: Number,
		a_uInfoBarResourcesModeHostiles: Number,
		a_uInfoBarResourcesModeTeammates: Number,
		a_uInfoBarResourcesModeOthers: Number,
		a_uInfoBarSpecialModeTarget: Number,
		a_uInfoBarSpecialModeHostiles: Number,
		a_uInfoBarSpecialModeTeammates: Number,
		a_uInfoBarSpecialModeOthers: Number,
		a_bInfoBarSpecialDefaultMode: Boolean,
		a_uInfoBarIndicatorMode: Number,
		a_fInfoBarPhantomDuration: Number,
		a_fInfoBarDamageCounterDuration: Number,
		a_fInfoBarOpacity: Number,
		a_fInfoBarWidth: Number,
		a_fInfoBarResourceWidth: Number,
		a_fInfoBarTargetWidth: Number,
		a_fInfoBarTargetResourceWidth: Number,
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
		uBarDirection = a_uInfoBarDirection;
		uDisplayName = a_uInfoBarDisplayName;
		bDisplayPhantomBars = a_bInfoBarDisplayPhantomBars;
		bDisplaySpecialPhantomBar = a_bInfoBarDisplaySpecialPhantomBar;
		uDisplayDamageCounter = a_uInfoBarDisplayDamageCounter;
		uDisplayIndicator = a_uInfoBarDisplayIndicator;
		uDamageCounterAlignment = a_uInfoBarDamageCounterAlignment;
		uResourcesModeTarget = a_uInfoBarResourcesModeTarget;
		uResourcesModeHostiles = a_uInfoBarResourcesModeHostiles;
		uResourcesModeTeammates = a_uInfoBarResourcesModeTeammates;
		uResourcesModeOthers = a_uInfoBarResourcesModeOthers;
		uSpecialModeTarget = a_uInfoBarSpecialModeTarget;
		uSpecialModeHostiles = a_uInfoBarSpecialModeHostiles;
		uSpecialModeTeammates = a_uInfoBarSpecialModeTeammates;
		uSpecialModeOthers = a_uInfoBarSpecialModeOthers;
		bSpecialDefaultMode = a_bInfoBarSpecialDefaultMode;
		uIndicatorMode = a_uInfoBarIndicatorMode;
		fPhantomDuration = a_fInfoBarPhantomDuration;
		fDamageCounterDuration = a_fInfoBarDamageCounterDuration;
		fOpacity = a_fInfoBarOpacity;
		fWidth = a_fInfoBarWidth;
		fResourceWidth = a_fInfoBarResourceWidth;
		fTargetWidth = a_fInfoBarTargetWidth;
		fTargetResourceWidth = a_fInfoBarTargetResourceWidth;
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

	public function setWidthTimeline(a_width: Number, a_resourceWidth: Number)
	{
		fCurrentWidth = a_width;
		fCurrentResourceWidth = a_resourceWidth;

		widthTimeline.clear();

		Bars.HealthBar.setWidthTimeline(a_width);
		Bars.Resources.MagickaBar.setWidthTimeline(a_resourceWidth + magickaWidthOffset);
		Bars.Resources.StaminaBar.setWidthTimeline(a_resourceWidth);
		Bars.Special.SpecialBar.setWidthTimeline(a_resourceWidth);

		// Resources
		widthTimeline.to(Bars.ResourceMask, widthTimelineDuration, {_xscale:a_resourceWidth + resourcesFrameWidthOffset, ease:Sine.easeIn}, 0)

		.to(Bars.Resources.Frame, widthTimelineDuration, {_width:a_resourceWidth + resourcesFrameWidthOffset, ease:Sine.easeIn}, 0)
		.to(Bars.Resources.Outline, widthTimelineDuration, {_width:a_resourceWidth + resourcesFrameOutlineWidthOffset, ease:Sine.easeIn}, 0)

		//Special
		.to(Bars.Special.Frame, widthTimelineDuration, {_width:a_resourceWidth + specialFrameWidthOffset, ease:Sine.easeIn}, 0)
		.to(Bars.Special.Outline, widthTimelineDuration, {_width:a_resourceWidth + specialFrameOutlineWidthOffset, ease:Sine.easeIn}, 0)

		// Damage text
		var damageCounterWidth = a_width + healthXOffset;
		widthTimeline.to(DamageCounter, widthTimelineDuration, {_width:damageCounterWidth, ease:Sine.easeIn}, 0)
		.to(DamageCounter, widthTimelineDuration, {_x:-damageCounterWidth / 2, ease:Sine.easeIn}, 0);

		// Indicator and mask
		var indicatorOffset = -60.5;
		if (a_width != 100)
		{
			indicatorOffset = -60.5 - ((a_width - 100) / 2);
		}
		widthTimeline.to(Indicator, widthTimelineDuration, {_x:indicatorOffset, ease:Sine.easeIn}, 0)
		.to(IndicatorMask, widthTimelineDuration, {_x:indicatorOffset, ease:Sine.easeIn}, 0);

		widthTimeline.restart();

		updateNameY();
		updateDamageY();
	}
}
