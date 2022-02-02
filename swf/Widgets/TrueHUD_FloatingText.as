import com.greensock.TimelineLite;
import com.greensock.easing.*;

class Widgets.TrueHUD_FloatingText extends MovieClip
{
    public var FloatingText: MovieClip;
    
    var showTimeline: TimelineLite;
    var fadeTimeline: TimelineLite;
    var fadeDuration = 0.5;

    var duration: Number;
    var bSpecial: Boolean;

    public function TrueHUD_FloatingText()
    {
        // constructor code
		this.stop();

        showTimeline = new TimelineLite({paused:true});
        fadeTimeline = new TimelineLite({paused:true});

        FloatingText.Text.autoSize = "center";

        // delay init till everything's ready
		this.onEnterFrame = function(): Void {
			init();
			delete this.onEnterFrame;
		}
    }

    public function init()
    {
        var fadeStart = Math.max(duration - fadeDuration, 0);
        showTimeline.clear();
        if (bSpecial)
        {
            showTimeline.fromTo(FloatingText, 0.3, {_xscale:500, _yscale:500, _alpha:0}, {_xscale:150, _yscale:150, _alpha:100, ease:Sine.easeIn}, 0);
        }
        else
        {
            showTimeline.fromTo(FloatingText, 0.1, {_alpha:0}, {_alpha:100, ease:Sine.easeIn}, 0);
        }
        showTimeline.call(startFade, null, this, fadeStart);

        fadeTimeline.clear();
        fadeTimeline.eventCallback("onComplete", dispose, [true], this);
        fadeTimeline.fromTo(FloatingText, fadeDuration, {_alpha:100}, {_alpha:0}, 0);

        showTimeline.play();
    }

    public function cleanUp()
    {
        showTimeline.clear();
		showTimeline.kill();
		showTimeline = null;

        fadeTimeline.eventCallback("onComplete", null);
        fadeTimeline.clear();
		fadeTimeline.kill();
		fadeTimeline = null;
    }

    public function dispose()
    {
        cleanUp();
        this.removeMovieClip();
    }

    public function startFade()
    {
        fadeTimeline.play();
    }

    public function loadConfig(a_text: String, a_color: Number, a_duration: Number, a_bSpecial: Boolean)
    {
        FloatingText.Text.text = a_text;
        FloatingText.Text.textColor = a_color;
        duration = a_duration;
        bSpecial = a_bSpecial;
    }
}
