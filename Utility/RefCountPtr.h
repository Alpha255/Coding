#pragma once

template <typename Type> class Ref 
{
public:
	__forceinline Ref(void)
		: m_pPtr(nullptr) 
	{
	}

	__forceinline Ref(Type* pSrc)
		: m_pPtr(pSrc) 
	{
		if (m_pPtr != nullptr)
		{
			m_pPtr->AddRef();
		}
	}

	__forceinline Ref(const Ref& refSrc)
		: m_pPtr(refSrc.m_pPtr) 
	{
		if (m_pPtr != nullptr)
		{
			m_pPtr->AddRef();
		}
	}
	__forceinline ~Ref(void) 
	{
		if (m_pPtr != nullptr) 
		{
			m_pPtr->Release();
			m_pPtr = nullptr;
		}
	}

	__forceinline Ref& operator= (const Ref& refSrc) 
	{
		if (refSrc.m_pPtr == m_pPtr)
		{
			return *this;
		}

		if (m_pPtr != nullptr)
		{
			m_pPtr->Release();
		}

		m_pPtr = refSrc.m_pPtr;

		if (m_pPtr)
		{
			m_pPtr->AddRef();
		}

		return *this;
	}

	__forceinline bool operator== (const Ref& refSrc) const 
	{
		return (m_pPtr == refSrc.m_pPtr);
	}

	__forceinline bool operator== (const Type* pSrc) const 
	{
		return (m_pPtr == pSrc);
	}

	__forceinline bool operator!= (const Ref& refSrc) const 
	{
		return (m_pPtr != refSrc.m_pPtr);
	}

	__forceinline bool operator!= (const Type* pSrc) const 
	{
		return (m_pPtr != pSrc);
	}

	__forceinline bool operator< (const Ref& refSrc) const 
	{
		return (m_pPtr < refSrc.m_pPtr);
	}

	__forceinline bool operator< (const Type* pSrc) const 
	{
		return (m_pPtr < pSrc);
	}

	__forceinline bool operator> (const Ref& refSrc) const 
	{
		return (m_pPtr > refSrc.m_pPtr);
	}

	__forceinline bool operator> (const Type* pSrc) const 
	{
		return (m_pPtr > pSrc);
	}

	__forceinline Type& operator* (void) 
	{
		return *m_pPtr;
	}

	__forceinline const Type& operator* (void) const 
	{
		return *m_pPtr;
	}

	__forceinline Type* operator-> (void) 
	{
		return m_pPtr;
	}

	__forceinline const Type* operator-> (void) const 
	{
		return m_pPtr;
	}

	__forceinline bool operator!(void) const 
	{
		return (nullptr == m_pPtr);
	}

	__forceinline bool Valid(void) const 
	{
		return (m_pPtr != nullptr);
	}

	__forceinline bool ValidRef(const Ref& refSrc) 
	{
		return (refSrc.m_pPtr != nullptr);
	}

	__forceinline Type* Ptr(void) const 
	{
		return m_pPtr;
	}

	__forceinline Type** Reference() 
	{
		return &m_pPtr;
	}
protected:
private:
	Type* m_pPtr;
};

