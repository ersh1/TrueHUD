import com.greensock.TweenLite;
import com.greensock.TimelineLite;
import com.greensock.easing.*;

class Widgets.TrueHUD_InfoBarBase extends MovieClip
{
	public var Bars: MovieClip;
	public var TargetName: TextField;
	public var DamageCounter: TextField;
	public var Indicator: MovieClip;
	public var IndicatorMask: MovieClip;

	var showHideTimeline: TimelineLite;
	var toggleWidgetTimeline: TimelineLite;
	var targetKilledTimeline: TimelineLite;
	var damageTimeline: TimelineLite;	
	var resourcesTween: TweenLite;
	var specialTween: TweenLite;
	var nameYTween: TweenLite;
	var damageYTween: TweenLite;

	var targetType: Number;
	var widgetState: Number = 2; // hidden
	var depth: Number;
	var targetLevel: Number;
	var levelColor: Number;
	var outlineColor: Number;
	var soulType: Number;
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
	var damage: Number = 0;

	var bInitialized = false;

	var bHidden = false;
	var bReadyToRemove = false;
	
	var uResourcesMode = 0;
	var uSpecialMode = 0;
	var fCurrentWidth = 0;
	var fCurrentResourceWidth = 0;
	var uDisplayedResources = 0;
	var bDisplayedSpecial = false;
	var fDesiredNameY;
	var fDesiredDamageCounterY;
	var bMagickaChangedFromDefault = false;
	var bStaminaChangedFromDefault = false;
	var bSpecialChangedFromDefault = false;
	var bSpecialHitThreshold = false;
	
	var uBarDirection: Number;
	var bDisplayPhantomBars: Boolean = false;
	var bDisplaySpecialPhantomBar: Boolean = false;
	var bDisplayDamageCounter: Boolean = false;
	var bDisplayIndicator: Boolean = false;
	var uDamageCounterAlignment: Number;
	var bSpecialDefaultMode: Boolean;
	var uIndicatorMode: Number;
	var fPhantomDuration: Number;
	var fDamageCounterDuration: Number;
	var fOpacity: Number;
	var fWidth: Number;
	var fResourceWidth: Number;
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
	
	var bHealthBarHasShadow = false;
	var bHealthBarHasShapeMask = false;
	var bHealthBarHasFlash = false;
	
	var magickaWidthOffset = 6;
	var	specialNameOffset = -6;
	var	resourcesYOffsetOne = 6;
	var	resourcesYOffsetTwo = 9;
	var resourcesXOffsetOne = 12; //17
	var resourcesXOffsetTwo = 18; //23

	var	resourcesFrameWidthOffset: Number;
	var	resourcesFrameOutlineWidthOffset: Number;
	var	specialFrameWidthOffset: Number;
	var	specialFrameOutlineWidthOffset: Number;
	var specialOutlineFlashWidthOffset: Number;
	var frameWidthOffset: Number;
	var outlineWidthOffset: Number;
	var healthXOffset = 0;
	var killDelay = 0;
	var nameY = 0;
	var damageCounterY = 0;
	var resourcesYNone = 0;
	var resourcesYStamina = 0;
	var resourcesYBoth = 0;
	var specialYHidden = 0;
	var specialYShown = 0;

	public function TrueHUD_InfoBarBase() 
	{
		// constructor code
		this.stop();
		
		showHideTimeline = new TimelineLite({paused:true});
		toggleWidgetTimeline = new TimelineLite({paused:true});
		targetKilledTimeline = new TimelineLite({paused:true});
		damageTimeline = new TimelineLite({paused:true});
		
		Indicator._visible = false;
		IndicatorMask._visible = false;

		TargetName._alpha = 0;
		DamageCounter._alpha = 0;

		resourcesFrameWidthOffset = Bars.Resources.Frame._width - 100;
		resourcesFrameOutlineWidthOffset = Bars.Resources.Outline._width - 100;
		specialFrameWidthOffset = Bars.Special.Frame._width - 100;
		specialFrameOutlineWidthOffset = Bars.Special.Outline._width - 100;
		specialOutlineFlashWidthOffset = Bars.Special.SpecialBar.Flash._width - 100;
		frameWidthOffset = Bars.HealthBar.Frame._width - 100;
		outlineWidthOffset = Bars.HealthBar.Outline._width - 100;	

		//Bars.Special.Flash._alpha = 0;
		//Bars.Special.Flash._y = 10;
		targetType = -1;
		fOpacity = 0;
		_alpha = 0;
	}

