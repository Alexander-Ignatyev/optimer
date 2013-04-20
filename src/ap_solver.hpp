#pragma once 

#include <memory>
#include <vector>

template <typename T> class AP_Solver
{
	AP_Solver(const AP_Solver &);
	AP_Solver &operator=(const AP_Solver &);

	std::vector<ptrdiff_t> int_buffer;
	std::vector<size_t> size_t_buffer;
	std::vector<T> value_type_buffer;

	T *v;
	T *u;
	ptrdiff_t *links;
	ptrdiff_t *visited;
	size_t *markIndices;
	T *mins;
	size_t m_rank;
	
	template <typename U> static void assign(U *data, size_t rank, const U &value)
	{
		for(size_t i = 0; i < rank; ++i)
		{
			data[i] = value;
		}
	}
	
public:

	AP_Solver(size_t rank = 0): v(nullptr), u(nullptr), links(nullptr), visited(nullptr), m_rank(rank)
	{
		alloc(rank);
	}

	void alloc(size_t rank)
	{
		m_rank = rank;
		if (m_rank == 0)
		{
			return;
		}
		int_buffer.resize(rank*2);
		links = &int_buffer[0];
		visited = &int_buffer[rank];

		size_t_buffer.resize(rank);
		markIndices = &size_t_buffer[0];

		value_type_buffer.resize(rank*3);
		mins = &value_type_buffer[0];
		u = &value_type_buffer[rank];
		v = &value_type_buffer[rank*2];
	}

	std::vector<size_t> transform(const T *data, size_t rank)
	{
		assign<T>(u, rank, 0);
		assign<T>(v, rank, 0);
		assign<size_t>(markIndices, rank, -1);

		for (size_t i = 0; i < rank; i++)
		{
			assign<ptrdiff_t>(links, rank, -1);
			assign<T>(mins, rank, M_VAL);
			assign<ptrdiff_t>(visited, rank, 0);

			ptrdiff_t markedI = i, markedJ = -1, j;
			while (markedI != -1)
			{
				j = -1;
				for (size_t j1 = 0; j1 < rank; ++j1)
					if (!visited[j1])
					{
						if (data[markedI*rank+j1] - u[markedI] - v[j1] < mins[j1])
						{
							mins[j1] = data[markedI*rank+j1] - u[markedI] - v[j1];
							links[j1] = markedJ;
						}
						if (j == -1 || mins[j1] < mins[j])
							j = j1;
					}

				T delta = mins[j];
				for (size_t j1 = 0; j1 < rank; j1++)
					if (visited[j1])
					{
						u[markIndices[j1]] += delta;
						v[j1] -= delta;
					}
					else
					{
						mins[j1] -= delta;
					}
				u[i] += delta;

				visited[j] = 1;
				markedJ = j;
				markedI = markIndices[j];
			}

			for (; links[j] != -1; j = links[j])
				markIndices[j] = markIndices[links[j]];
			markIndices[j] = i;
		}

		return std::vector<size_t>(markIndices, markIndices+rank);
	}
};
