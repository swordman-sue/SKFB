#ifndef RAOBJLIST_H
#define RAOBJLIST_H

#include <new>
#include <stdlib.h>

/*
	RAObjList

	用法：	(1) Insert时返回一个index，以后存取该元素均用该index直接进行[]下标操作
	(2) 提供Iterator、Const_Iterator进行遍历操作

	性能：	(1) 取值、增加等操作时间复杂度为O(1)
	(2) 删除操作如果元素位于beg或end，时间复杂度为O(n)，否则为O(1)

	适用：	用于可分配ID的对象管理
*/

template<class T>
class RAObjList
{
public:
	template<class U, class PARE=RAObjList>
	class _Iterator
	{
		friend class RAObjList;
	public:
		_Iterator():m_index(-1), m_parents(0){}

		_Iterator<U, PARE>& operator ++()
		{
			++m_index;
			while (m_index < m_parents->m_end.m_index && !m_parents->m_obj_use[m_index])
			{
				++m_index;
			}
			return *this;
		}
		const _Iterator<U, PARE> operator++(int)
		{
			_Iterator<U, PARE> t = *this;
			operator ++();
			return t;
		}
		_Iterator<U, PARE>& operator --()
		{
			--m_index;
			while (m_index >= m_parents->m_beg.m_index && !m_parents->m_obj_use[m_index])
			{
				--m_index;
			}
			return *this;
		}
		const _Iterator<U, PARE> operator--(int)
		{
			_Iterator<U, PARE> t = *this;
			operator --();
			return t;
		}

		operator U*() { return &m_parents->m_obj[m_index]; }
		U* operator->() { return &m_parents->m_obj[m_index]; }
		U& operator*() { return m_parents->m_obj[m_index]; }
		bool operator==(const _Iterator<U, PARE> &_right) const { return (m_index == _right.m_index && m_parents == _right.m_parents); }
		bool operator!=(const _Iterator<U, PARE> &_right) const { return (m_index != _right.m_index || m_parents != _right.m_parents); }

		unsigned int m_index;
	protected:
		PARE *m_parents;
		_Iterator(unsigned int index, PARE *parent=0):m_index(index), m_parents(parent){}
	};
	typedef _Iterator<T> Iterator;
	typedef _Iterator<const T, const RAObjList<T> > Const_Iterator;

	#define DEFAULT_RAOBJLISE_SIZE 32
	RAObjList(unsigned int size=DEFAULT_RAOBJLISE_SIZE)
		:m_obj(0), m_obj_use(0), m_obj_cur(0), m_obj_max(0), 
		m_recovery_id(0), m_recovery_id_cur(0), m_recovery_id_max(0), 
		m_beg(0), m_end(0), m_size(0)
	{
		m_beg.m_parents = this;
		m_end.m_parents = this;
		_ResizeObj(size);
	}

	RAObjList(const RAObjList<T> &_raol)
	{
		_ConstructInit(_raol);
	}
	~RAObjList()
	{
		_Release();
	}
	const RAObjList<T> & operator=(const RAObjList<T> &_raol)
	{
		_Release();
		_ConstructInit(_raol);
		return *this;
	}

	void Clear()
	{
		for (unsigned int i = m_beg.m_index; i < m_end.m_index; ++i)
		{
			if (m_obj_use[i])
			{
				m_obj[i].~T();
			}
		}
		m_obj_cur = 0;
		m_recovery_id_cur = 0;
		m_beg.m_index = 0;
		m_end.m_index = 0;
		m_size = 0;
	}

	unsigned int Insert(const T &value)
	{
		unsigned int index = -1;
		if (m_recovery_id_cur == 0)
		{
			if (m_obj_cur == m_obj_max)
			{
				unsigned int size = (m_obj_max != 0 ? m_obj_max * 2 : DEFAULT_RAOBJLISE_SIZE);
				_ResizeObj(size);
			}

			index = m_obj_cur++;
		}
		else
		{
			--m_recovery_id_cur;
			index = m_recovery_id[m_recovery_id_cur];
		}

		new (&m_obj[index])T(value);
		m_obj_use[index] = true;

		if (m_beg.m_index == m_end.m_index) m_end.m_index = (m_beg.m_index = index) + 1;
		else
		{
			if (m_beg.m_index > index) m_beg.m_index = index;
			if (m_end.m_index <= index) m_end.m_index = index + 1;
		}
		++m_size;
		return index;
	}

