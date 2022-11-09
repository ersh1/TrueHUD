import com.greensock.TimelineLite;
import com.greensock.easing.*;

class Widgets.TrueHUD_PlayerWidget extends MovieClip
{
    public var Health: MovieClip;
    public var Magicka: MovieClip;
    public var Stamina: MovieClip;
	public var SpecialMask: MovieClip;
    public var Special: MovieClip;
    public var ShoutIndicator: MovieClip;
	public var EnchantmentChargeMeter: MovieClip;

	var widthTimeline: TimelineLite;
	var healthTimeline: TimelineLite;
	var magickaTimeline: TimelineLite;
	var staminaTimeline: TimelineLite;
	var specialTimeline: TimelineLite;
	var shoutTimeline: TimelineLite;
	var enchantmentChargeTimeline: TimelineLite;

	var uHealthBarDirection: Number;
	var uMagickaBarDirection: Number;
	var uStaminaBarDirection: Number;
	var bDisplayPhantomBars: Boolean;
	var bDisplaySpecialPhantomBar: Boolean;
	var uHealthMode: Number;
	var uMagickaMode: Number;
	var uStaminaMode: Number;
	var uShoutIndicatorMode: Number;
	var uSpecialMode: Number;
	var bSpecialDefaultMode: Boolean;
	var bDisplayEnchantmentChargeMeter: Boolean;
	var fPhantomDuration: Number;
	var fScale: Number;
	var fOpacity: Number;
	var fDefaultBarWidth: Number;
	var fSpecialBarWidth: Number;
	var bScaleBars: Boolean;
	var fMinBarWidth: Number;
	var fMaxBarWidth: Number;
	var fHealthBarScaleMult: Number;
	var fMagickaBarScaleMult: Number;
	var fStaminaBarScaleMult: Number;
	var fPlayerWidgetX: Number;
	var fPlayerWidgetY: Number;
	var fPlayerWidgetEnchantmentChargeMeterX: Number;
	var fPlayerWidgetEnchantmentChargeMeterY: Number;
	var bPlayerWidgetCombined: Boolean;
	var fPlayerWidgetHealthX: Number;
	var fPlayerWidgetHealthY: Number;
	var fPlayerWidgetMagickaX: Number;
	var fPlayerWidgetMagickaY: Number;
	var fPlayerWidgetStaminaX: Number;
	var fPlayerWidgetStaminaY: Number;
	var uHealthColor: Number;
	var uHealthPhantomColor: Number;
	var uHealthBackgroundColor: Number;
	var uHealthPenaltyColor: Number;
	var uHealthFlashColor: Number;
	var uMagickaColor: Number;
	var uMagickaPhantomColor: Number;
	var uMagickaBackgroundColor: Number;
	var uMagickaPenaltyColor: Number;
	var uMagickaFlashColor: Number;
	var uStaminaColor: Number;
	var uStaminaPhantomColor: Number;
	var uStaminaBackgroundColor: Number;
	var uStaminaPenaltyColor: Number;
	var uStaminaFlashColor: Number;
	var uSpecialColor: Number;
	var uSpecialPhantomColor: Number;
	var uSpecialBackgroundColor: Number;
	var uSpecialPenaltyColor: Number;
	var uSpecialFlashColor: Number;

	var bIsInCombat: Boolean = false;
	var bIsInMountMode: Boolean = false;
	var maxHealth: Number = -1;
	var health: Number = -1;
	var healthPct: Number = -1;
	var maxMagicka: Number = -1;
	var magicka: Number = -1;
	var magickaPct: Number = -1;
	var maxStamina: Number = -1;
	var stamina: Number = -1;
	var staminaPct: Number = -1;
	var maxSpecial: Number = -1;
	var special: Number = -1;
	var specialPct: Number = -1;

	var healthPenaltyPct: Number = -1;
	var magickaPenaltyPct: Number = -1;
	var staminaPenaltyPct: Number = -1;

	var dynamicBarFadeInTime = 0.25;
	var dynamicBarFadeOutTime = 1;
	var dynamicBarVisibilityDuration = 5;

	var specialXOffset = 50;
	var specialYShownOffset = 12;
	var specialYHiddenOffset = 5;
	var specialFrameWidthOffset: Number;
	var	specialFrameOutlineWidthOffset: Number;

    var bInitialized = false;
	var bDisplayedHealth = false;
	var bDisplayedMagicka = false;
	var bDisplayedStamina = false;
	var bDisplayedSpecial = false;
	var bDisplayedShout = false;
	var bSpecialHitThreshold = false;
	var bDisplayedLeftChargeBar = false;
	var bDisplayedRightChargeBar = false;

