#include "Scaleform/TrueHUDMenu.h"

#include "Settings.h"
#include "Offsets.h"
#include "HUDHandler.h"
#include "Widgets/ActorInfoBar.h"
#include "Widgets/FloatingText.h"

namespace Scaleform
{
	void TrueHUDMenu::Register()
	{
		auto ui = RE::UI::GetSingleton();
		if (ui) {
			ui->Register(MENU_NAME, Creator);
			logger::info("Registered {}"sv, MENU_NAME);
		}
	}

	bool TrueHUDMenu::IsOpen() const
	{
		return _bIsOpen;
	}

	RE::GPtr<RE::GFxMovieView> TrueHUDMenu::GetView() const
	{
		return _view;
	}

	void TrueHUDMenu::SetTarget(RE::ObjectRefHandle a_actorHandle)
	{
		_targetHandle = a_actorHandle;

		if (Settings::bEnableActorInfoBars) {
			if (a_actorHandle) {
				AddActorInfoBar(a_actorHandle);
			}
		}		
	}

	void TrueHUDMenu::SetSoftTarget(RE::ObjectRefHandle a_actorHandle)
	{
		_softTargetHandle = a_actorHandle;

		if (Settings::bEnableActorInfoBars) {
			if (a_actorHandle) {
				AddActorInfoBar(a_actorHandle);
			}
		}		
	}

	RE::ObjectRefHandle TrueHUDMenu::GetTarget() const
	{
		return _targetHandle;
	}

	RE::ObjectRefHandle TrueHUDMenu::GetSoftTarget() const
	{
		return _softTargetHandle;
	}

	bool TrueHUDMenu::HasActorInfoBar(RE::ObjectRefHandle a_actorHandle)
	{
		return _actorInfoBarMap.contains(a_actorHandle);
	}

	bool TrueHUDMenu::AddActorInfoBar(RE::ObjectRefHandle a_actorHandle)
	{	
		using WidgetStateMode = InfoBarBase::WidgetStateMode;

		if (_view && !HasActorInfoBar(a_actorHandle) && !HasBossInfoBar(a_actorHandle)) {
			Locker locker(_lock);
			auto widget = std::make_shared<ActorInfoBar>(_view, a_actorHandle);

			auto iter = _actorInfoBarMap.emplace(a_actorHandle, widget);
			if (iter.second) {
				RE::GFxValue arg;
				arg.SetNumber(widget->_widgetID);

				RE::GFxValue obj;
				_view->Invoke("_root.TrueHUD.AddInfoBarWidget", &obj, &arg, 1);
				if (!obj.IsDisplayObject()) {
					_actorInfoBarMap.erase(iter.first);
					return false;
				}
				widget->_object = obj;
				widget->Initialize();
				return true;
			}
		}

		return false;
	}

	bool TrueHUDMenu::RemoveActorInfoBar(RE::ObjectRefHandle a_actorHandle, WidgetRemovalMode a_removalMode)
	{
		using WidgetStateMode = InfoBarBase::WidgetStateMode;

		if (_view) {
			Locker locker(_lock);
			auto it = _actorInfoBarMap.find(a_actorHandle);
			if (it != _actorInfoBarMap.end()) {
				auto& widget = it->second;

				switch (a_removalMode)
				{
				case WidgetRemovalMode::Immediate:
					if (widget->_object.IsDisplayObject()) {
						RE::GFxValue arg;
						arg.SetNumber(widget->_widgetID);

						widget->Dispose();
						_view->Invoke("_root.TrueHUD.RemoveInfoBarWidget", nullptr, &arg, 1);
					}
					_actorInfoBarMap.erase(it);
					break;

				case WidgetRemovalMode::Normal:
					widget->SetWidgetState(WidgetStateMode::kRemove);
					break;

				case WidgetRemovalMode::Delayed:
					widget->SetWidgetState(WidgetStateMode::kTargetKilled);
					break;
				}

				return true;
			}
		}

		return false;
	}

	bool TrueHUDMenu::HasBossInfoBar(RE::ObjectRefHandle a_actorHandle)
	{
		bool bInQueue = std::find(_bossQueue.begin(), _bossQueue.end(), a_actorHandle) != _bossQueue.end();

		return bInQueue || _bossInfoBarMap.contains(a_actorHandle);
	}

	bool TrueHUDMenu::AddBossInfoBar(RE::ObjectRefHandle a_actorHandle)
	{
		using WidgetStateMode = InfoBarBase::WidgetStateMode;

		if (_view && !HasBossInfoBar(a_actorHandle)) {
			if (_bossInfoBarMap.size() < Settings::uBossBarMaxCount) {
				// Add a boss bar
				return AddBossInfoBarWidget(a_actorHandle);
			} else {
				// Maximum count of boss bars, add to queue
				_bossQueue.emplace_back(a_actorHandle);
			}			
		}

		return false;
	}

