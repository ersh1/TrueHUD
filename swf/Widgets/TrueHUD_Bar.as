import com.greensock.TimelineLite;
import com.greensock.easing.*;
import flash.geom.*

class Widgets.TrueHUD_Bar extends MovieClip
{
    public var Flash: MovieClip;
    public var Shadow: MovieClip;
    public var Mask: MovieClip;
    public var Bar: MovieClip;
    public var Penalty: MovieClip;
    public var Phantom: MovieClip;
    public var Background: MovieClip;
    public var Outline: MovieClip;
    public var Frame: MovieClip;

    public var FlashMount: MovieClip;
    public var OutlineMount: MovieClip;
    public var FrameMount: MovieClip;
    
    var phantomTimeline: TimelineLite;
    var widthTimeline: TimelineLite;
    var flashTimeline: TimelineLite;
    var shortFlashTimeline: TimelineLite;
    var spamCooldownTimeline: TimelineLite;

    var uBarDirection: Number;
    var bStandalone: Boolean;
    var bHasShadow: Boolean;
    var bHasShapeMask: Boolean;
    var bHasFlash: Boolean;
    var bHasPenaltyBar: Boolean;
    var fPhantomDuration: Number;
    var bShowPhantomBar: Boolean;
    var fFrameWidthOffset: Number;
    var fOutlineWidthOffset: Number;
    var fOutlineFlashWidthOffset: Number;
    
    var fFrameWidthMountOffset: Number;
    var fOutlineWidthMountOffset: Number;
    var fOutlineFlashWidthMountOffset: Number;

    var percent: Number;
    var desiredPhantomScale: Number;
    var bMountMode: Boolean = false;

    var spamCooldownDuration = 0.1;
    var widthTimelineDuration = 0.25;
    var phantomAnimationDuration = 0.25;

    public function TrueHUD_Bar()
    {
        // constructor code
		this.stop();

        if (Phantom != undefined) {
            phantomTimeline = new TimelineLite({paused:true});
            spamCooldownTimeline = new TimelineLite({paused:true});
        }
        
        widthTimeline = new TimelineLite({paused:true});        

        if (Penalty != undefined)
        {
            Penalty._visible = false;
        }

        fFrameWidthOffset = Frame._width - 100;
        fOutlineWidthOffset = Outline._width - 100;
        fOutlineFlashWidthOffset = Flash._width - 100;
    
        if (FrameMount != undefined) {
            fFrameWidthMountOffset = FrameMount._width - 100;
        }
        if (OutlineMount != undefined) {
            fOutlineWidthMountOffset = OutlineMount._width - 100;
        }
        if (FlashMount != undefined) {
            fOutlineFlashWidthMountOffset = FlashMount._width - 100;
        }
    }

    public function resetValues()
    {
        desiredPhantomScale = 100;
        Bar.Overlay._xscale = 100;
        Bar.Overlay._x = 0;
        Bar.MaskHolder.Mask._xscale = 100;
        Bar.MaskHolder.Mask._x = -50;
        if (Phantom != undefined) {
            Phantom.Bar.MaskHolder.Mask._xscale = 100;
            Phantom.Bar.MaskHolder.Mask._x = -50;
            Phantom.Bar.MaskHolder._xscale = 100;
        }
        Bar.Overlay._x = 0;
        if (bHasPenaltyBar) {
            Penalty.Bar.Overlay._xscale = 100;
            Penalty.Bar.Overlay._x = 0;
            Penalty.Bar.MaskHolder.Mask._xscale = 0;
            Penalty.Bar.MaskHolder.Mask._x = -50;
        }
    }

