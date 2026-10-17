#include <Geode/Geode.hpp>
#include <Geode/ui/Notification.hpp>
#include <Geode/modify/PauseLayer.hpp>

using namespace geode::prelude;

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
