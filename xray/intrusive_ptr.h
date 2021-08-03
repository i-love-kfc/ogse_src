#pragma once
/*
struct intrusive_base
{
  unsigned int m_ref_count;
};

template <typename object_type, typename base_type = intrusive_base>
class intrusive_ptr
{
private:
	object_type *m_object;

public:
	object_type			*operator->		() const;
};

template <typename object_type, typename base_type>
inline	typename object_type *intrusive_ptr<object_type,base_type>::operator->() const
{
	VERIFY			(m_object);
	return			(m_object);
}*/