	bool TrueHUDMenu::RemoveBossInfoBar(RE::ObjectRefHandle a_actorHandle, WidgetRemovalMode a_removalMode)
	{
		using WidgetStateMode = InfoBarBase::WidgetStateMode;

		if (_view) {
			Locker locker(_lock);

			// Remove from queue
			_bossQueue.remove(a_actorHandle);

			auto it = _bossInfoBarMap.find(a_actorHandle);
			if (it != _bossInfoBarMap.end()) {
				// Remove widget
				auto& widget = it->second;

				switch (a_removalMode) {
				case WidgetRemovalMode::Immediate:
					{
						auto index = widget->GetIndex();
						if (widget->_object.IsDisplayObject()) {
							RE::GFxValue arg;
							arg.SetNumber(widget->_widgetID);

							widget->Dispose();
							_view->Invoke("_root.TrueHUD.RemoveBossInfoBarWidget", nullptr, &arg, 1);
						}
						_bossInfoBarMap.erase(it);
						RefreshBossBarIndexes(index);
						UpdateBossQueue();
						break;
					}

				case WidgetRemovalMode::Normal:
					{
						widget->SetWidgetState(WidgetStateMode::kRemove);
						break;
					}

				case WidgetRemovalMode::Delayed:
					{
						widget->SetWidgetState(WidgetStateMode::kTargetKilled);
						break;
					}
				}

				return true;
			}
		}

		return false;
	}

	bool TrueHUDMenu::AddShoutIndicator()
	{
		if (_view) {
			Locker locker(_lock);

			if (_shoutIndicator) {
				RemoveShoutIndicator();
			}

			_shoutIndicator = std::make_shared<ShoutIndicator>(_view);

			if (_shoutIndicator) {
				RE::GFxValue obj;
				_view->Invoke("_root.TrueHUD.AddShoutIndicatorWidget", &obj, nullptr, 0);
				if (!obj.IsDisplayObject()) {
					_shoutIndicator = nullptr;
					return false;
				} else {
					_shoutIndicator->_object = obj;
					_shoutIndicator->Initialize();
				}

				return true;
			}
		}

		return false;
	}

	bool TrueHUDMenu::RemoveShoutIndicator()
	{
		if (_view) {
			Locker locker(_lock);
			if (_shoutIndicator) {
				if (_shoutIndicator->_object.IsDisplayObject()) {
					_shoutIndicator->Dispose();
					_view->Invoke("_root.TrueHUD.RemoveShoutIndicatorWidget", nullptr, nullptr, 0);
				}
				_shoutIndicator = nullptr;

				return true;
			}
		}

		return false;
	}

	bool TrueHUDMenu::AddPlayerWidget()
	{
		if (_view) {
			Locker locker(_lock);

			if (_playerWidget) {
				RemovePlayerWidget();
			}

			_playerWidget = std::make_shared<PlayerWidget>(_view);

			if (_playerWidget) {
				RE::GFxValue obj;
				_view->Invoke("_root.TrueHUD.AddPlayerWidget", &obj, nullptr, 0);
				if (!obj.IsDisplayObject()) {
					_playerWidget = nullptr;
					return false;
				}

				_playerWidget->_object = obj;
				_playerWidget->Initialize();
				return true;
			}
		}

		return false;
	}

	bool TrueHUDMenu::RemovePlayerWidget()
	{
		if (_view) {
			Locker locker(_lock);
			if (_playerWidget) {
				if (_playerWidget->_object.IsDisplayObject()) {
					_playerWidget->Dispose();
					_view->Invoke("_root.TrueHUD.RemovePlayerWidget", nullptr, nullptr, 0);
				}
				_playerWidget = nullptr;

				return true;
			}
		}

		return false;
	}

	void TrueHUDMenu::UpdatePlayerWidgetChargeMeters(float a_percent, bool a_bForce, bool a_bLeftHand, bool a_bShow)
	{
		if (_playerWidget) {
			_playerWidget->UpdatePlayerWidgetChargeMeters(a_percent, a_bForce, a_bLeftHand, a_bShow);
		}
	}

	bool TrueHUDMenu::AddFloatingWorldTextWidget(std::string a_text, uint32_t a_color, float a_duration, bool a_bSpecial, RE::NiPoint3 a_worldPosition)
	{
		if (_view) {
			Locker locker(_lock);
			auto widget = std::make_shared<FloatingText>(_view, _nextFloatingTextID, a_text, a_color, a_duration, a_bSpecial, a_worldPosition);
			auto iter = _floatingTextMap.emplace(_nextFloatingTextID, widget);
			++_nextFloatingTextID;
			if (iter.second) {
				RE::GFxValue arg;
				arg.SetNumber(widget->_widgetID);

				RE::GFxValue obj;
				_view->Invoke("_root.TrueHUD.AddFloatingTextWidget", &obj, &arg, 1);
				if (!obj.IsDisplayObject()) {
					_floatingTextMap.erase(iter.first);
					return false;
				}

				widget->_object = obj;
				widget->Initialize();
				return true;
			}
		}

		return false;
	}

	bool TrueHUDMenu::AddFloatingScreenTextWidget(std::string a_text, uint32_t a_color, float a_duration, bool a_bSpecial, RE::NiPoint2 a_screenPosition)
	{
		if (_view) {
			Locker locker(_lock);
			auto widget = std::make_shared<FloatingText>(_view, _nextFloatingTextID, a_text, a_color, a_duration, a_bSpecial, a_screenPosition);
			auto iter = _floatingTextMap.emplace(_nextFloatingTextID, widget);
			++_nextFloatingTextID;
			if (iter.second) {
				RE::GFxValue arg;
				arg.SetNumber(widget->_widgetID);

				RE::GFxValue obj;
				_view->Invoke("_root.TrueHUD.AddFloatingTextWidget", &obj, &arg, 1);
				if (!obj.IsDisplayObject()) {
					_floatingTextMap.erase(iter.first);
					return false;
				}

				widget->_object = obj;
				widget->Initialize();
				return true;
			}
		}

		return false;
	}

