#include <Geode/modify/SetupTriggerPopup.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/ui/GeodeUI.hpp>

using namespace geode::prelude;

bool enabled = true;

float minimumZoom = 0.0000f;
float maximumZoom = 1000.0f;

#define ASSIGN_LOCAL_VARIABLES\
	enabled = Mod::get()->getSettingValue<bool>("enabled");\
	minimumZoom = static_cast<float>(Mod::get()->getSettingValue<double>("minimumZoom"));\
	maximumZoom = static_cast<float>(Mod::get()->getSettingValue<double>("maximumZoom"));

// function recreation by @hiimjasmine00
// customization, hook prio by @raydeeux
class $modify(MyGJBaseGameLayer, GJBaseGameLayer) {
	static void onModify(auto& self) {
		(void) self.setHookPriority("GJBaseGameLayer::updateZoom", Priority::Stub);
	}
	void updateZoom(float zoom, float duration, int easing, float rate, int uniqueID, int controlID) {
		if (!enabled) return GJBaseGameLayer::updateZoom(zoom, duration, easing, rate, uniqueID, controlID);
		const float targetZoom = std::clamp(zoom > -0.01f ? zoom : minimumZoom, minimumZoom, maximumZoom);
		m_gameState.m_targetCameraZoom = targetZoom;
		if (duration > 0.0f) m_gameState.tweenValue(m_gameState.m_cameraZoom, targetZoom, 14, duration, easing, rate, uniqueID, controlID);
		else {
			m_gameState.stopTweenAction(14);
			m_gameState.m_cameraZoom = targetZoom;
		}
		m_gameState.m_unkUint1 = duration;
		if (!this->m_unk2aa5 && m_gameState.m_unkBool7) updateDualGround(m_player1, static_cast<int>(m_player1->getActiveMode()), duration == 0.0f, duration);
	}
};

class $modify(MySetupTriggerPopup, SetupTriggerPopup) {
	bool init(EffectGameObject* trigger, cocos2d::CCArray* triggers, float width, float height, int background) {
		if (!SetupTriggerPopup::init(trigger, triggers, width, height, background)) return false;
		if (!m_mainLayer || !m_mainLayer->getChildByType<CCMenu>(0) || !typeinfo_cast<SetupZoomTriggerPopup*>(this)) return true;

		CircleButtonSprite* buttonSprite = CircleButtonSprite::create(CCSprite::createWithSpriteFrameName("edit_eZoomBtn_001.png"), CircleBaseColor::DarkPurple);
		buttonSprite->setScale(.6f);
		CCMenuItemSpriteExtra* settings = CCMenuItemSpriteExtra::create(buttonSprite, this, menu_selector(MySetupTriggerPopup::onModSettingsFromAkti));
		settings->setPosition({-162.f, 150.f});
		m_mainLayer->getChildByType<CCMenu>(0)->addChild(settings);

		return true;
	}
	void onModSettingsFromAkti(CCObject* sender) {
		openSettingsPopup(Mod::get());
	}
};

$on_mod(Loaded) {
	ASSIGN_LOCAL_VARIABLES
	listenForAllSettingChanges([](std::shared_ptr<SettingV3> _){ ASSIGN_LOCAL_VARIABLES });
}