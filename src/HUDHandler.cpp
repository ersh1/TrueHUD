#include "HUDHandler.h"

#include "Offsets.h"
#include "Settings.h"
#include "Utils.h"

void HUDHandler::Register()
{
	auto scriptEventSourceHolder = RE::ScriptEventSourceHolder::GetSingleton();
	scriptEventSourceHolder->GetEventSource<RE::TESCombatEvent>()->AddEventSink(HUDHandler::GetSingleton());
	logger::info("Registered {}"sv, typeid(RE::TESCombatEvent).name());
	scriptEventSourceHolder->GetEventSource<RE::TESDeathEvent>()->AddEventSink(HUDHandler::GetSingleton());
	logger::info("Registered {}"sv, typeid(RE::TESDeathEvent).name());
	scriptEventSourceHolder->GetEventSource<RE::TESEnterBleedoutEvent>()->AddEventSink(HUDHandler::GetSingleton());
	logger::info("Registered {}"sv, typeid(RE::TESEnterBleedoutEvent).name());
	scriptEventSourceHolder->GetEventSource<RE::TESHitEvent>()->AddEventSink(HUDHandler::GetSingleton());
	logger::info("Registered {}"sv, typeid(RE::TESHitEvent).name());
	auto ui = RE::UI::GetSingleton();
	ui->AddEventSink<RE::MenuOpenCloseEvent>(HUDHandler::GetSingleton());
	logger::info("Registered {}"sv, typeid(RE::MenuOpenCloseEvent).name());
}

// On combat start, add info/boss bar; on combat end, remove bar
HUDHandler::EventResult HUDHandler::ProcessEvent(const RE::TESCombatEvent* a_event, RE::BSTEventSource<RE::TESCombatEvent>*)
{
	using WidgetRemovalMode = TRUEHUD_API::WidgetRemovalMode;

	bool bInfoBarsEnabled = Settings::bEnableActorInfoBars && 
							(Settings::uInfoBarDisplayHostiles > InfoBarsDisplayMode::kNever ||
								Settings::uInfoBarDisplayTeammates > InfoBarsDisplayMode::kNever ||
								Settings::uInfoBarDisplayOthers > InfoBarsDisplayMode::kNever);

	if (bInfoBarsEnabled || Settings::bEnableBossBars) {
		using CombatState = RE::ACTOR_COMBAT_STATE;

		const auto isPlayerRef = [](auto&& a_ref) {
			return a_ref && a_ref->IsPlayerRef();
		};

		if (a_event && a_event->actor && a_event->targetActor) {
			bool bIsRelevant = a_event->targetActor->IsPlayerRef();
			if (!bIsRelevant && bInfoBarsEnabled) {
				auto actor = a_event->actor->As<RE::Actor>();
				if (Utils::IsPlayerTeammateOrSummon(actor)) {
					bIsRelevant = true;
				}
			}

			if (bIsRelevant) {
				auto actorHandle = a_event->actor->GetHandle();
				if (a_event->newState == CombatState::kCombat) {
					bool bHasBossBarAlready = HUDHandler::GetTrueHUDMenu()->HasBossInfoBar(actorHandle);
					if (bHasBossBarAlready) {
						return EventResult::kContinue;
					}

					if (Settings::bEnableBossBars && CheckActorForBoss(actorHandle)) {  // Add a boss bar
						HUDHandler::GetSingleton()->AddBossInfoBar(actorHandle);
					} else if (bInfoBarsEnabled) {  // Not a boss, add a normal info bar
						auto playerCharacter = RE::PlayerCharacter::GetSingleton();
						auto actor = a_event->actor->As<RE::Actor>();

						if (actor->IsHostileToActor(playerCharacter)) {
							if (Settings::uInfoBarDisplayHostiles > InfoBarsDisplayMode::kOnHit) {
								HUDHandler::GetSingleton()->AddActorInfoBar(actorHandle);
							}
						} else if (Utils::IsPlayerTeammateOrSummon(actor)) {
							if (Settings::uInfoBarDisplayTeammates > InfoBarsDisplayMode::kOnHit) {
								HUDHandler::GetSingleton()->AddActorInfoBar(actorHandle);
							}
						} else if (Settings::uInfoBarDisplayOthers > InfoBarsDisplayMode::kOnHit) {
							HUDHandler::GetSingleton()->AddActorInfoBar(actorHandle);
						}
					}

				} else if (a_event->newState == CombatState::kNone) {  // Combat end, remove bar
					if (Settings::bEnableBossBars) {
						HUDHandler::GetSingleton()->RemoveBossInfoBar(actorHandle, WidgetRemovalMode::Normal);
					}

					if (bInfoBarsEnabled) {
						HUDHandler::GetSingleton()->RemoveActorInfoBar(actorHandle, WidgetRemovalMode::Normal);
					}
				}
			}
		}
	}

	return EventResult::kContinue;
}