	bool TrueHUDMenu::LoadCustomWidgets(SKSE::PluginHandle a_myPluginHandle, std::string_view a_filePath, APIResultCallback&& a_successCallback)
	{
		if (_view) {
			RE::GFxValue args[2];
			args[0].SetNumber(a_myPluginHandle);
			args[1].SetString(a_filePath);
			RE::GFxValue result;

			_view->Invoke("_root.TrueHUD.LoadCustomWidgets", &result, args, 2);

			bool bSuccess = result.GetBool();

			if (a_successCallback) {
				a_successCallback(bSuccess ? TRUEHUD_API::APIResult::OK : TRUEHUD_API::APIResult::WidgetFailedToLoad);
			}

			return bSuccess;
		}

		return false;
	}

	bool TrueHUDMenu::RegisterNewWidgetType(SKSE::PluginHandle a_myPluginHandle, uint32_t a_widgetType)
	{
		if (_view) {
			RE::GFxValue args[2];
			args[0].SetNumber(a_myPluginHandle);
			args[1].SetNumber(a_widgetType);
			
			_view->Invoke("_root.TrueHUD.RegisterNewWidgetType", nullptr, args, 2);
		}

		return false;
	}

	bool TrueHUDMenu::AddWidget(SKSE::PluginHandle a_myPluginHandle, uint32_t a_widgetType, uint32_t a_widgetID, std::string_view a_symbolIdentifier, std::shared_ptr<WidgetBase> a_widget)
	{
		if (_view && a_widget) {
			Locker locker(_lock);
			RE::GFxValue args[4];
			args[0].SetNumber(a_myPluginHandle);
			args[1].SetNumber(a_widgetType);
			args[2].SetNumber(a_widgetID);
			args[3].SetString(a_symbolIdentifier);

			RE::GFxValue obj;
			_view->Invoke("_root.TrueHUD.AddCustomWidget", &obj, args, 4);
			if (!obj.IsDisplayObject()) {
				return false;
			}
			a_widget->_object = obj;
			_customWidgets[a_myPluginHandle][a_widgetType].emplace(a_widgetID, a_widget);
			a_widget->_view = _view;

			a_widget->Initialize();
			return true;
		}

		return false;
	}

	bool TrueHUDMenu::RemoveWidget(SKSE::PluginHandle a_myPluginHandle, uint32_t a_widgetType, uint32_t a_widgetID, WidgetRemovalMode a_removalMode)
	{
		using WidgetState = TRUEHUD_API::WidgetBase::WidgetState;

		if (_view) {
			Locker locker(_lock);

			auto it = _customWidgets.find(a_myPluginHandle);
			if (it != _customWidgets.end()) {
				auto& pluginWidgets = it->second;
				auto iter = pluginWidgets.find(a_widgetType);
				if (iter != pluginWidgets.end()) {
					auto& typeWidgets = iter->second;
					auto widgetIt = typeWidgets.find(a_widgetID);
					if (widgetIt != typeWidgets.end()) {
						auto& widget = widgetIt->second;

						switch (a_removalMode) {
						case WidgetRemovalMode::Immediate:
							if (widget->_object.IsDisplayObject()) {
								RE::GFxValue args[3];
								args[0].SetNumber(a_myPluginHandle);
								args[1].SetNumber(a_widgetType);
								args[2].SetNumber(a_widgetID);

								widget->Dispose();
								_view->Invoke("_root.TrueHUD.RemoveCustomWidget", nullptr, args, 3);
							}
							typeWidgets.erase(widgetIt);
							break;

						case WidgetRemovalMode::Normal:
							widget->SetWidgetState(WidgetState::kPendingRemoval);
							break;

						case WidgetRemovalMode::Delayed:
							widget->SetWidgetState(WidgetState::kDelayedRemoval);
							break;
						}

						return true;
					}
				}				
			}
		}

		return false;
	}

	void TrueHUDMenu::FlashActorValue(RE::ObjectRefHandle a_actorHandle, RE::ActorValue a_actorValue, bool a_bLong)
	{
		if (a_actorHandle.native_handle() == 0x100000) {  // 0x100000 == player
			if (_shoutIndicator && a_actorValue == RE::ActorValue::kVoiceRate) {
				_shoutIndicator->FlashShoutWidget();
				return;
			}
			if (_playerWidget) {
				_playerWidget->FlashActorValue(a_actorValue, a_bLong);
				return;
			}
		}

		// try the boss bars first
		auto it = _bossInfoBarMap.find(a_actorHandle);
		if (it != _bossInfoBarMap.end()) {
			auto& widget = it->second;
			widget->FlashActorValue(a_actorValue, a_bLong);
			return;
		}

		// try normal info bars
		auto iter = _actorInfoBarMap.find(a_actorHandle);
		if (iter != _actorInfoBarMap.end()) {
			auto& widget = iter->second;
			widget->FlashActorValue(a_actorValue, a_bLong);
			return;
		}
	}

