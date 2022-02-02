import com.greensock.TimelineLite;
import com.greensock.easing.*;

class Widgets.TrueHUD_ShoutIndicator extends MovieClip
{
	public var Glow: MovieClip;
	public var GlowRing: MovieClip;
	public var InitialRing: MovieClip;
	public var OutlineFlash: MovieClip;
	public var FrameFlash: MovieClip;

	var progressTimeline: TimelineLite;
	var cooldownStartTimeline: TimelineLite;
	var cooldownCompleteFlashTimeline: TimelineLite;
	var flashTimeline: TimelineLite;
	var fadeOutTimeline: TimelineLite;

	var fOpacity: Number;

	var bStandalone = false;
	var bInitialized = false;

	public function TrueHUD_ShoutIndicator() 
	{
		// constructor code
		this.stop();
		
		progressTimeline = new TimelineLite({paused:true});
		cooldownStartTimeline = new TimelineLite({paused:true});
		cooldownCompleteFlashTimeline = new TimelineLite({paused:true});
		flashTimeline = new TimelineLite({paused:true});
	}

	public function init()
	{
		Glow._visible = false;
		Glow.Background._xscale = 0;
		Glow.Background._yscale = 0;
		Glow.Mask._xscale = 15
		Glow.Mask._yscale = 15;
		GlowRing._xscale = 15;
		GlowRing._yscale = 15;
		InitialRing._xscale = 0;
		InitialRing._yscale = 0;
		OutlineFlash._alpha = 0;
		FrameFlash._alpha = 0;

		progressTimeline.clear();

		progressTimeline.set(Glow.Mask, {_xscale:15, _yscale:15}, 0);
		progressTimeline.set(GlowRing, {_xscale:15, _yscale:15}, 0);
		progressTimeline.set(FrameFlash, {_alpha:0}, 0);
		progressTimeline.to(Glow.Mask, 80, {_xscale:75, _yscale:75}, 0);
		progressTimeline.to(Glow.Mask, 20, {_xscale:100, _yscale:100}, 80);
		progressTimeline.to(GlowRing, 80, {_xscale:75, _yscale:75}, 0);
		progressTimeline.to(GlowRing, 20, {_xscale:100, _yscale:100}, 80);
		progressTimeline.to(FrameFlash, 20, {_alpha:100}, 80);

		progressTimeline.set(Glow.Mask, {_xscale:15, _yscale:15}, 100);
		progressTimeline.set(GlowRing, {_xscale:15, _yscale:15}, 100);
		progressTimeline.set(GlowRing, {_xscale:15, _yscale:15}, 100);
		progressTimeline.set(FrameFlash, {_alpha:0}, 100);
		progressTimeline.set(Glow, {_visible:false}, 100);

		progressTimeline.eventCallback("onStart", playCooldownStart, null, this);
		progressTimeline.eventCallback("onComplete", playCooldownFinishedFlash, null, this);
		progressTimeline.progress(1, true);


		cooldownStartTimeline.clear();
		cooldownStartTimeline.set(Glow, {_visible:true}, 0);
		cooldownStartTimeline.set(Glow.Background, {_xscale:0, _yscale:0}, 0);
		cooldownStartTimeline.set(InitialRing, {_xscale:0, _yscale:0}, 0);
		cooldownStartTimeline.to(Glow.Background, 0.5, {_xscale:100, _yscale:100}, 0);
		cooldownStartTimeline.to(InitialRing, 0.5, {_xscale:100, _yscale:100}, 0);
		cooldownStartTimeline.to(InitialRing, 0.25, {_xscale:150, _yscale:150}, 0.5);
		cooldownStartTimeline.set(InitialRing, {_xscale:0, _yscale:0}, 0.75);
		cooldownStartTimeline.set(Glow, {_visible:false}, 0.75);

		cooldownCompleteFlashTimeline.clear();
		cooldownCompleteFlashTimeline.set(Glow, {_visible:false}, 0);
		cooldownCompleteFlashTimeline.set(OutlineFlash, {_alpha:0}, 0);
		cooldownCompleteFlashTimeline.set(FrameFlash, {_alpha:100}, 0);
		cooldownCompleteFlashTimeline.to(OutlineFlash, 0.25, {_alpha:100}, 0);
		cooldownCompleteFlashTimeline.to(OutlineFlash, 0.5, {_alpha:0}, 0.25);
		cooldownCompleteFlashTimeline.to(FrameFlash, 0.5, {_alpha:0}, 0.25);

		flashTimeline.clear();
		flashTimeline.set(FrameFlash, {_alpha:0}, 0);
		flashTimeline.to(FrameFlash, 0.30, {_alpha:100}, 0);
		flashTimeline.to(FrameFlash, 0.30, {_alpha:0}, 0.30);
		flashTimeline.to(FrameFlash, 0.30, {_alpha:100}, 0.60);
		flashTimeline.to(FrameFlash, 0.30, {_alpha:0}, 0.90);

		if (bStandalone)
		{
			this._alpha = 0;

			if (fadeOutTimeline == undefined)
			{
				fadeOutTimeline = new TimelineLite({paused:true});
			}

			fadeOutTimeline.clear();
			fadeOutTimeline.set(this, {_alpha: fOpacity}, 0);
			fadeOutTimeline.to(this, 1, {_alpha: 0}, 3);
		}

		//bInitialized = true;
	}

	// only called in the standalone bossbar variant
	public function loadConfig(a_fOpacity: Number)
	{
		fOpacity = a_fOpacity;

		bStandalone = true;

		// delay init till everything's ready
		this.onEnterFrame = function(): Void {
			init();
			delete this.onEnterFrame;
		}
	}

	public function cleanUp()
	{
		progressTimeline.eventCallback("onStart", null);
		progressTimeline.eventCallback("onComplete", null);
		progressTimeline.clear();
		progressTimeline.kill();
		progressTimeline = null;

		cooldownStartTimeline.clear();
		cooldownStartTimeline.kill();
		cooldownStartTimeline = null;

		cooldownCompleteFlashTimeline.clear();
		cooldownCompleteFlashTimeline.kill();
		cooldownCompleteFlashTimeline = null;

		flashTimeline.clear();
		flashTimeline.kill();
		flashTimeline = null;

		if (bStandalone)
		{
			fadeOutTimeline.clear();
			fadeOutTimeline.kill();
			fadeOutTimeline = null;
		}
	}

	public function setProgress(a_progress: Number)
	{
		if (bInitialized == false)
		{
			progressTimeline.progress(a_progress, true);
			if (a_progress != 1)
			{
				playCooldownStart();
			}
			bInitialized = true;
		}
		progressTimeline.progress(a_progress);

		if (bStandalone && a_progress < 1)
		{
			fadeOutTimeline.restart();
		}
	}

	public function playCooldownStart()
	{
		cooldownStartTimeline.restart();
	}

	public function playCooldownFinishedFlash()
	{
		cooldownCompleteFlashTimeline.restart();
	}

	public function playFlash(a_bLong: Boolean)
	{
		flashTimeline.restart();
	}
}