	public function init()
	{
		//TargetName._visible = bShowName;
		//Bars.PhantomBar._visible = bShowPhantom;
		//DamageCounter._visible = bDisplayDamageCounter;

		Bars.HealthBar.init(bDisplayPhantomBars, fPhantomDuration, uBarDirection, true, bHealthBarHasShadow, bHealthBarHasShapeMask, bHealthBarHasFlash, false);
		Bars.HealthBar.setColor(uHealthColor, uHealthPhantomColor, uHealthBackgroundColor, uHealthPenaltyColor, uHealthFlashColor);

		Bars.Resources.MagickaBar.init(bDisplayPhantomBars, fPhantomDuration, uBarDirection, false, false, true, false, false);
		Bars.Resources.MagickaBar.setColor(uMagickaColor, uMagickaPhantomColor, uMagickaBackgroundColor, uMagickaPenaltyColor, uMagickaFlashColor);

		Bars.Resources.StaminaBar.init(bDisplayPhantomBars, fPhantomDuration, uBarDirection, false, false, true, false, false);
		Bars.Resources.StaminaBar.setColor(uStaminaColor, uStaminaPhantomColor, uStaminaBackgroundColor, uStaminaPenaltyColor, uStaminaFlashColor);

		Bars.Special.SpecialBar.init(bDisplaySpecialPhantomBar, fPhantomDuration, uBarDirection, false, false, true, true, false);
		Bars.Special.SpecialBar.setColor(uSpecialColor, uSpecialPhantomColor, uSpecialBackgroundColor, uSpecialPenaltyColor, uSpecialFlashColor);

		switchIndicatorMode();

		showHideTimeline.clear();
		showHideTimeline.set(this, {_alpha:0}, 0);
		showHideTimeline.to(this, 0.25, {_alpha:fOpacity}, 0);
		showHideTimeline.eventCallback("onStart", setHidden, [false], this);
		showHideTimeline.eventCallback("onReverseComplete", setHidden, [true], this);

		toggleWidgetTimeline.clear();
		toggleWidgetTimeline.set(this, {_alpha:0}, 0);
		toggleWidgetTimeline.to(this, 0.25, {_alpha:fOpacity}, 0);
		toggleWidgetTimeline.eventCallback("onStart", setReadyToRemove, [false], this);
		toggleWidgetTimeline.eventCallback("onReverseComplete", setReadyToRemove, [true], this);

		targetKilledTimeline.clear();
		targetKilledTimeline.to(this, 0.25, {_alpha:0}, killDelay);
		targetKilledTimeline.call(setReadyToRemove, [true], this);

		damageTimeline.clear();
		damageTimeline.set(DamageCounter, {_alpha:100}, 0);
		damageTimeline.to(DamageCounter, 0, {_alpha:0}, fDamageCounterDuration);
		damageTimeline.eventCallback("onComplete", resetDamage, null, this);

		var format = new TextFormat();
		switch (uDamageCounterAlignment) 
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

		DamageCounter.setTextFormat(format);
		DamageCounter.setNewTextFormat(format);

		bInitialized = true;
	}

	public function cleanUp()
	{
		Bars.HealthBar.cleanUp();
		Bars.Resources.MagickaBar.cleanUp();
		Bars.Resources.StaminaBar.cleanUp();
		Bars.Special.SpecialBar.cleanUp();

		showHideTimeline.eventCallback("onStart", null);
		showHideTimeline.eventCallback("onReverseComplete", null);
		showHideTimeline.clear();
		showHideTimeline.kill();
		showHideTimeline = null;

		toggleWidgetTimeline.eventCallback("onStart", null);
		toggleWidgetTimeline.eventCallback("onReverseComplete", null);
		toggleWidgetTimeline.clear();
		toggleWidgetTimeline.kill();
		toggleWidgetTimeline = null;

		targetKilledTimeline.clear();
		targetKilledTimeline.kill();
		targetKilledTimeline = null;

		damageTimeline.clear();
		damageTimeline.kill();
		damageTimeline = null;

		resourcesTween.kill();
		resourcesTween = null;

		specialTween.kill();
		specialTween = null;

		nameYTween.kill();
		nameYTween = null;

		damageYTween.kill();
		damageYTween = null;
	}