// On death, remove info/boss bar
HUDHandler::EventResult HUDHandler::ProcessEvent(const RE::TESDeathEvent* a_event, RE::BSTEventSource<RE::TESDeathEvent>*)
{
	using WidgetRemovalMode = TRUEHUD_API::WidgetRemovalMode;

	if (a_event && a_event->actorDying) {
		bool bInfoBarsEnabled = Settings::uInfoBarDisplayHostiles > InfoBarsDisplayMode::kNever ||
		                        Settings::uInfoBarDisplayTeammates > InfoBarsDisplayMode::kNever ||
		                        Settings::uInfoBarDisplayOthers > InfoBarsDisplayMode::kNever;

		if (bInfoBarsEnabled || Settings::bEnableBossBars) {
			auto actorHandle = a_event->actorDying->GetHandle();
			if (Settings::bEnableBossBars) {
				HUDHandler::GetSingleton()->RemoveBossInfoBar(actorHandle, WidgetRemovalMode::Delayed);
			}

			if (bInfoBarsEnabled) {
				HUDHandler::GetSingleton()->RemoveActorInfoBar(actorHandle, WidgetRemovalMode::Delayed);
			}
		}
	}

	return EventResult::kContinue;
}

// On bleedout, for essential targets
HUDHandler::EventResult HUDHandler::ProcessEvent(const RE::TESEnterBleedoutEvent* a_event, RE::BSTEventSource<RE::TESEnterBleedoutEvent>*)
{
	using WidgetRemovalMode = TRUEHUD_API::WidgetRemovalMode;

	if (a_event && a_event->actor) {
		auto actor = a_event->actor->As<RE::Actor>();
		if (actor && actor->IsEssential()) {
			bool bInfoBarsEnabled = Settings::uInfoBarDisplayHostiles > InfoBarsDisplayMode::kNever ||
			                        Settings::uInfoBarDisplayTeammates > InfoBarsDisplayMode::kNever ||
			                        Settings::uInfoBarDisplayOthers > InfoBarsDisplayMode::kNever;

			if (bInfoBarsEnabled || Settings::bEnableBossBars) {
				auto actorHandle = a_event->actor->GetHandle();
				if (Settings::bEnableBossBars) {
					HUDHandler::GetSingleton()->RemoveBossInfoBar(actorHandle, WidgetRemovalMode::Delayed);
				}

				if (bInfoBarsEnabled) {
					HUDHandler::GetSingleton()->RemoveActorInfoBar(actorHandle, WidgetRemovalMode::Delayed);
				}
			}
		}
	}

	return EventResult::kContinue;
}