    public function TrueHUD_PlayerWidget()
    {
        // constructor code
        this.stop();

		widthTimeline = new TimelineLite({paused:true});
		healthTimeline = new TimelineLite({paused:true});
		magickaTimeline = new TimelineLite({paused:true});
		staminaTimeline = new TimelineLite({paused:true});
		specialTimeline = new TimelineLite({paused:true});
		shoutTimeline = new TimelineLite({paused:true});
		enchantmentChargeTimeline = new TimelineLite({paused:true});

		specialFrameWidthOffset = Special.Frame._width - 100;
		specialFrameOutlineWidthOffset = Special.Outline._width - 100;
    }

    public function init()
    {
        Health.init(bDisplayPhantomBars, fPhantomDuration, uHealthBarDirection, true, true, true, true, true);
        Health.setColor(uHealthColor, uHealthPhantomColor, uHealthBackgroundColor, uHealthPenaltyColor, uHealthFlashColor);

        Magicka.init(bDisplayPhantomBars, fPhantomDuration, uMagickaBarDirection, true, true, true, true, true);
        Magicka.setColor(uMagickaColor, uMagickaPhantomColor, uMagickaBackgroundColor, uMagickaPenaltyColor, uMagickaFlashColor);

        Stamina.init(bDisplayPhantomBars, fPhantomDuration, uStaminaBarDirection, true, true, true, true, true);
        Stamina.setColor(uStaminaColor, uStaminaPhantomColor, uStaminaBackgroundColor, uStaminaPenaltyColor, uStaminaFlashColor);

		Special.SpecialBar.init(bDisplaySpecialPhantomBar, fPhantomDuration, uHealthBarDirection, false, false, true, true, false);
		Special.SpecialBar.setColor(uSpecialColor, uSpecialPhantomColor, uSpecialBackgroundColor, uSpecialPenaltyColor, uSpecialFlashColor);

		ShoutIndicator.init();

		EnchantmentChargeMeter.BarLeft.init(false, 0, 0, true, false, true, false, false);
		EnchantmentChargeMeter.BarLeft.setColor(0x8C9ABC, 0x000000, 0x000000, 0x000000);
		EnchantmentChargeMeter.BarRight.init(false, 0, 0, true, false, true, false, false);
		EnchantmentChargeMeter.BarRight.setColor(0x8C9ABC, 0x000000, 0x000000, 0x000000);

		Special._alpha = fOpacity;

		var specialYHidden;
		var specialYShown;

		if (bPlayerWidgetCombined)
		{
			Health._y = -20;
			Magicka._y = 0;
			Stamina._y = 20;
			ShoutIndicator._x = 0;
			ShoutIndicator._y = 0;
			EnchantmentChargeMeter._x = 100;
			EnchantmentChargeMeter._y = 40;
			specialYHidden = -20 - specialYHiddenOffset;
			specialYShown = -20 - specialYShownOffset;
			Special._y = specialYHidden;
			SpecialMask._y = -20;
		}
		else
		{
			Health._y = fPlayerWidgetHealthY / fScale;
			Magicka._y = fPlayerWidgetMagickaY / fScale;
			Stamina._y = fPlayerWidgetStaminaY / fScale;
			ShoutIndicator._x = fPlayerWidgetX / fScale;
			ShoutIndicator._y = fPlayerWidgetY / fScale;
			EnchantmentChargeMeter._x = fPlayerWidgetEnchantmentChargeMeterX / fScale;
			EnchantmentChargeMeter._y = fPlayerWidgetEnchantmentChargeMeterY / fScale;
			specialYHidden = (fPlayerWidgetHealthY / fScale) - specialYHiddenOffset;
			specialYShown = (fPlayerWidgetHealthY / fScale) - specialYShownOffset;
			Special._y = specialYHidden;
			SpecialMask._y = fPlayerWidgetHealthY / fScale;
		}

		healthTimeline.pause();
		healthTimeline.clear();
		healthTimeline.to(Health, dynamicBarFadeInTime, {_alpha: fOpacity});
		healthTimeline.call(function() { bDisplayedHealth = false; }, null, this, dynamicBarVisibilityDuration);
		healthTimeline.to(Health, dynamicBarFadeOutTime, {_alpha: 0}, dynamicBarVisibilityDuration);

		magickaTimeline.pause();
		magickaTimeline.clear();
		magickaTimeline.to(Magicka, dynamicBarFadeInTime, {_alpha: fOpacity});
		magickaTimeline.call(function() { bDisplayedMagicka= false; }, null, this, dynamicBarVisibilityDuration);
		magickaTimeline.to(Magicka, dynamicBarFadeOutTime, {_alpha: 0}, dynamicBarVisibilityDuration);

		staminaTimeline.pause();
		staminaTimeline.clear();
		staminaTimeline.to(Stamina, dynamicBarFadeInTime, {_alpha: fOpacity});
		staminaTimeline.call(function() { bDisplayedStamina = false; }, null, this, dynamicBarVisibilityDuration);
		staminaTimeline.to(Stamina, dynamicBarFadeOutTime, {_alpha: 0}, dynamicBarVisibilityDuration);

		specialTimeline.pause();
		specialTimeline.clear();
		specialTimeline.to(Special, 0.25, {_y: specialYShown, ease:Sine.easeIn});
		specialTimeline.call(function() { bDisplayedSpecial = false; }, null, this, dynamicBarVisibilityDuration);
		specialTimeline.to(Special, 0.25, {_y: specialYHidden, ease:Sine.easeIn}, dynamicBarVisibilityDuration);

		shoutTimeline.pause();
		shoutTimeline.clear();
		shoutTimeline.to(ShoutIndicator, dynamicBarFadeInTime, {_alpha: fOpacity});
		shoutTimeline.call(function() { bDisplayedShout = false; }, null, this, dynamicBarVisibilityDuration);
		shoutTimeline.to(ShoutIndicator, dynamicBarFadeOutTime, {_alpha: 0}, dynamicBarVisibilityDuration);

		enchantmentChargeTimeline.pause();
		enchantmentChargeTimeline.clear();
		enchantmentChargeTimeline.to(EnchantmentChargeMeter, dynamicBarFadeInTime, {_alpha: fOpacity});
		enchantmentChargeTimeline.to(EnchantmentChargeMeter, dynamicBarFadeOutTime, {_alpha: 0}, dynamicBarVisibilityDuration);

		if (uHealthMode == 4)
		{
			Health._alpha = fOpacity;
			bDisplayedHealth = true;
		}
		else
		{
			Health._alpha = 0;
			bDisplayedHealth = false;
		}

		if (uMagickaMode == 4)
		{
			Magicka._alpha = fOpacity;
			bDisplayedMagicka = true;
		}
		else
		{
			Magicka._alpha = 0;
			bDisplayedMagicka = false;
		}

		if (uStaminaMode == 4)
		{
			Stamina._alpha = fOpacity;
			bDisplayedStamina = true;
		}
		else
		{
			Stamina._alpha = 0;
			bDisplayedStamina = false;
		}

		if (uShoutIndicatorMode == 4)
		{
			ShoutIndicator._alpha = fOpacity;
			bDisplayedShout = true;
		}
		else
		{
			ShoutIndicator._alpha = 0;
			bDisplayedShout = false;
		}

		if (uSpecialMode == 4)
		{
			Special._y = specialYShown;
			bDisplayedSpecial = true;
		}
		else
		{
			Special._y = specialYHidden;
			bDisplayedSpecial = false;
		}

		var chargeBarWidth = 40;
		EnchantmentChargeMeter.BarLeft.setWidth(chargeBarWidth);
		EnchantmentChargeMeter.BarLeft._x = -(chargeBarWidth / 2 + 6);
		EnchantmentChargeMeter.BarRight.setWidth(chargeBarWidth);
		EnchantmentChargeMeter.BarRight._x = chargeBarWidth / 2 + 6;

		bDisplayedLeftChargeBar = false;
		bDisplayedRightChargeBar = false;

		EnchantmentChargeMeter.BarLeft._visible = bDisplayedLeftChargeBar;
		EnchantmentChargeMeter.BackgroundLeft._visible = !bDisplayedLeftChargeBar;
		EnchantmentChargeMeter.BarRight._visible = bDisplayedRightChargeBar;
		EnchantmentChargeMeter.BackgroundRight._visible = !bDisplayedRightChargeBar;

		EnchantmentChargeMeter._alpha = 0;

		updateWidth(true);

		if (maxHealth != -1) {
			updateData(maxHealth, health, healthPct, maxMagicka, magicka, magickaPct, maxStamina, stamina, staminaPct, bIsInCombat, bIsInMountMode, true);
		}

		if (healthPenaltyPct != -1 && magickaPenaltyPct != -1 && staminaPenaltyPct != -1) {
			updatePenalty(healthPenaltyPct, magickaPenaltyPct, staminaPenaltyPct, true);
		}

        bInitialized = true;
    }

