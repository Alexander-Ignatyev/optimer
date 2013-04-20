#ifndef __TREE_HPP__
#define __TREE_HPP__

#include <cstdint>
#include <cassert>

#include <mutex>

template <typename D>
struct Node
{
	const Node<D> *parent;
	D data;
};


template <typename D>
class MemoryManager
{
	std::recursive_mutex mutex;
	int refs;
	int inc_refs;
#pragma pack (push)
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
		Element * elem = reinterpret_cast<Element *>((int8_t *)node - sizeof(size_t));
		(elem->header.refs)++;
		assert(elem >= m_area && elem <= &m_area[m_capacity]);
		assert(elem->header.refs == 1 || elem->header.refs == 2);
#else
		++(reinterpret_cast<Element *>((int8_t *)node-sizeof(size_t))->header.refs);
#endif
	}

	static size_t DecRefs(Node<D> *node)
	{
#ifndef NDEBUG
		Element * elem = reinterpret_cast<Element *>((int8_t *)node-sizeof(size_t));
		assert(elem->header.refs == 1 || elem->header.refs == 2);
		return --(elem->header.refs);
#else
		return --(reinterpret_cast<Element *>((int8_t *)node-sizeof(size_t))->header.refs);
#endif
	}
public:
	MemoryManager(): refs(0), inc_refs(0), m_area(nullptr), m_free_list(nullptr), m_capacity(0)
	{
	}

	~MemoryManager()
	{
		delete [] m_area;
		assert(refs == 0);
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
		std::lock_guard<std::recursive_mutex> lock(mutex);
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
		return result;
	}

	bool CheckCycle(const Node<D> *start)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);;
		const Node<D> *node = start->parent;
		while(node != NULL)
		{
			if(node == start)
			{
				return false;
			}
			node = node->parent;
		}
		return true;
	}

	bool CheckRefs(const Node<D> *node)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		Element * elem = reinterpret_cast<Element *>((int8_t *)node-sizeof(size_t));
		return elem->header.refs <= 2;
	}

	void free(Node<D> *ptr)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		--refs;
		ptr->data.ap_solve.clear();
		Element *elem = reinterpret_cast<Element *>((int8_t *)ptr-sizeof(elem->header));
		assert(elem->header.refs == 0);
		elem->header.next = m_free_list;
		m_free_list = elem;
		if(elem->data.parent != NULL && !DecRefs(const_cast<Node<D> *>(elem->data.parent)))
		{
			assert(ptr->parent == elem->data.parent);
			ptr = const_cast<Node<D> *>(elem->data.parent);
			this->free(ptr);
		}
	}
};
#endif //__TREE_HPP__
