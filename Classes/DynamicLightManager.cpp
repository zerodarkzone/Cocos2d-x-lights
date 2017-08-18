#include "DynamicLightManager.h"

USING_NS_CC;


bool DynamicLightManager::init()
{
	if (!Node::init())
		return false;
	lightSources.reserve(20);
	initDarkArea();
	scheduleUpdate();
	return true;
}

void DynamicLightManager::initDarkArea()
{
	CC_SAFE_RELEASE(darkArea);
	CC_SAFE_RELEASE(darkAreaSprite);
	auto visibleSize = Director::getInstance()->getVisibleSize() / DynamicLight::downScale;

	darkArea = RenderTexture::create(visibleSize.width, visibleSize.height);
	darkArea->retain();

	darkAreaSprite = Sprite::createWithTexture(darkArea->getSprite()->getTexture());
	darkAreaSprite->setCameraMask((unsigned short)CameraFlag::USER1, true);
	darkAreaSprite->setBlendFunc({ GL_DST_COLOR ,GL_ONE_MINUS_SRC_ALPHA });
	darkAreaSprite->setAnchorPoint({ 0, 0 });
	darkAreaSprite->setFlippedY(true);
	darkAreaSprite->getTexture()->setAntiAliasTexParameters();
	darkAreaSprite->setScale(DynamicLight::downScale);
	darkAreaSprite->retain();
}

DynamicLightManager::~DynamicLightManager()
{
	CC_SAFE_RELEASE(darkArea);
	CC_SAFE_RELEASE(darkAreaSprite);
	lightSources.clear();
}

void DynamicLightManager::draw(cocos2d::Renderer * renderer, const cocos2d::Mat4 & transform, uint32_t flags)
{
	darkAreaSprite->visit(renderer, transform, flags);
}

void DynamicLightManager::update(float dt)
{
	for (auto&& light : lightSources)
	{
		light.second->drawLight();
	}
	darkArea->beginWithClear(ambientColor.r, ambientColor.g, ambientColor.b, ambientColor.a);
	for (auto&& light : lightSources)
	{
		light.second->finalShadowMapSprite->visit();
	}
	darkArea->end();
}

void DynamicLightManager::addLightSource(DynamicLight * lightSource, std::string name)
{
	if (name == "")
		name = defaultName + std::to_string(defaultNameLights++);
	lightSource->setName(name);
	lightSources.insert(name, lightSource);
}

void DynamicLightManager::removeLightSource(DynamicLight * lightSource)
{
	lightSources.erase(lightSource->getName());
}

void DynamicLightManager::removeLightSource(const std::string& name)
{
	lightSources.erase(name);
}

DynamicLight * DynamicLightManager::getLightSource(const std::string& name)
{
	return lightSources.find(name)->second;
}

void DynamicLightManager::setAmbientColor(const cocos2d::Color3B & color)
{
	ambientColor = { color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, ambientColor.a };
}

void DynamicLightManager::setAmbientSize(const cocos2d::Size & size)
{
	CC_SAFE_RELEASE(darkArea);
	CC_SAFE_RELEASE(darkAreaSprite);
	darkArea = RenderTexture::create(size.width / DynamicLight::downScale, size.height / DynamicLight::downScale);
	darkArea->retain();

	darkAreaSprite = Sprite::createWithTexture(darkArea->getSprite()->getTexture());
	darkAreaSprite->setCameraMask((unsigned short)CameraFlag::USER1, true);
	darkAreaSprite->setBlendFunc({ GL_DST_COLOR ,GL_ONE_MINUS_SRC_ALPHA });
	darkAreaSprite->setAnchorPoint({ 0, 0 });
	darkAreaSprite->setFlippedY(true);
	darkAreaSprite->getTexture()->setAntiAliasTexParameters();
	darkAreaSprite->setScale(DynamicLight::downScale);
	darkAreaSprite->retain();
}