	public function setHidden(a_hidden: Boolean)
	{
		bHidden = a_hidden;
	}

	public function setReadyToRemove(a_readyToRemove: Boolean)
	{
		bReadyToRemove = a_readyToRemove;
	}

	public function toggleVisibility(a_show: Boolean)
	{
		if (a_show)
		{
			if (showHideTimeline.progress() == 1) {
				showHideTimeline.play(0);
			} else {
				showHideTimeline.play();
			}
		}
		else
		{
			if (showHideTimeline.progress() == 0) {
				showHideTimeline.reverse(0);
			} else {
				showHideTimeline.reverse();
			}
		}
	}

	public function toggleWidget(a_show: Boolean)
	{
		if (a_show)
		{
			if (toggleWidgetTimeline.progress() == 1) {
				toggleWidgetTimeline.play(0);
			} else {
				toggleWidgetTimeline.play();
			}
		}
		else
		{
			if (toggleWidgetTimeline.progress() == 0) {
				toggleWidgetTimeline.reverse(0);
			} else {
				toggleWidgetTimeline.reverse();
			}
		}
	}

	public function targetKilled()
	{
		if (uDisplayedResources > 0)
		{
			moveResources(0);
		}

		if (bDisplayedSpecial)
		{
			moveSpecial(false);
		}

		targetKilledTimeline.play();
	}

	public function switchIndicatorMode()
	{
		switch(uIndicatorMode)
		{
		case 0: // soul icon
			Indicator.SoulIcon._visible = true;
			Indicator.LevelText._visible = false;
			Indicator.LevelIcon._visible = false;
			break;
		case 1: // level text
			Indicator.SoulIcon._visible = false;
			Indicator.LevelText._visible = true;
			Indicator.LevelIcon._visible = false;
			break;
		case 2: // square icon
			Indicator.SoulIcon._visible = false;
			Indicator.LevelText._visible = false;
			Indicator.LevelIcon._visible = true;
			break;
		}
	}

	public function toggleIndicator(a_enable: Boolean)
	{
		// Do nothing in base class
	}

	public function updateTargetType(a_newTargetType: Number)
	{
		targetType = a_newTargetType;
	}

	public function calculateDamageCounterY() : Number
	{
		switch (uDisplayedResources)
		{
		case 0: // none
			return damageCounterY;
			break;
		case 1: // stamina
			return damageCounterY + resourcesYOffsetOne;
			break;
		case 2: // both
			return damageCounterY + resourcesYOffsetTwo;
			break;
		}
	}

	public function calculateNameY() : Number
	{
		if (bDisplayedSpecial)
		{
			return nameY + specialNameOffset;
		}
		else
		{
			return nameY;
		}
	}

	public function updateDamageY()
	{
		var damageCounterYWithOffset = calculateDamageCounterY();

		if (fDesiredDamageCounterY != damageCounterYWithOffset)
		{
			fDesiredDamageCounterY = damageCounterYWithOffset;
			damageYTween = TweenLite.to(DamageCounter, 0.1, {_y:damageCounterYWithOffset, ease:Sine.easeIn});
		}
	}

	public function updateNameY()
	{
		var nameYWithOffset = calculateNameY();

		if (fDesiredNameY != nameYWithOffset)
		{
			fDesiredNameY = nameYWithOffset;
			nameYTween = TweenLite.to(TargetName, 0.1, {_y:nameYWithOffset, ease:Sine.easeIn});
		}
	}

