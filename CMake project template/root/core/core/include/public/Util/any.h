#ifndef _ANY_H_
#define _ANY_H_
#include <typeinfo>
#include <memory>

//TODO: Small object optimization
namespace ipengine {
	class any
	{
	private:
		//CONCEPT
		struct any_concept
		{
			using ptr = std::unique_ptr<any_concept>;
			virtual ~any_concept() {}
			virtual const std::type_info& type() const = 0;
			virtual any_concept* clone() const = 0;
			virtual size_t size() const = 0;
		};

		//MODEL
		template <typename T>
		struct any_model : public any_concept
		{
			any_model(T _val) :
				value(std::move(_val))
			{
			}

			virtual const std::type_info & type() const override
			{
				return typeid(T);
			}

			virtual any_concept * clone() const override
			{
				return new any_model(value);
			}

			virtual size_t size() const override
			{
				return sizeof(T);
			}

			T& operator*()
			{
				return value;
			}

			const T& operator*() const
			{
				return value;
			}

		private:
			T value;
		};

	public:
		//PUBLIC INTERFACE
		//default ctor
		any() :
			m_value(std::make_unique<any_model<int>>(0))
		{
		}

		//ctor
		template <typename T>
		any(T val) :
			m_value(std::make_unique<any_model<T>>(std::forward<T>(val)))
		{
		}

		//assign probably needs const fix in the future
		template <typename T>
		any& operator=(T val)
		{
			m_value = std::make_unique<any_model<T>>(std::forward<T>(val));
			return *this;
		}

		//cp ctor
		any(const any& other) :
			m_value(other.m_value->clone())
		{
		}

		//mv ctor
		any(any&& other) :
			m_value(std::move(other.m_value))
		{
		}

		//cp assign
		any& operator=(const any& other)
		{
			if (this == &other)
				return *this;
			any tmp(other);
			swap(tmp);
			return *this;
		}

		//mv assign
		any& operator=(any&& other) noexcept
		{
			if (this == &other)
				return *this;
			m_value = std::move(other.m_value);
			return *this;
		}

		//swap
		void swap(any& other) noexcept
		{
			m_value.swap(other.m_value);
		}

		size_t size() const
		{
			return m_value->size();
		}

		const std::type_info& type() const
		{
			return m_value->type();
		}

		template <typename T>
		T& cast()
		{
			return *dynamic_cast<any_model<T>&>(*m_value);
		}

		template <typename T>
		const T& cast() const
		{
			return *dynamic_cast<any_model<T>&>(*m_value);
		}

		template <typename T>
		operator T&()
		{
			return cast<T>();
		}

		template <typename T>
		operator const T&() const
		{
			return cast<T>();
		}

		void clear()
		{
			m_value = std::make_unique<any_model<int>>(0);
		}


	private:
		any_concept::ptr m_value;
	};


	//sizeof(soo_any) is 32 bytes. SOO will work with up to 16 bytes: 8 bytes are reserved for the vtable pointer. another 8 bytes are needed for the "small" flag.
#define ANY_SOB_SIZE 24

	class soo_any
	{
	private:
		//CONCEPT
		struct soo_any_concept
		{
			using ptr = std::unique_ptr<soo_any_concept>;
			virtual ~soo_any_concept() {}
			virtual const std::type_info& type() const = 0;
			virtual soo_any_concept* clone() const = 0;
			virtual soo_any_concept* move() = 0;
			virtual soo_any_concept* placement_clone(void* ptr) const = 0;
			virtual soo_any_concept* placement_move(void* ptr) = 0;
			virtual size_t size() const = 0;
			virtual size_t internal_size() const = 0;
		};

		//MODEL
		template <typename T>
		struct soo_any_model : public soo_any_concept
		{
			soo_any_model(T _val) :
				value(std::move(_val))
			{
			}

			virtual const std::type_info & type() const override
			{
				return typeid(T);
			}

			virtual soo_any_concept * clone() const override
			{
				return new soo_any_model(value);
			}

			virtual soo_any_concept * move() override
			{
				return new soo_any_model(std::move(value));
			}

