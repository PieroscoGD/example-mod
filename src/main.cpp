#include <Geode/Geode.hpp>
#include <Geode/ui/Notification.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/CCScheduler.hpp>

using namespace geode::prelude;

class ModMenuPopup : public Popup {
protected:
    CCLabelBMFont* m_speedLabel = nullptr;

    bool init() {
        if (!Popup::init(260.f, 180.f)) return false;
        this->setTitle("Mod Menu");

        auto noclipToggle = CCMenuItemToggler::createWithStandardSprites(
            this, menu_selector(ModMenuPopup::onNoclip), 0.8f);
        noclipToggle->toggle(Mod::get()->getSavedValue<bool>("noclip", false));
        m_buttonMenu->addChildAtPosition(noclipToggle, Anchor::Center, ccp(-80.f, 20.f));

        auto noclipLabel = CCLabelBMFont::create("Noclip", "bigFont.fnt");
        noclipLabel->setScale(0.6f);
        noclipLabel->setAnchorPoint(ccp(0.f, 0.5f));
        m_buttonMenu->addChildAtPosition(noclipLabel, Anchor::Center, ccp(-55.f, 20.f));

        auto speedBtn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Speed"), this,
            menu_selector(ModMenuPopup::onSpeed));
        m_buttonMenu->addChildAtPosition(speedBtn, Anchor::Center, ccp(-50.f, -30.f));

        float speed = Mod::get()->getSavedValue<float>("speed", 1.f);
        m_speedLabel = CCLabelBMFont::create(fmt::format("{}x", speed).c_str(), "bigFont.fnt");
        m_speedLabel->setScale(0.6f);
        m_buttonMenu->addChildAtPosition(m_speedLabel, Anchor::Center, ccp(50.f, -30.f));

        return true;
    }

    void onNoclip(CCObject* sender) {
        auto toggler = static_cast<CCMenuItemToggler*>(sender);
        Mod::get()->setSavedValue<bool>("noclip", !toggler->isOn());
    }

    void onSpeed(CCObject*) {
        static const std::vector<float> speeds = {0.25f, 0.5f, 1.f, 1.5f, 2.f};
        float current = Mod::get()->getSavedValue<float>("speed", 1.f);
        size_t next = 0;
        for (size_t i = 0; i < speeds.size(); i++) {
            if (speeds[i] == current) { next = (i + 1) % speeds.size(); break; }
        }
        Mod::get()->setSavedValue<float>("speed", speeds[next]);
        m_speedLabel->setString(fmt::format("{}x", speeds[next]).c_str());
    }

public:
    static ModMenuPopup* create() {
        auto ret = new ModMenuPopup();
        if (ret->init()) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }
};

class $modify(MyMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        auto sprite = CircleButtonSprite::create(
            CCLabelBMFont::create("M", "bigFont.fnt"));
        auto btn = CCMenuItemSpriteExtra::create(
            sprite, this, menu_selector(MyMenuLayer::onModMenu));

        if (auto menu = this->getChildByID("bottom-menu")) {
            menu->addChild(btn);
            menu->updateLayout();
        }
        return true;
    }

    void onModMenu(CCObject*) {
        ModMenuPopup::create()->show();
    }
};

class $modify(MyPlayLayer, PlayLayer) {
    void destroyPlayer(PlayerObject* player, GameObject* object) {
        if (Mod::get()->getSavedValue<bool>("noclip", false) &&
            object != m_anticheatSpike) {
            return;
        }
        PlayLayer::destroyPlayer(player, object);
    }
};

class $modify(MyPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();

        auto menu = CCMenu::create();
        menu->setPosition(ccp(0.f, 0.f));
        auto winSize = CCDirector::get()->getWinSize();

        auto btn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Sync Music"), this,
            menu_selector(MyPauseLayer::onSyncMusic));
        btn->setPosition(ccp(winSize.width - 80.f, 30.f));
        menu->addChild(btn);
        this->addChild(menu);
    }

    void onSyncMusic(CCObject*) {
        auto pl = PlayLayer::get();
        if (!pl) return;

        float t = pl->timeForXPos(pl->m_player1->getPositionX());
        float offset = pl->m_levelSettings->m_songOffset;
        unsigned int ms = static_cast<unsigned int>((t + offset) * 1000.f);

        FMODAudioEngine::sharedEngine()->setMusicTimeMS(ms, true, 0);

        Notification::create("Music synced", NotificationIcon::Success, 1.f)->show();
    }
};

class $modify(MyScheduler, CCScheduler) {
    void update(float dt) {
        float speed = Mod::get()->getSavedValue<float>("speed", 1.f);
        CCScheduler::update(dt * speed);
    }
};