	void TrueHUDMenu::FlashActorSpecialBar(RE::ObjectRefHandle a_actorHandle, bool a_bLong)
	{
		if (a_actorHandle.native_handle() == 0x100000 && _playerWidget) {  // 0x100000 == player
			_playerWidget->FlashSpecial(a_bLong);
			return;
		}

		// try the boss bars first
		auto it = _bossInfoBarMap.find(a_actorHandle);
		if (it != _bossInfoBarMap.end()) {
			auto& widget = it->second;
			widget->FlashSpecial(a_bLong);
			return;
		}

		// try normal info bars
		auto iter = _actorInfoBarMap.find(a_actorHandle);
		if (iter != _actorInfoBarMap.end()) {
			auto& widget = iter->second;
			widget->FlashSpecial(a_bLong);
			return;
		}
	}

	void TrueHUDMenu::OverrideBarColor(RE::ObjectRefHandle a_actorHandle, RE::ActorValue a_actorValue, BarColorType a_colorType, uint32_t a_color)
	{
		switch (a_actorValue) {
		case RE::ActorValue::kHealth:
			_colorOverrides[a_actorHandle][BarType::kHealth].SetOverride(a_colorType, a_color);
			break;
		case RE::ActorValue::kMagicka:
			_colorOverrides[a_actorHandle][BarType::kMagicka].SetOverride(a_colorType, a_color);
			break;
		case RE::ActorValue::kStamina:
			_colorOverrides[a_actorHandle][BarType::kStamina].SetOverride(a_colorType, a_color);
			break;
		}

		_pendingColorChanges.emplace(a_actorHandle);
	}

	void TrueHUDMenu::OverrideSpecialBarColor(RE::ObjectRefHandle a_actorHandle, BarColorType a_colorType, uint32_t a_color)
	{
		_colorOverrides[a_actorHandle][BarType::kSpecial].SetOverride(a_colorType, a_color);

		_pendingColorChanges.emplace(a_actorHandle);
	}

	void TrueHUDMenu::RevertBarColor(RE::ObjectRefHandle a_actorHandle, RE::ActorValue a_actorValue, BarColorType a_colorType)
	{
		switch (a_actorValue) {
		case RE::ActorValue::kHealth:
			_colorOverrides[a_actorHandle][BarType::kHealth].RemoveOverride(a_colorType);
			if (_colorOverrides[a_actorHandle][BarType::kHealth].IsEmpty()) {
				_colorOverrides[a_actorHandle].erase(BarType::kHealth);
				if (_colorOverrides[a_actorHandle].empty()) {
					_colorOverrides.erase(a_actorHandle);
				}
			}
			break;
		case RE::ActorValue::kMagicka:
			_colorOverrides[a_actorHandle][BarType::kMagicka].RemoveOverride(a_colorType);
			if (_colorOverrides[a_actorHandle][BarType::kMagicka].IsEmpty()) {
				_colorOverrides[a_actorHandle].erase(BarType::kMagicka);
				if (_colorOverrides[a_actorHandle].empty()) {
					_colorOverrides.erase(a_actorHandle);
				}
			}
			break;
		case RE::ActorValue::kStamina:
			_colorOverrides[a_actorHandle][BarType::kStamina].RemoveOverride(a_colorType);
			if (_colorOverrides[a_actorHandle][BarType::kStamina].IsEmpty()) {
				_colorOverrides[a_actorHandle].erase(BarType::kStamina);
				if (_colorOverrides[a_actorHandle].empty()) {
					_colorOverrides.erase(a_actorHandle);
				}
			}
			break;
		}

		_pendingColorChanges.emplace(a_actorHandle);
	}

	void TrueHUDMenu::RevertSpecialBarColor(RE::ObjectRefHandle a_actorHandle, BarColorType a_colorType)
	{
		_colorOverrides[a_actorHandle][BarType::kSpecial].RemoveOverride(a_colorType);
		if (_colorOverrides[a_actorHandle][BarType::kSpecial].IsEmpty()) {
			_colorOverrides[a_actorHandle].erase(BarType::kSpecial);
			if (_colorOverrides[a_actorHandle].empty()) {
				_colorOverrides.erase(a_actorHandle);
			}
		}

		_pendingColorChanges.emplace(a_actorHandle);
	}

	uint32_t TrueHUDMenu::GetBarColor(RE::ObjectRefHandle a_actorHandle, RE::ActorValue a_actorValue, BarColorType a_colorType) const
	{
		uint32_t retColor = 0xFFFFFF;

		bool bColorOverrideFound = false;

		BarType barType = BarType::kHealth;

		switch (a_actorValue) {
		case RE::ActorValue::kHealth:
			barType = BarType::kHealth;
			break;
		case RE::ActorValue::kMagicka:
			barType = BarType::kMagicka;
			break;
		case RE::ActorValue::kStamina:
			barType = BarType::kStamina;
			break;
		}
		
		auto it = _colorOverrides.find(a_actorHandle);
		if (it != _colorOverrides.end()) {
			auto& bars = it->second;
			auto iter = bars.find(barType);
			if (iter != bars.end()) {
				auto& colors = iter->second;
				bColorOverrideFound = colors.GetColor(a_colorType, retColor);
			}
		}

		if (!bColorOverrideFound) {
			// no override found, return defaults
			retColor = GetDefaultColor(barType, a_colorType);
		}

		return retColor;
	}