			virtual soo_any_concept * placement_clone(void* ptr) const override
			{
				return new(ptr)soo_any_model(value);
			}

			virtual soo_any_concept * placement_move(void* ptr) override
			{
				return new(ptr)soo_any_model(std::move(value));
			}

			virtual size_t size() const override
			{
				return sizeof(T);
			}

			virtual size_t internal_size() const override
			{
				return sizeof(soo_any_model<T>);
			}

			T& operator*()
			{
				return value;
			}

			const T& operator*() const
			{
				return value;
			}

		private:
			T value;
		};

	public:
		//PUBLIC INTERFACE
		//default ctor
		soo_any()
		{
			new(reinterpret_cast<void*>(&m_value))soo_any_model<int>(0);
			small = true;
			//m_value = nullptr;
		}

		~soo_any()
		{
			destruct_old();
		}

		void destruct_old()
		{
			if (small)
			{
				reinterpret_cast<soo_any_concept*>(&m_value)->~soo_any_concept();
				new(reinterpret_cast<void*>(&m_value))soo_any_model<int>(0);
				small = true;
			}
			else
			{
				delete m_value;
				new(reinterpret_cast<void*>(&m_value))soo_any_model<int>(0);
				small = true;
			}
		}

		//ctor
		template <typename T>
		soo_any(T val)
		{
			if (sizeof(soo_any_model<T>) > ANY_SOB_SIZE)
			{
				m_value = new soo_any_model<T>(std::forward<T>(val));
				small = false;
			}
			else
			{
				new(reinterpret_cast<void*>(&m_value))soo_any_model<T>(std::forward<T>(val));
				small = true;
			}
		}

		//assign probably needs const fix in the future
		template <typename T>
		soo_any& operator=(T val)
		{
			destruct_old();
			if (sizeof(soo_any_model<T>) > ANY_SOB_SIZE)
			{
				m_value = new soo_any_model<T>(std::forward<T>(val));
				small = false;
			}
			else
			{
				new(reinterpret_cast<void*>(&m_value))soo_any_model<T>(std::forward<T>(val));
				small = true;
			}

			return *this;
			/*m_value = std::make_unique<soo_any_model<T>>(std::forward<T>(val));
			return *this;*/
		}

		//cp ctor
		soo_any(const soo_any& other)
		{
			if (other.small)
			{
				reinterpret_cast<const soo_any_concept*>(&other.m_value)->placement_clone(reinterpret_cast<void*>(&m_value));
				small = true;
			}
			else
			{
				m_value = other.m_value->clone();
				small = false;
			}
		}

		//mv ctor
		soo_any(soo_any&& other)
		{
			if (other.small)
			{
				reinterpret_cast<soo_any_concept*>(&other.m_value)->placement_move(reinterpret_cast<void*>(&m_value));
				new(reinterpret_cast<void*>(&other.m_value))soo_any_model<int>(0);
				other.small = true;
				small = true;
			}
			else
			{
				m_value = other.m_value;
				new(reinterpret_cast<void*>(&other.m_value))soo_any_model<int>(0);
				other.small = true;
				small = false;
			}
		}

		//cp assign
		soo_any& operator=(const soo_any& other)
		{
			if (this == &other)
				return *this;
			destruct_old();
			if (other.small)
			{
				reinterpret_cast<const soo_any_concept*>(&other.m_value)->placement_clone(reinterpret_cast<void*>(&m_value));
				small = true;
			}
			else
			{
				m_value = other.m_value->clone();
				small = false;
			}
			return *this;
		}

		//mv assign
		soo_any& operator=(soo_any&& other)
		{
			if (this == &other)
				return *this;
			destruct_old();
			if (other.small)
			{
				reinterpret_cast<soo_any_concept*>(&other.m_value)->placement_move(reinterpret_cast<void*>(&m_value));
				new(reinterpret_cast<void*>(&other.m_value))soo_any_model<int>(0);
				other.small = true;
				small = true;
			}
			else
			{
				m_value = other.m_value;
				new(reinterpret_cast<void*>(&other.m_value))soo_any_model<int>(0);
				other.small = true;
				small = false;
			}
			return *this;
		}

