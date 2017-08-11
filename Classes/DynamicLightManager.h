#ifndef LIGHTS_DYNAMICLIGHT_MANAGER_H
#define LIGHTS_DYNAMICLIGHT_MANAGER_H

#include "cocos2d.h"
#include "DynamicLight.h"

class DynamicLightManager : public cocos2d::Node
{
	cocos2d::RenderTexture* darkArea = nullptr;
	cocos2d::Sprite* darkAreaSprite = nullptr;
	cocos2d::Map<std::string, DynamicLight*> lightSources;
	std::string defaultName = "_lightSource_";
	int defaultNameLights = 0;
	cocos2d::Color4F ambientColor = { 0.01f, 0.01f, 0.01f, 1.0f };

	void initDarkArea();
	DynamicLightManager() :cocos2d::Node() { setName("LightManager"); }
public:
	virtual bool init();
	virtual ~DynamicLightManager();

	void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags) override;
	void update(float dt) override;

	void addLightSource(DynamicLight* lightSource, std::string name = "");
	void removeLightSource(DynamicLight* lightSource);
	void removeLightSource(const std::string& name);
	DynamicLight* getLightSource(const std::string& name);
	void setAmbientColor(const cocos2d::Color3B &color);
	void setAmbientSize(const cocos2d::Size& size);

	CREATE_FUNC(DynamicLightManager)
};
#endif /* LIGHTS_DYNAMICLIGHT_MANAGER_H */