	public function moveResources(a_mode: Number)
	{
		switch (a_mode)
		{
		case 0: // none
			uDisplayedResources = 0;
			resourcesTween = TweenLite.to(Bars.Resources, 0.25, {_y: resourcesYNone, ease:Sine.easeIn}, 0);
			break;

		case 1: // stamina
			uDisplayedResources = 1;
			resourcesTween = TweenLite.to(Bars.Resources, 0.25, {_y: resourcesYStamina, ease:Sine.easeIn}, 0);
			break;

		case 2: // magicka and stamina
			uDisplayedResources = 2;
			resourcesTween = TweenLite.to(Bars.Resources, 0.25, {_y: resourcesYBoth, ease:Sine.easeIn}, 0);
			break;
		}

		updateDamageY();
	}

	public function toggleResources(a_mode: Number)
	{
		if (a_mode == uResourcesMode)
		{
			return;
		}

		uResourcesMode = a_mode;

		switch (a_mode)
		{
		case 0: // never
			if (uDisplayedResources != 0)
			{
				moveResources(0);
			}
			break;
		
		case 1: // when changed from max
			if (bMagickaChangedFromDefault)
			{
				if (uDisplayedResources < 2)
				{
					moveResources(2);
				}
			}
			else if (bStaminaChangedFromDefault)
			{
				if (uDisplayedResources < 1)
				{
					moveResources(1);
				}
			}
			else
			{
				if (uDisplayedResources > 0)
				{
					moveResources(0);
				}
			}
			break;

		case 2: // always
			if (uDisplayedResources < 2)
			{
				moveResources(2);
			}
			break;
		}
	}

	public function moveSpecial(a_enable: Boolean)
	{
		if (a_enable)
		{
			bDisplayedSpecial = true;
			Bars.Special.Flash._visible = true;
			specialTween = TweenLite.to(Bars.Special, 0.25, {_y:specialYShown, ease:Sine.easeIn});
		}
		else
		{
			bDisplayedSpecial = false;
			Bars.Special.Flash._visible = false;
			specialTween = TweenLite.to(Bars.Special, 0.25, {_y:specialYHidden, ease:Sine.easeIn});
		}

		updateNameY();
	}

	public function toggleSpecial(a_mode: Number)
	{
		if (a_mode == uSpecialMode)
		{
			return;
		}

		uSpecialMode = a_mode;

		switch (a_mode)
		{
		case 0: // never
			if (bDisplayedSpecial == true)
			{
				moveSpecial(false);
			}
			break;
		
		case 1: // when changed from max
			if (bSpecialChangedFromDefault && bDisplayedSpecial == false)
			{
				moveSpecial(true);
			}
			else if (bDisplayedSpecial == true)
			{
				moveSpecial(false);
			}
			break;

		case 2: // always
			if (bDisplayedSpecial == false)
			{
				moveSpecial(true);
			}
			break;
		}
	}

	public function flashAV(a_actorValue: Number, a_bLong: Boolean)
	{
		switch (a_actorValue)
		{
			case 24: // health
				Bars.HealthBar.playFlash(a_bLong);
				break;
			case 25: // magicka
				Bars.Resources.MagickaBar.playFlash(a_bLong);
				break;
			case 26: // stamina
				Bars.Resources.StaminaBar.playFlash(a_bLong);
				break;
		}
	}

	public function flashSpecial(a_bLong: Boolean)
	{
		if (bDisplayedSpecial)
		{
			//specialFlashTimeline.restart();
			Bars.Special.SpecialBar.playFlash(a_bLong);
		}
	}

