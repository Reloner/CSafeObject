#pragma once

#include <mutex>
#include <map>
using namespace std;
typedef enum {
	em�����ڸö��� = 0,
	em����ʹ���� = 1,
	em������� = 2,
	em���ö���ɹ� = 2
}enCSafeMapStatus;

#define def�ж϶���״̬(id)						\
{												\
	do                                          \
	{	\
		std::lock_guard<std::mutex> hold(m_mtx);\
		auto n = m_count[id];					\
		if (n == em�����ڸö���)				 \
			return enCSafeMapStatus(n);			 \
		if (n == em����ʹ����)					 \
			continue;							 \
		m_count[id] = em����ʹ����;					\
		break;									 \
	} while (1);								 \
}

template<class T1, class T2>
class CSafeMap
{
public:
	CSafeMap() {}
	CSafeMap(const CSafeMap& cc) { *this = cc; }
	virtual ~CSafeMap(void) { }

	enCSafeMapStatus CtrlItem(T1 id, std::function<void(T1, T2&)> Func)
	{
		NowLogStringEx(_T("CSafeMap::CtrlItem Begin"));
		def�ж϶���״̬(id);
		Func(id, m_data[id]);
		{
			std::lock_guard<std::mutex> hold(m_mtx);
			m_count[id] = em�������;
		}
		NowLogStringEx(_T("CSafeMap::CtrlItem End"));
		return em���ö���ɹ�;
	}
	void CtrlItems(std::function<bool(T1, T2&)> Func)
	{
		int ncount = 0, nSum = 0;
		do
		{
			std::lock_guard<std::mutex> hold(m_mtx);
			for (auto itr = m_count.begin(); itr != m_count.end(); itr++) {
				if (itr->second != em�����ڸö���) {
					if (itr->second != em�������)continue;
					if (Func(itr->first, m_data[itr->first]) == false)
						return;
				}
				ncount++;
			}
			nSum = m_count.size();
		} while (nSum != ncount);
	}
	bool AddItem(T1 id, T2 t, std::function<bool(T2&)> Func = [](T2&itr) {})
	{
		std::lock_guard<std::mutex> hold(m_mtx);
		if (m_count[id] != em�����ڸö���)
			return false;
		if (Func(t))
		{
			m_count[id] = em�������;
			m_data[id] = t;
		}
		return true;
	}
	
	enCSafeMapStatus RemoveItem(T1 id) {
		NowLogStringEx(_T("CSafeMap::RemoveItem Begin"));
		def�ж϶���״̬(id);
		auto pt = m_data.find(id);
		m_data.erase(pt);
		{
			std::lock_guard<std::mutex> hold(m_mtx);
			auto pt2 = m_count.find(id);
			m_count.erase(pt2);
		}
		NowLogStringEx(_T("CSafeMap::RemoveItem End"));
		return em���ö���ɹ�;
	}
	enCSafeMapStatus DeleteItem(T1 id, std::function<void(T2&)> Func = [](T2&itr) {if (itr)delete itr; itr = nullptr; })
	{
		NowLogStringEx(_T("CSafeMap::DeleteItem Begin"));
		def�ж϶���״̬(id);
		Func(m_data[id]);
		auto pt = m_data.find(id);
		m_data.erase(pt);
		{
			std::lock_guard<std::mutex> hold(m_mtx);
			auto pt2 = m_count.find(id);
			m_count.erase(pt2);
		}
		NowLogStringEx(_T("CSafeMap::DeleteItem End"));
		return em���ö���ɹ�;
	}
	void RemoveAll()
	{
		int ncount = 0;
		do 
		{
			std::lock_guard<std::mutex> hold(m_mtx);
			for (auto itr = m_count.begin(), auto itrDt = m_data.begin(); itr != m_count.end();) {
				if (itr == em�������)
				{
					m_data.erase(itrDt);
					m_count.erase(itr);
				}
				else
				{
					itrDt++;
					itr++;
				}
			}
			ncount = m_count.count();
		} while (ncount != 0);
	}
	void DeleteAll(std::function<void(T2&)> Func = [](T2&itr) {if (itr)delete itr; itr = nullptr; })
	{
		int ncount = 0;
		do
		{
			std::lock_guard<std::mutex> hold(m_mtx);
			auto itrDt = m_data.begin();
			for (auto itr = m_count.begin(); itr != m_count.end();) {
				if (itr->second == em�������)
				{
					Func(itrDt->second);
					m_data.erase(itrDt);
					m_count.erase(itr);
				}
				else
				{
					itrDt++;
					itr++;
				}
			}
			ncount = m_count.size();
		} while (ncount != 0);
	}
	int GetSize()
	{
		std::lock_guard<std::mutex> hold(m_mtx);
		return 	 m_data.size();
	}
	bool GetItem(T1 nid,T2&data)
	{
		std::lock_guard<std::mutex> hold(m_mtx);
		if (m_count[nid] == em�����ڸö���)
			return false;
		data = m_data[nid];
		return true;
	}
	map<T1, T2> Copy()
	{
		std::lock_guard<std::mutex> hold(m_mtx);
		return 	 m_data;
	}
private:
	map<T1, T2> m_data;
	map<T1, int> m_count;
	std::mutex m_mtx;
};


