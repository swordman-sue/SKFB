#ifndef TECHNIQUE_H
#define TECHNIQUE_H

#include "emitter/emitter.h"
#include "affector/affector.h"
#include "cocos2d.h"
#include <vector>
#include <string>
#include <list>
#include <map>

NS_CC_BEGIN

class CCParticleEmitter;
class CCParticleAffector;
class MyParticleSystem;
class CCParticleRenderer;
class CCParticle;
class ParticleNode;

class CCParticleTechnique : public Node
{
public:
	static const float DEFAULT_WIDTH;
	static const float DEFAULT_HEIGHT;
	static const float DEFAULT_DEPTH;
	static const size_t DEFAULT_PARTICLE_QUOTA;
	static const size_t DEFAULT_EMITTER_QUOTA;
	static const Vec3 DEFAULT_AXIS;
	static const Vec3 DEFAULT_POS;
	static const float DEFAULT_ANGLE;

	typedef std::vector<CCParticleAffector*> ParticleAffectorList;
	typedef ParticleAffectorList::const_iterator ParticleAffectorIterator;

	typedef std::list<CCParticle*> ParticleList;
	typedef std::vector<CCParticle*> ParticleArray;

	typedef std::list<CCParticleEmitter*> EmitterList;
	typedef std::map<std::string, EmitterList> EmitterStrToListMap;

	typedef std::vector<CCParticleEmitter*> EmitterArray;
	typedef std::map<std::string, EmitterArray> EmitterStrToArrayMap;

public:
	CCParticleTechnique(MyParticleSystem* ps);
	virtual ~CCParticleTechnique();

public:
	MyParticleSystem* GetParticleSystem(void) const;

	//����renderer
	CCParticleRenderer* GetRender() const;
	void SetRender(CCParticleRenderer* render, bool flag = true);
	CCParticleRenderer* CreateRender(const std::string& type, bool flag = true);

	//��������
	unsigned int GetNumEmitter();
	int IndexOfEmitter(const std::string& name);
	bool IsExistEmitter(const std::string& name);
	CCParticleEmitter* GetEmitter(const std::string& name);
	CCParticleEmitter* GetEmitter(unsigned int index);
	CCParticleEmitter* CreateEmitter(const std::string& type);
	bool DestoryEmitter(unsigned int index);
	bool DestoryEmitter(const std::string& name);

	//����Ӱ����
	unsigned int GetNumAffector();
	int IndexOfAffector(const std::string& name);
	bool IsExistAffector(const std::string& name);
	CCParticleAffector* GetAffector(const std::string& name);
	CCParticleAffector* GetAffector(unsigned int index);
	CCParticleAffector* CreateAffector(const std::string& type, size_t& index, bool is_sort = true);
	bool DestoryAffector(CCParticleAffector* affector);
	bool DestoryAffector(unsigned int index);
	bool DestoryAffector(const std::string& name);

public:	
	EmitterList* FindFreeEmittedEmitter (const std::string& name);
	void RemoveFromActiveEmittedEmitters (CCParticleEmitter* emitter);
	void Prepare();
	void Expire(float timeElapsed);	//��������������
	CCParticle* CreateEmitterParticle(const std::string& name); //��������������
	void ExecuteTriggerEmitters(CCParticleEmitter* emitter, unsigned int requested, float time_elapsed, bool is_emitted = false);//��ָ����������ʼ������
	void EmitParticles(float time_elapsed);					//��������
	void TriggerAffectors(float time_elapsed);				//��������Ӱ����Ӱ��
	void ApplyMotion(float time_elapsed);					//�������ӵ��ƶ���Ϣ
	void CaclTechToWorldOnUpdate();
	virtual void UpdateFromPS(float time_elapsed);

public:
	virtual void draw();									//����renderQueue
	virtual void visit(Renderer *renderer, const Mat4& parentTransform, uint32_t parentFlags);

public:
	void setPosition(float x, float y, float z);

	const std::string& GetName() const {return m_name;}
	void SetName(const std::string& name){m_name = name;}

