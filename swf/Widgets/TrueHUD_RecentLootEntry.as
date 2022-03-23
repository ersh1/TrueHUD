import flash.geom.ColorTransform;
import flash.geom.Transform;

import com.greensock.TimelineLite;
import com.greensock.TweenLite;
import com.greensock.easing.*;

class Widgets.TrueHUD_RecentLootEntry extends MovieClip
{
	public var ItemText: TextField;
	public var ItemIcon: MovieClip;

	private var iconLoader: MovieClipLoader;
	private var iconSource: String = "skyui/icons_item_psychosteve.swf";

	var showHideTimeline: TimelineLite;
	var positionTween: TweenLite;

	var fRecentLootMessageDuration: Number;

	var animDuration = 0.25;

	var bIconsLoaded = false;
	var bFresh = true;
	var bReadyToRemove = false;

	var iconLabel: String;
	var iconColor: Number;

	public function TrueHUD_RecentLootEntry() 
	{
		// constructor code
		this.stop();

		if (this._url.indexOf("exported") != -1 || this._url.indexOf("Exported") != -1) 
		{
			this.iconSource = "../" + this.iconSource 
		}

		var iconLoader = new MovieClipLoader();
		iconLoader.addListener(this);
		iconLoader.loadClip(this.iconSource, ItemIcon);
		showHideTimeline = new TimelineLite({paused:true});
	}

	private function onLoadInit(a_icon: MovieClip): Void
	{
		bIconsLoaded = true;

		updateIcon();

		ItemIcon._width = 24;
		ItemIcon._height = 24;
		ItemIcon._visible = true;
	}

	public function init()
	{
		showHideTimeline.clear();
		showHideTimeline.set(this, {_alpha:0, _x:50}, 0);
		showHideTimeline.to(this, animDuration, {_alpha:100, _x:0}, animDuration);
		showHideTimeline.to(this, animDuration, {_alpha:0}, fRecentLootMessageDuration);
		showHideTimeline.set(this, {bReadyToRemove:true});
		showHideTimeline.eventCallback("onStart", setReadyToRemove, [false], this);
		showHideTimeline.eventCallback("onComplete", setReadyToRemove, [true], this);
	}

	public function cleanUp()
	{
		showHideTimeline.eventCallback("onStart", null);
		showHideTimeline.eventCallback("onComplete", null);
		showHideTimeline.clear();
		showHideTimeline.kill();
		showHideTimeline = null;

		positionTween.kill();
		positionTween = null;
	}

	public function loadConfig(a_fRecentLootMessageDuration: Number)
	{
		fRecentLootMessageDuration = a_fRecentLootMessageDuration;

		init();
	}

	public function updateIcon()
	{
		ItemIcon.gotoAndStop(iconLabel);
		
		var colorTransform = new ColorTransform();
		var transform = new Transform(MovieClip(ItemIcon));

		colorTransform.rgb = iconColor;
		transform.colorTransform = colorTransform;
	}

	public function addMessage(a_itemName: String, a_itemCount: Number, a_iconLabel: String, a_iconColor: Number, a_bInstant: Boolean)
	{
		if (a_itemCount > 1)
		{
			a_itemName = a_itemName + " (" + a_itemCount.toString() + ")";
		}
		ItemText.html = true;
		ItemText.textAutoSize = "shrink";
		ItemText.htmlText = a_itemName;

		iconLabel = a_iconLabel;
		iconColor = a_iconColor;
		
		if (bIconsLoaded) {
			updateIcon();
		}

		setReadyToRemove(false);
		if (a_bInstant)
		{
			showHideTimeline.play(animDuration);
		}
		else
		{
			showHideTimeline.restart();
		}
		
		bFresh = true;
	}

	public function setY(a_y: Number)
	{
		if (bFresh)
		{
			this._y = a_y;
			bFresh = false;
		}
		else
		{
			positionTween = TweenLite.to(this, animDuration, {_y:a_y, ease:Sine.easeIn});
		}
	}

	public function hideMessage()
	{
		if (showHideTimeline.time() < animDuration + fRecentLootMessageDuration)
		{
			showHideTimeline.play(fRecentLootMessageDuration);
		}
	}

	public function isReadyToRemove() : Boolean
	{
		return bReadyToRemove;
	}

	public function setReadyToRemove(a_readyToRemove: Boolean)
	{
		bReadyToRemove = a_readyToRemove;
	}
}