	uint32_t TrueHUDMenu::GetSpecialBarColor(RE::ObjectRefHandle a_actorHandle, BarColorType a_colorType) const
	{
		uint32_t retColor = 0xFFFFFF;

		bool bColorOverrideFound = false;

		auto it = _colorOverrides.find(a_actorHandle);
		if (it != _colorOverrides.end()) {
			auto& bars = it->second;
			auto iter = bars.find(BarType::kSpecial);
			if (iter != bars.end()) {
				auto& colors = iter->second;
				bColorOverrideFound = colors.GetColor(a_colorType, retColor);
			}
		}

		if (!bColorOverrideFound) {
			// no override found, return defaults
			retColor = GetDefaultColor(BarType::kSpecial, a_colorType);
		}

		return retColor;
	}

	uint32_t TrueHUDMenu::GetDefaultColor(BarType a_barType, BarColorType a_barColorType) const
	{
		switch (a_barType) {
		case BarType::kHealth:
			switch (a_barColorType) {
			case BarColorType::BarColor:
				return Settings::uHealthColor;
			case BarColorType::PhantomColor:
				return Settings::uHealthPhantomColor;
			case BarColorType::BackgroundColor:
				return Settings::uHealthBackgroundColor;
			case BarColorType::PenaltyColor:
				return Settings::uHealthPenaltyColor;
			case BarColorType::FlashColor:
				return Settings::uHealthFlashColor;
			}
		case BarType::kMagicka:
			switch (a_barColorType) {
			case BarColorType::BarColor:
				return Settings::uMagickaColor;
			case BarColorType::PhantomColor:
				return Settings::uMagickaPhantomColor;
			case BarColorType::BackgroundColor:
				return Settings::uMagickaBackgroundColor;
			case BarColorType::PenaltyColor:
				return Settings::uMagickaPenaltyColor;
			case BarColorType::FlashColor:
				return Settings::uMagickaFlashColor;
			}
		case BarType::kStamina:
			switch (a_barColorType) {
			case BarColorType::BarColor:
				return Settings::uStaminaColor;
			case BarColorType::PhantomColor:
				return Settings::uStaminaPhantomColor;
			case BarColorType::BackgroundColor:
				return Settings::uStaminaBackgroundColor;
			case BarColorType::PenaltyColor:
				return Settings::uStaminaPenaltyColor;
			case BarColorType::FlashColor:
				return Settings::uStaminaFlashColor;
			}
		case BarType::kSpecial:
			switch (a_barColorType) {
			case BarColorType::BarColor:
				return Settings::uSpecialColor;
			case BarColorType::PhantomColor:
				return Settings::uSpecialPhantomColor;
			case BarColorType::BackgroundColor:
				return Settings::uSpecialBackgroundColor;
			case BarColorType::PenaltyColor:
				return Settings::uSpecialPenaltyColor;
			case BarColorType::FlashColor:
				return Settings::uSpecialFlashColor;
			}
		}

		return 0xFFFFFF;
	}

	void TrueHUDMenu::ToggleMenu(bool a_enable)
	{
		if (_bMenuToggled == a_enable) {
			_bMenuToggled = !a_enable;

			if (_hideCount > 0 && a_enable == true) {
				--_hideCount;
			} else if (a_enable == false) {
				++_hideCount;
			}

			UpdateVisibility();
		}
	}

	void TrueHUDMenu::SetCartMode(bool a_enable)
	{
		if (_bCartMode != a_enable) {
			_bCartMode = a_enable;

			if (_hideCount > 0 && a_enable == false) {
				--_hideCount;
			} else if (a_enable == true) {
				++_hideCount;
			}

			UpdateVisibility();
		}
	}

	void TrueHUDMenu::UpdateSettings()
	{
		// Actor info bars
		for (auto& entry : _actorInfoBarMap) {
			auto& widget = entry.second;
			if (widget->_object.IsDisplayObject()) {
				widget->Initialize();
			}
		}

		// Boss bars
		for (auto& entry : _bossInfoBarMap) {
			auto& widget = entry.second;
			if (widget->_object.IsDisplayObject()) {
				widget->Initialize();
			}
		}

		// Player widget
		if (_playerWidget) {
			if (_playerWidget->_object.IsDisplayObject()) {
				_playerWidget->Initialize();
			}
		}

		// Plugin widgets
		for (auto& pluginWidgets : _customWidgets) {
			for (auto& widgetTypes : pluginWidgets.second) {
				for (auto& entry : widgetTypes.second) {
					auto& widget = entry.second;

					if (widget->_object.IsDisplayObject()) {
						widget->Initialize();
					}
				}
			}
		}
	}