    public function cleanUp()
    {
		Health.cleanUp();
		Magicka.cleanUp();
		Stamina.cleanUp();
		Special.cleanUp();
		ShoutIndicator.cleanUp();
		
		widthTimeline.clear();
		widthTimeline.kill();
		widthTimeline = null;

		healthTimeline.clear();
		healthTimeline.kill();
		healthTimeline = null;

		magickaTimeline.clear();
		magickaTimeline.kill();
		magickaTimeline = null;

		staminaTimeline.clear();
		staminaTimeline.kill();
		staminaTimeline = null;

		specialTimeline.clear();
		specialTimeline.kill();
		specialTimeline = null;

		shoutTimeline.clear();
		shoutTimeline.kill();
		shoutTimeline = null;

		enchantmentChargeTimeline.clear();
		enchantmentChargeTimeline.kill();
		enchantmentChargeTimeline = null;
    }
	
	public function showHealth(a_bRefreshOtherBars: Boolean)
	{
		if (uHealthMode > 0 && uHealthMode < 4)
		{
			bDisplayedHealth = true;
			if (a_bRefreshOtherBars) {
				if (uMagickaMode == 3) {
					showMagicka(false);
				}
				if (uStaminaMode == 3) {
					showStamina(false);
				}
				if (uSpecialMode == 3) {
					showSpecial(false);
				}
				if (uShoutIndicatorMode == 3) {
					showShout(false);
				}
			}

			if (!healthTimeline.isActive())
			{
				healthTimeline.restart();
			}
			else
			{
				var playFrom = healthTimeline.time();
				if (isNaN(playFrom))
				{
					playFrom = 0;
				}
				playFrom = Math.min(playFrom, dynamicBarFadeInTime);
				healthTimeline.play(playFrom);
			}
		}
	}