		//swap
		void swap(soo_any& other)
		{
			if (other.small)
			{
				if (small)
				{
					unsigned char tmp[ANY_SOB_SIZE];
					reinterpret_cast<soo_any_concept*>(&other.m_value)->placement_move(static_cast<void*>(tmp));
					reinterpret_cast<soo_any_concept*>(&m_value)->placement_move(static_cast<void*>(&other.m_value));
					reinterpret_cast<soo_any_concept*>(&tmp)->placement_move(static_cast<void*>(&m_value));
					reinterpret_cast<soo_any_concept*>(&tmp)->~soo_any_concept();
				}
				else
				{
					unsigned char tmp[ANY_SOB_SIZE];
					reinterpret_cast<soo_any_concept*>(&other.m_value)->placement_move(static_cast<void*>(tmp));
					other.small = false;
					other.m_value = m_value;
					reinterpret_cast<soo_any_concept*>(&tmp)->placement_move(static_cast<void*>(&m_value));
					reinterpret_cast<soo_any_concept*>(&tmp)->~soo_any_concept();
					small = true;
				}
			}
			else
			{
				if (small)
				{
					soo_any_concept* tmp = other.m_value;
					reinterpret_cast<soo_any_concept*>(&m_value)->placement_move(static_cast<void*>(&other.m_value));
					other.small = true;
					reinterpret_cast<soo_any_concept*>(&m_value)->~soo_any_concept();
					m_value = tmp;
					small = false;
				}
				else
				{
					soo_any_concept* tmp = other.m_value;
					other.m_value = m_value;
					m_value = tmp;
				}
			}
		}

		size_t size() const
		{
			if (small)
			{
				return reinterpret_cast<const soo_any_concept*>(&m_value)->size();
			}
			else
			{
				return m_value->size();
			}

		}

		const std::type_info& type() const
		{
			if (small)
			{
				return reinterpret_cast<const soo_any_concept*>(&m_value)->type();
			}
			else
			{
				return m_value->type();
			}
		}

		template <typename T>
		T& cast()
		{
			if (small)
			{
				return *dynamic_cast<soo_any_model<T>&>(*reinterpret_cast<soo_any_concept*>(&m_value));
			}
			else
			{
				return *dynamic_cast<soo_any_model<T>&>(*m_value);
			}

		}

		template <typename T>
		const T& cast() const
		{
			if (small)
			{
				return *dynamic_cast<soo_any_model<T>&>(*reinterpret_cast<soo_any_concept*>(&m_value));
			}
			else
			{
				return *dynamic_cast<soo_any_model<T>&>(*m_value);
			}
		}

		template <typename T>
		operator T&()
		{
			return cast<T>();
		}

		template <typename T>
		operator const T&() const
		{
			return cast<T>();
		}

		void clear()
		{
			destruct_old();
			new(reinterpret_cast<void*>(&m_value))soo_any_model<int>(0);
			small = true;
		}


	private:
		bool small;
		union
		{
			soo_any_concept* m_value;
			unsigned char m_sob[ANY_SOB_SIZE];
		};
	};

	template<size_t SMALL_OBJECT_SIZE = 16>
	class soo_any_s
	{
	private:
		//some size calc stuff

		using this_type = soo_any_s<SMALL_OBJECT_SIZE>;
		static constexpr size_t VPTR_SIZE = sizeof(void*); //hopefully msvc doesn't do weird stuff to vptr in the future
		static constexpr size_t BUF_SIZE = SMALL_OBJECT_SIZE + VPTR_SIZE;
		static constexpr size_t SO_SIZE = SMALL_OBJECT_SIZE;

		//CONCEPT
		struct soo_any_s_concept
		{
			using ptr = std::unique_ptr<soo_any_s_concept>;
			virtual ~soo_any_s_concept() {}
			virtual const std::type_info& type() const = 0;
			virtual soo_any_s_concept* clone() const = 0;
			virtual soo_any_s_concept* move() = 0;
			virtual soo_any_s_concept* placement_clone(void* ptr) const = 0;
			virtual soo_any_s_concept* placement_move(void* ptr) = 0;
			virtual size_t size() const = 0;
			virtual size_t internal_size() const = 0;
		};