	void TrueHUDMenu::RemoveAllWidgets()
	{
		Locker locker(_lock);

		for (auto& entry : _actorInfoBarMap)
		{
			auto& widget = entry.second;
			if (widget->_object.IsDisplayObject()) {
				RE::GFxValue arg;
				arg.SetNumber(widget->_widgetID);

				widget->Dispose();
				_view->Invoke("_root.TrueHUD.RemoveInfoBarWidget", nullptr, &arg, 1);
			}
		}
		_actorInfoBarMap.clear();

		for (auto& entry : _bossInfoBarMap) {
			auto& widget = entry.second;
			if (widget->_object.IsDisplayObject()) {
				RE::GFxValue arg;
				arg.SetNumber(widget->_widgetID);

				widget->Dispose();
				_view->Invoke("_root.TrueHUD.RemoveBossInfoBarWidget", nullptr, &arg, 1);
			}
		}
		_bossInfoBarMap.clear();

		_bossQueue.clear();

		RemoveShoutIndicator();

		RemovePlayerWidget();

		for (auto& entry : _floatingTextMap) {
			auto& widget = entry.second;
			if (widget->_object.IsDisplayObject()) {
				widget->Dispose();
			}
		}
		_floatingTextMap.clear();

		for (auto& pluginWidgets : _customWidgets) {
			for (auto& widgetTypes : pluginWidgets.second) {
				for (auto& entry : widgetTypes.second) {
					auto& widget = entry.second;

					if (widget->_object.IsDisplayObject()) {
						RE::GFxValue args[3];
						args[0].SetNumber(pluginWidgets.first);
						args[1].SetNumber(widgetTypes.first);
						args[2].SetNumber(entry.first);

						widget->Dispose();
						_view->Invoke("_root.TrueHUD.RemoveCustomWidget", nullptr, args, 3);
					}
				}
				widgetTypes.second.clear();
			}
			pluginWidgets.second.clear();
		}
		_customWidgets.clear();

		_view->Invoke("_root.TrueHUD.RemoveAllCustomWidgets", nullptr, nullptr, 0);
	}

	void TrueHUDMenu::OnOpen()
	{
		_bIsOpen = true;
	}

	void TrueHUDMenu::OnClose()
	{
		RemoveAllWidgets();

		if (_bSubtitleYSaved) {
			auto hud = RE::UI::GetSingleton()->GetMenu(RE::HUDMenu::MENU_NAME);
			hud.get()->uiMovie->SetVariable("HUDMovieBaseInstance.SubtitleTextHolder._y", _savedSubtitleY);
		}		

		if (_bCompassAlphaSaved) {
			auto hud = RE::UI::GetSingleton()->GetMenu(RE::HUDMenu::MENU_NAME);
			hud.get()->uiMovie->SetVariable("HUDMovieBaseInstance.CompassShoutMeterHolder._alpha", _savedCompassAlpha);
		}

		_bIsOpen = false;
	}

	void TrueHUDMenu::ProcessDelegate(float a_deltaTime)
	{
		HUDHandler::GetSingleton()->Process(*this, a_deltaTime);
	}

	void TrueHUDMenu::Update(float a_deltaTime)
	{
		using WidgetState = WidgetBase::WidgetState;

		if (RE::UI::GetSingleton()->GameIsPaused()) {
			return;
		}

		if (Settings::bHideVanillaTargetBar || Settings::uBossBarModifyHUD == BossBarModifyHUD::kHideCompass && !_bossInfoBarMap.empty()) {
			auto hudPtr = RE::UI::GetSingleton()->GetMenu(RE::HUDMenu::MENU_NAME);
			if (hudPtr) {
				auto hud = hudPtr.get();
				if (hud && hud->uiMovie) {
					if (!_bVanillaEnemyHealthAlphaSaved) {
						hud->uiMovie->GetVariable(&_savedVanillaEnemyHealthAlpha, "HUDMovieBaseInstance.EnemyHealth_mc._alpha");
						_bVanillaEnemyHealthAlphaSaved = true;
					}

					hud->uiMovie->SetVariable("HUDMovieBaseInstance.EnemyHealth_mc._alpha", 0.f);
					_bVanillaEnemyHealthHidden = true;
				}				
			}
		} else if (_bVanillaEnemyHealthHidden) {
			auto hudPtr = RE::UI::GetSingleton()->GetMenu(RE::HUDMenu::MENU_NAME);
			if (hudPtr) {
				auto hud = hudPtr.get();
				if (hud && hud->uiMovie) {
					hud->uiMovie->SetVariable("HUDMovieBaseInstance.EnemyHealth_mc._alpha", _savedVanillaEnemyHealthAlpha);
					_bVanillaEnemyHealthHidden = false;
				}				
			}
		}

		if (Settings::bEnablePlayerWidget && !_playerWidget) {
			AddPlayerWidget();

		} else if (!Settings::bEnablePlayerWidget && _playerWidget) {
			RemovePlayerWidget();
		}

		RE::GFxValue depthArray;
		_view->CreateArray(&depthArray);

		// actor info bars
		for (auto widget_it = _actorInfoBarMap.begin(), next_widget_it = widget_it; widget_it != _actorInfoBarMap.end(); widget_it = next_widget_it) {
			++next_widget_it;
		
			auto& entry = *widget_it;
			auto& widget = entry.second;
				
			widget->ProcessDelegates();
			widget->Update(a_deltaTime);

			// remove widgets that need to be removed
			if (widget->_widgetState == WidgetState::kRemoved) {
				RemoveActorInfoBar(entry.first, WidgetRemovalMode::Immediate);
				continue;
			}

			// add to depths array
			AddToDepthsArray(widget, static_cast<uint32_t>(TrueHUDWidgetType::kInfoBar), depthArray);
		}
		
		// boss bars
		for (auto widget_it = _bossInfoBarMap.begin(), next_widget_it = widget_it; widget_it != _bossInfoBarMap.end(); widget_it = next_widget_it) {
			++next_widget_it;
		
			auto& entry = *widget_it;
			auto& widget = entry.second;
				
			widget->ProcessDelegates();
			widget->Update(a_deltaTime);

			// remove widgets that need to be removed
			if (widget->_widgetState == WidgetState::kRemoved) {
				RemoveBossInfoBar(entry.first, WidgetRemovalMode::Immediate);
				continue;
			}

			// add to depths array
			AddToDepthsArray(widget, static_cast<uint32_t>(TrueHUDWidgetType::kBossBar), depthArray);
		}
		
		if (_shoutIndicator) {
			_shoutIndicator->ProcessDelegates();
			_shoutIndicator->Update(a_deltaTime);

			AddToDepthsArray(_shoutIndicator, static_cast<uint32_t>(TrueHUDWidgetType::kStandaloneShoutIndicator), depthArray);
		}

		if (_playerWidget) {
			_playerWidget->ProcessDelegates();
			_playerWidget->Update(a_deltaTime);

			AddToDepthsArray(_playerWidget, static_cast<uint32_t>(TrueHUDWidgetType::kPlayerWidget), depthArray);
		}

		// floating text
		for (auto widget_it = _floatingTextMap.begin(), next_widget_it = widget_it; widget_it != _floatingTextMap.end(); widget_it = next_widget_it) {
			++next_widget_it;

			auto& entry = *widget_it;
			auto& widget = entry.second;

			// remove widgets that need to be removed
			RE::GFxValue::DisplayInfo displayInfo;
			if (!widget->_object.GetDisplayInfo(&displayInfo)) {
				_floatingTextMap.erase(widget_it);
				continue;
			}

			widget->ProcessDelegates();
			widget->Update(a_deltaTime);

			// add to depths array
			AddToDepthsArray(widget, static_cast<uint32_t>(TrueHUDWidgetType::kFloatingText), depthArray);
		}

		// custom widgets
		for (auto& pluginWidgets : _customWidgets) {
			RE::GFxValue args[2];
			args[0].SetNumber(pluginWidgets.first);
			_view->CreateArray(&args[1]);
			for (auto& widgetTypes : pluginWidgets.second) {
				for (auto widget_it = widgetTypes.second.begin(), next_widget_it = widget_it; widget_it != widgetTypes.second.end(); widget_it = next_widget_it) {
					++next_widget_it;

					auto& entry = *widget_it;
					auto& widget = entry.second;

					widget->ProcessDelegates();
					widget->Update(a_deltaTime);

					// remove widgets that need to be removed
					if (widget->_widgetState == WidgetState::kRemoved) {
						RemoveWidget(pluginWidgets.first, widgetTypes.first, entry.first, WidgetRemovalMode::Immediate);
						continue;
					}

					// add to depths array
					AddToDepthsArray(widget, widgetTypes.first, args[1]);
				}
			}
			_view->Invoke("_root.TrueHUD.SortCustomWidgetDepths", nullptr, args, 2);
		}

		// sort widget depths
		_view->Invoke("_root.TrueHUD.SortDepths", nullptr, &depthArray, 1);

		UpdateColors();
	}