	public function updateData(a_targetType: Number,
		a_widgetState: Number,
		a_depth: Number,
		a_targetState: Number,
		a_targetName: String,
		a_targetLevel: Number,
		a_levelColor: Number,
		a_outlineColor: Number,
		a_soulType: Number,
		a_maxHealth: Number,
		a_health: Number,
		a_healthPct: Number,
		a_maxMagicka: Number,
		a_magicka: Number,
		a_magickaPct: Number,
		a_maxStamina: Number,
		a_stamina: Number,
		a_staminaPct: Number)
	{
		depth = a_depth;

		if (!bInitialized)
		{
			return;
		}
		
		if (a_widgetState != widgetState)
		{
			if (widgetState < 3)
			{
				if (a_widgetState == 1) // now should be hidden
				{
					toggleVisibility(false);
					
				}
				else if (a_widgetState == 0) // widget hidden, should be made visible
				{
					toggleVisibility(true);
				}
			}

			if (a_widgetState == 0 && widgetState == 4) // was pending removal, should go back to active
			{
				toggleWidget(true);
			}
			else if (a_widgetState == 4) // pending removal
			{
				toggleWidget(false);
			}
			else if (a_widgetState == 3) // delayed removal
			{
				targetKilled();
			}
			widgetState = a_widgetState;
		}

		if (TargetName.text != a_targetName) 
		{
			TargetName.text = a_targetName;
			updateNameY();
		}
		
		soulType = a_soulType;

		if (a_targetType != targetType)
		{
			updateTargetType(a_targetType);
		}

		if (a_targetLevel != targetLevel)
		{
			targetLevel = a_targetLevel;
			if (uIndicatorMode == 1)
			{
				Indicator.LevelText.text = targetLevel.toString();
			}
			else if (uIndicatorMode == 0)
			{
				Indicator.SoulIcon._visible = true;
				Indicator.SoulIcon.IconDragon._visible = false;
				Indicator.SoulIcon.IconBlack._visible = false;
				Indicator.SoulIcon.IconGrand._visible = false;
				Indicator.SoulIcon.IconGreater._visible = false;
				Indicator.SoulIcon.IconCommon._visible = false;
				Indicator.SoulIcon.IconLesser._visible = false;
				Indicator.SoulIcon.IconPetty._visible = false;
				Indicator.SoulIcon.IconNone._visible = false;

				switch (a_soulType)
				{
				case 0:
					Indicator.SoulIcon.IconNone._visible = true;
					break;
				case 1:
					Indicator.SoulIcon.IconPetty._visible = true;
					break;
				case 2:
					Indicator.SoulIcon.IconLesser._visible = true;
					break;
				case 3:
					Indicator.SoulIcon.IconCommon._visible = true;
					break;
				case 4:
					Indicator.SoulIcon.IconGreater._visible = true;
					break;
				case 5:
					Indicator.SoulIcon.IconGrand._visible = true;
					break;
				case 6:
					Indicator.SoulIcon.IconBlack._visible = true;
					break;
				case 7:
					Indicator.SoulIcon.IconDragon._visible = true;
					break;
				}
			}
		}

		if (a_levelColor != levelColor || a_outlineColor != outlineColor)
		{
			levelColor = a_levelColor;
			outlineColor = a_outlineColor;
			updateColor();
		}

		if (a_health != health || a_maxHealth != maxHealth || a_healthPct != healthPct)
		{
			var newDamage = health - a_health;
			var bInstant = false;
			if (health == -1) // first update, health not set yet
			{
				newDamage = a_maxHealth - a_health;
				bInstant = true;
			}
			
			if (bDisplayDamageCounter && newDamage > 0)
			{
				addDamage(newDamage);
			}

			Bars.HealthBar.updatePercent(a_healthPct, bInstant);
			
			health = a_health;
			maxHealth = a_maxHealth;
			healthPct = a_healthPct;
		}

		if (a_magicka != magicka || a_maxMagicka != maxMagicka || a_magickaPct != magickaPct)
		{
			var bInstant = false;
			if (magicka == -1) // first update, magicka not set yet
			{
				bInstant = true;
			}

			Bars.Resources.MagickaBar.updatePercent(a_magickaPct, bInstant);
			
			magicka = a_magicka;
			maxMagicka = a_maxMagicka;
			magickaPct = a_magickaPct;

			if (!bMagickaChangedFromDefault && magicka < maxMagicka) {
				bMagickaChangedFromDefault = true;

				if (uResourcesMode == 1 && uDisplayedResources < 2)
				{
					moveResources(2);
				}
			}
		}

		if (a_stamina != stamina || a_maxStamina != maxStamina || a_staminaPct != staminaPct)
		{
			var bInstant = false;
			if (staminaPct == -1) // first update, stamina not set yet
			{
				bInstant = true;
			}

			Bars.Resources.StaminaBar.updatePercent(a_staminaPct, bInstant);
			
			stamina = a_stamina;
			maxStamina = a_maxStamina;
			staminaPct = a_staminaPct;

			if (!bStaminaChangedFromDefault && stamina < maxStamina) {
				bStaminaChangedFromDefault = true;

				if (uResourcesMode == 1 && uDisplayedResources < 1)
				{
					moveResources(1);
				}
			}
		}

		if (a_targetState > 2) // Essential and bleeding out, or Dead
		{
			targetKilled();
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
			var bInstant = false;
			if (specialPct == -1) // first update, special not set yet
			{
				bInstant = true;
			}

			Bars.Special.SpecialBar.updatePercent(a_specialPct, bInstant);
			
			special = a_special;
			maxSpecial = a_maxSpecial;
			specialPct = a_specialPct;

			// Check if special changed from default
			if (!bSpecialChangedFromDefault && ((bSpecialDefaultMode && special < maxSpecial) || (!bSpecialDefaultMode && special != 0))) {
				bSpecialChangedFromDefault = true;

				if (uSpecialMode == 1 && bDisplayedSpecial == false)
				{
					moveSpecial(true);
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

	public function updateColor()
	{
		// outline color
		var healthBarOutlineColor: Color = new Color(Bars.HealthBar.Outline);
		var resourcesOutlineColor: Color = new Color(Bars.Resources.Outline);
		var specialBarOutlineColor: Color = new Color(Bars.Special.Outline);
		healthBarOutlineColor.setRGB(outlineColor);
		resourcesOutlineColor.setRGB(outlineColor);
		specialBarOutlineColor.setRGB(outlineColor);

		switch (uIndicatorMode)
		{
		case 0: // soul icon
			var color: Color = new Color(Indicator.Frame.Outline);
			color.setRGB(outlineColor);
			break;
		case 1: // level text
			Indicator.LevelText.textColor = levelColor;
			var color: Color = new Color(Indicator.Frame.Outline);
			color.setRGB(outlineColor);
			break;
		case 2: // square icon
			var iconColor: Color = new Color(Indicator.LevelIcon);
			iconColor.setRGB(levelColor);
			var color: Color = new Color(Indicator.Frame.Outline);
			color.setRGB(outlineColor);
			break;
		}	
	}
		
	public function addDamage(a_damage: Number)
	{
		damage = damage + a_damage;
		DamageCounter.text = damage.toString();
		damageTimeline.restart();

		updateDamageY();
	}

	public function resetDamage()
	{
		damage = 0;
	}
	
	public function isHidden() : Boolean
	{
		return bHidden;
	}

	public function isReadyToRemove() : Boolean
	{
		return bReadyToRemove;
	}
	
	public function setWidth(a_width: Number, a_resourceWidth: Number)
	{	
		fCurrentWidth = a_width;
		fCurrentResourceWidth = a_resourceWidth;

		Bars.HealthBar.setWidth(a_width);
		Bars.Resources.MagickaBar.setWidth(a_resourceWidth + magickaWidthOffset);
		Bars.Resources.StaminaBar.setWidth(a_resourceWidth);
		Bars.Special.SpecialBar.setWidth(a_resourceWidth);

		// Resources
		Bars.ResourceMask._width = a_resourceWidth + resourcesFrameWidthOffset;
		Bars.Resources.Frame._width = a_resourceWidth + resourcesFrameWidthOffset;
		Bars.Resources.Outline._width = a_resourceWidth + resourcesFrameOutlineWidthOffset;

		// Special
		Bars.Special.Frame._width = a_resourceWidth + specialFrameWidthOffset;
		Bars.Special.Outline._width = a_resourceWidth + specialFrameOutlineWidthOffset;

		// Damage counter
		var damageCounterWidth = a_width + healthXOffset;
		DamageCounter._width = damageCounterWidth;
		DamageCounter._x = -damageCounterWidth / 2;

		updateNameY();
		updateDamageY();
	}
}