// On hit, add info/boss bar
HUDHandler::EventResult HUDHandler::ProcessEvent(const RE::TESHitEvent* a_event, RE::BSTEventSource<RE::TESHitEvent>*)
{
	bool bInfoBarsEnabled = Settings::bEnableActorInfoBars &&
	                        (Settings::uInfoBarDisplayHostiles > InfoBarsDisplayMode::kNever ||
								Settings::uInfoBarDisplayTeammates > InfoBarsDisplayMode::kNever ||
								Settings::uInfoBarDisplayOthers > InfoBarsDisplayMode::kNever);

	if (bInfoBarsEnabled || Settings::bEnableBossBars) {
		if (a_event && a_event->cause && a_event->target) {
			auto causeActorHandle = a_event->cause->GetHandle();
			auto targetActorHandle = a_event->target->GetHandle();

			if (causeActorHandle == targetActorHandle) {
				return EventResult::kContinue;
			}

			if (causeActorHandle && targetActorHandle) {
				auto causeActor = causeActorHandle.get()->As<RE::Actor>();
				auto targetActor = targetActorHandle.get()->As<RE::Actor>();

				if (causeActor && targetActor) {
					if (causeActor->IsDead() || targetActor->IsDead()) {
						return EventResult::kContinue;
					}

					bool bCausePlayer = causeActor->IsPlayerRef();
					bool bCausePlayerTeammate = Utils::IsPlayerTeammateOrSummon(causeActor);
					bool bTargetPlayer = targetActor->IsPlayerRef();
					bool bTargetPlayerTeammate = Utils::IsPlayerTeammateOrSummon(targetActor);

					if (bCausePlayer || bCausePlayerTeammate) {
						bool bHasBossBarAlready = HUDHandler::GetTrueHUDMenu()->HasBossInfoBar(targetActorHandle);
						if (bHasBossBarAlready) {
							return EventResult::kContinue;
						}

						if (Settings::bEnableBossBars && targetActor->IsInCombat() && CheckActorForBoss(targetActorHandle)) {
							HUDHandler::GetSingleton()->AddBossInfoBar(targetActorHandle);
						} else if (bInfoBarsEnabled) {  // Not a boss, add a normal info bar
							if (targetActor->IsHostileToActor(RE::PlayerCharacter::GetSingleton())) {
								if (Settings::uInfoBarDisplayHostiles > InfoBarsDisplayMode::kNever && targetActor->IsInCombat()) {
									HUDHandler::GetSingleton()->AddActorInfoBar(targetActorHandle);
								}
							} else if (!bTargetPlayerTeammate && Settings::uInfoBarDisplayOthers > InfoBarsDisplayMode::kNever) {  // don't show info bars on teammates when hit by player
								if (Settings::uInfoBarDisplayOthers > InfoBarsDisplayMode::kNever && targetActor->IsInCombat()) {
									HUDHandler::GetSingleton()->AddActorInfoBar(targetActorHandle);
								}
							}
						}
					} else if (bTargetPlayer || bTargetPlayerTeammate) {
						bool bHasBossBarAlready = HUDHandler::GetTrueHUDMenu()->HasBossInfoBar(causeActorHandle);
						if (bHasBossBarAlready) {
							return EventResult::kContinue;
						}

						if (Settings::bEnableBossBars && causeActor->IsInCombat() && CheckActorForBoss(causeActorHandle)) {
							HUDHandler::GetSingleton()->AddBossInfoBar(causeActorHandle);
						} else if (bInfoBarsEnabled) {  // Not a boss, add a normal info bar
							if (causeActor->IsHostileToActor(RE::PlayerCharacter::GetSingleton())) {
								if (Settings::uInfoBarDisplayHostiles > InfoBarsDisplayMode::kNever && causeActor->IsInCombat()) {
									HUDHandler::GetSingleton()->AddActorInfoBar(causeActorHandle);
								}
							} else if (!bCausePlayerTeammate && Settings::uInfoBarDisplayOthers > InfoBarsDisplayMode::kNever) {  // don't show info bars on teammates when they hit player
								if (Settings::uInfoBarDisplayOthers > InfoBarsDisplayMode::kNever && causeActor->IsInCombat()) {
									HUDHandler::GetSingleton()->AddActorInfoBar(causeActorHandle);
								}
							}
						}
					}
				}
			}
		}
	}

	return EventResult::kContinue;
}

HUDHandler::EventResult HUDHandler::ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*)
{
	using ContextID = RE::UserEvents::INPUT_CONTEXT_ID;
	// On HUD menu open/close - open/close the plugin's HUD menu
	if (a_event && a_event->menuName == RE::HUDMenu::MENU_NAME) {
		if (a_event->opening) {
			HUDHandler::GetSingleton()->OpenTrueHUDMenu();
		} else {
			HUDHandler::GetSingleton()->CloseTrueHUDMenu();
		}
	}

	// Hide the widgets when a menu is open
	auto controlMap = RE::ControlMap::GetSingleton();
	if (controlMap) {
		auto& priorityStack = controlMap->contextPriorityStack;
		if (priorityStack.empty()) {
			HUDHandler::GetSingleton()->SetMenuVisibilityMode(MenuVisibilityMode::kHidden);
		} else if (priorityStack.back() == ContextID::kGameplay ||
				   priorityStack.back() == ContextID::kFavorites ||
				   priorityStack.back() == ContextID::kConsole) {
			HUDHandler::GetSingleton()->SetMenuVisibilityMode(MenuVisibilityMode::kVisible);
		} else if ((priorityStack.back() == ContextID::kCursor ||
					   priorityStack.back() == ContextID::kItemMenu ||
					   priorityStack.back() == ContextID::kMenuMode ||
					   priorityStack.back() == ContextID::kInventory) &&
				   (RE::UI::GetSingleton()->IsMenuOpen(RE::DialogueMenu::MENU_NAME) ||
					   !Settings::bRecentLootHideInCraftingMenus && RE::UI::GetSingleton()->IsMenuOpen(RE::CraftingMenu::MENU_NAME) ||
					   !Settings::bRecentLootHideInInventoryMenus && (RE::UI::GetSingleton()->IsMenuOpen(RE::BarterMenu::MENU_NAME) ||
																		 RE::UI::GetSingleton()->IsMenuOpen(RE::ContainerMenu::MENU_NAME) ||
																		 RE::UI::GetSingleton()->IsMenuOpen(RE::GiftMenu::MENU_NAME) ||
																		 RE::UI::GetSingleton()->IsMenuOpen(RE::InventoryMenu::MENU_NAME)))) {
			HUDHandler::GetSingleton()->SetMenuVisibilityMode(MenuVisibilityMode::kPartial);
		} else {
			HUDHandler::GetSingleton()->SetMenuVisibilityMode(MenuVisibilityMode::kHidden);
		}
	}

	return EventResult::kContinue;
}