	void TrueHUDMenu::UpdateColors()
	{
		auto playerHandle = RE::PlayerCharacter::GetSingleton()->GetHandle();
		for (auto actorHandle : _pendingColorChanges) {
			if (actorHandle == playerHandle) {
				if (_playerWidget) {
					_playerWidget->RefreshColors();
					continue;
				}
			}

			// try the boss bars first
			auto it = _bossInfoBarMap.find(actorHandle);
			if (it != _bossInfoBarMap.end()) {
				auto& widget = it->second;
				widget->RefreshColors();
				continue;
			}

			// try normal info bars
			auto iter = _actorInfoBarMap.find(actorHandle);
			if (iter != _actorInfoBarMap.end()) {
				auto& widget = iter->second;
				widget->RefreshColors();
				continue;
			}
		}
		_pendingColorChanges.clear();
	}

	void TrueHUDMenu::UpdateVisibility()
	{
		if (_view) {
			if (_hideCount > 0) {
				_view->SetVisible(false);
			} else {
				_view->SetVisible(true);
			}
		}
	}

	bool TrueHUDMenu::AddBossInfoBarWidget(RE::ObjectRefHandle a_actorHandle)
	{
		Locker locker(_lock);

		RemoveActorInfoBar(a_actorHandle, WidgetRemovalMode::Immediate);

		auto widget = std::make_shared<BossInfoBar>(_view, a_actorHandle);

		auto iter = _bossInfoBarMap.emplace(a_actorHandle, widget);
		if (iter.second) {
			RE::GFxValue arg;
			arg.SetNumber(widget->_widgetID);

			_view->Invoke("_root.TrueHUD.AddBossInfoBarWidget", &widget->_object, &arg, 1);
			if (!widget->_object.IsDisplayObject()) {
				_bossInfoBarMap.erase(iter.first);
				return false;
			}

			widget->SetIndex(GetNextBossBarIndex());
			RefreshBossBarIndexes(-1);
			widget->Initialize();
			return true;
		}

		return false;
	}

	int32_t TrueHUDMenu::GetNextBossBarIndex() const
	{
		return static_cast<int32_t>(_bossInfoBarMap.size() - 1);
	}

