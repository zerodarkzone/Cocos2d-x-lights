#ifndef AVALON_GRAPHICS_DYNAMICLIGHT_H
#define AVALON_GRAPHICS_DYNAMICLIGHT_H

#include "cocos2d.h"
class DynamicLightManager;
class DynamicLight : public cocos2d::Node
{
	friend class DynamicLightManager;
private:
	bool hasTexture = false;
	bool bakedMapIsValid = false;
	bool softShadows = true;
	bool additive = true;
	float upScale = 1.0f;
	float accuracy = 1.0f;
	int lightSize = 64 / downScale;
	float finalSize = lightSize * upScale;
	float rotation = 0.0f;
	cocos2d::Color4B color = { 64, 130, 77, 255 };
	cocos2d::Node* shadowCasters = nullptr;

	cocos2d::RenderTexture* occlusionMap = nullptr;
	cocos2d::Sprite* occlusionMapSprite = nullptr;
	cocos2d::RenderTexture* shadowMap1D = nullptr;
	cocos2d::Sprite* shadowMap1DSprite = nullptr;
	cocos2d::RenderTexture* finalShadowMap = nullptr;
	cocos2d::Sprite* finalShadowMapSprite = nullptr;
	cocos2d::Texture2D* lightTexture = nullptr;
	cocos2d::Sprite* lightSprite = nullptr;

	cocos2d::GLProgramState* shadowMapShader = nullptr;
	cocos2d::GLProgramState* shadowRenderShader = nullptr;

	void initOcclusionMap();
	void initShadowMap1D();
	void initFinalShadowMap(cocos2d::Texture2D *texture = nullptr);
	void updateUniforms();
	void createOcclusionMap();
	void createShadowMap();
	void updateShadowMap();

	void drawLight();
	void DebugdrawLight(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, bool transformUpdated);
public:
	static DynamicLight* create(cocos2d::Texture2D *texture = nullptr);
	static const int downScale;
	int updateFrequency = 0;
	int updateCount = 0;
	bool debugDrawEnabled = false;
	virtual bool initWithTexture(cocos2d::Texture2D *texture);
	virtual bool init() override;
	~DynamicLight();

	void setPosition(const cocos2d::Point& position) override;
	void setPosition3D(const cocos2d::Vec3& position) override;
	void setSoftShadows(bool shadows);
	void setLightSize(int lightSize);
	void setRotation(float rotation);
	void setUpScale(float upScale);
	void setAccuracy(float accuracy);
	void setAdditive(bool additive);
	void setColor(const cocos2d::Color4B& color);
	void setShadowCasters(cocos2d::Node* casters);

	int getLightSize();
	int getUpScale();
	const cocos2d::Color4B& getColor();
};
#endif /* AVALON_GRAPHICS_DYNAMICLIGHT_H */