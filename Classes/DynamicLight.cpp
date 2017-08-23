#include "DynamicLight.h"

USING_NS_CC;

#ifdef CC_PLATFORM_MOBILE
const int DynamicLight::downScale = 2;
#else
const int DynamicLight::downScale = 1;
#endif
DynamicLight::~DynamicLight()
{
	CC_SAFE_RELEASE(shadowRenderShader);
	CC_SAFE_RELEASE(shadowMapShader);
	CC_SAFE_RELEASE(occlusionMap);
	CC_SAFE_RELEASE(shadowMap1D);
	CC_SAFE_RELEASE(finalShadowMap);
	CC_SAFE_RELEASE(shadowCasters);
	CC_SAFE_RELEASE(lightTexture);
	CC_SAFE_RELEASE(lightSprite);
}

DynamicLight * DynamicLight::create(Texture2D* texture)
{
	DynamicLight* pDlight = new(std::nothrow) DynamicLight;
	if (pDlight && pDlight->initWithTexture(texture) && pDlight->init())
	{
		pDlight->autorelease();
		return pDlight;
	}
	CC_SAFE_DELETE(pDlight);
	return nullptr;
}

bool DynamicLight::initWithTexture(cocos2d::Texture2D * texture)
{
	if (!texture)
	{
		return true;
	}

	CC_SAFE_RELEASE(lightTexture);
	lightTexture = texture;
	lightTexture->retain();
	hasTexture = true;

	return true;
}

bool DynamicLight::init()
{
	if (!Node::init()) {
		return false;
	}

	CCLOG("INIT CALLED");

	GLProgram* shadowMapShaderP = GLProgram::createWithFilenames("shaders/pass.vsh", "shaders/shadowMap.fsh");
	GLProgram* shadowRenderShaderP = nullptr;

	if (lightTexture)
		shadowRenderShaderP = GLProgram::createWithFilenames("shaders/pass.vsh", "shaders/shadowRenderTexture.fsh");
	else
		shadowRenderShaderP = GLProgram::createWithFilenames("shaders/pass.vsh", "shaders/shadowRender.fsh");

	shadowMapShader = GLProgramState::getOrCreateWithGLProgram(shadowMapShaderP);
	shadowRenderShader = GLProgramState::getOrCreateWithGLProgram(shadowRenderShaderP);

	initOcclusionMap();
	initShadowMap1D();
	initFinalShadowMap(lightTexture);
	bakedMapIsValid = false;

	return true;
}

void DynamicLight::initOcclusionMap()
{
	CC_SAFE_RELEASE(occlusionMap);
	CC_SAFE_RELEASE(occlusionMapSprite);

	occlusionMap = RenderTexture::create(lightSize, lightSize);
	occlusionMap->retain();

	occlusionMapSprite = Sprite::createWithTexture(occlusionMap->getSprite()->getTexture());
	occlusionMapSprite->setFlippedY(true);
	occlusionMapSprite->setAnchorPoint({ 0.0, 0.0 });
	occlusionMapSprite->setPosition({ 0.0f,0.0f });
	occlusionMapSprite->setGLProgramState(shadowMapShader);
	occlusionMapSprite->retain();
}

void DynamicLight::initShadowMap1D()
{
	CC_SAFE_RELEASE(shadowMap1D);
	CC_SAFE_RELEASE(shadowMap1DSprite);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	// seems like 16 pixel is the minimum height of a texture (on ios)
	shadowMap1D = RenderTexture::create(lightSize, 16);
#else
	shadowMap1D = RenderTexture::create(lightSize, 2);
#endif
	shadowMap1D->retain();

	shadowMap1DSprite = Sprite::createWithTexture(shadowMap1D->getSprite()->getTexture());
	shadowMap1DSprite->retain();
}

void DynamicLight::initFinalShadowMap(Texture2D *texture)
{
	CC_SAFE_RELEASE(finalShadowMap);
	CC_SAFE_RELEASE(finalShadowMapSprite);
	CC_SAFE_RELEASE(lightSprite);

	finalSize = lightSize * upScale;

	finalShadowMap = RenderTexture::create(finalSize, finalSize);
	finalShadowMap->retain();

	if (texture)
	{
		lightSprite = Sprite::createWithTexture(texture);
		lightSprite->retain();
		lightSprite->setAnchorPoint({ 0.5, 0.5 });
		lightSprite->setRotation(rotation);
		lightSprite->setScale(finalSize / lightSprite->getContentSize().width);
		lightSprite->setPosition({ finalSize / 2, finalSize / 2 });
		lightSprite->setFlippedY(true);
		finalShadowMap->begin();
		lightSprite->visit();
		finalShadowMap->end();
	}

	finalShadowMapSprite = Sprite::createWithTexture(finalShadowMap->getSprite()->getTexture());
	finalShadowMapSprite->setGLProgramState(shadowRenderShader);
	finalShadowMapSprite->setAnchorPoint({ 0.5, 0.5 });
	finalShadowMapSprite->getTexture()->setAntiAliasTexParameters();
	finalShadowMapSprite->retain();
}

