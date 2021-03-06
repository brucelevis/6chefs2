//
//  StartUpScene.cpp
//  LastSupper
//
//  Created by Ryoya Ino on 2015/12/13.
//
//

#include "Scenes/StartUpScene.h"
#include "Scenes/TitleScene.h"
#include "Datas/Scene/StartUpSceneData.h"
#include "Layers/EventListener/ConfigEventListenerLayer.h"
#include "Layers/LoadingLayer.h"
#include "Utils/JsonUtils.h"
#include "Utils/CsvUtils.h"
#include "Utils/AssertUtils.h"

// 初期化
bool StartUpScene::init()
{
    if (!BaseScene::init(StartUpSceneData::create())) return false;
    
    _configListener->setKeyconfigEnabled(false);
    
    // キーコンフィグの取得
    KeyconfigManager::getInstance()->setCursorKey(PlayerDataManager::getInstance()->getGlobalData()->getCursorKey());
    KeyconfigManager::getInstance()->setDashKey(PlayerDataManager::getInstance()->getGlobalData()->getDashKey());
    
    return true;
}

// シーン生成直後
void StartUpScene::onEnter()
{
    BaseScene::onEnter();
    
    // データ準備
    ConfigDataManager::getInstance();
    CsvDataManager::getInstance();
    PlayerDataManager::getInstance();
}

// データ読み込み後
void StartUpScene::onPreloadFinished(LoadingLayer *loadingLayer)
{
    // ローディング終了
    loadingLayer->onLoadFinished();
    
    // ロゴ生成
    Sprite* logo {Sprite::createWithSpriteFrameName("the_last_dinner_log.png")};
    logo->setPosition(WINDOW_WIDTH/2, WINDOW_HEIGHT/2);
    logo->setScale(2.0f);
    logo->setOpacity(0);
    this->addChild(logo);
    
    // 効果音
    SoundManager::getInstance()->playVoice(Resource::VOICE::THE_LAST_DINNER_NANIWO, 1.0f);
    
    // ロゴのアニメーション
    logo->runAction(
            EaseCubicActionOut::create(
                Spawn::createWithTwoActions(
                    FadeIn::create(0.6f),
                    ScaleTo::create(0.6f, 0.65f)
            )
        ));
    
    // シーンのアニメーション
    this->runAction(
        Sequence::create(
            DelayTime::create(1.5f),
            TargetedAction::create(logo,FadeOut::create(1.0f)),
            CallFunc::create([](){
                Director::getInstance()->replaceScene(TitleScene::create());
            }),
            nullptr
        )
    );
}