void HUDHandler::OpenTrueHUDMenu()
{
	if (!IsTrueHUDMenuOpen()) {
		auto msgQ = RE::UIMessageQueue::GetSingleton();
		if (msgQ) {
			msgQ->AddMessage(TrueHUDMenu::MenuName(), RE::UI_MESSAGE_TYPE::kShow, nullptr);
		}
	}
}

void HUDHandler::CloseTrueHUDMenu()
{
	if (IsTrueHUDMenuOpen()) {
		auto msgQ = RE::UIMessageQueue::GetSingleton();
		if (msgQ) {
			msgQ->AddMessage(TrueHUDMenu::MenuName(), RE::UI_MESSAGE_TYPE::kHide, nullptr);
		}
	}
}

bool HUDHandler::IsTrueHUDMenuOpen()
{
	auto trueHUD = GetTrueHUDMenu();
	if (trueHUD) {
		return trueHUD->IsOpen();
	}
	return false;
}

RE::GPtr<Scaleform::TrueHUDMenu> HUDHandler::GetTrueHUDMenu()
{
	auto ui = RE::UI::GetSingleton();
	return ui ? ui->GetMenu<TrueHUDMenu>(TrueHUDMenu::MenuName()) : nullptr;
}

void HUDHandler::Update()
{

}

RE::ObjectRefHandle HUDHandler::GetTarget() const
{
	return _currentTarget;
}

RE::ObjectRefHandle HUDHandler::GetSoftTarget() const
{
	return _currentSoftTarget;
}

void HUDHandler::SetTarget(RE::ObjectRefHandle a_actorHandle)
{
	if (a_actorHandle.native_handle() == 0x100000) {
		return;
	}

	AddHUDTask([a_actorHandle](TrueHUDMenu& a_menu) {
		a_menu.SetTarget(a_actorHandle);
	});

	_currentTarget = a_actorHandle;
}

void HUDHandler::SetSoftTarget(RE::ObjectRefHandle a_actorHandle)
{
	if (a_actorHandle.native_handle() == 0x100000) {
		return;
	}

	AddHUDTask([a_actorHandle](TrueHUDMenu& a_menu) {
		a_menu.SetSoftTarget(a_actorHandle);
	});

	_currentSoftTarget = a_actorHandle;
}

bool HUDHandler::HasActorInfoBar(RE::ObjectRefHandle a_actorHandle)
{
	return GetTrueHUDMenu()->HasActorInfoBar(a_actorHandle);
}

void HUDHandler::AddActorInfoBar(RE::ObjectRefHandle a_actorHandle)
{
	if (!Settings::bEnableActorInfoBars) {
		return;
	}

	if (a_actorHandle.native_handle() == 0x100000) {
		return;
	}

	if (a_actorHandle) {
		AddHUDTask([a_actorHandle](TrueHUDMenu& a_menu) {
			a_menu.AddActorInfoBar(a_actorHandle);
		});
	}
}

void HUDHandler::RemoveActorInfoBar(RE::ObjectRefHandle a_actorHandle, WidgetRemovalMode a_removalMode /*= kImmediate*/)
{
	if (a_actorHandle) {
		AddHUDTask([a_actorHandle, a_removalMode](TrueHUDMenu& a_menu) {
			a_menu.RemoveActorInfoBar(a_actorHandle, a_removalMode);
		});
	}
}

bool HUDHandler::HasBossInfoBar(RE::ObjectRefHandle a_actorHandle)
{
	return GetTrueHUDMenu()->HasBossInfoBar(a_actorHandle);
}