    public function init(a_bShowPhantomBar: Boolean, a_fPhantomDuration: Number, a_uBarDirection: Number, a_bStandalone: Boolean, a_bHasShadow: Boolean, a_bHasShapeMask: Boolean, a_bHasFlash: Boolean, a_bHasPenaltyBar: Boolean)
    {
        updatePercent(1, true);
        
        bShowPhantomBar = a_bShowPhantomBar;
        fPhantomDuration = a_fPhantomDuration;
        uBarDirection = a_uBarDirection;
        bStandalone = a_bStandalone;
        bHasShadow = a_bHasShadow;
        bHasShapeMask = a_bHasShapeMask;
        bHasFlash = a_bHasFlash;
        bHasPenaltyBar = a_bHasPenaltyBar;

        resetValues();
        if (Phantom != undefined) {
            Phantom._visible = bShowPhantomBar;

            Phantom.Bar.Overlay._visible = false;
            Phantom.Bar.CenteredOverlay._visible = false;
            Phantom.Bar.Shine._alpha = 30;

            Phantom.Bar.setMask(Phantom.TransparencyMask);

            spamCooldownTimeline.clear();
            spamCooldownTimeline.call(updatePhantom, null, this, spamCooldownDuration);
        }
        
        if (bHasPenaltyBar)
        {
            Penalty.Bar.setMask(Penalty.TransparencyMask);
        }

        switch (uBarDirection)
        {
        case 0: // normal
            Bar.Overlay._visible = true;
            Bar.CenteredOverlay._visible = false;
            if (bHasPenaltyBar)
            {
                Penalty.Bar.Overlay._visible = true;
                Penalty.Bar.CenteredOverlay._visible = false;
            }
            break;
        case 1: // centered
            Bar.Overlay._visible = false;
            Bar.CenteredOverlay._visible = true;
            if (bHasPenaltyBar)
            {
                Penalty.Bar.Overlay._visible = false;
                Penalty.Bar.CenteredOverlay._visible = true;
            }
            break;
        case 2: // reverse
            Bar.Overlay._visible = true;
            Bar.CenteredOverlay._visible = false;
            Bar.Overlay._xscale = -100; // flip horizontally
            Bar.MaskHolder.Mask._x = 50;
            if (Phantom != undefined) {
                Phantom.Bar.MaskHolder.Mask._x = 50;
            }            
            if (bHasPenaltyBar)
            {
                Penalty.Bar.Overlay._visible = true;
                Penalty.Bar.CenteredOverlay._visible = false;
                Penalty.Bar.Overlay._xscale = -100; // flip horizontally
                //Penalty.Bar.MaskHolder.Mask._x = 50;
            }
            break;
        }

        if (bHasFlash)
        {
            if (flashTimeline == undefined)
            {
                flashTimeline = new TimelineLite({paused:true});
            }

            Flash._alpha = 0;
            flashTimeline.clear();
		    flashTimeline.set(Flash, {_alpha:0}, 0);
		    flashTimeline.fromTo(Flash, 0.30, {_alpha:0}, {_alpha:100}, 0);
		    flashTimeline.fromTo(Flash, 0.30, {_alpha:100}, {_alpha:0}, 0.30);
		    flashTimeline.fromTo(Flash, 0.30, {_alpha:0}, {_alpha:100}, 0.60);
		    flashTimeline.fromTo(Flash, 0.30, {_alpha:100}, {_alpha:0}, 0.90);

            if (FlashMount != undefined) {
                FlashMount._alpha = 0;
		        flashTimeline.set(FlashMount, {_alpha:0}, 0);
		        flashTimeline.fromTo(FlashMount, 0.30, {_alpha:0}, {_alpha:100}, 0);
		        flashTimeline.fromTo(FlashMount, 0.30, {_alpha:100}, {_alpha:0}, 0.30);
		        flashTimeline.fromTo(FlashMount, 0.30, {_alpha:0}, {_alpha:100}, 0.60);
		        flashTimeline.fromTo(FlashMount, 0.30, {_alpha:100}, {_alpha:0}, 0.90);
            }

            if (shortFlashTimeline == undefined)
            {
                shortFlashTimeline = new TimelineLite({paused:true});
            }

            shortFlashTimeline.clear();
		    shortFlashTimeline.set(Flash, {_alpha:0}, 0);
		    shortFlashTimeline.fromTo(Flash, 0.30, {_alpha:0}, {_alpha:100}, 0);
		    shortFlashTimeline.fromTo(Flash, 0.30, {_alpha:100}, {_alpha:0}, 0.30);

            if (FlashMount != undefined) {
		        shortFlashTimeline.set(FlashMount, {_alpha:0}, 0);
		        shortFlashTimeline.fromTo(FlashMount, 0.30, {_alpha:0}, {_alpha:100}, 0);
		        shortFlashTimeline.fromTo(FlashMount, 0.30, {_alpha:100}, {_alpha:0}, 0.30);
            }
        }

        if (bHasPenaltyBar)
        {
            Penalty._visible = true;
        }

        if (FrameMount != undefined && OutlineMount != undefined && FlashMount != undefined) {
            setMountMode(bMountMode);
        }

        updatePercent(percent, true);

        Flash._alpha = 0;
        if (FlashMount != undefined) {
            FlashMount._alpha = 0;
        }        
    }