	public function showMagicka(a_bRefreshOtherBars: Boolean)
	{
		if (uMagickaMode > 0 && uMagickaMode < 4)
		{
			bDisplayedMagicka = true;
			if (a_bRefreshOtherBars) {
				if (uHealthMode == 3) {
					showHealth(false);
				}
				if (uStaminaMode == 3) {
					showStamina(false);
				}
				if (uSpecialMode == 3) {
					showSpecial(false);
				}
				if (uShoutIndicatorMode == 3) {
					showShout(false);
				}
			}

			//updateVisibility();

			if (!magickaTimeline.isActive())
			{
				magickaTimeline.restart();
			}
			else
			{
				var playFrom = magickaTimeline.time();
				if (isNaN(playFrom))
				{
					playFrom = 0;
				}
				playFrom = Math.min(playFrom, dynamicBarFadeInTime);
				magickaTimeline.play(playFrom);
			}
		}
	}

	public function showStamina(a_bRefreshOtherBars: Boolean)
	{
		if (uStaminaMode > 0 && uStaminaMode < 4)
		{
			bDisplayedStamina = true;
			if (a_bRefreshOtherBars) {
				if (uHealthMode == 3) {
					showHealth(false);
				}
				if (uMagickaMode == 3) {
					showMagicka(false);
				}
				if (uSpecialMode == 3) {
					showSpecial(false);
				}
				if (uShoutIndicatorMode == 3) {
					showShout(false);
				}
			}

			//updateVisibility();

			if (!staminaTimeline.isActive())
			{
				staminaTimeline.restart();
			}
			else
			{
				var playFrom = staminaTimeline.time();
				if (isNaN(playFrom))
				{
					playFrom = 0;
				}
				playFrom = Math.min(playFrom, dynamicBarFadeInTime);
				staminaTimeline.play(playFrom);
			}
		}
	}

	public function showSpecial(a_bRefreshOtherBars: Boolean)
	{
		showHealth(a_bRefreshOtherBars);

		if (uSpecialMode > 0 && uSpecialMode < 4)
		{
			bDisplayedSpecial = true;
		
			//updateVisibility();

			if (!specialTimeline.isActive())
			{
				specialTimeline.restart();
			}
			else
			{
				var playFrom = specialTimeline.time();
				if (isNaN(playFrom))
				{
					playFrom = 0;
				}
				playFrom = Math.min(playFrom, dynamicBarFadeInTime);
				specialTimeline.play(playFrom);
			}
		}
	}

	public function showShout(a_bRefreshOtherBars: Boolean)
	{
		if (uShoutIndicatorMode > 0 && uShoutIndicatorMode < 4)
		{
			bDisplayedShout = true;
			if (a_bRefreshOtherBars) {
				if (uHealthMode == 3) {
					showHealth(false);
				}
				if (uMagickaMode == 3) {
					showMagicka(false);
				}
				if (uStaminaMode == 3) {
					showStamina(false);
				}
				if (uSpecialMode == 3) {
					showSpecial(false);
				}
			}
			//updateVisibility();

			if (!shoutTimeline.isActive())
			{
				shoutTimeline.restart();
			}
			else
			{
				var playFrom = shoutTimeline.time();
				if (isNaN(playFrom))
				{
					playFrom = 0;
				}
				playFrom = Math.min(playFrom, dynamicBarFadeInTime);
				shoutTimeline.play(playFrom);
			}
		}
	}

	public function showEnchantmentChargeMeter()
	{
		if (bDisplayEnchantmentChargeMeter)
		{
			if (!enchantmentChargeTimeline.isActive())
			{
				enchantmentChargeTimeline.restart();
			}
			else
			{
				var playFrom = enchantmentChargeTimeline.time();
				if (isNaN(playFrom))
				{
					playFrom = 0;
				}
				playFrom = Math.min(playFrom, dynamicBarFadeInTime);
				enchantmentChargeTimeline.play(playFrom);
			}
		}
	}

	public function flashAV(a_actorValue: Number, a_bLong: Boolean)
	{
		switch (a_actorValue)
		{
			case 24: // health
				Health.playFlash(a_bLong);
				break;
			case 25: // magicka
				Magicka.playFlash(a_bLong);
				break;
			case 26: // stamina
				Stamina.playFlash(a_bLong);
				break;
			case 38: // shout
				ShoutIndicator.playFlash(a_bLong);
				break;
		}
	}