void HUDHandler::AddBossInfoBar(RE::ObjectRefHandle a_actorHandle)
{
	if (!Settings::bEnableBossBars) {
		return;
	}

	if (a_actorHandle.native_handle() == 0x100000) {
		return;
	}

	if (a_actorHandle) {
		AddHUDTask([a_actorHandle](TrueHUDMenu& a_menu) {
			a_menu.AddBossInfoBar(a_actorHandle);
		});
	}
}

void HUDHandler::RemoveBossInfoBar(RE::ObjectRefHandle a_actorHandle, WidgetRemovalMode a_removalMode /*= kImmediate*/)
{
	if (a_actorHandle) {
		AddHUDTask([a_actorHandle, a_removalMode](TrueHUDMenu& a_menu) {
			a_menu.RemoveBossInfoBar(a_actorHandle, a_removalMode);
		});
	}
}

void HUDHandler::AddShoutIndicator()
{
	AddHUDTask([](TrueHUDMenu& a_menu) {
		a_menu.AddShoutIndicator();
	});
}

void HUDHandler::RemoveShoutIndicator()
{
	AddHUDTask([](TrueHUDMenu& a_menu) {
		a_menu.RemoveShoutIndicator();
	});
}

void HUDHandler::AddPlayerWidget()
{
	if (!Settings::bEnablePlayerWidget) {
		return;
	}

	AddHUDTask([](TrueHUDMenu& a_menu) {
		a_menu.AddPlayerWidget();
	});
}

void HUDHandler::RemovePlayerWidget()
{
	AddHUDTask([](TrueHUDMenu& a_menu) {
		a_menu.RemovePlayerWidget();
	});
}

void HUDHandler::UpdatePlayerWidgetChargeMeters(float a_percent, bool a_bForce, bool a_bLeftHand, bool a_bShow)
{
	if (!Settings::bEnablePlayerWidget || !Settings::bPlayerWidgetDisplayEnchantmentChargeMeter) {
		return;
	}

	AddHUDTask([a_percent, a_bForce, a_bLeftHand, a_bShow](TrueHUDMenu& a_menu) {
		a_menu.UpdatePlayerWidgetChargeMeters(a_percent, a_bForce, a_bLeftHand, a_bShow);
	});
}

void HUDHandler::AddRecentLootWidget()
{
	if (!Settings::bEnableRecentLoot) {
		return;
	}

	AddHUDTask([](TrueHUDMenu& a_menu) {
		a_menu.AddRecentLootWidget();
	});
}

void HUDHandler::RemoveRecentLootWidget()
{
	AddHUDTask([](TrueHUDMenu& a_menu) {
		a_menu.RemoveRecentLootWidget();
	});
}

void HUDHandler::AddRecentLootMessage(RE::TESBoundObject* a_object, std::string_view a_name, uint32_t a_count)
{
	if (!Settings::bEnableRecentLoot) {
		return;
	}

	if (a_name.length() == 0 || a_name == ""sv || a_name == " "sv) {
		return;
	}

	// Don't show message when those menus are open
	if (Settings::bRecentLootHideInInventoryMenus &&
		(RE::UI::GetSingleton()->IsMenuOpen(RE::BarterMenu::MENU_NAME) ||
			RE::UI::GetSingleton()->IsMenuOpen(RE::ContainerMenu::MENU_NAME) ||
			RE::UI::GetSingleton()->IsMenuOpen(RE::GiftMenu::MENU_NAME))) {
		return;
	}

	if (Settings::bRecentLootHideInCraftingMenus &&
		RE::UI::GetSingleton()->IsMenuOpen(RE::CraftingMenu::MENU_NAME)) {
		return;
	}

	AddHUDTask([a_object, a_name, a_count](TrueHUDMenu& a_menu) {
		a_menu.AddRecentLootMessage(a_object, a_name, a_count);
	});
}

void HUDHandler::AddFloatingWorldTextWidget(std::string a_text, uint32_t a_color, float a_duration, bool a_bSpecial, RE::NiPoint3 a_worldPosition)
{
	if (!Settings::bEnableFloatingText) {
		return;
	}

	AddHUDTask([a_text, a_color, a_duration, a_bSpecial, a_worldPosition](TrueHUDMenu& a_menu) {
		a_menu.AddFloatingWorldTextWidget(a_text, a_color, a_duration, a_bSpecial, a_worldPosition);
	});
}

void HUDHandler::AddFloatingScreenTextWidget(std::string a_text, uint32_t a_color, float a_duration, bool a_bSpecial, RE::NiPoint2 a_screenPosition)
{
	if (!Settings::bEnableFloatingText) {
		return;
	}

	AddHUDTask([a_text, a_color, a_duration, a_bSpecial, a_screenPosition](TrueHUDMenu& a_menu) {
		a_menu.AddFloatingScreenTextWidget(a_text, a_color, a_duration, a_bSpecial, a_screenPosition);
	});
}