void DynamicLight::setShadowCasters(Sprite* casters)
{
	CC_SAFE_RELEASE(shadowCasters);

	bakedMapIsValid = false;

	shadowCasters = Sprite::createWithTexture(casters->getTexture());
	shadowCasters->setContentSize(shadowCasters->getContentSize() / downScale);
	shadowCasters->setAnchorPoint(casters->getAnchorPoint());
	shadowCasters->setPosition(casters->getPosition() / downScale);
	shadowCasters->retain();
}

int DynamicLight::getLightSize()
{
	return lightSize * downScale;
}

int DynamicLight::getUpScale()
{
	return upScale;
}

const cocos2d::Color4B& DynamicLight::getColor()
{
	return color;
}

void DynamicLight::updateShadowMap()
{
	createOcclusionMap();
	createShadowMap();
}

void DynamicLight::setPosition(const Point& position)
{
	if (position / downScale == getPosition()) {
		return;
	}

	Node::setPosition(position / downScale);
	++updateCount;
	if (updateCount > updateFrequency) {
		updateCount = 0;
		bakedMapIsValid = false;
	}
}

void DynamicLight::setPosition3D(const cocos2d::Vec3 & position)
{
	if (position / downScale == getPosition3D())
		return;

	Node::setPosition3D(position / downScale);
	++updateCount;
	if (updateCount > updateFrequency) {
		updateCount = 0;
		bakedMapIsValid = false;
	}
}

void DynamicLight::drawLight()
{
	if (!bakedMapIsValid) {
		bakedMapIsValid = true;

		updateUniforms();
		updateShadowMap();

		// update shadowRenderShader textures		
		shadowRenderShader->setUniformTexture("u_texture2", shadowMap1DSprite->getTexture());
		finalShadowMapSprite->setColor({ color.r, color.g, color.b });
		finalShadowMapSprite->setOpacity(color.a);
		if (additive)
			finalShadowMapSprite->setBlendFunc(BlendFunc::ADDITIVE);
		else
			finalShadowMapSprite->setBlendFunc({ GL_SRC_COLOR , GL_ONE });
		finalShadowMapSprite->setPosition((getPositionX() + finalSize / 2) / 2, (getPositionY() + finalSize / 2) / 2);
	}	
}

void DynamicLight::updateUniforms()
{
	// update other uniforms
	shadowMapShader->setUniformVec2("resolution", Vec2(lightSize, lightSize));
	shadowMapShader->setUniformFloat("upScale", upScale);
	shadowMapShader->setUniformFloat("accuracy", accuracy);

	shadowRenderShader->setUniformVec2("resolution", Vec2(lightSize, lightSize));
	shadowRenderShader->setUniformFloat("softShadows", softShadows ? 1.0f : 0.0f);
}

void DynamicLight::createOcclusionMap()
{
	if (!shadowCasters) {
		occlusionMap->beginWithClear(0.0f, 0.0f, 0.0f, 0.0f);
		occlusionMap->end();
		return;
	}

	Point p2 = shadowCasters->getPosition();
	auto x = shadowCasters->getPositionX() - getPositionX() + lightSize / 2;
	auto y = shadowCasters->getPositionY() - getPositionY() + lightSize / 2;
	// Render light region to occluder FBO	
	occlusionMap->beginWithClear(0.0f, 0.0f, 0.0f, 0.0f);
	shadowCasters->setPosition(x, y);
	shadowCasters->visit();
	occlusionMap->end();
	shadowCasters->setPosition(p2);
}

void DynamicLight::createShadowMap()
{
	// Build a 1D shadow map from occlude FBO
	shadowMap1D->beginWithClear(0.0f, 0.0f, 0.0f, 0.0f);
	occlusionMapSprite->visit();
	shadowMap1D->end();
}

void DynamicLight::setSoftShadows(bool shadows)
{
	if (softShadows != shadows) {
		softShadows = shadows;
		bakedMapIsValid = false;
	}
}

void DynamicLight::setLightSize(int lightSize)
{
	if (this->lightSize != lightSize / downScale) {
		if (lightSize < 0) lightSize = 0;
		this->lightSize = lightSize / downScale > 1200 ? 1200 : lightSize / downScale;
		initOcclusionMap();
		initShadowMap1D();
		initFinalShadowMap(lightTexture);

		bakedMapIsValid = false;
	}
}

void DynamicLight::setRotation(float rotation)
{
	if (hasTexture && this->rotation != rotation)
	{
		this->rotation = rotation;

		lightSprite->setRotation(rotation);
		finalShadowMap->beginWithClear(0.0f, 0.0f, 0.0f, 0.0f);
		lightSprite->visit();
		finalShadowMap->end();
		bakedMapIsValid = false;
	}
}

void DynamicLight::setUpScale(float upScale)
{
	if (this->upScale != upScale) {
		this->upScale = upScale;
		bakedMapIsValid = false;
	}
}

void DynamicLight::setAccuracy(float accuracy)
{
	if (this->accuracy != accuracy) {
		this->accuracy = accuracy;
		bakedMapIsValid = false;
	}
}

void DynamicLight::setAdditive(bool additive)
{
	if (this->additive != additive) {
		this->additive = additive;
		bakedMapIsValid = false;
	}
}

void DynamicLight::setColor(const Color4B& color)
{
	if (this->color != color) {
		this->color = color;
		bakedMapIsValid = false;
	}
}