	float GetDefaultWidth() const{return m_default_width;}
	void SetDefaultWidth(float width){m_default_width = width;}
	float GetDefaultHeight() const{return m_default_height;}
	void SetDefaultHeight(float height){m_default_height = height;}
	float GetDefaultDepth(void) const{return m_default_depth;}
	void SetDefaultDepth(float depth){m_default_depth = depth;}
	size_t GetVisualNum(void) const{return m_visual_particle_quota;}
	void SetVisualParticleQuota(size_t quota);

	size_t GetEmittedEmitterQuota(void) const{return m_emitted_emitter_quota;}
	void SetEmittedEmitterQuota(size_t quota);

	void NotityLocalSpace(bool space);

	virtual void CopyAttributesTo (CCParticleTechnique* technique);

	ParticleList* GetActiveList() { return &m_active_particle_list; }

	void SetEnabled(bool enabled) { m_enabled = enabled; }

	bool SetAttribute(const char* name, const char* value);
	bool GetAttribute(const char* name, char *value, size_t size);

	void SetTechAxis(const Vec3& axis)
	{
		m_tech_axis = axis;
		m_bTransformDirty = m_bInverseDirty = true;
	}
	void SetTechAngle(float angle)
	{
		m_tech_angle = angle;
		m_bTransformDirty = m_bInverseDirty = true;
	}
	void SetEmitterDirty(bool dirty){m_emitter_dirty = dirty;}
	bool GetEmitterDirty(){return m_emitter_dirty;}
	size_t GetActiveNum()
	{
		return m_active_particle_list.size();
	}
	bool IsLocalSpace() { return m_is_local; }
	bool IsInRect(CCParticle* particle,bool is_local_space);
	const Mat4& GetTechToWorldMat() { return m_tech_to_world_mat; }
	const Mat4& GetTechToParentMat();
	void SetStopEmitted(bool stop) { m_stop_emitted = stop; }
	bool IsTemplate();
	void ResetData();
	bool HasLiveForeverEmitter();
	const Vec3& GetOutsidePos() { return m_outside_pos; }
	float GetUpdateLastTime() { return m_last_time; }
	void SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element);
	void SetIsPSEmit(bool is_ps_emit){ m_is_ps_emit = is_ps_emit; }
	void MakeGrey(bool is_grey);

protected:
	void DestoryParticlePool();
	void DestoryAllEmittedEmitters();

private:
	void DestoryRender();
	void DestoryAllEmitter();
	void DestoryAllAffector();

	CCParticle* CreateParticle(void);				// �ӿ����б�ȡ�����ӷŵ���Ч�б�
	void SortByZorder(std::list<CCParticle*>& active_particles);

private:
	Vec3 m_tech_axis;
	float m_tech_angle;
	Mat4 m_tech_to_world_mat;				// �������ת��
	Mat4 m_tech_to_parent_mat;
	Vec3 m_outside_pos;						// ������ϵͳ�ⲿ������

	size_t m_visual_particle_quota;			// ���ɼ�������
	size_t m_emitted_emitter_quota;			// �����������
	float m_default_width;
	float m_default_height;
	float m_default_depth;
	float m_last_time;
	std::string m_name;
	std::string m_render_type;
	bool m_stop_emitted;
	bool m_once_init_emitted;
	bool m_enabled;
	bool m_is_local;
	bool m_bTransformDirty;
	bool m_bInverseDirty;
	bool m_emitter_dirty;
	AffineTransform m_sTransform;	// ������3.2�汾 3.2�汾û��m_sTransform��Ҫ�Լ���� add by zhuyi 2014-08-27
	bool m_is_ps_emit;				// �Ƿ�ϵͳ��Ⱦ����ϵͳ��Ⱦ���·Ǳ��ؿռ䲻����ת������������

	MyParticleSystem* m_particle_system;			// ע������ʲôʱ����m_particle_system��ʲôʱ����m_pParent
	CCParticleRenderer* m_render;
	ParticleAffectorList m_particle_affector_list;

	ParticleList m_active_particle_list;
	ParticleArray m_particle_array;
	ParticleList m_free_particle_list;
	EmitterArray m_emitter_array;
	EmitterStrToArrayMap m_str_to_emitter_array_map;
	EmitterList m_active_emitter_list;
	EmitterStrToListMap m_str_to_list_free_emitter_map;
};

NS_CC_END
#endif