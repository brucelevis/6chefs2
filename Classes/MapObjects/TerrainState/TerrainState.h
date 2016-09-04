//
//  TerrainState.h
//  6chefs2
//
//  Created by Kohei Asami on 2016/08/31.
//
//

#ifndef TerrainState_h
#define TerrainState_h

#define NS_MO_T_STATE_BEGIN namespace MapObjectState { namespace Terrain {
#define NS_MO_T_STATE_END } }
#define USING_NS_MO_T_STATE using namespace MapObjectState::Terrain;

#include "define.h"

class MapObject;
class Character;

class TerrainState : public Ref
{
// インスタンス変数
protected:
    bool _isRunnable { true };
    bool _consumeStaminaWalking { false };
    float _staminaConsumptionRatio { 1.0 };
    
// インスタンスメソッド
protected:
    TerrainState();
    virtual ~TerrainState();
    virtual bool init();
public:
    bool isRunnable() const;
    bool consumeStaminaWalking() const;
    float getStaminaConsumptionRatio() const;

// インターフェース
public:
    virtual void move(MapObject* target, const vector<Direction>& directions, function<void()> onMoved, const float ratio) = 0;
    virtual void stamp(Character* target, const Direction& direction, const float ratio) = 0;
};

#endif /* TerrainState_h */
