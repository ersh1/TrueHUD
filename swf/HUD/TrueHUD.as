class HUD.TrueHUD extends MovieClip
{
	private var mcLoader: MovieClipLoader;

	private var infoBarWidgets: Object;
	private var bossBarWidgets: Object;
	private var floatingTextWidgets: Object;
	private var shoutIndicatorWidget: MovieClip;
	private var playerWidget: MovieClip;

	private var trueHUDContainer: MovieClip;
	
	private var customWidgets: Object;
	private var customWidgetContainers: Object;

	public function TrueHUD()
	{
		super();

		mcLoader = new MovieClipLoader();
		//mcLoader.addListener(this);

		infoBarWidgets = {};
		bossBarWidgets = {};
		floatingTextWidgets = {};
		customWidgetContainers = {};
		customWidgets = {};
		
		trueHUDContainer = this.createEmptyMovieClip("TrueHUD_Widgets", this.getNextHighestDepth());
		
		mcLoader.loadClip("TrueHUD_Widgets.swf", trueHUDContainer);
	}

	public function LoadCustomWidgets(a_pluginHandle: Number, a_path: String) : Boolean
	{
		var pluginHandle = a_pluginHandle.toString();

		customWidgetContainers[pluginHandle] = this.createEmptyMovieClip(pluginHandle, this.getNextHighestDepth());
		customWidgets[pluginHandle] = {};

		return mcLoader.loadClip("../" + a_path, customWidgetContainers[pluginHandle]);
	}

	public function RegisterNewWidgetType(a_pluginHandle: Number, a_widgetType: Number)
	{
		var pluginHandle = a_pluginHandle.toString();
		var widgetType = a_widgetType.toString();

		customWidgets[pluginHandle][widgetType] = {};
	}

	public function SortDepths(a_depthArray: Array) : Void
	{
		a_depthArray.sortOn(["zIndex"], Array.DESCENDING);
		
		for (var i = 0; i < a_depthArray.length; i++)
		{
			switch (a_depthArray[i].widgetType)
			{
			case 0: // info bar
				var key = a_depthArray[i].id.toString();
				infoBarWidgets[key].swapDepths(i);
				break;
			case 1: // boss bar
				var key = a_depthArray[i].id.toString();
				bossBarWidgets[key].swapDepths(i);
				break;
			case 2: // shout indicator
				shoutIndicatorWidget.swapDepths(i);
				break;
			case 3: // player widget
				playerWidget.swapDepths(i);
				break;
			case 4: // floating text
				var key = a_depthArray[i].id.toString();
				floatingTextWidgets[key].swapDepths(i);
				break;
			}
		}
	}

	public function SortCustomWidgetDepths(a_pluginHandle: Number, a_depthArray: Array) : Void
	{
		a_depthArray.sortOn(["zIndex"], Array.DESCENDING);

		var pluginHandle = a_pluginHandle.toString();
		
		for (var i = 0; i < a_depthArray.length; i++)
		{
			var type = a_depthArray[i].widgetType.toString();
			var id = a_depthArray[i].id.toString();
			customWidgets[pluginHandle][type][id].swapDepths(i);
		}
	}

	public function AddInfoBarWidget(a_actorHandle: Number) : MovieClip
	{
		var key = a_actorHandle.toString();
		infoBarWidgets[key] = trueHUDContainer.attachMovie("TrueHUD_InfoBar", key, trueHUDContainer.getNextHighestDepth());
		return infoBarWidgets[key];
	}

	public function RemoveInfoBarWidget(a_actorHandle: Number) : Void
	{
		var key = a_actorHandle.toString();
		if (infoBarWidgets[key] == undefined)
		{
			return;
		}
		infoBarWidgets[key].removeMovieClip();
	}

	public function AddBossInfoBarWidget(a_actorHandle: Number) : MovieClip
	{
		var key = a_actorHandle.toString();
		bossBarWidgets[key] = trueHUDContainer.attachMovie("TrueHUD_BossBar", key, trueHUDContainer.getNextHighestDepth());

		return bossBarWidgets[key];
	}

	public function RemoveBossInfoBarWidget(a_actorHandle: Number) : Void
	{
		var key = a_actorHandle.toString();
		if (bossBarWidgets[key] == undefined)
		{
			return;
		}
		bossBarWidgets[key].removeMovieClip();
	}

	public function AddShoutIndicatorWidget() : MovieClip
	{
		var key = "TrueHUD_ShoutIndicator";
		shoutIndicatorWidget = trueHUDContainer.attachMovie("TrueHUD_ShoutIndicator", key, trueHUDContainer.getNextHighestDepth());

		return shoutIndicatorWidget;
	}

	public function RemoveShoutIndicatorWidget() : Void
	{
		if (shoutIndicatorWidget == undefined)
		{
			return;
		}
		shoutIndicatorWidget.removeMovieClip();
	}

	public function AddPlayerWidget() : MovieClip
	{
		var key = "TrueHUD_PlayerWidget";
		playerWidget = trueHUDContainer.attachMovie("TrueHUD_PlayerWidget", key, trueHUDContainer.getNextHighestDepth());

		return playerWidget;
	}

	public function RemovePlayerWidget() : Void
	{
		if (playerWidget == undefined)
		{
			return;
		}
		playerWidget.removeMovieClip();
	}

	public function AddFloatingTextWidget(a_widgetID: Number) : MovieClip
	{
		var key = a_widgetID.toString();

		floatingTextWidgets[key] = trueHUDContainer.attachMovie("TrueHUD_FloatingText", key, trueHUDContainer.getNextHighestDepth());

		return floatingTextWidgets[key];
	}

	public function AddCustomWidget(a_pluginHandle: Number, a_widgetType: Number, a_widgetID: Number, a_symbolIdentifier: String) : MovieClip
	{
		var pluginHandle = a_pluginHandle.toString();
		var type = a_widgetType.toString();
		var id = a_widgetID.toString();

		customWidgets[pluginHandle][type][id] = customWidgetContainers[pluginHandle].attachMovie(a_symbolIdentifier, id, customWidgetContainers[pluginHandle].getNextHighestDepth());

		return customWidgets[pluginHandle][type][id];
	}

	public function RemoveCustomWidget(a_pluginHandle: Number, a_widgetType: Number, a_widgetID: Number) : Void
	{
		var pluginHandle = a_pluginHandle.toString();
		var type = a_widgetType.toString();
		var id = a_widgetID.toString();

		if (customWidgets[pluginHandle][type][id] == undefined)
		{
			return;
		}
		customWidgets[pluginHandle][type][id].removeMovieClip();
	}

	public function RemoveAllCustomWidgets() : Void
	{
		for (var pluginWidgetsIndex in customWidgets)
		{
			for (var widgetTypesIndex in customWidgets[pluginWidgetsIndex])
			{
				for (var widgetIndex in customWidgets[pluginWidgetsIndex][widgetTypesIndex])
				{
					if (customWidgets[pluginWidgetsIndex][widgetTypesIndex][widgetIndex] != undefined)
					{
						customWidgets[pluginWidgetsIndex][widgetTypesIndex][widgetIndex].removeMovieClip();
					}
				}
			}
		}
	}
}