void HUDHandler::AddStackingDamageWorldTextWidget(RE::ObjectRefHandle a_refHandle, float a_damage)
{
	if (!Settings::bEnableFloatingText) {
		return;
	}

	auto it = _stackingDamage.find(a_refHandle);
	if (it != _stackingDamage.end()) {
		it->second.damage += a_damage;
	} else {
		_stackingDamage.emplace(a_refHandle, a_damage);
	}
}

void HUDHandler::OverrideBarColor(RE::ActorHandle a_actorHandle, RE::ActorValue a_actorValue, BarColorType a_colorType, uint32_t a_color)
{
	AddHUDTask([a_actorHandle, a_actorValue, a_colorType, a_color](TrueHUDMenu& a_menu) {
		a_menu.OverrideBarColor(a_actorHandle, a_actorValue, a_colorType, a_color);
	});
}

void HUDHandler::OverrideSpecialBarColor(RE::ActorHandle a_actorHandle, BarColorType a_colorType, uint32_t a_color)
{
	AddHUDTask([a_actorHandle, a_colorType, a_color](TrueHUDMenu& a_menu) {
		a_menu.OverrideSpecialBarColor(a_actorHandle, a_colorType, a_color);
	});
}

void HUDHandler::RevertBarColor(RE::ActorHandle a_actorHandle, RE::ActorValue a_actorValue, BarColorType a_colorType)
{
	AddHUDTask([a_actorHandle, a_actorValue, a_colorType](TrueHUDMenu& a_menu) {
		a_menu.RevertBarColor(a_actorHandle, a_actorValue, a_colorType);
	});
}

void HUDHandler::RevertSpecialBarColor(RE::ActorHandle a_actorHandle, BarColorType a_colorType)
{
	AddHUDTask([a_actorHandle, a_colorType](TrueHUDMenu& a_menu) {
		a_menu.RevertSpecialBarColor(a_actorHandle, a_colorType);
	});
}

void HUDHandler::LoadCustomWidgets(SKSE::PluginHandle a_pluginHandle, std::string_view a_filePath, APIResultCallback&& a_successCallback)
{
	AddHUDTask([a_pluginHandle, a_filePath, a_successCallback = std::forward<APIResultCallback>(a_successCallback)](TrueHUDMenu& a_menu) mutable {
		a_menu.LoadCustomWidgets(a_pluginHandle, a_filePath, std::move(a_successCallback));
	});
}

void HUDHandler::RegisterNewWidgetType(SKSE::PluginHandle a_pluginHandle, uint32_t a_widgetType)
{
	AddHUDTask([a_pluginHandle, a_widgetType](TrueHUDMenu& a_menu) {
		a_menu.RegisterNewWidgetType(a_pluginHandle, a_widgetType);
	});
}

void HUDHandler::AddWidget(SKSE::PluginHandle a_pluginHandle, uint32_t a_widgetType, uint32_t a_widgetID, std::string_view a_symbolIdentifier, std::shared_ptr<WidgetBase> a_widget)
{
	AddHUDTask([a_pluginHandle, a_widgetType, a_widgetID, a_symbolIdentifier, a_widget](TrueHUDMenu& a_menu) {
		a_menu.AddWidget(a_pluginHandle, a_widgetType, a_widgetID, a_symbolIdentifier, a_widget);
	});
}

void HUDHandler::RemoveWidget(SKSE::PluginHandle a_pluginHandle, uint32_t a_widgetType, uint32_t a_widgetID, WidgetRemovalMode a_removalMode)
{
	AddHUDTask([a_pluginHandle, a_widgetType, a_widgetID, a_removalMode](TrueHUDMenu& a_menu) mutable {
		a_menu.RemoveWidget(a_pluginHandle, a_widgetType, a_widgetID, a_removalMode);
	});
}

void HUDHandler::FlashActorValue(RE::ObjectRefHandle a_actorHandle, RE::ActorValue a_actorValue, bool a_bLong)
{
	AddHUDTask([a_actorHandle, a_actorValue, a_bLong](TrueHUDMenu& a_menu) {
		a_menu.FlashActorValue(a_actorHandle, a_actorValue, a_bLong);
	});
}

