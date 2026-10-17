#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/CCScheduler.hpp>

using namespace geode::prelude;

class ModMenuPopup : public Popup<> {
protected:
    CCLabelBMFont* m_speedLabel = nullptr;

    bool setup() override {
        this->setTitle("Mod Menu");

        auto noclipToggle = CCMenuItemToggler::createWithStandardSprites(
            this, menu_selector(ModMenuPopup::onNoclip), 0.8f);
        noclipToggle->toggle(Mod::get()->getSavedValue<bool>("noclip", false));
        m_buttonMenu->addChildAtPosition(noclipToggle, Anchor::Center, {-80.f, 20.f});

        auto noclipLabel = CCLabelBMFont::create("Noclip", "bigFont.fnt");
        noclipLabel->setScale(0.6f);
        noclipLabel->setAnchorPoint(
