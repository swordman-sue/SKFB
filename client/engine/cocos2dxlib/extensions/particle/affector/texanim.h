#ifndef AFFECTOR_TEXANIM_H
#define AFFECTOR_TEXANIM_H

#include "affector.h"
#include "particle/particlefactory.h"
#include "cocos2d.h"
#include <string>
#include <vector>

NS_CC_BEGIN

class CCParticleTechnique;
class CCParticle;
class CCParticleAffector;
class CCParticleAffectorFactory;

class CCParticleTexAnimAffector : public CCParticleAffector
{
public:
	static const float DEFAULT_TIME_STEP;
	static const bool DEFAULT_FRAME_RANDOM;
	static const size_t DEFAULT_START_FRAME;
	enum TextureAnimationType
	{
		TAT_LOOP,
		TAT_NORMAL
	};

	struct  FloatRect 
	{
		float left;
		float top;
		float right;
		float bottom;

		FloatRect()
			: left(0.0), top(0.0), right(1.0), bottom(1.0)
		{
		}

		FloatRect(const FloatRect& o)
			: left(o.left), top(o.top), right(o.right), bottom(o.bottom)
		{
		}

		FloatRect& operator=(const FloatRect& o)
		{
			left = o.left;
			top = o.top;
			right = o.right;
			bottom = o.bottom;
			return *this;
		}
	};

	typedef std::vector<FloatRect> TextureCoordSets;

	static const TextureAnimationType DEFAULT_ANIM_TYPE;

	CCParticleTexAnimAffector(CCParticleTechnique* pt);

	virtual ~CCParticleTexAnimAffector() {}

	virtual bool IsFitParticle(CCParticle* particle);

	virtual void EffectParticle(CCParticle *particle, float elapsed);

	virtual void InitParticle(CCParticle* particle);

	virtual void CopyAttributesTo(CCParticleAffector* pa);


	void SetNumTex(size_t num)
	{
		if (m_num > m_row * m_col)
		{
			m_num = m_row * m_col;
		}
		if (m_num < 1)
		{
			m_num = 1;
		}
		m_num = num;
	}

	size_t GetNumTex()
	{
		return m_num;
	}

	void SetStartFrame( size_t start_frame)
	{
		if( start_frame >= 0 && start_frame < m_num)
		{
			m_start_frame = start_frame;
		}
	}
	size_t GetStartFrame() const
	{
		return m_start_frame;
	}

	void SetIsRandomStart( bool b)
	{
		m_is_random_start = b;
	}

	bool IsRandomStart() const
	{
		return m_is_random_start;
	}

	void UpdateTexcoords();

	void SetRow(size_t rows)
	{
		m_row = rows;
		if (m_row == 0)
		{
			m_row = 1;
		}
		UpdateTexcoords();
	}

	void SetCol(size_t col)
	{
		m_col = col;
		if (m_col == 0)
		{
			m_col = 1;
		}
		UpdateTexcoords();
	}

	void SetTextureCoord(TextureCoordSets& texCoords)
	{
		m_texture_coords = texCoords;
	}

	void SetTimeStep(float step)
	{
		m_time_step = step;
	}

	size_t GetRowNum() const
	{
		return m_row;
	}

	size_t GetColNum() const
	{
		return m_col;
	}

	void SetAnimType(TextureAnimationType type)
	{
		m_anim_type = type;
	}
	
	TextureAnimationType GetAnimType() const
	{
		return m_anim_type;
	}

	void SetPlayOrder(bool play_order);

	virtual void SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element);

	virtual bool SetAttribute(const char* type, const char* value);

	virtual bool GetAttribute(const char* type, char* value, size_t size);

protected:
	TextureCoordSets m_texture_coords;
	TextureAnimationType m_anim_type;
	float m_time_step;
		
	size_t  m_num;
	size_t	m_start_frame;		// 起始帧
	size_t	m_random_start_frame;
	bool	m_is_random_start;	// 是否使用随机起始帧
	size_t m_row;
	size_t m_col;
	bool m_play_order;			//是否顺序播放,true为顺序播放，false为倒叙播放
};


class  CCParticleTexAnimAffectorFactory : public CCParticleAffectorFactory
{
public:
	CCParticleTexAnimAffectorFactory() {}

	virtual ~CCParticleTexAnimAffectorFactory() {}

	virtual std::string GetType() const
	{
		return "TextureAnimator";
	}

	virtual CCParticleAffector* CreateAffector(CCParticleTechnique* pt);
};

NS_CC_END

#endif