	public function flashSpecial(a_bLong: Boolean)
	{
		if (bDisplayedSpecial)
		{
			//specialFlashTimeline.restart();
			Special.SpecialBar.playFlash(a_bLong);
		}
	}

    public function updateData(a_maxHealth: Number,
		a_health: Number,
		a_healthPct: Number,
		a_maxMagicka: Number,
		a_magicka: Number,
		a_magickaPct: Number,
		a_maxStamina: Number,
		a_stamina: Number,
		a_staminaPct: Number,
		a_bIsInCombat: Boolean,
		a_bIsInMountMode: Boolean,
		a_bForce: Boolean)
	{
		if (!bInitialized)
		{
			return;
		}

		var bShouldUpdateWidth = false;
		var bFirstUpdate = false;

		bIsInCombat = a_bIsInCombat;

		if (bIsInMountMode != a_bIsInMountMode) {
			Stamina.setMountMode(a_bIsInMountMode);
			bIsInMountMode = a_bIsInMountMode;
		}
		
		if (a_bForce || (a_health != health || a_maxHealth != maxHealth || a_healthPct != healthPct || a_health != a_maxHealth))
		{
			var bInstant = false;
			if (a_bForce || health == -1) // first update, health not set yet
			{
				bInstant = true;
			}
			else
			{
				showHealth(true);
			}

			Health.updatePercent(a_healthPct, bInstant);

			if (a_maxHealth != maxHealth)
			{
				bShouldUpdateWidth = true;
				if (maxHealth == -1)
				{
					bFirstUpdate = true;
				}
			}
			
			health = a_health;
			maxHealth = a_maxHealth;
			healthPct = a_healthPct;
		}

		if (a_bForce || (a_magicka != magicka || a_maxMagicka != maxMagicka || a_magickaPct != magickaPct || a_magicka != a_maxMagicka))
		{
			var bInstant = false;
			if (a_bForce || magicka == -1) // first update, magicka not set yet
			{
				bInstant = true;
			}
			else
			{
				showMagicka(true);
			}

			Magicka.updatePercent(a_magickaPct, bInstant);

			if (a_maxMagicka != maxMagicka)
			{
				bShouldUpdateWidth = true;
				if (maxMagicka == -1)
				{
					bFirstUpdate = true;
				}
			}
			
			magicka = a_magicka;
			maxMagicka = a_maxMagicka;
			magickaPct = a_magickaPct;
		}

		if (a_bForce || (a_stamina != stamina || a_maxStamina != maxStamina || a_staminaPct != staminaPct || a_stamina != a_maxStamina))
		{
			var bInstant = false;
			if (a_bForce || stamina == -1) // first update, stamina not set yet
			{
				bInstant = true;
			}
			else
			{
				showStamina(true);
			}

			Stamina.updatePercent(a_staminaPct, bInstant);

			if (a_maxStamina != maxStamina)
			{
				bShouldUpdateWidth = true;
				if (maxStamina == -1)
				{
					bFirstUpdate = true;
				}
			}
			
			stamina = a_stamina;
			maxStamina = a_maxStamina;
			staminaPct = a_staminaPct;
		}

		if (bIsInCombat) {
			if (uHealthMode == 2) {
				showHealth(true);
			}
			if (uMagickaMode == 2) {
				showMagicka(true);
			}
			if (uStaminaMode == 2) {
				showStamina(true);
			}
			if (uSpecialMode == 2) {
				showSpecial(true);
			}
			if (uShoutIndicatorMode == 2) {
				showShout(true);
			}
		}

		if (bShouldUpdateWidth)
		{
			updateWidth(bFirstUpdate);
		}
	}

	public function updateSpecial(a_maxSpecial: Number,
		a_special: Number,
		a_specialPct: Number)
	{
		if (!bInitialized)
		{
			return;
		}

		if (a_special != special || a_maxSpecial != maxSpecial || a_specialPct != specialPct)
		{
			Special.SpecialBar.updatePercent(a_specialPct);
			
			special = a_special;
			maxSpecial = a_maxSpecial;
			specialPct = a_specialPct;

			if (uSpecialMode == 1)
			{
				// Display special if changed from default
				if ((bSpecialDefaultMode && special < maxSpecial) || (!bSpecialDefaultMode && special != 0))
				{
					showSpecial(true);
				}
			}

			// Check if special reached threshold
			if ((bSpecialDefaultMode && special <= 0) || (!bSpecialDefaultMode && special >= maxSpecial))
			{
				if (!bSpecialHitThreshold)
				{
					bSpecialHitThreshold = true;
					flashSpecial(true);
				}
			}
			else if (bSpecialHitThreshold) // If it did, and is not at threshold anymore, reset state
			{
				bSpecialHitThreshold = false;
			}
		}
	}