void HUDHandler::FlashActorSpecialBar(RE::ObjectRefHandle a_actorHandle, bool a_bLong)
{
	AddHUDTask([a_actorHandle, a_bLong](TrueHUDMenu& a_menu) {
		a_menu.FlashActorSpecialBar(a_actorHandle, a_bLong);
	});
}

void HUDHandler::DrawLine(const RE::NiPoint3& a_start, const RE::NiPoint3& a_end, float a_duration, uint32_t a_color, float a_thickness)
{
	AddHUDTask([a_start, a_end, a_duration, a_color, a_thickness](TrueHUDMenu& a_menu) {
		a_menu.DrawLine(a_start, a_end, a_duration, a_color, a_thickness);
	});
}

void HUDHandler::DrawPoint(const RE::NiPoint3& a_position, float a_size, float a_duration, uint32_t a_color)
{
	AddHUDTask([a_position, a_size, a_duration, a_color](TrueHUDMenu& a_menu) {
		a_menu.DrawPoint(a_position, a_size, a_duration, a_color);
	});
}

void HUDHandler::DrawArrow(const RE::NiPoint3& a_start, const RE::NiPoint3& a_end, float a_size, float a_duration, uint32_t a_color, float a_thickness)
{
	AddHUDTask([a_start, a_end, a_size, a_duration, a_color, a_thickness](TrueHUDMenu& a_menu) {
		a_menu.DrawArrow(a_start, a_end, a_size, a_duration, a_color, a_thickness);
	});
}

void HUDHandler::DrawBox(const RE::NiPoint3& a_center, const RE::NiPoint3& a_extent, const RE::NiQuaternion& a_rotation, float a_duration, uint32_t a_color, float a_thickness)
{
	AddHUDTask([a_center, a_extent, a_rotation, a_duration, a_color, a_thickness](TrueHUDMenu& a_menu) {
		a_menu.DrawBox(a_center, a_extent, a_rotation, a_duration, a_color, a_thickness);
	});
}

void HUDHandler::DrawCircle(const RE::NiPoint3& a_center, const RE::NiPoint3& a_x, const RE::NiPoint3& a_y, float a_radius, uint32_t a_segments, float a_duration, uint32_t a_color, float a_thickness)
{
	AddHUDTask([a_center, a_x, a_y, a_radius, a_segments, a_duration, a_color, a_thickness](TrueHUDMenu& a_menu) {
		a_menu.DrawCircle(a_center, a_x, a_y, a_radius, a_segments, a_duration, a_color, a_thickness);
	});
}

void HUDHandler::DrawHalfCircle(const RE::NiPoint3& a_center, const RE::NiPoint3& a_x, const RE::NiPoint3& a_y, float a_radius, uint32_t a_segments, float a_duration, uint32_t a_color, float a_thickness)
{
	AddHUDTask([a_center, a_x, a_y, a_radius, a_segments, a_duration, a_color, a_thickness](TrueHUDMenu& a_menu) {
		a_menu.DrawHalfCircle(a_center, a_x, a_y, a_radius, a_segments, a_duration, a_color, a_thickness);
	});
}

void HUDHandler::DrawSphere(const RE::NiPoint3& a_origin, float a_radius, uint32_t a_segments, float a_duration, uint32_t a_color, float a_thickness)
{
	AddHUDTask([a_origin, a_radius, a_segments, a_duration, a_color, a_thickness](TrueHUDMenu& a_menu) {
		a_menu.DrawSphere(a_origin, a_radius, a_segments, a_duration, a_color, a_thickness);
	});
}

void HUDHandler::DrawCylinder(const RE::NiPoint3& a_start, const RE::NiPoint3& a_end, float a_radius, uint32_t a_segments, float a_duration, uint32_t a_color, float a_thickness)
{
	AddHUDTask([a_start, a_end, a_radius, a_segments, a_duration, a_color, a_thickness](TrueHUDMenu& a_menu) {
		a_menu.DrawCylinder(a_start, a_end, a_radius, a_segments, a_duration, a_color, a_thickness);
	});
}

void HUDHandler::DrawCone(const RE::NiPoint3& a_origin, const RE::NiPoint3& a_direction, float a_length, float a_angleWidth, float a_angleHeight, uint32_t a_segments, float a_duration, uint32_t a_color, float a_thickness)
{
	AddHUDTask([a_origin, a_direction, a_length, a_angleWidth, a_angleHeight, a_segments, a_duration, a_color, a_thickness](TrueHUDMenu& a_menu) {
		a_menu.DrawCone(a_origin, a_direction, a_length, a_angleWidth, a_angleHeight, a_segments, a_duration, a_color, a_thickness);
	});
}