    public function cleanUp()
    {
        widthTimeline.clear();
		widthTimeline.kill();
		widthTimeline = null;

        if (Phantom != undefined) {
            phantomTimeline.clear();
            phantomTimeline.kill();
            phantomTimeline = null;

            spamCooldownTimeline.clear();
            spamCooldownTimeline.kill();
            spamCooldownTimeline = null;
        }        

        if (bHasFlash)
        {
            flashTimeline.clear();
		    flashTimeline.kill();
		    flashTimeline = null;

            shortFlashTimeline.clear();
		    shortFlashTimeline.kill();
		    shortFlashTimeline = null;
        }
    }

    public function updatePercent(a_percent: Number, a_bInstant: Boolean)
    {
        percent = a_percent;
        var scale = a_percent * 100;
        desiredPhantomScale = scale;

        switch (uBarDirection)
        {
        case 0: // normal
            Bar.MaskHolder.Mask._xscale = scale;
            Phantom.TransparencyMask.Left._xscale = 100 - scale;
            Phantom.TransparencyMask.Right._xscale = 0;
            Bar.Overlay._x = -((1 - a_percent) * 100);
            break;
        case 1: // centered
            Bar.MaskHolder._xscale = scale;
            if (Phantom != undefined) {
                Phantom.TransparencyMask.Left._xscale = (100 - scale) * 0.5;
                Phantom.TransparencyMask.Right._xscale = (100 - scale) * 0.5;
            }            
            // don't move overlay
            break;
        case 2: // reverse
            Bar.MaskHolder.Mask._xscale = -scale;
            Phantom.TransparencyMask.Left._xscale = 0;
            Phantom.TransparencyMask.Right._xscale = 100 - scale;
            Bar.Overlay._x = ((1 - a_percent) * 100);
            break;
        }

		if (bShowPhantomBar)
		{
            var bPhantomShouldNotBeVisible = false;
            switch (uBarDirection)
            {
            case 0: // normal
                bPhantomShouldNotBeVisible = Phantom.Bar.MaskHolder.Mask._xscale < scale;
                break;
            case 1: // centered
                bPhantomShouldNotBeVisible = Phantom.Bar.MaskHolder._xscale < scale;
                break;
            case 2: // reverse
                bPhantomShouldNotBeVisible = -Phantom.Bar.MaskHolder.Mask._xscale < scale;
                break;
            }

            if (a_bInstant || bPhantomShouldNotBeVisible)
            {
                Phantom._visible = false;
                switch (uBarDirection)
                {
                case 0: // normal
                    Phantom.Bar.MaskHolder.Mask._xscale = scale;
                    break;
                case 1: // centered
                    Phantom.Bar.MaskHolder._xscale = scale;
                    break;
                case 2: // reverse
                    Phantom.Bar.MaskHolder.Mask._xscale = -scale;
                    break;
                }
                phantomTimeline.pause();
                phantomTimeline.clear();
                phantomTimeline.progress(0);

                spamCooldownTimeline.progress(1, true);
            }
            else
            {
                if (!spamCooldownTimeline.isActive())
                {
                    updatePhantom();
                }
            }
		}
    }

    public function updatePhantom()
    {
        if (!phantomTimeline.isActive())
        {
            Phantom._visible = true;
            phantomTimeline.clear();
            phantomTimeline.progress(0);
            phantomTimeline.restart();
        }
        
        switch (uBarDirection)
        {
        case 0: // normal
            phantomTimeline.to(Phantom.Bar.MaskHolder.Mask, phantomAnimationDuration, {_xscale: desiredPhantomScale}, phantomTimeline.time() + fPhantomDuration);  
            break;
        case 1: // centered
            phantomTimeline.to(Phantom.Bar.MaskHolder, phantomAnimationDuration, {_xscale: desiredPhantomScale}, phantomTimeline.time() + fPhantomDuration); 
            break;
        case 2: // reverse
            phantomTimeline.to(Phantom.Bar.MaskHolder.Mask, phantomAnimationDuration, {_xscale: -desiredPhantomScale}, phantomTimeline.time() + fPhantomDuration); 
            break;
        }

        phantomTimeline.play();
        spamCooldownTimeline.restart();
    }