	public function updatePenalty(a_healthPenaltyPct: Number,
		a_magickaPenaltyPct: Number,
		a_staminaPenaltyPct: Number,
		a_bForce: Boolean)
	{
		if (!bInitialized)
		{
			return;
		}

		if (a_bForce || a_healthPenaltyPct != healthPenaltyPct) {
			Health.updatePenaltyPercent(a_healthPenaltyPct);
			healthPenaltyPct = a_healthPenaltyPct;
		}

		if (a_bForce || a_magickaPenaltyPct != magickaPenaltyPct) {
			Magicka.updatePenaltyPercent(a_magickaPenaltyPct);
			magickaPenaltyPct = a_magickaPenaltyPct;
		}

		if (a_bForce || a_staminaPenaltyPct != staminaPenaltyPct) {
			Stamina.updatePenaltyPercent(a_staminaPenaltyPct);
			staminaPenaltyPct = a_staminaPenaltyPct;
		}
	}

	public function updateEnchantmentChargeMeter(a_pct: Number, a_bForce: Boolean, a_bLeftHand: Boolean, a_bShow: Boolean)
	{
		if (bDisplayEnchantmentChargeMeter)
		{
			if (a_bLeftHand) {
				EnchantmentChargeMeter.BarLeft._visible = a_bShow;
				EnchantmentChargeMeter.BackgroundLeft._visible = !a_bShow;
				bDisplayedLeftChargeBar = a_bShow;
			} else {
				EnchantmentChargeMeter.BarRight._visible = a_bShow;
				EnchantmentChargeMeter.BackgroundRight._visible = !a_bShow;
				bDisplayedRightChargeBar = a_bShow;
			}

			// hide meter if both bars are hidden
			if (!bDisplayedLeftChargeBar && !bDisplayedRightChargeBar) {
				enchantmentChargeTimeline.progress(1);
				return;
			}

			if (a_pct > 100) {
				return;
			}

			if (a_bLeftHand) {
				//EnchantmentChargeMeter.BarLeft.ChargeBar.Bar.MaskHolder.Mask._xscale = a_pct;
				//EnchantmentChargeMeter.BarLeft.ChargeBar.Bar.Overlay._x = -((100 - a_pct));
				EnchantmentChargeMeter.BarLeft.updatePercent(a_pct * 0.01);
			} else {
				//EnchantmentChargeMeter.BarRight.ChargeBar.Bar.MaskHolder.Mask._xscale = a_pct;
				//EnchantmentChargeMeter.BarRight.ChargeBar.Bar.Overlay._x = -((100 - a_pct));
				EnchantmentChargeMeter.BarRight.updatePercent(a_pct * 0.01);
			}

			showEnchantmentChargeMeter();
		}
	}

