/****************************************************************************
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2012 		cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#ifndef __CCSHADER_H__
#define __CCSHADER_H__
/// @cond DO_NOT_SHOW

#include "platform/CCGL.h"
#include "platform/CCPlatformMacros.h"

/**
 * @addtogroup renderer
 * @{
 */

NS_CC_BEGIN

extern CC_DLL const GLchar * ccPosition_uColor_frag;
extern CC_DLL const GLchar * ccPosition_uColor_vert;

extern CC_DLL const GLchar * ccPositionColor_frag;
extern CC_DLL const GLchar * ccPositionColor_vert;

extern CC_DLL const GLchar * ccPositionColorTextureAsPointsize_vert;

extern CC_DLL const GLchar * ccPositionTexture_frag;
extern CC_DLL const GLchar * ccPositionTexture_vert;

extern CC_DLL const GLchar * ccPositionTextureA8Color_frag;
extern CC_DLL const GLchar * ccPositionTextureA8Color_vert;

extern CC_DLL const GLchar * ccPositionTextureColor_frag;
extern CC_DLL const GLchar * ccPositionTextureColor_vert;

extern CC_DLL const GLchar * ccPositionTextureColor_noMVP_frag;
extern CC_DLL const GLchar * ccPositionTextureColor_noMVP_vert;

extern CC_DLL const GLchar * ccPositionTextureColorAlphaTest_frag;

extern CC_DLL const GLchar * ccPositionTexture_uColor_frag;
extern CC_DLL const GLchar * ccPositionTexture_uColor_vert;

extern CC_DLL const GLchar * ccPositionColorLengthTexture_frag;
extern CC_DLL const GLchar * ccPositionColorLengthTexture_vert;

extern CC_DLL const GLchar * ccPositionTexture_GrayScale_frag;

extern CC_DLL const GLchar * ccLabelDistanceFieldNormal_frag;
extern CC_DLL const GLchar * ccLabelDistanceFieldGlow_frag;
extern CC_DLL const GLchar * ccLabelNormal_frag;
extern CC_DLL const GLchar * ccLabelOutline_frag;

extern CC_DLL const GLchar * ccLabel_vert;

extern CC_DLL const GLchar * cc3D_PositionTex_vert;
extern CC_DLL const GLchar * cc3D_SkinPositionTex_vert;
extern CC_DLL const GLchar * cc3D_ColorTex_frag;
extern CC_DLL const GLchar * cc3D_Color_frag;
extern CC_DLL const GLchar * cc3D_PositionNormalTex_vert;
extern CC_DLL const GLchar * cc3D_SkinPositionNormalTex_vert;
extern CC_DLL const GLchar * cc3D_ColorNormalTex_frag;
extern CC_DLL const GLchar * cc3D_ColorNormal_frag;
extern CC_DLL const GLchar * cc3D_Particle_vert;
extern CC_DLL const GLchar * cc3D_Particle_tex_frag;
extern CC_DLL const GLchar * cc3D_Particle_color_frag;
extern CC_DLL const GLchar * cc3D_Skybox_vert;
extern CC_DLL const GLchar * cc3D_Skybox_frag;
extern CC_DLL const GLchar * cc3D_Terrain_vert;
extern CC_DLL const GLchar * cc3D_Terrain_frag;
extern CC_DLL const GLchar * ccCameraClearVert;
extern CC_DLL const GLchar * ccCameraClearFrag;
// ETC1 ALPHA supports.
extern CC_DLL const GLchar* ccETC1ASPositionTextureColor_frag;
extern CC_DLL const char* ccETC1ASPositionTextureGray_frag;

// by zquan 2014.03.28 添加对粒子系统着色器的支持
extern CC_DLL const GLchar * ccModelBlend_vert;
extern CC_DLL const GLchar * ccModelBlend_frag;
extern CC_DLL const GLchar * ccModelBlend_grey_frag;

extern CC_DLL const GLchar * ccModelBlend_AddMode_frag;

extern CC_DLL const GLchar * ccModelBlend_NotColor_vert;
extern CC_DLL const GLchar * ccModelBlend_NotColor_frag;

extern CC_DLL const GLchar * ccModelBlend_NotColor_grey_frag;
extern CC_DLL const GLchar * ccModelBlend_NotColor_AddMode_frag;

extern CC_DLL const GLchar * ccParticleAddBlend_vert;
extern CC_DLL const GLchar * ccParticleAddBlend_frag;

extern CC_DLL const GLchar * ccParticleNoMVP_vert;
//end

// add by hongyi 2014-4-24
extern CC_DLL const GLchar * ccPositionTextureColor_grey;
extern CC_DLL const GLchar * ccPositionTextureColor_black;
extern CC_DLL const GLchar * ccPositionTextureColor_bright;
//end

// add by swordman sue 2017-9-30
extern CC_DLL const GLchar * ccRichTextPositionTextureColor_frag;
//end

// add by swordman sue 2018-11-15
extern CC_DLL const GLchar * ccPositionTextureColor_blur_frag;
//end

// add by swordman sue 2018-11-16
extern CC_DLL const GLchar * ccPositionTextureColor_feather_frag;
//end

NS_CC_END
/**
 end of support group
 @}
 */
/// @endcond
#endif /* __CCSHADER_H__ */