	void TrueHUDMenu::RefreshBossBarIndexes(int32_t a_removedIndex)
	{
		if (a_removedIndex != -1) {
			for (auto& widget : _bossInfoBarMap) {
				int32_t index = widget.second->GetIndex();
				if (index > a_removedIndex) {
					widget.second->SetIndex(index - 1);
				}
			}
		}

		switch (Settings::uBossBarModifyHUD) {
		case BossBarModifyHUD::kMoveSubtitles:
			{
				auto hud = RE::UI::GetSingleton()->GetMenu(RE::HUDMenu::MENU_NAME);

				if (!_bSubtitleYSaved) {
					hud.get()->uiMovie->GetVariable(&_savedSubtitleY, "HUDMovieBaseInstance.SubtitleTextHolder._y");
					_bSubtitleYSaved = true;
				}

				hud.get()->uiMovie->SetVariable("HUDMovieBaseInstance.SubtitleTextHolder._y", _savedSubtitleY.GetNumber() - Settings::fMultipleBossBarsOffset * _bossInfoBarMap.size());
				break;
			}

		case BossBarModifyHUD::kHideCompass:
			{
				auto hud = RE::UI::GetSingleton()->GetMenu(RE::HUDMenu::MENU_NAME);

				if (!_bCompassAlphaSaved) {
					hud.get()->uiMovie->GetVariable(&_savedCompassAlpha, "HUDMovieBaseInstance.CompassShoutMeterHolder._alpha");
					_bCompassAlphaSaved = true;
				}

				RE::GFxValue zeroAlpha;
				zeroAlpha.SetNumber(0.f);
				hud.get()->uiMovie->SetVariable("HUDMovieBaseInstance.CompassShoutMeterHolder._alpha", _bossInfoBarMap.empty() ? _savedCompassAlpha : zeroAlpha);

				if (Settings::bDisplayStandaloneShoutWidgetWhenHidingCompass && (!Settings::bEnablePlayerWidget || Settings::uPlayerWidgetShoutIndicatorMode == PlayerWidgetDisplayMode::kNever)) {
					if (!_bossInfoBarMap.empty()) {
						AddShoutIndicator();
					} else {
						RemoveShoutIndicator();
					}
				}
				break;
			}
		}
	}

	void TrueHUDMenu::UpdateBossQueue()
	{
		if (_bossQueue.size() > 0 && _bossInfoBarMap.size() < Settings::uBossBarMaxCount)
		{
			auto boss = _bossQueue.begin();

			bool bSuccess = AddBossInfoBarWidget(*boss);

			if (bSuccess) {
				_bossQueue.pop_front();
			}
		}
	}

	void TrueHUDMenu::AddToDepthsArray(std::shared_ptr<WidgetBase> a_widget, uint32_t a_widgetType, RE::GFxValue& a_array)
	{
		RE::GFxValue data;
		_view->CreateObject(&data);

		RE::GFxValue id;
		id.SetNumber(a_widget->_widgetID);
		RE::GFxValue zIndex;
		zIndex.SetNumber(a_widget->_depth);
		RE::GFxValue widgetType;
		widgetType.SetNumber(a_widgetType);

		data.SetMember("id", id);
		data.SetMember("zIndex", zIndex);
		data.SetMember("widgetType", widgetType);

		a_array.PushBack(data);
	}

	bool TrueHUDMenu::BarColorOverride::IsEmpty() const
	{
		return !bOverrideBarColor && !bOverridePhantomColor && !bOverrideBackgroundColor && !bOverridePenaltyColor && !bOverrideFlashColor;
	}

	bool TrueHUDMenu::BarColorOverride::GetColor(BarColorType a_colorType, uint32_t& a_outColor) const
	{
		switch (a_colorType) {
		case BarColorType::BarColor:
			if (bOverrideBarColor) {
				a_outColor = barColor;
				return true;
			}
			return false;
		case BarColorType::PhantomColor:
			if (bOverridePhantomColor) {
				a_outColor = phantomColor;
				return true;
			}
			return false;
		case BarColorType::BackgroundColor:
			if (bOverrideBackgroundColor) {
				a_outColor = backgroundColor;
				return true;
			}
			return false;
		case BarColorType::PenaltyColor:
			if (bOverridePenaltyColor) {
				a_outColor = penaltyColor;
				return true;
			}
			return false;
		case BarColorType::FlashColor:
			if (bOverrideFlashColor) {
				a_outColor = flashColor;
				return true;
			}
			return false;
		}
		return false;
	}

	void TrueHUDMenu::BarColorOverride::SetOverride(BarColorType a_colorType, uint32_t a_color)
	{
		switch (a_colorType) {
		case BarColorType::BarColor:
			barColor = a_color;
			bOverrideBarColor = true;
			break;
		case BarColorType::PhantomColor:
			phantomColor = a_color;
			bOverridePhantomColor = true;
			break;
		case BarColorType::BackgroundColor:
			backgroundColor = a_color;
			bOverrideBackgroundColor = true;
			break;
		case BarColorType::PenaltyColor:
			penaltyColor = a_color;
			bOverridePenaltyColor = true;
			break;
		case BarColorType::FlashColor:
			flashColor = a_color;
			bOverrideFlashColor = true;
			break;
		}
	}

	void TrueHUDMenu::BarColorOverride::RemoveOverride(BarColorType a_colorType)
	{
		switch (a_colorType) {
		case BarColorType::BarColor:
			bOverrideBarColor = false;
			break;
		case BarColorType::PhantomColor:
			bOverridePhantomColor = false;
			break;
		case BarColorType::BackgroundColor:
			bOverrideBackgroundColor = false;
			break;
		case BarColorType::PenaltyColor:
			bOverridePenaltyColor = false;
			break;
		case BarColorType::FlashColor:
			bOverrideFlashColor = false;
			break;
		}
	}

}