	public function loadConfig(a_uPlayerWidgetHealthBarDirection: Number,
		a_uPlayerWidgetMagickaBarDirection: Number,
		a_uPlayerWidgetStaminaBarDirection: Number,
		a_bPlayerWidgetDisplayPhantomBars: Boolean,
		a_bPlayerWidgetDisplaySpecialPhantomBar: Boolean,
		a_uPlayerWidgetHealthMode: Number,
		a_uPlayerWidgetMagickaMode: Number,
		a_uPlayerWidgetStaminaMode: Number,
		a_uPlayerWidgetShoutIndicatorMode: Number,
		a_uPlayerWidgetSpecialMode: Number,
		a_bPlayerWidgetSpecialDefaultMode: Boolean,
		a_bPlayerWidgetDisplayEnchantmentChargeMeter: Boolean,
		a_fPlayerWidgetPhantomDuration: Number,
		a_fPlayerWidgetScale: Number,
		a_fPlayerWidgetOpacity: Number,
		a_fPlayerWidgetDefaultBarWidth: Number,
		a_fPlayerWidgetSpecialBarWidth: Number,
		a_bPlayerWidgetScaleBars: Boolean,
		a_fPlayerWidgetMinBarWidth: Number,
		a_fPlayerWidgetMaxBarWidth: Number,
		a_fPlayerWidgetHealthBarScaleMult: Number,
		a_fPlayerWidgetMagickaBarScaleMult: Number,
		a_fPlayerWidgetStaminaBarScaleMult: Number,
		a_fPlayerWidgetX: Number,
		a_fPlayerWidgetY: Number,
		a_fPlayerWidgetEnchantmentChargeMeterX: Number,
		a_fPlayerWidgetEnchantmentChargeMeterY: Number,
		a_bPlayerWidgetCombined: Boolean,
		a_fPlayerWidgetHealthX: Number,
		a_fPlayerWidgetHealthY: Number,
		a_fPlayerWidgetMagickaX: Number,
		a_fPlayerWidgetMagickaY: Number,
		a_fPlayerWidgetStaminaX: Number,
		a_fPlayerWidgetStaminaY: Number,
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
		uHealthBarDirection = a_uPlayerWidgetHealthBarDirection;
		uMagickaBarDirection = a_uPlayerWidgetMagickaBarDirection;
		uStaminaBarDirection = a_uPlayerWidgetStaminaBarDirection;
		bDisplayPhantomBars = a_bPlayerWidgetDisplayPhantomBars;
		bDisplaySpecialPhantomBar = a_bPlayerWidgetDisplaySpecialPhantomBar;
		uHealthMode = a_uPlayerWidgetHealthMode;
		uMagickaMode = a_uPlayerWidgetMagickaMode;
		uStaminaMode = a_uPlayerWidgetStaminaMode;
		uShoutIndicatorMode = a_uPlayerWidgetShoutIndicatorMode;
		uSpecialMode = a_uPlayerWidgetSpecialMode;
		bSpecialDefaultMode = a_bPlayerWidgetSpecialDefaultMode;
		bDisplayEnchantmentChargeMeter = a_bPlayerWidgetDisplayEnchantmentChargeMeter;
		fPhantomDuration = a_fPlayerWidgetPhantomDuration;
		fScale = a_fPlayerWidgetScale;
		fOpacity = a_fPlayerWidgetOpacity;
		fDefaultBarWidth = a_fPlayerWidgetDefaultBarWidth;
		fSpecialBarWidth = a_fPlayerWidgetSpecialBarWidth;
		bScaleBars = a_bPlayerWidgetScaleBars;
		fMinBarWidth = a_fPlayerWidgetMinBarWidth;
		fMaxBarWidth = a_fPlayerWidgetMaxBarWidth;
		fHealthBarScaleMult = a_fPlayerWidgetHealthBarScaleMult;
		fMagickaBarScaleMult = a_fPlayerWidgetMagickaBarScaleMult;
		fStaminaBarScaleMult = a_fPlayerWidgetStaminaBarScaleMult;
		fPlayerWidgetX = a_fPlayerWidgetX;
		fPlayerWidgetY = a_fPlayerWidgetY;
		fPlayerWidgetEnchantmentChargeMeterX = a_fPlayerWidgetEnchantmentChargeMeterX;
		fPlayerWidgetEnchantmentChargeMeterY = a_fPlayerWidgetEnchantmentChargeMeterY;
		bPlayerWidgetCombined = a_bPlayerWidgetCombined;
		fPlayerWidgetHealthX = a_fPlayerWidgetHealthX;
		fPlayerWidgetHealthY = a_fPlayerWidgetHealthY;
		fPlayerWidgetMagickaX = a_fPlayerWidgetMagickaX;
		fPlayerWidgetMagickaY = a_fPlayerWidgetMagickaY;
		fPlayerWidgetStaminaX = a_fPlayerWidgetStaminaX;
		fPlayerWidgetStaminaY = a_fPlayerWidgetStaminaY;
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

	public function setColors(
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

        Health.setColor(uHealthColor, uHealthPhantomColor, uHealthBackgroundColor, uHealthPenaltyColor, uHealthFlashColor);
        Magicka.setColor(uMagickaColor, uMagickaPhantomColor, uMagickaBackgroundColor, uMagickaPenaltyColor, uMagickaFlashColor);
        Stamina.setColor(uStaminaColor, uStaminaPhantomColor, uStaminaBackgroundColor, uStaminaPenaltyColor, uStaminaFlashColor);
		Special.SpecialBar.setColor(uSpecialColor, uSpecialPhantomColor, uSpecialBackgroundColor, uSpecialPenaltyColor, uSpecialFlashColor);
	}

	public function updateWidth(a_bInstant: Boolean)
	{
		var healthWidth = fDefaultBarWidth;
		var magickaWidth = fDefaultBarWidth;
		var staminaWidth = fDefaultBarWidth;

		if (bScaleBars)
		{
			// calc width based on max amount
			healthWidth *= (maxHealth / 100);
			magickaWidth *= (maxMagicka / 100);
			staminaWidth *= (maxStamina / 100);

			healthWidth *= fHealthBarScaleMult;
			magickaWidth *= fMagickaBarScaleMult;
			staminaWidth *= fStaminaBarScaleMult;

			// get longest bar
			var longestWidth = healthWidth;
			if (magickaWidth > longestWidth)
			{
				longestWidth = magickaWidth;
			}
			if (staminaWidth > longestWidth)
			{
				longestWidth = staminaWidth;
			}

			if (longestWidth > fMaxBarWidth)
			{
				var rescaleMult = fMaxBarWidth / longestWidth;
				healthWidth *= rescaleMult;
				magickaWidth *= rescaleMult;
				staminaWidth *= rescaleMult;
			}

			// make sure no bar was scaled to be shorter than min width
			healthWidth = Math.max(healthWidth, fMinBarWidth);
			magickaWidth = Math.max(magickaWidth, fMinBarWidth);
			staminaWidth = Math.max(staminaWidth, fMinBarWidth);

			var specialWidth = Math.min(fSpecialBarWidth, healthWidth - 30);
		}

		var healthX: Number;
		var magickaX: Number;
		var staminaX: Number;

		if (bPlayerWidgetCombined)
		{
			healthX = 50 + healthWidth / 2;
			magickaX = 50 + magickaWidth / 2;
			staminaX = 50 + staminaWidth / 2;
		}
		else
		{
			switch (uHealthBarDirection)
			{
			case 0: // normal
				healthX = (fPlayerWidgetHealthX / fScale) + healthWidth / 2;
				break;
			case 1: // centered
				healthX = fPlayerWidgetHealthX / fScale;
				break;
			case 2: // reverse
				healthX = (fPlayerWidgetHealthX / fScale) - healthWidth / 2;
				break;
			}

			switch (uMagickaBarDirection)
			{
			case 0: // normal
				magickaX = (fPlayerWidgetMagickaX / fScale) + magickaWidth / 2;
				break;
			case 1: // centered
				magickaX = fPlayerWidgetMagickaX / fScale;
				break;
			case 2: // reverse
				magickaX = (fPlayerWidgetMagickaX / fScale) - magickaWidth / 2;
				break;
			}

			switch (uStaminaBarDirection)
			{
			case 0: // normal
				staminaX = (fPlayerWidgetStaminaX / fScale) + staminaWidth / 2;
				break;
			case 1: // centered
				staminaX = fPlayerWidgetStaminaX / fScale;
				break;
			case 2: // reverse
				staminaX = (fPlayerWidgetStaminaX / fScale) - staminaWidth / 2;
				break;
			}
		}

		if (a_bInstant)
		{			
			Health.setWidth(healthWidth);
			Magicka.setWidth(magickaWidth);
			Stamina.setWidth(staminaWidth);
			Health._x = healthX;
			Magicka._x = magickaX;
			Stamina._x = staminaX;

			// Special
			SpecialMask._width = healthWidth;
			SpecialMask._x = healthX;
			Special.SpecialBar.setWidth(specialWidth);

			Special.Frame._width = specialWidth + specialFrameWidthOffset;
			Special.Outline._width = specialWidth + specialFrameOutlineWidthOffset;

			switch (uHealthBarDirection)
			{
			case 0: // normal
				Special._x = healthX + Math.min(specialXOffset, ((healthWidth / 2))) - (healthWidth / 2);
				break;
			case 1: // centered
				Special._x = healthX;
				break;
			case 2: // reverse
				Special._x = healthX - Math.min(specialXOffset, ((healthWidth / 2))) + (healthWidth / 2);
				break;
			}
		}
		else
		{
			Health.setWidthTimeline(healthWidth);
			Magicka.setWidthTimeline(magickaWidth);
			Stamina.setWidthTimeline(staminaWidth);

			Special.SpecialBar.setWidthTimeline(specialWidth);

			widthTimeline.clear();
			var widthTimelineDuration = 0.25;

        	widthTimeline.to(Health, widthTimelineDuration, {_x:healthX, ease:Sine.easeIn}, 0);
			widthTimeline.to(Magicka, widthTimelineDuration, {_x:magickaX, ease:Sine.easeIn}, 0);
			widthTimeline.to(Stamina, widthTimelineDuration, {_x:staminaX, ease:Sine.easeIn}, 0);

			widthTimeline.to(SpecialMask, widthTimelineDuration, {_x:healthX, _width:healthWidth, ease:Sine.easeIn}, 0);
			widthTimeline.to(Special.Frame, widthTimelineDuration, {_width:specialWidth + specialFrameWidthOffset, ease:Sine.easeIn}, 0);
			widthTimeline.to(Special.Outline, widthTimelineDuration, {_width:specialWidth + specialFrameOutlineWidthOffset, ease:Sine.easeIn}, 0);

			switch (uHealthBarDirection)
			{
			case 0: // normal
				widthTimeline.to(Special, widthTimelineDuration, {_x:healthX + Math.min(specialXOffset, ((healthWidth / 2))) - (healthWidth / 2), ease:Sine.easeIn}, 0);
				break;
			case 1: // centered
				widthTimeline.to(Special, widthTimelineDuration, {_x:healthX, ease:Sine.easeIn}, 0);
				break;
			case 2: // reverse
				widthTimeline.to(Special, widthTimelineDuration, {_x:healthX - Math.min(specialXOffset, ((healthWidth / 2))) + (healthWidth / 2), ease:Sine.easeIn}, 0);
				break;
			}

			widthTimeline.restart();
		}
	}

	public function setProgress(a_progress: Number)
	{
		ShoutIndicator.setProgress(a_progress);

		if (a_progress != 1) 
		{
			showShout(true);
		}
	}

	public function isInitialized() : Boolean
	{
		return bInitialized;
	}
}