void HUDHandler::DrawCapsule(const RE::NiPoint3& a_origin, float a_halfHeight, float a_radius, const RE::NiQuaternion& a_rotation, float a_duration, uint32_t a_color, float a_thickness)
{
	AddHUDTask([a_origin, a_halfHeight, a_radius, a_rotation, a_duration, a_color, a_thickness](TrueHUDMenu& a_menu) {
		a_menu.DrawCapsule(a_origin, a_halfHeight, a_radius, a_rotation, a_duration, a_color, a_thickness);
	});
}

void HUDHandler::SetMenuVisibilityMode(MenuVisibilityMode a_mode)
{
	AddHUDTask([a_mode](TrueHUDMenu& a_menu) {
		a_menu.SetMenuVisibilityMode(a_mode);
	});
}

void HUDHandler::SetCartMode(bool a_enable)
{
	AddHUDTask([a_enable](TrueHUDMenu& a_menu) {
		a_menu.SetCartMode(a_enable);
	});
}

void HUDHandler::RemoveAllWidgets()
{
	AddHUDTask([](TrueHUDMenu& a_menu) {
		a_menu.RemoveAllWidgets();
	});
}

bool HUDHandler::CheckActorForBoss(RE::ObjectRefHandle a_refHandle)
{
	if (!a_refHandle) {
		return false;
	}

	auto playerCharacter = RE::PlayerCharacter::GetSingleton();
	auto actor = a_refHandle.get()->As<RE::Actor>();
	if (actor && playerCharacter && (actor != playerCharacter)) {
		// Check whether the target is even alive or hostile first
		if (actor->IsDead() || (actor->IsBleedingOut() && actor->IsEssential()) || !actor->IsHostileToActor(playerCharacter)) {
			return false;
		}

		auto actorBase = actor->GetActorBase();
		RE::TESActorBase* originalBase = nullptr;

		auto extraLeveledCreature = static_cast<RE::ExtraLeveledCreature*>(actor->extraList.GetByType(RE::ExtraDataType::kLeveledCreature));
		if (extraLeveledCreature) {
			originalBase = extraLeveledCreature->originalBase;
		}

		// Check blacklist
		if (Settings::bossNPCBlacklist.contains(actorBase) || (originalBase && Settings::bossNPCBlacklist.contains(originalBase))) {
			return false;
		}

		// Check race
		if (Settings::bossRaces.contains(actor->race)) {
			return true;
		}

		// Check NPC
		if (Settings::bossNPCs.contains(actorBase) || (originalBase && Settings::bossNPCs.contains(originalBase))) {
			return true;
		}

		// Check current loc refs
		if (playerCharacter->currentLocation) {
			for (auto& ref : playerCharacter->currentLocation->specialRefs) {
				if (ref.type && Settings::bossLocRefTypes.contains(ref.type) && ref.refData.refID == actor->formID) {
					return true;
				}
			}
		}
	}

	return false;
}

void HUDHandler::OnPreLoadGame()
{
	//RemoveAllWidgets();
	//_bossTargets.clear();
}

void HUDHandler::OnSettingsUpdated()
{
	if (IsTrueHUDMenuOpen()) {
		AddHUDTask([](TrueHUDMenu& a_menu) {
			logger::info("Updating settings...");
			a_menu.UpdateSettings();
			logger::info("...success");
		});
	}
}

void HUDHandler::Initialize()
{
	
}

void HUDHandler::Process(TrueHUDMenu& a_menu, float a_deltaTime)
{
	while (!_taskQueue.empty()) {
		auto& task = _taskQueue.front();
		task(a_menu);
		_taskQueue.pop();
	}

	for (auto it = _stackingDamage.begin(), next_it = it; it != _stackingDamage.end(); it = next_it) {
		++next_it;
		DamageStack& damageStack = it->second;
		damageStack.timeElapsed += a_deltaTime;
		if (damageStack.timeElapsed > _stackingPeriodDuration) {
			if (auto actor = it->first.get().get()) {
				HUDHandler::GetSingleton()->AddFloatingWorldTextWidget(std::to_string(static_cast<int32_t>(ceil(damageStack.damage))), 0xFFFFFF, 2.f, false, actor->GetLookingAtLocation());
			}
			_stackingDamage.erase(it);
		}
	}
}

HUDHandler::HUDHandler() :
	_lock()
{}

void HUDHandler::AddHUDTask(HUDTask a_task)
{
	OpenTrueHUDMenu();
	Locker locker(_lock);
	_taskQueue.push(std::move(a_task));
}
