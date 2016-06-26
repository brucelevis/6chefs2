//
//  VideoSprite.cpp
//  6chefs2
//
//  Created by Kohei Asami on 2016/06/04.
//
//

#include "UI/VideoSprite.h"

#include <opencv2/opencv.hpp>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

// コンストラクタ
VideoSprite::VideoSprite() { FUNCLOG }

// デストラクタ
VideoSprite::~VideoSprite()
{
    FUNCLOG
    
    delete this->_videoCapture;
}

// 初期化
bool VideoSprite::init(const string& filename)
{
    if(!Node::init()) return false;
    
    string fullPath { FileUtils::getInstance()->fullPathForFilename(filename) };
    
    CCASSERT(fullPath.size() > 0, "Invalid filename for sprite");
    
    // videoのインスタンス生成
    cv::VideoCapture* videoCapture { new cv::VideoCapture(filename) };
    this->_videoCapture = videoCapture;
    
    CC_ASSERT(videoCapture->isOpened());

    this->_batchNode = nullptr;
    
    this->_recursiveDirty = false;
    setDirty(false);
    
    this->_opacityModifyRGB = true;
    
    this->_blendFunc = cocos2d::BlendFunc::ALPHA_PREMULTIPLIED;
    
    this->_flippedX = _flippedY = false;
    
    // default transform anchor: center
    setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));
    
    // zwoptex default values
    this->_offsetPosition.setZero();
    
    // clean the Quad
    memset(&_quad, 0, sizeof(_quad));
    
    // Atlas: Color
    this->_quad.bl.colors = cocos2d::Color4B::WHITE;
    this->_quad.br.colors = cocos2d::Color4B::WHITE;
    this->_quad.tl.colors = cocos2d::Color4B::WHITE;
    this->_quad.tr.colors = cocos2d::Color4B::WHITE;
    
    // shader state
    setGLProgramState(cocos2d::GLProgramState::getOrCreateWithGLProgramName(cocos2d::GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR_NO_MVP));
    
    // by default use "Self Render".
    // if the sprite is added to a batchnode, then it will automatically switch to "batchnode Render"
    setBatchNode(nullptr);
    
    this->_recursiveDirty = true;
    setDirty(true);
    
    return true;
}

// 再生
void VideoSprite::play()
{
    this->_isPlaying = true;
    this->scheduleUpdate();
}

// update
void VideoSprite::update(float delta)
{
    //動画から1Frame分読み込む
    cv::Mat m;
    this->_videoCapture->operator>>(m);
    
    if (m.empty()) {
        //スケジュールを止めてこれ以上動画を読み込まない
        this->unscheduleUpdate();
        this->_isPlaying = false;
        if(this->_playFinishCallback) _playFinishCallback(this);
        return;
    }
    
    //mはBGRで格納されるのでRGBAへと変換させる
    cv::Mat dst;
    cv::cvtColor(m, dst, CV_BGR2RGBA);
    
    if (!this->_currentTexture) {
        //まだテクスチャを作ってない場合のみ作成する
        this->_currentTexture = new cocos2d::Texture2D();
        //そのままだとRGBA8888と1px表現するのに4byte消費と多いため、RGB565の16bitへと圧縮させる
        //もし透過動画を使用したい場合は、ここをコメントアウトさせるか、PixelFormat::RGBA4444を使うのがいい
        this->_currentTexture->setDefaultAlphaPixelFormat(cocos2d::Texture2D::PixelFormat::RGB565);
        setTexture(this->_currentTexture);
    }
    
    
    //テクスチャの大きさを指定
    auto rect = new cocos2d::Rect();
    
    rect->setRect(0, 0, dst.cols, dst.rows);
    setTextureRect(*rect);
    
    //データでテクスチャを作成
    this->_currentTexture->initWithData(dst.data, dst.channels()*dst.cols*dst.rows, cocos2d::Texture2D::PixelFormat::RGBA8888, dst.cols, dst.rows, cocos2d::Size(dst.cols, dst.rows));
    
    //不要になったRect型は削除する
    delete rect;
}
