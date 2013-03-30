#ifndef __TREE_HPP__
#define __TREE_HPP__
#include <pthread.h>
#include <stdint.h>
#include <assert.h>

template <typename D>
struct Node
{
	const Node<D> *parent;
	D data;
};


template <typename D>
class MemoryManager
{
	pthread_mutex_t m_mutex;
	int refs;
	int inc_refs;
#pragma pack (push, 1)
#pragma pack (1)
	struct Element
	{
		union header_t
		{
			Element *next;
			size_t refs;
		} header;
		Node<D> data;
	};
#pragma pack (pop)

	Element *m_area;

	Element *m_free_list;
	size_t m_capacity;

	void IncRefs(Node<D> *node)
	{
#ifndef NDEBUG
		//Element * elem = reinterpret_cast<Element *>((int8_t *)node-sizeof(Element::header_t));
		Element * elem = reinterpret_cast<Element *>((int8_t *)node - sizeof(size_t));
		(elem->header.refs)++;
		assert(elem >= m_area && elem <= &m_area[m_capacity]);
		assert(elem->header.refs == 1 || elem->header.refs == 2);
#else
		++(reinterpret_cast<Element *>((int8_t *)node-sizeof(size_t))->header.refs);
#endif
	}

	size_t DecRefs(Node<D> *node)
	{
#ifndef NDEBUG
		//Element * elem = reinterpret_cast<Element *>((int8_t *)node-sizeof(Element::header_t));
		Element * elem = reinterpret_cast<Element *>((int8_t *)node-sizeof(size_t));
		assert(elem->header.refs == 1 || elem->header.refs == 2);
		return --(elem->header.refs);
#else
		return --(reinterpret_cast<Element *>((int8_t *)node-sizeof(size_t))->header.refs);
#endif
	}
public:
	MemoryManager()
	{
		m_capacity = 0;
		m_area = NULL;
		refs = 0;
		inc_refs = 0;
		pthread_mutex_init(&m_mutex, NULL);
	}

	~MemoryManager()
	{
		delete [] m_area;
		pthread_mutex_destroy(&m_mutex);
		//assert(refs == 0);
	}

	void init(size_t capacity)
	{
		if(m_capacity < capacity)
		{
			m_capacity = capacity;
			delete [] m_area;
			m_area = new Element[capacity];
			for(size_t i = 0; i < capacity-1; ++i)
			{
				m_area[i].header.next = &m_area[i+1];
			}
			m_area[capacity-1].header.next = NULL;
			m_free_list = m_area;
		}
	}

	Node<D> *alloc(const Node<D> *parent)
	{
		pthread_mutex_lock(&m_mutex);
		++refs;
		++inc_refs;
		if(m_free_list == NULL)
		{
			std::cerr << "MemoryManager: not enough memory\n";
			throw std::bad_alloc();
		}
		if(parent != NULL)
		{
			assert(CheckRefs(parent));
			IncRefs(const_cast<Node<D> *>(parent));
		}
		Element *elem = m_free_list;
		m_free_list = m_free_list->header.next;
		elem->header.refs = 0;

		Node<D> *result = &(elem->data);
		result->parent = parent;
		assert(CheckCycle(result));
		pthread_mutex_unlock(&m_mutex);
		return result;
	}

	bool CheckCycle(const Node<D> *start)
	{
		/*pthread_mutex_lock(&m_mutex);
		const Node<D> *node = start->parent;
		while(node != NULL)
		{
			if(node == start)
			{
				pthread_mutex_unlock(&m_mutex);
				return false;
			}
			node = node->parent;
		}
		pthread_mutex_unlock(&m_mutex);*/
		return true;
	}

	bool CheckRefs(const Node<D> *node)
	{
		/*pthread_mutex_lock(&m_mutex);
		//Element * elem = reinterpret_cast<Element *>((int8_t *)node-sizeof(Element::header_t));
		Element * elem = reinterpret_cast<Element *>((int8_t *)node-sizeof(size_t));
		pthread_mutex_unlock(&m_mutex);
		return elem->header.refs >= 0 && elem->header.refs <= 2;*/
		return true;
	}

	void free(Node<D> *ptr)
	{
		pthread_mutex_lock(&m_mutex);
		--refs;
		ptr->data.ap_solve.clear();
		Element *elem = reinterpret_cast<Element *>((int8_t *)ptr-sizeof(elem->header));
		assert(elem->header.refs == 0);
		elem->header.next = m_free_list;
		m_free_list = elem;
		//pthread_mutex_unlock(&m_mutex);
		if(elem->data.parent != NULL && !DecRefs(const_cast<Node<D> *>(elem->data.parent)))
		{
			assert(ptr->parent == elem->data.parent);
			ptr = const_cast<Node<D> *>(elem->data.parent);
			pthread_mutex_unlock(&m_mutex);
			//this->free(const_cast<Node<D> *>(elem->data.parent));
			this->free(ptr);
		}
		else
		{
			pthread_mutex_unlock(&m_mutex);
		}
	}
};
#endif //__TREE_HPP__