	void Erase(unsigned int index)
	{
		if (index >= m_obj_cur || !m_obj_use[index]) return;

		m_obj[index].~T();

		if (m_recovery_id_cur == m_recovery_id_max)
		{
			unsigned int size = (m_recovery_id_max != 0 ? m_recovery_id_max * 2 : 32);
			_ResizeRecoveryid(size);
		}
		m_recovery_id[m_recovery_id_cur++] = index;

		m_obj_use[index] = false;

		if (m_beg.m_index == index)
		{
			unsigned int i = index;
			while(i < m_obj_cur && !m_obj_use[i] && i != m_end.m_index) ++i;
			m_beg.m_index = i;
		}
		if (m_end.m_index == index + 1 && m_beg.m_index != m_end.m_index)
		{
			unsigned int i = index;
			while(i < m_obj_cur && !m_obj_use[i] && i != m_beg.m_index) --i;
			m_end.m_index = i + 1;
		}
		--m_size;
	}

	Iterator Find(unsigned int index)
	{
		if (index < m_obj_cur && m_obj_use[index])
		{
			return Iterator(index, this);
		}
		return m_end;
	}

	Const_Iterator Find(unsigned int index) const
	{
		if (index < m_obj_cur && m_obj_use[index])
		{
			return Const_Iterator(index, this);
		}
		return Const_Iterator(m_end.m_index, this);
	}

	bool Exist(unsigned int index)
	{
		if (index < m_obj_cur && m_obj_use[index])
		{
			return true;
		}
		return false;
	}

	Iterator Beg() { return m_beg; }
	Iterator End() { return m_end; }
	Const_Iterator Beg() const { return Const_Iterator(m_beg.m_index, this); }
	Const_Iterator End() const { return Const_Iterator(m_end.m_index, this); }
	T& operator[](unsigned int i) { return m_obj[i]; }	// 此处不检查是否已经分配，由外部保证，如需检查请用Find
	const T& operator[](unsigned int i) const { return m_obj[i]; }

	bool Empty() const { return (0 == m_size); }
	unsigned int Size() const { return m_size; }
protected:
	T							*m_obj;
	bool						*m_obj_use;

	unsigned int				m_obj_cur;
	unsigned int				m_obj_max;

	unsigned int				*m_recovery_id;
	unsigned int				m_recovery_id_cur;
	unsigned int				m_recovery_id_max;

	Iterator					m_beg;
	Iterator					m_end;
	unsigned int				m_size;

	void _ResizeObj(unsigned int size)
	{
		if (size <= m_obj_max) return ;
		m_obj = (T*)realloc((void *)m_obj, sizeof(T) * size);
		m_obj_use = (bool*)realloc(m_obj_use, sizeof(bool) * size);
		m_obj_max = size;
	}

	void _ResizeRecoveryid(unsigned int size)
	{
		if (size <= m_recovery_id_max) return;
		m_recovery_id = (unsigned int *)realloc(m_recovery_id, sizeof(unsigned int) * size);
		m_recovery_id_max = size;
	}

	void _ConstructInit(const RAObjList<T> &_raol)
	{
		m_obj = 0;
		m_obj_use = 0;
		m_obj_max = 0;
		_ResizeObj(_raol.m_obj_max);
		m_obj_cur = _raol.m_obj_cur;
		memcpy(m_obj_use, _raol.m_obj_use, sizeof(bool) * m_obj_cur);

		m_recovery_id = 0;
		m_recovery_id_max = 0;
		_ResizeRecoveryid(_raol.m_recovery_id_max);
		m_recovery_id_cur = _raol.m_recovery_id_cur;
		memcpy(m_recovery_id, _raol.m_recovery_id, sizeof(unsigned int) * m_recovery_id_cur);

		m_beg.m_index = _raol.m_beg.m_index;
		m_beg.m_parents = this;
		m_end.m_index = _raol.m_end.m_index;
		m_end.m_parents = this;
		m_size = _raol.m_size;

		for (unsigned int i = m_beg.m_index; i < m_end.m_index; ++i)
		{
			if (m_obj_use[i])
				new (&m_obj[i])T(_raol.m_obj[i]);
		}
	}
	void _Release()
	{
		for (unsigned int i = m_beg.m_index; i < m_end.m_index; ++i)
		{
			if (m_obj_use[i])
				m_obj[i].~T();
		}
		if (m_obj != 0) free((void*)m_obj);
		if (m_obj_use != 0) free(m_obj_use);
		if (m_recovery_id != 0) free(m_recovery_id);
	}
};

#endif

