# Cocos2d-x-lights
Shader based simple 2d dynamic lighting engine (WIP).

A lot of the code is based on https://github.com/mattdesl/lwjgl-basics/wiki/2D-Pixel-Perfect-Shadows

![Demo](https://github.com/zerodarkzone/Cocos2d-Light2D/blob/master/test.gif "demo shadow") ![Demo](https://github.com/zerodarkzone/Cocos2d-Light2D/blob/master/test2.gif "demo shadow2")

### Example code
Inside main scene
```
// create the light manager
auto lightManager = DynamicLightManager::create();
// use a "large" z-order to make sure this is drawn on top of everything
this->addChild(lightManager, 20);

// load the image used as the shadow casters, solid pixels block the lights
auto shadowCasters = Sprite::create("shadowCasters.png");

// add some lights

// light without use of a light texture
auto light1 = DynamicLight::create();
light1->setShadowCasters(shadowCasters);
light1->setSoftShadows(true);
light1->setAdditive(true);
light1->setLightSize(256);
light1->setColor(Color4B(158, 255, 121, 230));
light1->setPosition(Vec2(256, 256));
lightManager->addLightSource(light1);

// light with light texture. Can be used to create spotLights (see light2.png inside images folder)
auto lightSpr = Sprite::create("light2.png");
auto light2 = DynamicLight::create(lightSpr->getTexture());
light2->setShadowCasters(shadowCasters);
light2->setSoftShadows(true);
light2->setAdditive(true);
light2->setLightSize(512);
light2->setColor(Color4B(158, 255, 121, 230));
light2->setPosition(Vec2(128, 256));
light2->setRotation(45); // textured lights can be rotated
lightManager->addLightSource(light2);

```