		//MODEL
		template <typename T>
		struct soo_any_s_model : public soo_any_s_concept
		{
			soo_any_s_model(T _val) :
				value(std::move(_val))
			{}

			virtual const std::type_info & type() const override
			{
				return typeid(T);
			}

			virtual soo_any_s_concept * clone() const override
			{
				return new soo_any_s_model(value);
			}

			virtual soo_any_s_concept * move() override
			{
				return new soo_any_s_model(std::move(value));
			}

			virtual soo_any_s_concept * placement_clone(void* ptr) const override
			{
				return new(ptr)soo_any_s_model(value);
			}

			virtual soo_any_s_concept * placement_move(void* ptr) override
			{
				return new(ptr)soo_any_s_model(std::move(value));
			}

			virtual size_t size() const override
			{
				return sizeof(T);
			}

			virtual size_t internal_size() const override
			{
				return sizeof(soo_any_s_model<T>);
			}

			T& operator*()
			{
				return value;
			}

			const T& operator*() const
			{
				return value;
			}

		private:
			T value;
		};

	public:
		//PUBLIC INTERFACE
		//default ctor
		soo_any_s()
		{
			new(reinterpret_cast<void*>(&m_value))soo_any_s_model<int>(0);
			small = true;
			//m_value = nullptr;
		}

		~soo_any_s()
		{
			destruct_old();
		}

		void destruct_old()
		{
			if (small)
			{
				reinterpret_cast<soo_any_s_concept*>(&m_value)->~soo_any_s_concept();
				new(reinterpret_cast<void*>(&m_value))soo_any_s_model<int>(0);
				small = true;
			}
			else
			{
				delete m_value;
				new(reinterpret_cast<void*>(&m_value))soo_any_s_model<int>(0);
				small = true;
			}
		}

		//ctor
		template <typename T>
		soo_any_s(T val)
		{
			if (sizeof(soo_any_s_model<T>) > BUF_SIZE)
			{
				m_value = new soo_any_s_model<T>(std::forward<T>(val));
				small = false;
			}
			else
			{
				new(reinterpret_cast<void*>(&m_value))soo_any_s_model<T>(std::forward<T>(val));
				small = true;
			}
		}

		//assign probably needs const fix in the future
		template <typename T>
		soo_any_s& operator=(T val)
		{
			destruct_old();
			if (sizeof(soo_any_s_model<T>) > BUF_SIZE)
			{
				m_value = new soo_any_s_model<T>(std::forward<T>(val));
				small = false;
			}
			else
			{
				new(reinterpret_cast<void*>(&m_value))soo_any_s_model<T>(std::forward<T>(val));
				small = true;
			}

			return *this;
			/*m_value = std::make_unique<soo_any_s_model<T>>(std::forward<T>(val));
			return *this;*/
		}

		//cp ctor
		soo_any_s(const this_type& other)
		{
			if (other.small)
			{
				reinterpret_cast<const soo_any_s_concept*>(&other.m_value)->placement_clone(reinterpret_cast<void*>(&m_value));
				small = true;
			}
			else
			{
				m_value = other.m_value->clone();
				small = false;
			}
		}

		//mv ctor
		soo_any_s(this_type&& other)
		{
			if (other.small)
			{
				reinterpret_cast<soo_any_s_concept*>(&other.m_value)->placement_move(reinterpret_cast<void*>(&m_value));
				new(reinterpret_cast<void*>(&other.m_value))soo_any_s_model<int>(0);
				other.small = true;
				small = true;
			}
			else
			{
				m_value = other.m_value;
				new(reinterpret_cast<void*>(&other.m_value))soo_any_s_model<int>(0);
				other.small = true;
				small = false;
			}
		}