template<class T1>
class CSafeItems
{
public:
	CSafeItems() { nCountMark = 0; }
	CSafeItems(const CSafeItems& cc) { *this = cc; nCountMark = 0; }
	virtual ~CSafeItems(void) { }
public:
	int FillItems(int nCount,std::function<T1()> Func)
	{
		std::lock_guard<std::mutex> hold(m_mtx);
		for (int n = 0; n < nCount;n++)
		{
			auto itr = Func();
			if (itr)
			{
				m_data.push(make_pair(itr, nCountMark));
				m_mark.insert(make_pair(nCountMark,(int)em�������));
				nCountMark++;
			}
			else
			{
				break;
			}
		}
		return m_data.size();
	}
	void CtrlItem(std::function<void(T1&)> Func)
	{
		bool bret = false;
		do
		{
			T1 t1;
			int nMark;
			{
				std::lock_guard<std::mutex> hold(m_mtx);
				if (m_data.size() == 0)continue;
				auto itr = m_data.front();
				t1 = itr.first;
				nMark = itr.second;
				m_mark[nMark] = em����ʹ����;
				m_data.pop();
			}
			Func(t1);
			{
				std::lock_guard<std::mutex> hold(m_mtx);
				m_mark[nMark] = em�������;
				m_data.push(make_pair(t1,nMark));
			}
			bret = true;
		} while (bret == false);
	}
	void RemoveAll()
	{
		while (m_mark.size())
		{
			std::lock_guard<std::mutex> hold(m_mtx);
			auto itr = m_data.front();
			m_mark.erase(m_mark[itr.second]);
			m_data.pop();
		}
	}
	void DeleteAll(std::function<void(T1&)> Func = [](T1&itr) {if (itr)delete itr; itr = nullptr; })
	{
		while (m_mark.size())
		{
			std::lock_guard<std::mutex> hold(m_mtx);
			auto itr = m_data.front();
			Func(itr.first);
			m_mark.erase(m_mark[itr.second]);
			m_data.pop();
		}
	}
	int GetSize()
	{
		std::lock_guard<std::mutex> hold(m_mtx);
		return m_count.size();
	}
private:
	int nCountMark;
	map<int,int> m_mark;				 
	queue<pair<T1,int>> m_data;
	std::mutex m_mtx;
};


template<class T1>
class CSafeObject
{
public:
	CSafeObject() { m_Mark = em�����ڸö���; }
	CSafeObject(const CSafeObject& cc) { *this = cc; }
	virtual ~CSafeObject(void) { }
public:
	bool CreateObject(std::function<bool(T1&tm)> Func)
	{
		std::lock_guard<std::mutex> hold(m_mtx);
		if (Func(m_data))
		{
			m_Mark = em�������;
			return true;
		}
		return false;
	}
	void CtrlItem(std::function<void(T1&)> Func)
	{
		bool bret = false;
		do
		{
			{
				std::lock_guard<std::mutex> hold(m_mtx);
				if (m_Mark == em�����ڸö���)return;
				if (m_Mark != em�������) continue;
				m_Mark = em����ʹ����;
			}
			Func(m_data);
			{
				std::lock_guard<std::mutex> hold(m_mtx);
				m_Mark = em�������;
			}
			bret = true;
		} while (bret == false);

	}
	void Delete(std::function<void(T1&)> Func = [](T1&itr) {if (itr)delete itr; itr = nullptr; })
	{

		bool bret = false;
		do
		{
			{
				std::lock_guard<std::mutex> hold(m_mtx);
				if (m_Mark == em�����ڸö���)return;
				if (m_Mark != em�������) continue;
				m_Mark = em����ʹ����;
			}
			Func(m_data);
			{
				std::lock_guard<std::mutex> hold(m_mtx);
				m_Mark = em�������;
			}
			bret = true;
		} while (bret == false);
		m_Mark = em�����ڸö���;
	}

private:
	T1 m_data;
	int m_Mark;
	std::mutex m_mtx;
};