    public function updatePenaltyPercent(a_percent: Number)
    {
        var scale = a_percent * 100;

        switch (uBarDirection)
        {
        case 0: // normal
            //Penalty.Bar.MaskHolder.Mask._xscale = scale;
            //Penalty.Bar.Overlay._x = -((1 - a_percent) * 100);
            Penalty.Bar.MaskHolder.Mask._xscale = 100;
            Penalty.TransparencyMask.Left._xscale = scale;
            Penalty.TransparencyMask.Right._xscale = 0;
            break;
        case 1: // centered
            Penalty.Bar.MaskHolder.Mask._xscale = 100;
            Penalty.TransparencyMask.Left._xscale = scale * 0.5;
            Penalty.TransparencyMask.Right._xscale = scale * 0.5;
            // don't move overlay
            break;
        case 2: // reverse
            //Penalty.Bar.MaskHolder.Mask._xscale = -scale;
            //Penalty.Bar.MaskHolder.Mask._x = 50;
            //Penalty.Bar.Overlay._x = ((1 - a_percent) * 100);
            Penalty.Bar.MaskHolder.Mask._xscale = 100;
            Penalty.TransparencyMask.Left._xscale = 0;
            Penalty.TransparencyMask.Right._xscale = scale;
            break;
        }
    }

    public function applyColor(a_colorNumber: Number, a_movieClip: MovieClip)
	{
        var alpha = a_movieClip._alpha;
		var newColor = new Color(a_movieClip);
		var colorTransform = { ra: 0, rb: (a_colorNumber >> 16) & 0xFF, ga: 0, gb: (a_colorNumber >> 8) & 0xFF, ba: 0, bb: a_colorNumber & 0xFF, aa: alpha, ab: 0 };
		newColor.setTransform(colorTransform);
	}

    public function setColor(a_barColor: Number, a_phantomColor: Number, a_backgroundColor: Number, a_penaltyColor: Number, a_flashColor: Number)
    {
        applyColor(a_barColor, Bar.Color);
        if (bHasFlash)
        {
            applyColor(a_flashColor, Flash);
            if (FlashMount != undefined) {
                applyColor(a_flashColor, FlashMount);
            }
        }
        if (Phantom != undefined) {
            applyColor(a_phantomColor, Phantom.Bar.Color);
        }
        if (Background != undefined) {
            applyColor(a_backgroundColor, Background);
        }
        if (bHasPenaltyBar)
        {
            applyColor(a_penaltyColor, Penalty.Bar.Color);
        }
    }

    public function playFlash(a_bLong: Boolean)
	{
        if (a_bLong) {
            flashTimeline.restart();
        } else {
            shortFlashTimeline.restart();
        }		
	}

    public function setMountMode(a_enable: Boolean)
    {
        bMountMode = a_enable;
        if (a_enable)
        {
            Frame._visible = false;
            Outline._visible = false;
            Flash._visible = false;
            FrameMount._visible = true;
            OutlineMount._visible = true;
            FlashMount._visible = true;
        }
        else
        {
            Frame._visible = true;
            Outline._visible = true;
            Flash._visible = true;
            FrameMount._visible = false;
            OutlineMount._visible = false;
            FlashMount._visible = false;
        }
    }