		//cp assign
		soo_any_s& operator=(const this_type& other)
		{
			if (this == &other)
				return *this;
			destruct_old();
			if (other.small)
			{
				reinterpret_cast<const soo_any_s_concept*>(&other.m_value)->placement_clone(reinterpret_cast<void*>(&m_value));
				small = true;
			}
			else
			{
				m_value = other.m_value->clone();
				small = false;
			}
			return *this;
		}

		//mv assign
		soo_any_s& operator=(this_type&& other)
		{
			if (this == &other)
				return *this;
			destruct_old();
			if (other.small)
			{
				reinterpret_cast<soo_any_s_concept*>(&other.m_value)->placement_move(reinterpret_cast<void*>(&m_value));
				new(reinterpret_cast<void*>(&other.m_value))soo_any_s_model<int>(0);
				other.small = true;
				small = true;
			}
			else
			{
				m_value = other.m_value;
				new(reinterpret_cast<void*>(&other.m_value))soo_any_s_model<int>(0);
				other.small = true;
				small = false;
			}
			return *this;
		}

		//swap
		void swap(this_type& other)
		{
			if (other.small)
			{
				if (small)
				{
					unsigned char tmp[BUF_SIZE];
					reinterpret_cast<soo_any_s_concept*>(&other.m_value)->placement_move(static_cast<void*>(tmp));
					reinterpret_cast<soo_any_s_concept*>(&m_value)->placement_move(static_cast<void*>(&other.m_value));
					reinterpret_cast<soo_any_s_concept*>(&tmp)->placement_move(static_cast<void*>(&m_value));
					reinterpret_cast<soo_any_s_concept*>(&tmp)->~soo_any_s_concept();
				}
				else
				{
					unsigned char tmp[BUF_SIZE];
					reinterpret_cast<soo_any_s_concept*>(&other.m_value)->placement_move(static_cast<void*>(tmp));
					other.small = false;
					other.m_value = m_value;
					reinterpret_cast<soo_any_s_concept*>(&tmp)->placement_move(static_cast<void*>(&m_value));
					reinterpret_cast<soo_any_s_concept*>(&tmp)->~soo_any_s_concept();
					small = true;
				}
			}
			else
			{
				if (small)
				{
					soo_any_s_concept* tmp = other.m_value;
					reinterpret_cast<soo_any_s_concept*>(&m_value)->placement_move(static_cast<void*>(&other.m_value));
					other.small = true;
					reinterpret_cast<soo_any_s_concept*>(&m_value)->~soo_any_s_concept();
					m_value = tmp;
					small = false;
				}
				else
				{
					soo_any_s_concept* tmp = other.m_value;
					other.m_value = m_value;
					m_value = tmp;
				}
			}
		}

		size_t size() const
		{
			if (small)
			{
				return reinterpret_cast<const soo_any_s_concept*>(&m_value)->size();
			}
			else
			{
				return m_value->size();
			}

		}

		const std::type_info& type() const
		{
			if (small)
			{
				return reinterpret_cast<const soo_any_s_concept*>(&m_value)->type();
			}
			else
			{
				return m_value->type();
			}
		}

		template <typename T>
		T& cast()
		{
			if (small)
			{
				return *dynamic_cast<soo_any_s_model<T>&>(*reinterpret_cast<soo_any_s_concept*>(&m_value));
			}
			else
			{
				return *dynamic_cast<soo_any_s_model<T>&>(*m_value);
			}

		}

		template <typename T>
		const T& cast() const
		{
			if (small)
			{
				return *dynamic_cast<soo_any_s_model<T>&>(*reinterpret_cast<soo_any_s_concept*>(&m_value));
			}
			else
			{
				return *dynamic_cast<soo_any_s_model<T>&>(*m_value);
			}
		}

		template <typename T>
		operator T&()
		{
			return cast<T>();
		}

		template <typename T>
		operator const T&() const
		{
			return cast<T>();
		}

		void clear()
		{
			destruct_old();
			new(reinterpret_cast<void*>(&m_value))soo_any_s_model<int>(0);
			small = true;
		}


	private:		
		union
		{
			soo_any_s_concept* m_value;
			unsigned char m_sob[BUF_SIZE];
		};
		bool small;
	};
}
#endif