    public function setWidth(a_width: Number)
    {
        if (bHasShapeMask)
        {
            Mask._width = a_width;
        }

        Bar._xscale = a_width;
        if (Phantom != undefined) {
            Phantom._xscale = a_width;
        }
        if (Background != undefined) {
            Background._xscale = a_width;
        }        

        if (bHasFlash)
        {
            Flash._width = a_width + fOutlineFlashWidthOffset;
            if (FlashMount != undefined)
            {
                var mountXOffsetMult = ((a_width - 100) / 200) * (100 / (100 + fOutlineFlashWidthMountOffset));
                FlashMount._width = a_width + fOutlineFlashWidthMountOffset;
                FlashMount._x = (fOutlineFlashWidthMountOffset - fOutlineFlashWidthOffset) * mountXOffsetMult;
            }
        }

        if (bHasPenaltyBar)
        {
            Penalty._xscale = a_width;
        }
		
        if (bStandalone)
        {
            if (bHasShadow)
            {
                Shadow._width = a_width;
            }

		    Frame._width = a_width + fFrameWidthOffset;
            Outline._width = a_width + fOutlineWidthOffset;
            if (FrameMount != undefined) {
                var mountXOffsetMult = ((a_width - 100) / 200) * (100 / (100 + fFrameWidthMountOffset));
                FrameMount._width = a_width + fFrameWidthMountOffset;
                FrameMount._x = (fFrameWidthMountOffset - fFrameWidthOffset) * mountXOffsetMult;
            }
            if (OutlineMount != undefined) {
                var mountXOffsetMult = ((a_width - 100) / 200) * (100 / (100 + fOutlineWidthMountOffset));
                OutlineMount._width = a_width + fOutlineWidthMountOffset;
                OutlineMount._x = (fOutlineWidthMountOffset - fOutlineWidthOffset) * mountXOffsetMult;
            }
        }
    }

    public function setWidthTimeline(a_width: Number)
    {        
		widthTimeline.clear();

        if (bHasShapeMask)
        {
            widthTimeline.to(Mask, widthTimelineDuration, {_width:a_width, ease:Sine.easeIn}, 0);
        }

        widthTimeline.to(Bar, widthTimelineDuration, {_xscale:a_width, ease:Sine.easeIn}, 0)
        if (Phantom != undefined) {
            widthTimeline.to(Phantom, widthTimelineDuration, {_xscale:a_width, ease:Sine.easeIn}, 0)
        }
        if (Background != undefined) {
            widthTimeline.to(Background, widthTimelineDuration, {_xscale:a_width, ease:Sine.easeIn}, 0)
        }

        if (bHasFlash)
        {
            widthTimeline.to(Flash, widthTimelineDuration, {_width:a_width + fOutlineFlashWidthOffset, ease:Sine.easeIn}, 0)
            if (FlashMount != undefined)
            {
                var mountXOffsetMult = ((a_width - 100) / 200) * (100 / (100 + fOutlineFlashWidthMountOffset));
                widthTimeline.to(FlashMount, widthTimelineDuration, {_width:a_width + fOutlineFlashWidthMountOffset, _x:(fOutlineFlashWidthMountOffset - fOutlineFlashWidthOffset) * mountXOffsetMult, ease:Sine.easeIn}, 0)
            }
        }

        if (bHasPenaltyBar)
        {
            widthTimeline.to(Penalty, widthTimelineDuration, {_xscale:a_width, ease:Sine.easeIn}, 0)
        }
		
        if (bStandalone)
        {
            if (bHasShadow)
            {
                widthTimeline.to(Shadow, widthTimelineDuration, {_width:a_width, ease:Sine.easeIn}, 0)
            }            

            widthTimeline.to(Frame, widthTimelineDuration, {_width:a_width + fFrameWidthOffset, ease:Sine.easeIn}, 0)
            widthTimeline.to(Outline, widthTimelineDuration, {_width:a_width + fOutlineWidthOffset, ease:Sine.easeIn}, 0)
            if (FrameMount != undefined) {
                var mountXOffsetMult = ((a_width - 100) / 200) * (100 / (100 + fFrameWidthMountOffset));
                widthTimeline.to(FrameMount, widthTimelineDuration, {_width:a_width + fFrameWidthMountOffset, _x:(fFrameWidthMountOffset - fFrameWidthOffset) * mountXOffsetMult, ease:Sine.easeIn}, 0)
            }
            if (OutlineMount != undefined) {
                var mountXOffsetMult = ((a_width - 100) / 200) * (100 / (100 + fOutlineWidthMountOffset));
                widthTimeline.to(OutlineMount, widthTimelineDuration, {_width:a_width + fOutlineWidthMountOffset, _x:(fOutlineWidthMountOffset - fOutlineWidthOffset) * mountXOffsetMult, ease:Sine.easeIn}, 0)
            }
        }

        widthTimeline.restart();
    }
}
